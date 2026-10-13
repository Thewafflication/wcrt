[CmdletBinding()]
param([string]$TinyCc, [string]$RuntimeDirectory, [switch]$CompileOnly)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$file = Join-Path $root ('build/tests/mscompat/tc-0048/utime-' +
    [guid]::NewGuid().ToString('N') + '.tmp')
try {
    Invoke-WcrtCompatibilityTest -TestCase TC-0048 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
        'presence/utime.c') -BehaviorSource (Join-Path $PSScriptRoot `
        'utime.c') -Arguments @($file) -CompileOnly:$CompileOnly
    Invoke-WcrtCompatibilityTest -TestCase TC-0048 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
        'presence/utime.c') -BehaviorSource (Join-Path $PSScriptRoot `
        'utime-errors.c') -ExcludedSources @('utime.c') `
        -CompileOnly:$CompileOnly
    if ($RuntimeDirectory) {
        Invoke-WcrtPackagedCompatibilityTest -TestCase TC-0048 `
            -RepositoryRoot $root -TinyCc $TinyCc `
            -RuntimeDirectory $RuntimeDirectory `
            -BehaviorSource (Join-Path $PSScriptRoot 'utime.c') `
            -Arguments @($file) -CompileOnly:$CompileOnly
    }
} finally {
    if (Test-Path -LiteralPath $file) {
        Remove-Item -LiteralPath $file -Force
    }
}
