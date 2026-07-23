[CmdletBinding()]
param([string]$TinyCc)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$buildDirectory = Join-Path $repoRoot 'build\tests\c89\tc-0012'
$executable = Join-Path $buildDirectory 'stdio-test.exe'
if ([string]::IsNullOrWhiteSpace($TinyCc)) {
    $TinyCc = Join-Path (Split-Path -Parent $repoRoot) `
        'tcc_package\out\build\x64-debug\package\tcc.exe'
}
New-Item -ItemType Directory -Force -Path $buildDirectory | Out-Null
$sources = @(
    'src\ctype.c', 'src\errno.c', 'src\string.c', 'src\stdlib.c',
    'src\stdio.c', 'src\stdio_format.c', 'src\stdio_scan.c',
    'src\platform\windows\heap.c', 'src\platform\windows\process.c',
    'src\platform\windows\file.c', 'tests\c89\stdio.c'
) | ForEach-Object { Join-Path $repoRoot $_ }
$arguments = @('-std=c89', '-Wall', '-Werror', '-I',
    (Join-Path $repoRoot 'include')) + $sources + @('-o', $executable)
$output = & $TinyCc @arguments 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "TC-0012 build failed:`n$($output | Out-String)"
}
Push-Location $buildDirectory
try {
    & $executable
    $exitCode = $LASTEXITCODE
} finally {
    Pop-Location
}
if ($exitCode -ne 0) {
    throw "TC-0012 behavior failed with code $exitCode."
}
[PSCustomObject]@{
    TestCase = 'TC-0012'
    Requirement = 'REQ-0012'
    Compiler = (& $TinyCc -v 2>&1 | Select-Object -First 1).ToString()
    FunctionsCovered = 41
    FilesAndTemporaryFiles = 'Pass'
    CharacterAndDirectIo = 'Pass'
    PositionAndIndicators = 'Pass'
    FormattedOutput = 'Pass'
    FormattedInput = 'Pass'
    TextTranslation = 'Pass'
    ExitCode = 0
}
