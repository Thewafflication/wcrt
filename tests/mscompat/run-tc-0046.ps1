[CmdletBinding()]
param([string]$TinyCc)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$file = Join-Path $root 'build\tests\mscompat\tc-0046\stdio.tmp'
Invoke-WcrtCompatibilityTest -TestCase TC-0046 -RepositoryRoot $root `
    -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
    'presence\secure-stdio.c') -BehaviorSource (Join-Path $PSScriptRoot `
    'secure-stdio.c') -Arguments $file
