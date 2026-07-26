[CmdletBinding()]
param([string]$TinyCc)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$buildDirectory = Join-Path $repoRoot 'build\tests\c99\tc-0019'
$presenceObject = Join-Path $buildDirectory 'stdio-presence.o'
$executable = Join-Path $buildDirectory 'stdio-test.exe'
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
New-Item -ItemType Directory -Force -Path $buildDirectory | Out-Null
$common = @('-std=c99', '-Wall', '-Werror', '-I',
    (Join-Path $repoRoot 'include'))
$presence = & $TinyCc @common -c `
    (Join-Path $repoRoot 'tests\c99\presence\stdio.c') `
    -o $presenceObject 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "TC-0019 presence build failed:`n$($presence | Out-String)"
}
$sources = @(
    'src\ctype.c', 'src\errno.c', 'src\string.c', 'src\stdlib.c',
    'src\stdio.c', 'src\stdio_format.c', 'src\stdio_scan.c',
    'src\platform\windows\heap.c', 'src\platform\windows\process.c',
    'src\platform\windows\file.c', 'tests\c99\stdio.c'
) | ForEach-Object { Join-Path $repoRoot $_ }
$output = & $TinyCc @common @sources -o $executable 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "TC-0019 build failed:`n$($output | Out-String)"
}
& $executable
if ($LASTEXITCODE -ne 0) {
    throw "TC-0019 behavior failed with code $LASTEXITCODE."
}
[PSCustomObject]@{
    TestCase = 'TC-0019'
    Requirement = 'REQ-0019'
    FunctionsCovered = 2
    Boundaries = 'Pass'
    ExitCode = 0
}
