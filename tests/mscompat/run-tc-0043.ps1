[CmdletBinding()]
param([string]$TinyCc)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$buildDirectory = Join-Path $repoRoot 'build\tests\mscompat\tc-0043'
$presenceObject = Join-Path $buildDirectory 'ms-io-presence.o'
$header = Join-Path $repoRoot 'include\io.h'
if ([string]::IsNullOrWhiteSpace($TinyCc)) {
    $TinyCc = Get-Command tcc.exe -ErrorAction SilentlyContinue |
        Select-Object -First 1 -ExpandProperty Source
}
if (-not $TinyCc) {
    $TinyCc = Join-Path (Split-Path -Parent $repoRoot) `
        'tcc_package\out\build\x64-debug\package\tcc.exe'
}
if (-not (Test-Path -LiteralPath $TinyCc)) {
    throw 'TinyCC was not found.'
}
if (-not (Test-Path -LiteralPath $header)) {
    throw 'TC-0043 failed: include\io.h is missing.'
}
New-Item -ItemType Directory -Force -Path $buildDirectory | Out-Null
$common = @('-std=c89', '-Wall', '-Werror', '-I',
    (Join-Path $repoRoot 'include'))
$presence = & $TinyCc @common -c `
    (Join-Path $repoRoot 'tests\mscompat\presence\io.c') `
    -o $presenceObject 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "TC-0043 presence build failed:`n$($presence | Out-String)"
}
$unimplemented = @('_open', '_close', '_read', '_write', '_lseek',
    '_tell', '_commit', '_dup', '_dup2', '_pipe', '_isatty', '_setmode')
$headerText = Get-Content -LiteralPath $header -Raw
foreach ($name in $unimplemented) {
    if ($headerText -match "(?<![A-Za-z0-9_])$name(?![A-Za-z0-9_])") {
        throw "TC-0043 exposed unimplemented interface $name."
    }
}
[PSCustomObject]@{
    TestCase = 'TC-0043'
    Requirement = 'REQ-0043'
    HeaderIsolation = 'Pass'
    ControlledSurface = 'Pass'
    ExitCode = 0
}

