[CmdletBinding()]
param(
    [string]$TinyCc
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$buildDirectory = Join-Path $repoRoot 'build\tests\c89\tc-0015'
$executable = Join-Path $buildDirectory 'time-test.exe'

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
$arguments = @(
    '-std=c89', '-Wall', '-Werror',
    '-I', (Join-Path $repoRoot 'include'),
    (Join-Path $repoRoot 'src\string.c'),
    (Join-Path $repoRoot 'src\time.c'),
    (Join-Path $repoRoot 'src\platform\windows\clock.c'),
    (Join-Path $repoRoot 'tests\c89\time.c'),
    '-o', $executable
)
$buildOutput = & $TinyCc @arguments 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "TC-0015 build failed:`n$($buildOutput | Out-String)"
}

& $executable
if ($LASTEXITCODE -ne 0) {
    throw "TC-0015 behavior failed with code $LASTEXITCODE."
}

[PSCustomObject]@{
    TestCase = 'TC-0015'
    Requirement = 'REQ-0015'
    Compiler = (& $TinyCc -v 2>&1 | Select-Object -First 1).ToString()
    FunctionsCovered = 9
    EpochAndCalendar = 'Pass'
    LocalRoundTrip = 'Pass'
    Normalization = 'Pass'
    CLocaleFormatting = 'Pass'
    BufferLimit = 'Pass'
    ProcessorClock = 'Pass'
    ExitCode = 0
}
