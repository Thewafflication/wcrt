[CmdletBinding()]
param([string]$TinyCc, [switch]$CompileOnly)

$result = & (Join-Path $PSScriptRoot 'Invoke-C99HeaderTest.ps1') `
    -TestCase 'TC-0035' -Requirement 'REQ-0035' `
    -Name 'math' `
    -PublicHeader 'include/math.h' `
    -PresenceSource 'tests/c99/presence/math.c' `
    -BehaviorSource 'tests/c99/math.c' -TinyCc $TinyCc `
    -RuntimeSource @('src/errno.c','src/fenv.c','src/math.c',
        'tests/c99/presence/math.c') `
    -C89Source 'tests/c99/presence/math-c89.c' `
    -C89Regression 'tests/c89/run-tc-0007.ps1' `
    -CompileOnly:$CompileOnly
& (Join-Path $PSScriptRoot 'Verify-FmaVectors.ps1') -TinyCc $TinyCc `
    -CompileOnly:$CompileOnly | Out-Null
$result
