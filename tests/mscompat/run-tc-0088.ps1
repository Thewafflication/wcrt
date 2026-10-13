[CmdletBinding()]
param([string]$TinyCc, [string]$RuntimeDirectory, [switch]$CompileOnly)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$build = Join-Path $root 'build/tests/mscompat/tc-0088'
New-Item -ItemType Directory -Force -Path $build | Out-Null
$fixture = Join-Path $build ("query-$([guid]::NewGuid().ToString('N')).bin")
try {
    Invoke-WcrtCompatibilityTest -TestCase TC-0088 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
        'presence/descriptor-size-position.c') -BehaviorSource (Join-Path `
        $PSScriptRoot 'descriptor-size-position.c') -Arguments @($fixture) `
        -CompileOnly:$CompileOnly
    Invoke-WcrtCompatibilityTest -TestCase TC-0088 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
        'presence/descriptor-size-position.c') -BehaviorSource (Join-Path `
        $PSScriptRoot 'descriptor-size-errors.c') `
        -ExcludedSources @('stdio.c') -CompileOnly:$CompileOnly
    if ($RuntimeDirectory) {
        Invoke-WcrtPackagedCompatibilityTest -TestCase TC-0088 `
            -RepositoryRoot $root -TinyCc $TinyCc `
            -RuntimeDirectory $RuntimeDirectory `
            -BehaviorSource (Join-Path $PSScriptRoot `
            'descriptor-size-position.c') -Arguments @($fixture) `
            -CompileOnly:$CompileOnly
    }
} finally {
    if (Test-Path -LiteralPath $fixture) {
        Remove-Item -LiteralPath $fixture -Force
    }
}
