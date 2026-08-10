[CmdletBinding()]
param([string]$TinyCc)

& (Join-Path $PSScriptRoot 'Invoke-C99HeaderTest.ps1') `
    -TestCase 'TC-0035' -Requirement 'REQ-0035' `
    -Name 'math' `
    -PublicHeader 'include/math.h' `
    -PresenceSource 'tests/c99/presence/math.c' `
    -BehaviorSource 'tests/c99/math.c' -TinyCc $TinyCc `
    -C89Source 'tests/c99/presence/math-c89.c' `
    -C89Regression 'tests/c89/run-tc-0007.ps1'
