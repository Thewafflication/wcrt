[CmdletBinding()]
param([string]$TinyCc, [string]$RuntimeDirectory, [switch]$CompileOnly)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$fixture = Join-Path $root ('build/tests/mscompat/tc-0085/owned-' +
    [guid]::NewGuid().ToString('N'))
try {
    Invoke-WcrtCompatibilityTest -TestCase TC-0085 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
        'presence/drive-directory.c') -BehaviorSource (Join-Path $PSScriptRoot `
        'drive-directory.c') -Arguments @($fixture) -CompileOnly:$CompileOnly
    Invoke-WcrtCompatibilityTest -TestCase TC-0085 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
        'presence/drive-directory.c') -BehaviorSource (Join-Path $PSScriptRoot `
        'drive-directory-errors.c') -ExcludedSources @('directory.c') `
        -CompileOnly:$CompileOnly
    if ($RuntimeDirectory) {
        Invoke-WcrtPackagedCompatibilityTest -TestCase TC-0085 `
            -RepositoryRoot $root -TinyCc $TinyCc `
            -RuntimeDirectory $RuntimeDirectory `
            -BehaviorSource (Join-Path $PSScriptRoot 'drive-directory.c') `
            -Arguments @($fixture) -CompileOnly:$CompileOnly
    }
} finally {
    if (Test-Path -LiteralPath $fixture) {
        $resolved = [IO.Path]::GetFullPath($fixture)
        $expected = [IO.Path]::GetFullPath((Join-Path $root `
            'build/tests/mscompat/tc-0085')) + [IO.Path]::DirectorySeparatorChar
        if (-not $resolved.StartsWith($expected,
            [StringComparison]::OrdinalIgnoreCase)) {
            throw 'Refusing fixture cleanup outside TC-0085 workspace.'
        }
        Remove-Item -LiteralPath $resolved -Recurse -Force
    }
}
