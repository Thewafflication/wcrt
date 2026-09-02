[CmdletBinding()]
param([string]$TinyCc)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$path = Join-Path $root 'build\tests\mscompat\tc-0063\access-target.txt'
Invoke-WcrtCompatibilityTest -TestCase TC-0063 -RepositoryRoot $root `
    -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
    'presence\access.c') -BehaviorSource (Join-Path $PSScriptRoot `
    'access.c') -Arguments @($path)
