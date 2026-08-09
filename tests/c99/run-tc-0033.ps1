[CmdletBinding()]
param([string]$TinyCc, [switch]$CompileOnly)

& (Join-Path $PSScriptRoot 'Invoke-C99WideTest.ps1') `
    -TestCase 'TC-0033' -Requirement 'REQ-0033' `
    -PublicHeader 'include/wchar.h' `
    -PresenceSource 'tests/c99/presence/wchar.c' `
    -C89Source 'tests/c99/presence/wchar-c89.c' `
    -BehaviorSource 'tests/c99/wide-abi.c' -TinyCc $TinyCc `
    -CompileOnly:$CompileOnly
