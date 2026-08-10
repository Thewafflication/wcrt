[CmdletBinding()]
param([string]$TinyCc)

& (Join-Path $PSScriptRoot 'Invoke-C99HeaderTest.ps1') `
    -TestCase 'TC-0036' -Requirement 'REQ-0036' `
    -Name 'fenv' `
    -PublicHeader 'include/fenv.h' `
    -PresenceSource 'tests/c99/presence/fenv.c' `
    -BehaviorSource 'tests/c99/fenv.c' -TinyCc $TinyCc `
    -C89Source 'tests/c99/presence/fenv-c89.c' `
    -C89Regression 'tests/c89/run-tc-0004.ps1'
