[CmdletBinding()]
param([string]$TinyCc)

& (Join-Path $PSScriptRoot 'Invoke-C99HeaderTest.ps1') `
    -TestCase TC-0022 -Requirement REQ-0022 -Name stdint `
    -PublicHeader 'include/stdint.h' `
    -PresenceSource 'tests/c99/presence/stdint.c' `
    -BehaviorSource 'tests/c99/stdint.c' -TinyCc $TinyCc `
    -C89Source 'tests/c99/presence/stdint-c89.c'
