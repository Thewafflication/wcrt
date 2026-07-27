[CmdletBinding()]
param([string]$TinyCc)

& (Join-Path $PSScriptRoot 'Invoke-C99HeaderTest.ps1') `
    -TestCase TC-0026 -Requirement REQ-0026 -Name iso646 `
    -PublicHeader 'include/iso646.h' `
    -PresenceSource 'tests/c99/presence/iso646.c' `
    -BehaviorSource 'tests/c99/iso646.c' -TinyCc $TinyCc
