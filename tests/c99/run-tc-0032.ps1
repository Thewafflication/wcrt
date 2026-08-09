[CmdletBinding()]
param([string]$TinyCc, [switch]$CompileOnly)

& (Join-Path $PSScriptRoot 'Invoke-C99WideTest.ps1') `
    -TestCase 'TC-0032' -Requirement 'REQ-0032' `
    -PublicHeader 'include/wctype.h' `
    -PresenceSource 'tests/c99/presence/wctype.c' `
    -C89Source 'tests/c99/presence/wctype-c89.c' `
    -BehaviorSource 'tests/c99/wctype.c' -TinyCc $TinyCc `
    -CompileOnly:$CompileOnly
