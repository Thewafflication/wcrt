[CmdletBinding()]
param(
    [string]$TinyCc
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$buildDirectory = Join-Path $repoRoot 'build\tests\c89\tc-0010'
$executable = Join-Path $buildDirectory 'stdarg-test.exe'

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
    (Join-Path $repoRoot 'tests\c89\stdarg.c'),
    '-o', $executable
)
$buildOutput = & $TinyCc @arguments 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "TC-0010 build failed:`n$($buildOutput | Out-String)"
}

& $executable
if ($LASTEXITCODE -ne 0) {
    throw "TC-0010 behavior failed with code $LASTEXITCODE."
}

[PSCustomObject]@{
    TestCase = 'TC-0010'
    Requirement = 'REQ-0010'
    Compiler = (& $TinyCc -v 2>&1 | Select-Object -First 1).ToString()
    IntegerPromotions = 'Pass'
    FloatingPromotion = 'Pass'
    PointerTraversal = 'Pass'
    AggregateTraversal = 'Pass'
    RepeatedCalls = 'Pass'
    EmptyTail = 'Pass'
    AbiProvider = 'TinyCC builtins'
    ExitCode = 0
}
