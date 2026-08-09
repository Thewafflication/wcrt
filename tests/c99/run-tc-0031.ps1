[CmdletBinding()]
param([string]$TinyCc, [switch]$CompileOnly)

& (Join-Path $PSScriptRoot 'Invoke-C99WideTest.ps1') `
    -TestCase 'TC-0031' -Requirement 'REQ-0031' `
    -PublicHeader 'include/wchar.h' `
    -PresenceSource 'tests/c99/presence/wchar.c' `
    -C89Source 'tests/c99/presence/wchar-c89.c' `
    -BehaviorSource 'tests/c99/wchar.c' -TinyCc $TinyCc `
    -CompileOnly:$CompileOnly
