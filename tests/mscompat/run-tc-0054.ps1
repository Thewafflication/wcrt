[CmdletBinding()]
param([string]$TinyCc)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
Invoke-WcrtCompatibilityTest -TestCase TC-0054 -RepositoryRoot $root `
    -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
    'presence\strdup.c') -BehaviorSource (Join-Path $PSScriptRoot 'strdup.c')
