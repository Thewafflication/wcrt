[CmdletBinding()]
param([string]$TinyCc)

& (Join-Path $PSScriptRoot 'Invoke-C99HeaderTest.ps1') `
    -TestCase TC-0024 -Requirement REQ-0024 -Name float `
    -PublicHeader 'include/float.h' `
    -PresenceSource 'tests/c99/presence/float.c' `
    -BehaviorSource 'tests/c99/float.c' -TinyCc $TinyCc `
    -C89Regression 'tests/c89/run-tc-0004.ps1'
