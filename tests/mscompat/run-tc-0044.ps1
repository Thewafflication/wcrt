[CmdletBinding()]
param([string]$TinyCc)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$buildDirectory = Join-Path $repoRoot 'build\tests\mscompat\tc-0044'
$presenceObject = Join-Path $buildDirectory 'ms-fileno-presence.o'
$loggerObject = Join-Path $buildDirectory 'wsp-log-no-tty.o'
$executable = Join-Path $buildDirectory 'ms-fileno-test.exe'
$testFile = Join-Path $buildDirectory 'fileno-test.tmp'
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
    (Join-Path $repoRoot 'tests\mscompat\presence\fileno.c') `
    -o $presenceObject 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "TC-0044 presence build failed:`n$($presence | Out-String)"
}
$sources = @(
    'src\ctype.c', 'src\errno.c', 'src\string.c', 'src\stdlib.c',
    'src\stdio.c', 'src\stdio_format.c', 'src\stdio_scan.c',
    'src\platform\windows\heap.c', 'src\platform\windows\process.c',
    'src\platform\windows\file.c', 'tests\mscompat\fileno.c'
) | ForEach-Object { Join-Path $repoRoot $_ }
$output = & $TinyCc @common @sources -o $executable 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "TC-0044 build failed:`n$($output | Out-String)"
}
& $executable $testFile
if ($LASTEXITCODE -ne 0) {
    throw "TC-0044 behavior failed with code $LASTEXITCODE."
}
$loggerFlags = @('-std=c99', '-Wall', '-Werror', '-DWSP_LOG_NO_TTY',
    '-I', (Join-Path $repoRoot 'include'), '-I',
    (Join-Path $repoRoot 'tools\wsp\tools\logging'))
$logger = & $TinyCc @loggerFlags -c `
    (Join-Path $repoRoot 'tools\wsp\tools\logging\wsp_log.c') `
    -o $loggerObject 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "TC-0044 WSP logger build failed:`n$($logger | Out-String)"
}
[PSCustomObject]@{
    TestCase = 'TC-0044'
    Requirement = 'REQ-0044'
    FunctionsCovered = 1
    LoggerCompatibility = 'Pass'
    ExitCode = 0
}

