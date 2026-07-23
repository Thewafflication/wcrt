[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('x86', 'x64', 'arm64')]
    [string]$Architecture,

    [Parameter(Mandatory = $true)]
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration,

    [Parameter(Mandatory = $true)]
    [string]$TinyCc,

    [string]$OutputRoot = 'output/build'
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$tinyCcPath = (Resolve-Path -LiteralPath $TinyCc).Path
$outputDirectory = Join-Path $repoRoot "$OutputRoot/$Architecture/$Configuration"
New-Item -ItemType Directory -Force -Path $outputDirectory | Out-Null

$compilerDescription = (& $tinyCcPath -v 2>&1 | Select-Object -First 1).ToString()
$expectedTarget = @{
    x86 = 'i386 Windows'
    x64 = 'x86_64 Windows'
    arm64 = 'AArch64 Windows'
}[$Architecture]
if ($compilerDescription -notmatch [regex]::Escape($expectedTarget)) {
    throw "TinyCC target mismatch. Expected '$expectedTarget', got '$compilerDescription'."
}

$sources = @(
    'src/assert.c', 'src/ctype.c', 'src/errno.c', 'src/locale.c',
    'src/math.c', 'src/setjmp.c', 'src/signal.c', 'src/stdio.c',
    'src/stdio_format.c', 'src/stdio_scan.c', 'src/stdlib.c',
    'src/string.c', 'src/time.c', 'src/platform/windows/clock.c',
    'src/platform/windows/file.c', 'src/platform/windows/heap.c',
    'src/platform/windows/process.c', 'src/platform/windows/setjmp.S',
    'src/platform/windows/signal.c'
) | ForEach-Object { Join-Path $repoRoot $_ }

$dllPath = Join-Path $outputDirectory 'wcrt.dll'
$arguments = @('-std=c89', '-Wall', '-Werror', '-shared', '-I',
    (Join-Path $repoRoot 'include'))
if ($Configuration -eq 'Debug') {
    # TinyCC invokes cv2pdb.exe from PATH for this option.
    $arguments += '-g.pdb'
} else {
    $arguments += '-O2'
}
$arguments += $sources
$arguments += @('-o', $dllPath)

& $tinyCcPath @arguments
if ($LASTEXITCODE -ne 0) {
    throw "TinyCC failed to build the $Architecture $Configuration DLL."
}
if (-not (Test-Path -LiteralPath $dllPath -PathType Leaf)) {
    throw "TinyCC did not produce $dllPath."
}
if ($Configuration -eq 'Debug') {
    $pdbPath = [IO.Path]::ChangeExtension($dllPath, '.pdb')
    if (-not (Test-Path -LiteralPath $pdbPath -PathType Leaf)) {
        throw "TinyCC did not produce $pdbPath. Ensure cv2pdb.exe is in PATH."
    }
}

[PSCustomObject]@{
    Architecture = $Architecture
    Configuration = $Configuration
    Compiler = $compilerDescription
    Dll = $dllPath
    Pdb = if ($Configuration -eq 'Debug') { $pdbPath } else { $null }
}
