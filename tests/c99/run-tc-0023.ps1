[CmdletBinding()]
param([string]$TinyCc)

& (Join-Path $PSScriptRoot 'Invoke-C99HeaderTest.ps1') `
    -TestCase TC-0023 -Requirement REQ-0023 -Name limits `
    -PublicHeader 'include/limits.h' `
    -PresenceSource 'tests/c99/presence/limits.c' `
    -BehaviorSource 'tests/c99/limits.c' -TinyCc $TinyCc `
    -C89Source 'tests/c99/presence/limits-c89.c' `
    -C89Regression 'tests/c89/run-tc-0005.ps1'
