[CmdletBinding()]
param([string]$TinyCc, [string]$RuntimeDirectory, [switch]$CompileOnly)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$fixture = Join-Path $root ('build/tests/mscompat/tc-0094/resize-' +
    [guid]::NewGuid().ToString('N') + '.bin')
try {
    Invoke-WcrtCompatibilityTest -TestCase TC-0094 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
        'presence/descriptor-resize.c') -BehaviorSource (Join-Path `
        $PSScriptRoot 'descriptor-resize.c') -Arguments @($fixture) `
        -CompileOnly:$CompileOnly
    Invoke-WcrtCompatibilityTest -TestCase TC-0094 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
        'presence/descriptor-resize.c') -BehaviorSource (Join-Path `
        $PSScriptRoot 'descriptor-resize-errors.c') `
        -ExcludedSources @('file.c') -CompileOnly:$CompileOnly
    if ($RuntimeDirectory) {
        Invoke-WcrtPackagedCompatibilityTest -TestCase TC-0094 `
            -RepositoryRoot $root -TinyCc $TinyCc `
            -RuntimeDirectory $RuntimeDirectory -BehaviorSource (Join-Path `
            $PSScriptRoot 'descriptor-resize.c') -Arguments @($fixture) `
            -CompileOnly:$CompileOnly
    }
} finally {
    if (Test-Path -LiteralPath $fixture) {
        Remove-Item -LiteralPath $fixture -Force
    }
}
