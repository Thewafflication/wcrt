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

$c89Object = Join-Path $repoRoot 'build\tests\c99\tc-0038\tgmath-c89.o'
New-Item -ItemType Directory -Force (Split-Path -Parent $c89Object) | Out-Null
$c89Output = & $TinyCc -std=c89 -DWCRT_C89=1 -Wall -Werror `
    -I (Join-Path $repoRoot 'include') -c `
    (Join-Path $repoRoot 'tests\c99\presence\tgmath-c89.c') `
    -o $c89Object 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "TC-0038 C89 isolation build failed:`n$($c89Output | Out-String)"
}

& (Join-Path $PSScriptRoot 'Verify-T5Inventory.ps1') | Out-Null

$capability = & (Join-Path $PSScriptRoot `
    'Test-TinyCcComplexCapability.ps1') -TinyCc $TinyCc
if (-not $capability.Supported) {
    if (-not $capability.ExpectedFailure) {
        throw "TC-0038 unexpected compiler failure:`n$($capability.Diagnostic)"
    }
    [PSCustomObject]@{
        TestCase = 'TC-0038'
        Requirement = 'REQ-0038'
        Status = 'ExpectedFail'
        Output = $capability.Diagnostic
        Reason = 'TinyCC compiler does not yet support _Complex.'
    }
    return
}

& (Join-Path $PSScriptRoot 'Invoke-C99HeaderTest.ps1') `
    -TestCase 'TC-0038' -Requirement 'REQ-0038' `
    -Name 'tgmath' -PublicHeader 'include/tgmath.h' `
    -PresenceSource 'tests/c99/presence/tgmath.c' `
    -BehaviorSource 'tests/c99/tgmath.c' -TinyCc $TinyCc `
    -RuntimeSource @('src/errno.c', 'src/fenv.c', 'src/math.c',
        'src/complex.c', 'tests/c99/presence/tgmath.c') `
    -CompileOnly:$CompileOnly
