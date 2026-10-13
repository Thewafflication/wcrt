[CmdletBinding()]
param([string]$TinyCc, [string]$RuntimeDirectory, [switch]$CompileOnly)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$fixture = Join-Path $root ('build/tests/mscompat/tc-0080/owned-' +
    [guid]::NewGuid().ToString('N'))
try {
    Invoke-WcrtCompatibilityTest -TestCase TC-0080 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
        'presence/directory-change.c') -BehaviorSource (Join-Path $PSScriptRoot `
        'directory-change.c') -Arguments @($fixture) -CompileOnly:$CompileOnly
    Invoke-WcrtCompatibilityTest -TestCase TC-0080 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
        'presence/directory-change.c') -BehaviorSource (Join-Path $PSScriptRoot `
        'directory-errors.c') -ExcludedSources @('directory.c') `
        -CompileOnly:$CompileOnly
    if ($RuntimeDirectory) {
        Invoke-WcrtPackagedCompatibilityTest -TestCase TC-0080 `
            -RepositoryRoot $root -TinyCc $TinyCc `
            -RuntimeDirectory $RuntimeDirectory `
            -BehaviorSource (Join-Path $PSScriptRoot 'directory-change.c') `
            -Arguments @($fixture) -CompileOnly:$CompileOnly
    }
} finally {
    if (Test-Path -LiteralPath $fixture) {
        $resolved = [IO.Path]::GetFullPath($fixture)
        $expected = [IO.Path]::GetFullPath((Join-Path $root `
            'build/tests/mscompat/tc-0080')) + [IO.Path]::DirectorySeparatorChar
        if (-not $resolved.StartsWith($expected,
            [StringComparison]::OrdinalIgnoreCase)) {
            throw 'Refusing fixture cleanup outside TC-0080 workspace.'
        }
        Remove-Item -LiteralPath $resolved -Recurse -Force
    }
}
