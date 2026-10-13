[CmdletBinding()]
param([string]$TinyCc, [string]$RuntimeDirectory, [switch]$CompileOnly)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$build = Join-Path $root 'build/tests/mscompat/tc-0090'
New-Item -ItemType Directory -Force -Path $build | Out-Null
$fixture = Join-Path $build ("query-$([guid]::NewGuid().ToString('N')).bin")
try {
    Invoke-WcrtCompatibilityTest -TestCase TC-0090 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
        'presence/native-handle.c') -BehaviorSource (Join-Path `
        $PSScriptRoot 'native-handle.c') -Arguments @($fixture) `
        -CompileOnly:$CompileOnly
    Invoke-WcrtCompatibilityTest -TestCase TC-0090 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
        'presence/native-handle.c') -BehaviorSource (Join-Path `
        $PSScriptRoot 'native-handle-errors.c') `
        -ExcludedSources @('ms_handle.c') -CompileOnly:$CompileOnly
    if ($RuntimeDirectory) {
        # Package behavior retains native ownership assertions.
        Invoke-WcrtPackagedCompatibilityTest -TestCase TC-0090 `
            -RepositoryRoot $root -TinyCc $TinyCc `
            -RuntimeDirectory $RuntimeDirectory `
            -BehaviorSource (Join-Path $PSScriptRoot `
            'native-handle.c') -Arguments @($fixture) `
            -CompileOnly:$CompileOnly
    }
} finally {
    if (Test-Path -LiteralPath $fixture) {
        Remove-Item -LiteralPath $fixture -Force
    }
}
