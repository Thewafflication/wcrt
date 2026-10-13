[CmdletBinding()]
param([string]$TinyCc, [string]$RuntimeDirectory, [switch]$CompileOnly)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$fixture = Join-Path $root ('build/tests/mscompat/tc-0102/owned-' +
    [guid]::NewGuid().ToString('N') + '.bin')
try {
    Invoke-WcrtCompatibilityTest -TestCase TC-0102 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
        'presence/default-translation.c') -BehaviorSource (Join-Path `
        $PSScriptRoot 'default-translation.c') -Arguments @($fixture) `
        -CompileOnly:$CompileOnly
    Invoke-WcrtCompatibilityTest -TestCase TC-0102 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
        'presence/default-translation.c') -BehaviorSource (Join-Path `
        $PSScriptRoot 'default-translation-errors.c') `
        -ExcludedSources @('file.c') -CompileOnly:$CompileOnly
    if ($RuntimeDirectory) {
        Invoke-WcrtPackagedCompatibilityTest -TestCase TC-0102 `
            -RepositoryRoot $root -TinyCc $TinyCc `
            -RuntimeDirectory $RuntimeDirectory -BehaviorSource (Join-Path `
            $PSScriptRoot 'default-translation.c') -Arguments @($fixture) `
            -CompileOnly:$CompileOnly
    }
} finally {
    if (Test-Path -LiteralPath $fixture) {
        Remove-Item -LiteralPath $fixture -Force
    }
}
