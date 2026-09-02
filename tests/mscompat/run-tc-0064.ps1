[CmdletBinding()]
param([string]$TinyCc)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$directory = Join-Path $root 'build\tests\mscompat\tc-0064\created'
$missingParent = Join-Path $root `
    'build\tests\mscompat\tc-0064\missing-parent\created'
try {
    Invoke-WcrtCompatibilityTest -TestCase TC-0064 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
        'presence\mkdir.c') -BehaviorSource (Join-Path $PSScriptRoot `
        'mkdir.c') -Arguments @($directory, $missingParent)
} finally {
    if (Test-Path -LiteralPath $directory -PathType Container) {
        Remove-Item -LiteralPath $directory -Force
    }
}
