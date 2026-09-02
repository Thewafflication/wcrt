[CmdletBinding()]
param([string]$TinyCc)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$path = Join-Path $root 'build\tests\mscompat\tc-0062\unlink-target.txt'
Invoke-WcrtCompatibilityTest -TestCase TC-0062 -RepositoryRoot $root `
    -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
    'presence\unlink.c') -BehaviorSource (Join-Path $PSScriptRoot `
    'unlink.c') -Arguments @($path)
