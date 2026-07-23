[CmdletBinding()]
param(
    [string]$TinyCc
)

$ErrorActionPreference = 'Stop'

$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$buildDirectory = Join-Path $repoRoot 'build\tests\c89\tc-0001'
$executable = Join-Path $buildDirectory 'assert-test.exe'

if ([string]::IsNullOrWhiteSpace($TinyCc)) {
    $command = Get-Command tcc.exe -ErrorAction SilentlyContinue |
        Select-Object -First 1 -ExpandProperty Source
    if ($command) {
        $TinyCc = $command
    } else {
        $packagedTinyCc = Join-Path (Split-Path -Parent $repoRoot) `
            'tcc_package\out\build\x64-debug\package\tcc.exe'
        if (Test-Path -LiteralPath $packagedTinyCc) {
            $TinyCc = $packagedTinyCc
        }
    }
}

if (-not $TinyCc -or -not (Test-Path -LiteralPath $TinyCc)) {
    throw 'TinyCC was not found in PATH or the sibling tcc_package build.'
}

New-Item -ItemType Directory -Force -Path $buildDirectory | Out-Null

$tinyCcInclude = Join-Path (Split-Path -Parent $TinyCc) 'include'
$tinyCcDefinitions = Join-Path $tinyCcInclude 'tccdefs.h'
if (-not (Test-Path -LiteralPath $tinyCcDefinitions)) {
    throw "TinyCC intrinsic header not found: $tinyCcDefinitions"
}

$presenceObject = Join-Path $buildDirectory 'assert-presence.o'
$presenceArguments = @(
    '-std=c89'
    '-Werror'
    '-nostdinc'
    '-I', (Join-Path $repoRoot 'include')
    '-I', $tinyCcInclude
    '-c', (Join-Path $repoRoot 'tests\c89\presence\assert.c')
    '-o', $presenceObject
)

& $TinyCc @presenceArguments
if ($LASTEXITCODE -ne 0) {
    throw "The standalone assert.h presence test failed with exit code $LASTEXITCODE."
}

$compileArguments = @(
    '-std=c89'
    '-Wall'
    '-Werror'
    '-I', (Join-Path $repoRoot 'include')
    (Join-Path $repoRoot 'src\assert.c')
    (Join-Path $repoRoot 'tests\c89\assert.c')
    (Join-Path $repoRoot 'tests\c89\assert_disabled.c')
    '-o', $executable
)

& $TinyCc @compileArguments
if ($LASTEXITCODE -ne 0) {
    throw "TinyCC failed to build TC-0001 with exit code $LASTEXITCODE."
}

& $executable
if ($LASTEXITCODE -ne 0) {
    throw "Enabled/NDEBUG assertion checks failed with exit code $LASTEXITCODE."
}

$failureOutput = & $executable fail 2>&1
$failureExitCode = $LASTEXITCODE
$failureText = ($failureOutput | ForEach-Object { $_.ToString() }) -join "`n"

if ($failureExitCode -eq 0) {
    throw 'A failed assertion returned success instead of terminating abnormally.'
}
if ($failureText -notmatch 'Assertion failed: 0') {
    throw "The diagnostic did not contain the expression text. Output: $failureText"
}
if ($failureText -notmatch 'assert\.c') {
    throw "The diagnostic did not contain the source filename. Output: $failureText"
}
if ($failureText -notmatch 'line [0-9]+') {
    throw "The diagnostic did not contain the source line. Output: $failureText"
}

[PSCustomObject]@{
    TestCase = 'TC-0001'
    Requirement = 'REQ-0001'
    Compiler = (& $TinyCc -v 2>&1 | Select-Object -First 1).ToString()
    StandaloneHeader = 'Pass'
    EnabledAndNDebug = 'Pass'
    FailureTermination = 'Pass'
    FailureExitCode = $failureExitCode
    Diagnostic = $failureText
}
