[CmdletBinding()]
param([string]$TinyCc)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$file = Join-Path $root 'build\tests\mscompat\tc-0048\utime.tmp'
Invoke-WcrtCompatibilityTest -TestCase TC-0048 -RepositoryRoot $root `
    -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
    'presence\utime.c') -BehaviorSource (Join-Path $PSScriptRoot 'utime.c') `
    -Arguments $file
