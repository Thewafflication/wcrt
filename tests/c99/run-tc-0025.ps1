[CmdletBinding()]
param([string]$TinyCc)

$result = & (Join-Path $PSScriptRoot 'Invoke-C99HeaderTest.ps1') `
    -TestCase TC-0025 -Requirement REQ-0025 -Name stdarg `
    -PublicHeader 'include/stdarg.h' `
    -PresenceSource 'tests/c99/presence/stdarg.c' `
    -BehaviorSource 'tests/c99/va-copy.c' -TinyCc $TinyCc `
    -C89Source 'tests/c99/presence/restrict-c89.c' `
    -C89Regression 'tests/c89/run-tc-0010.ps1'
$audit = & (Join-Path $PSScriptRoot 'Verify-RestrictDeclarations.ps1') `
    -RepositoryRoot (Split-Path -Parent (Split-Path -Parent $PSScriptRoot))
$result
$audit
