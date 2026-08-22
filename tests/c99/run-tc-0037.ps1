[CmdletBinding()]
param([string]$TinyCc, [switch]$CompileOnly)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
if ([string]::IsNullOrWhiteSpace($TinyCc)) {
    $TinyCc = Get-Command tcc.exe -ErrorAction SilentlyContinue |
        Select-Object -First 1 -ExpandProperty Source
}
if (-not $TinyCc) {
    $TinyCc = Join-Path (Split-Path -Parent $repoRoot) `
        'tcc_package\out\build\x64-debug\package\tcc.exe'
}
if (-not (Test-Path -LiteralPath $TinyCc -PathType Leaf)) {
    throw 'TinyCC was not found.'
}

$c89Object = Join-Path $repoRoot 'build\tests\c99\tc-0037\complex-c89.o'
New-Item -ItemType Directory -Force (Split-Path -Parent $c89Object) | Out-Null
$c89Output = & $TinyCc -std=c89 -DWCRT_C89=1 -Wall -Werror `
    -I (Join-Path $repoRoot 'include') -c `
    (Join-Path $repoRoot 'tests\c99\presence\complex-c89.c') `
    -o $c89Object 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "TC-0037 C89 isolation build failed:`n$($c89Output | Out-String)"
}

& (Join-Path $PSScriptRoot 'Verify-ComplexVectors.ps1') | Out-Null
& (Join-Path $PSScriptRoot 'Verify-T5Inventory.ps1') | Out-Null

$capability = & (Join-Path $PSScriptRoot `
    'Test-TinyCcComplexCapability.ps1') -TinyCc $TinyCc
if (-not $capability.Supported) {
    if (-not $capability.ExpectedFailure) {
        throw "TC-0037 unexpected compiler failure:`n$($capability.Diagnostic)"
    }
    [PSCustomObject]@{
        TestCase = 'TC-0037'
        Requirement = 'REQ-0037'
        Status = 'ExpectedFail'
        Output = $capability.Diagnostic
        Reason = 'TinyCC compiler does not yet support _Complex.'
    }
    return
}

& (Join-Path $PSScriptRoot 'Invoke-C99HeaderTest.ps1') `
    -TestCase 'TC-0037' -Requirement 'REQ-0037' `
    -Name 'complex' -PublicHeader 'include/complex.h' `
    -PresenceSource 'tests/c99/presence/complex.c' `
    -BehaviorSource 'tests/c99/complex.c' -TinyCc $TinyCc `
    -RuntimeSource @('src/errno.c', 'src/fenv.c', 'src/math.c',
        'src/complex.c', 'tests/c99/presence/complex.c') `
    -C89Regression @('tests/c89/run-tc-0007.ps1',
        'tests/c99/run-tc-0035.ps1') -CompileOnly:$CompileOnly
