[CmdletBinding()]
param(
    [string]$TinyCc
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$buildDirectory = Join-Path $repoRoot 'build\tests\c89\tc-0009'
$executable = Join-Path $buildDirectory 'signal-test.exe'
$defaultExecutable = Join-Path $buildDirectory 'signal-default-test.exe'

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
    (Join-Path $repoRoot 'src\signal.c'),
    (Join-Path $repoRoot 'src\platform\windows\signal.c')
)
$buildOutput = & $TinyCc @common `
    (Join-Path $repoRoot 'tests\c89\signal.c') '-o' $executable 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "TC-0009 build failed:`n$($buildOutput | Out-String)"
}
$defaultBuildOutput = & $TinyCc @common `
    (Join-Path $repoRoot 'tests\c89\signal_default.c') `
    '-o' $defaultExecutable 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "TC-0009 child build failed:`n$($defaultBuildOutput | Out-String)"
}

& $executable require-reset
if ($LASTEXITCODE -ne 0) {
    throw "TC-0009 behavior failed with code $LASTEXITCODE."
}
& $defaultExecutable
$defaultExitCode = $LASTEXITCODE
if ($defaultExitCode -ne (128 + 15)) {
    throw "TC-0009 default action returned code $defaultExitCode."
}

[PSCustomObject]@{
    TestCase = 'TC-0009'
    Requirement = 'REQ-0009'
    Compiler = (& $TinyCc -v 2>&1 | Select-Object -First 1).ToString()
    SupportedSignals = 6
    Registration = 'Pass'
    SynchronousRaise = 'Pass'
    HandlerReset = 'Pass'
    IgnoreAction = 'Pass'
    InvalidRequests = 'Pass'
    DefaultTermination = 'Pass'
    DefaultExitCode = $defaultExitCode
    AsynchronousMapping = 'None'
    ExitCode = 0
}
