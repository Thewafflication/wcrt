function Get-WcrtStartupToolchain {
    param(
        [Parameter(Mandatory)][string]$TinyCc,
        [Parameter(Mandatory)][string]$Architecture
    )

    $compilerRoot = Split-Path -Parent $TinyCc
    $prefix = switch ($Architecture) {
        'x86' { 'i386-win32' }
        'x64' { 'x86_64-win32' }
        'arm64' { 'arm64-win32' }
    }
    [PSCustomObject]@{
        CompilerRoot = $compilerRoot
        CompilerSupport = Join-Path $compilerRoot "lib/$prefix-libtcc1.a"
        KernelDefinition = Join-Path $compilerRoot 'lib/kernel32.def'
    }
}

function Get-WcrtPeInformation {
    param([Parameter(Mandatory)][string]$Path)

    $bytes = [IO.File]::ReadAllBytes($Path)
    $peOffset = [BitConverter]::ToInt32($bytes, 0x3c)
    if ([BitConverter]::ToUInt32($bytes, $peOffset) -ne 0x00004550) {
        throw "$Path is not a PE image."
    }
    $optional = $peOffset + 24
    $text = [Text.Encoding]::ASCII.GetString($bytes)
    [PSCustomObject]@{
        Machine = [BitConverter]::ToUInt16($bytes, $peOffset + 4)
        EntryPoint = [BitConverter]::ToUInt32($bytes, $optional + 16)
        Subsystem = [BitConverter]::ToUInt16($bytes, $optional + 68)
        HostCrtImport = $text -match `
            '(?i)(msvcrt|ucrtbase|vcruntime[^.]*)\.dll'
    }
}

function Invoke-WcrtStartupLink {
    param(
        [Parameter(Mandatory)][string]$TinyCc,
        [Parameter(Mandatory)][string]$Architecture,
        [Parameter(Mandatory)][string]$BuildDirectory,
        [Parameter(Mandatory)][string]$StartupObject,
        [Parameter(Mandatory)][string]$TestSource,
        [Parameter(Mandatory)][string]$Subsystem,
        [Parameter(Mandatory)][string]$OutputPath
    )

    $toolchain = Get-WcrtStartupToolchain $TinyCc $Architecture
    foreach ($path in @($toolchain.CompilerSupport,
        $toolchain.KernelDefinition)) {
        if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
            throw "Required TinyCC link input was not found: $path"
        }
    }
    $arguments = @(
        '-std=c89', '-Wall', '-Werror', '-nostdlib', '-Wl,-nostdlib',
        "-Wl,-subsystem=$Subsystem", '-I',
        (Join-Path $BuildDirectory 'include'), $StartupObject, $TestSource,
        (Join-Path $BuildDirectory 'libwcrt.a'),
        $toolchain.CompilerSupport, $toolchain.KernelDefinition,
        '-o', $OutputPath
    )
    $lines = @(& $TinyCc @arguments 2>&1)
    if ($LASTEXITCODE -ne 0) {
        throw "Startup consumer failed to link:`n$($lines -join "`n")"
    }
    ($lines | ForEach-Object { $_.ToString() }) -join "`n"
}

function Invoke-WcrtOrdinaryLink {
    param(
        [Parameter(Mandatory)][string]$TinyCc,
        [Parameter(Mandatory)][string]$BuildDirectory,
        [Parameter(Mandatory)][string]$TestSource,
        [Parameter(Mandatory)][string]$OutputPath
    )

    $arguments = @(
        '-std=c89', '-Wall', '-Werror', '-I',
        (Join-Path $BuildDirectory 'include'), $TestSource,
        (Join-Path $BuildDirectory 'libwcrt.a'), '-o', $OutputPath
    )
    $lines = @(& $TinyCc @arguments 2>&1)
    if ($LASTEXITCODE -ne 0) {
        throw "Ordinary static consumer failed to link:`n" +
            ($lines -join "`n")
    }
    ($lines | ForEach-Object { $_.ToString() }) -join "`n"
}

function Invoke-WcrtStartupProcess {
    param(
        [Parameter(Mandatory)][string]$Path,
        [string]$ArgumentList,
        [switch]$Hidden
    )

    $parameters = @{
        FilePath = $Path
        PassThru = $true
        Wait = $true
    }
    if (-not [string]::IsNullOrEmpty($ArgumentList)) {
        $parameters.ArgumentList = $ArgumentList
    }
    if ($Hidden) {
        $parameters.WindowStyle = 'Hidden'
    }
    (Start-Process @parameters).ExitCode
}

function Test-WcrtNativeArchitecture {
    param([Parameter(Mandatory)][string]$Architecture)

    $native = [Runtime.InteropServices.RuntimeInformation]::OSArchitecture
    ($Architecture -eq 'x86' -and $native -in 'X86', 'X64', 'Arm64') -or
        ($Architecture -eq 'x64' -and $native -in 'X64', 'Arm64') -or
        ($Architecture -eq 'arm64' -and $native -eq 'Arm64')
}
