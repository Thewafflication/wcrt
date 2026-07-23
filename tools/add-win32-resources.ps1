[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$Binary,

    [Parameter(Mandatory = $true)]
    [string]$CompiledResource
)

$ErrorActionPreference = 'Stop'
Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;

public static class WcrtResourceApi
{
    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    public static extern IntPtr BeginUpdateResource(string file, bool deleteExisting);

    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    public static extern bool UpdateResource(IntPtr update, IntPtr type,
        IntPtr name, ushort language, byte[] data, uint size);

    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern bool EndUpdateResource(IntPtr update, bool discard);
}
'@

function Read-ResourceIdentifier {
    param([IO.BinaryReader]$Reader)
    $first = $Reader.ReadUInt16()
    if ($first -eq 0xffff) {
        return [PSCustomObject]@{ Ordinal = $Reader.ReadUInt16(); Text = $null }
    }
    $characters = [Collections.Generic.List[char]]::new()
    while ($first -ne 0) {
        $characters.Add([char]$first)
        $first = $Reader.ReadUInt16()
    }
    [PSCustomObject]@{ Ordinal = $null; Text = -join $characters }
}

function Get-IdentifierPointer {
    param($Identifier)
    if ($null -ne $Identifier.Ordinal) {
        return [PSCustomObject]@{ Pointer = [IntPtr]$Identifier.Ordinal; Allocated = $false }
    }
    [PSCustomObject]@{
        Pointer = [Runtime.InteropServices.Marshal]::StringToHGlobalUni($Identifier.Text)
        Allocated = $true
    }
}

$binaryPath = (Resolve-Path -LiteralPath $Binary).Path
$resourcePath = (Resolve-Path -LiteralPath $CompiledResource).Path
$stream = [IO.File]::OpenRead($resourcePath)
$reader = [IO.BinaryReader]::new($stream)
$records = [Collections.Generic.List[object]]::new()
try {
    while ($stream.Position -lt $stream.Length) {
        while (($stream.Position % 4) -ne 0) { $null = $reader.ReadByte() }
        if ($stream.Position -ge $stream.Length) { break }
        $recordStart = $stream.Position
        $dataSize = $reader.ReadUInt32()
        $headerSize = $reader.ReadUInt32()
        $type = Read-ResourceIdentifier $reader
        $name = Read-ResourceIdentifier $reader
        $stream.Position = $recordStart + $headerSize - 10
        $language = $reader.ReadUInt16()
        $stream.Position = $recordStart + $headerSize
        $data = $reader.ReadBytes($dataSize)
        if ($dataSize -gt 0) {
            $records.Add([PSCustomObject]@{
                Type = $type; Name = $name; Language = $language; Data = $data
            })
        }
    }
} finally {
    $reader.Dispose()
    $stream.Dispose()
}

$handle = [WcrtResourceApi]::BeginUpdateResource($binaryPath, $false)
if ($handle -eq [IntPtr]::Zero) {
    throw "BeginUpdateResource failed: $([Runtime.InteropServices.Marshal]::GetLastWin32Error())"
}
$success = $false
try {
    foreach ($record in $records) {
        $typePointer = Get-IdentifierPointer $record.Type
        $namePointer = Get-IdentifierPointer $record.Name
        try {
            if (-not [WcrtResourceApi]::UpdateResource($handle,
                $typePointer.Pointer, $namePointer.Pointer, $record.Language,
                $record.Data, $record.Data.Length)) {
                throw "UpdateResource failed: $([Runtime.InteropServices.Marshal]::GetLastWin32Error())"
            }
        } finally {
            if ($typePointer.Allocated) {
                [Runtime.InteropServices.Marshal]::FreeHGlobal($typePointer.Pointer)
            }
            if ($namePointer.Allocated) {
                [Runtime.InteropServices.Marshal]::FreeHGlobal($namePointer.Pointer)
            }
        }
    }
    $success = $true
} finally {
    if (-not [WcrtResourceApi]::EndUpdateResource($handle, -not $success)) {
        throw "EndUpdateResource failed: $([Runtime.InteropServices.Marshal]::GetLastWin32Error())"
    }
}
