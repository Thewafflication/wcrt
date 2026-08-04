[CmdletBinding()]
param([string]$TinyCc)

& (Join-Path $PSScriptRoot 'Invoke-C99UtilityTest.ps1') `
    -TestCase TC-0028 -Requirement REQ-0028 -Name stdlib-c99 `
    -PublicHeader 'include/stdlib.h' `
    -PresenceSource 'tests/c99/presence/stdlib.c' `
    -BehaviorSource 'tests/c99/stdlib.c' `
    -ExitSource 'tests/c99/stdlib-exit.c' `
    -C89Source 'tests/c99/presence/stdlib-c89.c' `
    -C89Regression 'tests/c89/run-tc-0013.ps1' -TinyCc $TinyCc
