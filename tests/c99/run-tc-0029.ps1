[CmdletBinding()]
param([string]$TinyCc, [switch]$CompileOnly)

& (Join-Path $PSScriptRoot 'Invoke-C99StdioTest.ps1') `
    -TestCase 'TC-0029' -Requirement 'REQ-0029' `
    -PresenceSource 'tests/c99/presence/stdio-t2.c' `
    -C89Source 'tests/c99/presence/stdio-t2-c89.c' `
    -BehaviorSource 'tests/c99/stdio-format.c' -TinyCc $TinyCc `
    -CompileOnly:$CompileOnly
