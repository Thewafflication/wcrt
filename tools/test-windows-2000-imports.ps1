[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [string]$Binary,

    [string]$Allowlist = 'tests/windows-2000-x86-imports.json',

    [string]$OutputRoot = 'output/test-results'
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$binaryPath = (Resolve-Path -LiteralPath $Binary).Path
$allowlistPath = (Resolve-Path -LiteralPath (
    Join-Path $repoRoot $Allowlist)).Path

function Read-WcrtUInt16 {
    param([byte[]]$Bytes, [int]$Offset)
    if ($Offset -lt 0 -or $Offset + 2 -gt $Bytes.Length) {
        throw "PE read exceeds the image at offset $Offset."
    }
    [BitConverter]::ToUInt16($Bytes, $Offset)
}

function Read-WcrtUInt32 {
    param([byte[]]$Bytes, [int]$Offset)
    if ($Offset -lt 0 -or $Offset + 4 -gt $Bytes.Length) {
        throw "PE read exceeds the image at offset $Offset."
    }
    [BitConverter]::ToUInt32($Bytes, $Offset)
}

function Read-WcrtUInt64 {
    param([byte[]]$Bytes, [int]$Offset)
    if ($Offset -lt 0 -or $Offset + 8 -gt $Bytes.Length) {
        throw "PE read exceeds the image at offset $Offset."
    }
    [BitConverter]::ToUInt64($Bytes, $Offset)
}

function Read-WcrtAsciiZ {
    param([byte[]]$Bytes, [int]$Offset)
    if ($Offset -lt 0 -or $Offset -ge $Bytes.Length) {
        throw "PE string offset $Offset is outside the image."
    }
    $end = $Offset
    while ($end -lt $Bytes.Length -and $Bytes[$end] -ne 0) {
        $end++
    }
    if ($end -eq $Bytes.Length) {
        throw "PE string at offset $Offset is not terminated."
    }
    [Text.Encoding]::ASCII.GetString($Bytes, $Offset, $end - $Offset)
}

function Get-WcrtPeImports {
    param([Parameter(Mandatory)][string]$Path)

    $bytes = [IO.File]::ReadAllBytes($Path)
    if ($bytes.Length -lt 0x40 -or
        [Text.Encoding]::ASCII.GetString($bytes, 0, 2) -ne 'MZ') {
        throw "$Path is not a DOS/PE image."
    }
    $peOffset = [int](Read-WcrtUInt32 $bytes 0x3c)
    if ((Read-WcrtUInt32 $bytes $peOffset) -ne 0x00004550) {
        throw "$Path has no PE signature."
    }

    $machine = Read-WcrtUInt16 $bytes ($peOffset + 4)
    $sectionCount = Read-WcrtUInt16 $bytes ($peOffset + 6)
    $optionalSize = Read-WcrtUInt16 $bytes ($peOffset + 20)
    $optionalOffset = $peOffset + 24
    $optionalMagic = Read-WcrtUInt16 $bytes $optionalOffset
    if ($optionalMagic -eq 0x10b) {
        $directoryOffset = $optionalOffset + 96
        $thunkSize = 4
        [uint64]$ordinalMask = 0x80000000L
    } elseif ($optionalMagic -eq 0x20b) {
        $directoryOffset = $optionalOffset + 112
        $thunkSize = 8
        [uint64]$ordinalMask = 0x8000000000000000L
    } else {
        throw ('Unsupported PE optional-header magic 0x{0:X}.' -f
            $optionalMagic)
    }
    $importRva = Read-WcrtUInt32 $bytes ($directoryOffset + 8)
    if ($importRva -eq 0) {
        return [PSCustomObject]@{ Machine = $machine; Imports = @() }
    }

    $sectionOffset = $optionalOffset + $optionalSize
    $sections = @(
        for ($index = 0; $index -lt $sectionCount; $index++) {
            $offset = $sectionOffset + 40 * $index
            [PSCustomObject]@{
                VirtualSize = Read-WcrtUInt32 $bytes ($offset + 8)
                VirtualAddress = Read-WcrtUInt32 $bytes ($offset + 12)
                RawSize = Read-WcrtUInt32 $bytes ($offset + 16)
                RawOffset = Read-WcrtUInt32 $bytes ($offset + 20)
            }
        }
    )

    function Convert-WcrtRvaToOffset {
        param([uint32]$Rva)
        foreach ($section in $sections) {
            $span = [Math]::Max($section.VirtualSize, $section.RawSize)
            if ($Rva -ge $section.VirtualAddress -and
                $Rva -lt $section.VirtualAddress + $span) {
                return [int]($section.RawOffset +
                    ($Rva - $section.VirtualAddress))
            }
        }
        throw ('PE RVA 0x{0:X8} is not mapped by a section.' -f $Rva)
    }

    $imports = [Collections.Generic.List[object]]::new()
    $descriptorOffset = Convert-WcrtRvaToOffset $importRva
    while ($true) {
        $lookupRva = Read-WcrtUInt32 $bytes $descriptorOffset
        $nameRva = Read-WcrtUInt32 $bytes ($descriptorOffset + 12)
        $addressRva = Read-WcrtUInt32 $bytes ($descriptorOffset + 16)
        if ($lookupRva -eq 0 -and $nameRva -eq 0 -and $addressRva -eq 0) {
            break
        }
        $module = Read-WcrtAsciiZ $bytes (Convert-WcrtRvaToOffset $nameRva)
        $thunkRva = if ($lookupRva -ne 0) { $lookupRva } else { $addressRva }
        $thunkOffset = Convert-WcrtRvaToOffset $thunkRva
        while ($true) {
            [uint64]$thunk = if ($thunkSize -eq 4) {
                Read-WcrtUInt32 $bytes $thunkOffset
            } else {
                Read-WcrtUInt64 $bytes $thunkOffset
            }
            if ($thunk -eq 0) { break }
            if (($thunk -band $ordinalMask) -ne 0) {
                $name = "#$($thunk -band 0xffff)"
            } else {
                $nameOffset = Convert-WcrtRvaToOffset ([uint32]$thunk)
                $name = Read-WcrtAsciiZ $bytes ($nameOffset + 2)
            }
            $imports.Add([PSCustomObject]@{
                Module = $module.ToLowerInvariant()
                Name = $name
            })
            $thunkOffset += $thunkSize
        }
        $descriptorOffset += 20
    }
    [PSCustomObject]@{ Machine = $machine; Imports = @($imports) }
}

function Compare-WcrtImports {
    param([object[]]$Imports, [object]$Policy)
    @(
        foreach ($import in $Imports) {
            $module = $Policy.modules.PSObject.Properties |
                Where-Object Name -IEQ $import.Module |
                Select-Object -First 1
            if ($null -eq $module -or
                $import.Name -notin @($module.Value)) {
                $import
            }
        }
    )
}

$policy = Get-Content -LiteralPath $allowlistPath -Raw | ConvertFrom-Json
$negativeControl = [PSCustomObject]@{
    Module = 'kernel32.dll'
    Name = 'GetTickCount64'
}
if ((Compare-WcrtImports @($negativeControl) $policy).Count -ne 1) {
    throw 'Windows 2000 import allowlist negative control did not fail.'
}

$pe = Get-WcrtPeImports $binaryPath
if ($pe.Machine -ne 0x014c) {
    throw ('Windows 2000 import gate requires x86 machine 0x014C; got ' +
        ('0x{0:X4}.' -f $pe.Machine))
}
if ($pe.Imports.Count -eq 0) {
    throw 'The x86 WCRT DLL unexpectedly has no imports to inspect.'
}
$unexpected = @(Compare-WcrtImports $pe.Imports $policy)
$outputDirectory = Join-Path $repoRoot "$OutputRoot/x86"
New-Item -ItemType Directory -Force $outputDirectory | Out-Null
$record = [PSCustomObject]@{
    MinimumTarget = $policy.minimumTarget
    Architecture = $policy.architecture
    Binary = [IO.Path]::GetRelativePath($repoRoot, $binaryPath).
        Replace('\', '/')
    Sha256 = (Get-FileHash -LiteralPath $binaryPath -Algorithm SHA256).Hash
    Machine = ('0x{0:X4}' -f $pe.Machine)
    Imports = @($pe.Imports | Sort-Object Module, Name)
    Unexpected = $unexpected
    Status = if ($unexpected.Count -eq 0) { 'Pass' } else { 'Fail' }
}
$jsonPath = Join-Path $outputDirectory 'windows-2000-imports.json'
$record | ConvertTo-Json -Depth 6 |
    Set-Content -LiteralPath $jsonPath -Encoding utf8NoBOM
$record
if ($unexpected.Count -ne 0) {
    $names = $unexpected | ForEach-Object { "$($_.Module)!$($_.Name)" }
    throw "Windows 2000 x86 import gate failed: $($names -join ', ')."
}

