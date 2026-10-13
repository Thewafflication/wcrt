[CmdletBinding()]
param([string]$TinyCc, [string]$RuntimeDirectory, [switch]$CompileOnly)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$owned = Join-Path $root 'build/tests/mscompat/tc-0096'
New-Item -ItemType Directory -Path $owned -Force | Out-Null
$fixture = Join-Path $owned ('owned-' + [guid]::NewGuid().ToString('N') + '.bin')
$pattern = $fixture.Replace('.bin', '*.bin')
try {
    Invoke-WcrtCompatibilityTest -TestCase TC-0096 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
        'presence/find32.c') -BehaviorSource (Join-Path $PSScriptRoot `
        'find32.c') -Arguments @($fixture, $pattern) -CompileOnly:$CompileOnly
    Invoke-WcrtCompatibilityTest -TestCase TC-0096 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
        'presence/find32.c') -BehaviorSource (Join-Path $PSScriptRoot `
        'find32-errors.c') -ExcludedSources @('ms_find32.c') `
        -CompileOnly:$CompileOnly
    if ($RuntimeDirectory) {
        Invoke-WcrtPackagedCompatibilityTest -TestCase TC-0096 `
            -RepositoryRoot $root -TinyCc $TinyCc `
            -RuntimeDirectory $RuntimeDirectory -BehaviorSource (Join-Path `
            $PSScriptRoot 'find32.c') -Arguments @($fixture, $pattern) `
            -CompileOnly:$CompileOnly
    }
    $compilerDescription = (& $TinyCc -v 2>&1 | Out-String)
    if ($compilerDescription -match 'i386 Windows') {
        Invoke-WcrtCompatibilityTest -TestCase TC-0096 -RepositoryRoot $root `
            -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
            'presence/find32.c') -BehaviorSource (Join-Path $PSScriptRoot `
            'vc6-find32-main.c') -CompileOnly:$CompileOnly
        if ($RuntimeDirectory) {
            Invoke-WcrtPackagedCompatibilityTest -TestCase TC-0096 `
                -RepositoryRoot $root -TinyCc $TinyCc `
                -RuntimeDirectory $RuntimeDirectory -BehaviorSource (Join-Path `
                $PSScriptRoot 'vc6-find32-main.c') -CompileOnly:$CompileOnly
        }
    } else {
        $rejection = Join-Path $owned 'wrong-target.c'
        Set-Content -LiteralPath $rejection -Value @'
#define WCRT_VC6_FIND32 1
#include <io.h>
'@
        $messages = @(& $TinyCc -I (Join-Path $root 'include') `
            -c $rejection `
            -o (Join-Path $owned 'rejected.o') 2>&1)
        if ($LASTEXITCODE -eq 0 -or
            ($messages -join "`n") -notmatch 'requires x86') {
            throw 'TC-0096 wrong-target source selection was not rejected.'
        }
    }
} finally {
    if (Test-Path -LiteralPath $fixture) {
        Remove-Item -LiteralPath $fixture -Force
    }
}
