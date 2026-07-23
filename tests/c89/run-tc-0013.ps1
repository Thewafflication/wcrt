[CmdletBinding()]
param(
    [string]$TinyCc
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$buildDirectory = Join-Path $repoRoot 'build\tests\c89\tc-0013'
$executable = Join-Path $buildDirectory 'stdlib-test.exe'
$exitExecutable = Join-Path $buildDirectory 'stdlib-exit-test.exe'

if ([string]::IsNullOrWhiteSpace($TinyCc)) {
    $TinyCc = Get-Command tcc.exe -ErrorAction SilentlyContinue |
        Select-Object -First 1 -ExpandProperty Source
    if (-not $TinyCc) {
        $TinyCc = Join-Path (Split-Path -Parent $repoRoot) `
            'tcc_package\out\build\x64-debug\package\tcc.exe'
    }
}
if (-not (Test-Path -LiteralPath $TinyCc)) {
    throw 'TinyCC was not found.'
}

New-Item -ItemType Directory -Force -Path $buildDirectory | Out-Null
$common = @(
    '-std=c89', '-Wall', '-Werror',
    '-I', (Join-Path $repoRoot 'include'),
    (Join-Path $repoRoot 'src\ctype.c'),
    (Join-Path $repoRoot 'src\errno.c'),
    (Join-Path $repoRoot 'src\string.c'),
    (Join-Path $repoRoot 'src\stdlib.c'),
    (Join-Path $repoRoot 'src\platform\windows\heap.c'),
    (Join-Path $repoRoot 'src\platform\windows\process.c')
)
$output = & $TinyCc @common (Join-Path $repoRoot 'tests\c89\stdlib.c') `
    '-o' $executable 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "TC-0013 build failed:`n$($output | Out-String)"
}
$output = & $TinyCc @common `
    (Join-Path $repoRoot 'tests\c89\stdlib_exit.c') `
    '-o' $exitExecutable 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "TC-0013 exit build failed:`n$($output | Out-String)"
}

& $executable
if ($LASTEXITCODE -ne 0) {
    throw "TC-0013 behavior failed with code $LASTEXITCODE."
}
& $exitExecutable
$exitStatus = $LASTEXITCODE
if ($exitStatus -ne 23) {
    throw "TC-0013 exit child returned code $exitStatus."
}

[PSCustomObject]@{
    TestCase = 'TC-0013'
    Requirement = 'REQ-0013'
    Compiler = (& $TinyCc -v 2>&1 | Select-Object -First 1).ToString()
    FunctionsCovered = 29
    NumericConversions = 'Pass'
    RandomSequence = 'Pass'
    Allocation = 'Pass'
    EnvironmentAndSystem = 'Pass'
    SearchAndSort = 'Pass'
    IntegerArithmetic = 'Pass'
    CLocaleMultibyte = 'Pass'
    AtexitAndExit = 'Pass'
    ExitChildCode = $exitStatus
    ExitCode = 0
}
