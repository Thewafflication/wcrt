[CmdletBinding()]
param([string]$TinyCc)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$file = Join-Path $root 'build\tests\mscompat\tc-0049\stat.tmp'
Invoke-WcrtCompatibilityTest -TestCase TC-0049 -RepositoryRoot $root `
    -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
    'presence\stat.c') -BehaviorSource (Join-Path $PSScriptRoot 'stat.c') `
    -Arguments $file
