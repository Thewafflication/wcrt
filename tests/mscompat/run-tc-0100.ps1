[CmdletBinding()]
param([string]$TinyCc, [string]$RuntimeDirectory, [switch]$CompileOnly)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$prefix = Join-Path $root ('build/tests/mscompat/tc-0100/owned-' +
    [guid]::NewGuid().ToString('N'))
$first = "$prefix-a.bin"
$second = "$prefix-b.bin"
$pattern = "$prefix-*.bin"
try {
    Invoke-WcrtCompatibilityTest -TestCase TC-0100 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
        'presence/find32i64.c') -BehaviorSource (Join-Path `
        $PSScriptRoot 'find32i64.c') -Arguments @($first, $second, $pattern) `
        -CompileOnly:$CompileOnly
    Invoke-WcrtCompatibilityTest -TestCase TC-0100 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
        'presence/find32i64.c') -BehaviorSource (Join-Path `
        $PSScriptRoot 'find32i64-errors.c') `
        -ExcludedSources @('ms_find32.c') -CompileOnly:$CompileOnly
    if ($RuntimeDirectory) {
        Invoke-WcrtPackagedCompatibilityTest -TestCase TC-0100 `
            -RepositoryRoot $root -TinyCc $TinyCc `
            -RuntimeDirectory $RuntimeDirectory -BehaviorSource (Join-Path `
            $PSScriptRoot 'find32i64.c') -Arguments @($first, $second, $pattern) `
            -CompileOnly:$CompileOnly
    }
} finally {
    foreach ($fixture in @($first, $second)) {
        if (Test-Path -LiteralPath $fixture) {
            Remove-Item -LiteralPath $fixture -Force
        }
    }
}
