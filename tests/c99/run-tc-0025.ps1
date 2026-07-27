[CmdletBinding()]
param([string]$TinyCc)

& (Join-Path $PSScriptRoot 'Invoke-C99HeaderTest.ps1') `
    -TestCase TC-0025 -Requirement REQ-0025 -Name stdarg `
    -PublicHeader 'include/stdarg.h' `
    -PresenceSource 'tests/c99/presence/stdarg.c' `
    -BehaviorSource 'tests/c99/va-copy.c' -TinyCc $TinyCc `
    -C89Regression 'tests/c89/run-tc-0010.ps1'
