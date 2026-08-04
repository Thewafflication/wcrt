[CmdletBinding()]
param([string]$TinyCc)

& (Join-Path $PSScriptRoot 'Invoke-C99UtilityTest.ps1') `
    -TestCase TC-0027 -Requirement REQ-0027 -Name inttypes `
    -PublicHeader 'include/inttypes.h' `
    -PresenceSource 'tests/c99/presence/inttypes.c' `
    -BehaviorSource 'tests/c99/inttypes.c' `
    -C89Source 'tests/c99/presence/inttypes-c89.c' -TinyCc $TinyCc
