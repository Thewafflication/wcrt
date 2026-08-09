[CmdletBinding()]
param([string]$TinyCc, [switch]$CompileOnly)

& (Join-Path $PSScriptRoot 'Invoke-C99StdioTest.ps1') `
    -TestCase 'TC-0030' -Requirement 'REQ-0030' `
    -PresenceSource 'tests/c99/presence/stdio-t2.c' `
    -C89Source 'tests/c99/presence/stdio-t2-c89.c' `
    -BehaviorSource 'tests/c99/stdio-scan.c' -TinyCc $TinyCc `
    -CompileOnly:$CompileOnly
