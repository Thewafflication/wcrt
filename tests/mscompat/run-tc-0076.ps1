[CmdletBinding()]
param([string]$TinyCc, [string]$RuntimeDirectory, [switch]$CompileOnly)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
Invoke-WcrtCompatibilityTest -TestCase TC-0076 -RepositoryRoot $root `
    -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
    'presence/path-components.c') -BehaviorSource (Join-Path $PSScriptRoot `
    'path-components.c') -CompileOnly:$CompileOnly
if ($RuntimeDirectory) {
    Invoke-WcrtPackagedCompatibilityTest -TestCase TC-0076 `
        -RepositoryRoot $root -TinyCc $TinyCc -RuntimeDirectory $RuntimeDirectory `
        -BehaviorSource (Join-Path $PSScriptRoot 'path-components.c') `
        -CompileOnly:$CompileOnly
}
