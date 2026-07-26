[CmdletBinding()]
param([string]$TinyCc)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$buildDirectory = Join-Path $repoRoot 'build\tests\mscompat\tc-0020'
$presenceObject = Join-Path $buildDirectory 'ms-stdio-presence.o'
$executable = Join-Path $buildDirectory 'ms-stdio-test.exe'
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
$common = @('-std=c89', '-Wall', '-Werror', '-I',
    (Join-Path $repoRoot 'include'))
$presence = & $TinyCc @common -c `
    (Join-Path $repoRoot 'tests\mscompat\presence\stdio.c') `
    -o $presenceObject 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "TC-0020 presence build failed:`n$($presence | Out-String)"
}
$sources = @(
    'src\ctype.c', 'src\errno.c', 'src\string.c', 'src\stdlib.c',
    'src\stdio.c', 'src\stdio_format.c', 'src\stdio_scan.c',
    'src\platform\windows\heap.c', 'src\platform\windows\process.c',
    'src\platform\windows\file.c', 'tests\mscompat\stdio.c'
) | ForEach-Object { Join-Path $repoRoot $_ }
$output = & $TinyCc @common @sources -o $executable 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "TC-0020 build failed:`n$($output | Out-String)"
}
& $executable
if ($LASTEXITCODE -ne 0) {
    throw "TC-0020 behavior failed with code $LASTEXITCODE."
}
[PSCustomObject]@{
    TestCase = 'TC-0020'
    Requirement = 'REQ-0020'
    FunctionsCovered = 2
    LegacyTruncation = 'Pass'
    ExitCode = 0
}
