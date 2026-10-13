[CmdletBinding()]
param([string]$TinyCc, [string]$RuntimeDirectory, [switch]$CompileOnly)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$fixture = Join-Path $root ('build/tests/mscompat/tc-0086/owned-' +
    [guid]::NewGuid().ToString('N') + '.bin')
try {
    Invoke-WcrtCompatibilityTest -TestCase TC-0086 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
        'presence/stat32.c') -BehaviorSource (Join-Path $PSScriptRoot `
        'stat32.c') -Arguments @($fixture) -CompileOnly:$CompileOnly
    Invoke-WcrtCompatibilityTest -TestCase TC-0086 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
        'presence/stat32.c') -BehaviorSource (Join-Path $PSScriptRoot `
        'stat32-errors.c') -ExcludedSources @('ms_stat32.c') `
        -CompileOnly:$CompileOnly
    $description = (& $TinyCc -v 2>&1 | Out-String)
    $isX86 = $description -match 'i386 Windows'
    $selected = Join-Path $PSScriptRoot 'stat32-selected.c'
    $object = Join-Path $root 'build/tests/mscompat/tc-0086/selected.o'
    $output = @(& $TinyCc -std=c89 -Wall -Werror `
        -I (Join-Path $root 'include') -c $selected -o $object 2>&1)
    $selectedExit = $LASTEXITCODE
    if ($isX86) {
        if ($selectedExit -ne 0) {
            throw "TC-0086 x86 selector failed: $($output -join "`n")"
        }
        Invoke-WcrtCompatibilityTest -TestCase TC-0086 `
            -RepositoryRoot $root -TinyCc $TinyCc `
            -PresenceSource (Join-Path $PSScriptRoot 'presence/stat32.c') `
            -BehaviorSource $selected -CompileOnly:$CompileOnly
    } elseif ($selectedExit -eq 0 -or ($output -join "`n") -notmatch
        'WCRT_VC6_STAT32 requires an x86 consumer') {
        throw 'TC-0086 wrong-target selector did not fail as specified.'
    }
    if ($RuntimeDirectory) {
        Invoke-WcrtPackagedCompatibilityTest -TestCase TC-0086 `
            -RepositoryRoot $root -TinyCc $TinyCc `
            -RuntimeDirectory $RuntimeDirectory `
            -BehaviorSource (Join-Path $PSScriptRoot 'stat32.c') `
            -Arguments @($fixture) -CompileOnly:$CompileOnly
        if ($isX86) {
            Invoke-WcrtPackagedCompatibilityTest -TestCase TC-0086 `
                -RepositoryRoot $root -TinyCc $TinyCc `
                -RuntimeDirectory $RuntimeDirectory `
                -BehaviorSource $selected -CompileOnly:$CompileOnly
        }
    }
} finally {
    if (Test-Path -LiteralPath $fixture) {
        Remove-Item -LiteralPath $fixture -Force
    }
}
