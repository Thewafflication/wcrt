[CmdletBinding()]
param([string]$TinyCc)

& (Join-Path $PSScriptRoot 'Invoke-C99HeaderTest.ps1') `
    -TestCase TC-0021 -Requirement REQ-0021 -Name stdbool `
    -PublicHeader 'include/stdbool.h' `
    -PresenceSource 'tests/c99/presence/stdbool.c' `
    -BehaviorSource 'tests/c99/stdbool.c' -TinyCc $TinyCc `
    -C89Source 'tests/c99/presence/stdbool-c89.c'
