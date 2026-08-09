[CmdletBinding()]
param([string]$TinyCc, [switch]$CompileOnly)

& (Join-Path $PSScriptRoot 'Invoke-C99StdioTest.ps1') `
    -TestCase 'TC-0039' -Requirement 'REQ-0039' `
    -PresenceSource 'tests/c99/presence/stdlib.c' `
    -C89Source 'tests/c99/presence/stdlib-c89.c' `
    -BehaviorSource 'tests/c99/numeric-text.c' -TinyCc $TinyCc `
    -CompileOnly:$CompileOnly
