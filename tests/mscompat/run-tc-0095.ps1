[CmdletBinding()]
param([string]$TinyCc, [string]$RuntimeDirectory, [switch]$CompileOnly)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$build = Join-Path $root 'build/tests/mscompat/tc-0095'
New-Item -ItemType Directory -Force -Path $build | Out-Null
$presence = Join-Path $PSScriptRoot 'presence/calendar-time32.c'
$explicit = Join-Path $PSScriptRoot 'calendar-time32-main.c'
$selected = Join-Path $PSScriptRoot 'calendar-time32-selected.c'
foreach ($mode in 'c89', 'c99') {
    & $TinyCc "-std=$mode" -Wall -Werror -DWCRT_POSIX=1 `
        -I (Join-Path $root 'include') -c $presence `
        -o (Join-Path $build "default-posix-$mode.o")
    if ($LASTEXITCODE -ne 0) {
        throw 'TC-0095 default/POSIX wide ABI compilation failed.'
    }
}
Invoke-WcrtCompatibilityTest -TestCase TC-0095 -RepositoryRoot $root `
    -TinyCc $TinyCc -PresenceSource $presence -BehaviorSource $explicit `
    -CompileOnly:$CompileOnly
Invoke-WcrtCompatibilityTest -TestCase TC-0095 -RepositoryRoot $root `
    -TinyCc $TinyCc -PresenceSource $presence -BehaviorSource (Join-Path `
    $PSScriptRoot 'calendar-clock-errors.c') -ExcludedSources @('clock.c') `
    -CompileOnly:$CompileOnly
if ($RuntimeDirectory) {
    Invoke-WcrtPackagedCompatibilityTest -TestCase TC-0095 `
        -RepositoryRoot $root -TinyCc $TinyCc -RuntimeDirectory $RuntimeDirectory `
        -BehaviorSource $explicit -CompileOnly:$CompileOnly
}
$description = (& $TinyCc -v 2>&1 | Out-String)
if ($description -match 'i386 Windows') {
    Invoke-WcrtCompatibilityTest -TestCase TC-0095 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource $selected -BehaviorSource $selected `
        -CompileOnly:$CompileOnly
    if ($RuntimeDirectory) {
        Invoke-WcrtPackagedCompatibilityTest -TestCase TC-0095 `
            -RepositoryRoot $root -TinyCc $TinyCc `
            -RuntimeDirectory $RuntimeDirectory -BehaviorSource $selected `
            -CompileOnly:$CompileOnly
    }
    $output = @(& $TinyCc -std=c89 -Wall -Werror -DWCRT_POSIX=1 `
        -I (Join-Path $root 'include') -c $selected `
        -o (Join-Path $build 'posix-rejected.o') 2>&1)
    if ($LASTEXITCODE -eq 0 -or ($output -join "`n") -notmatch
        'WCRT_VC6_TIME32 cannot select the wide POSIX timestamp ABI') {
        throw 'TC-0095 incompatible POSIX selection did not fail as specified.'
    }
    $lateProfile = Join-Path $build 'late-posix.c'
    Set-Content -LiteralPath $lateProfile -Value @'
#define WCRT_VC6_TIME32 1
#include <time.h>
#define WCRT_POSIX 1
#include <time.h>
'@
    $output = @(& $TinyCc -std=c89 -Wall -Werror `
        -I (Join-Path $root 'include') -c $lateProfile `
        -o (Join-Path $build 'late-posix-rejected.o') 2>&1)
    if ($LASTEXITCODE -eq 0 -or ($output -join "`n") -notmatch
        'WCRT_VC6_TIME32 cannot select the wide POSIX timestamp ABI') {
        throw 'TC-0095 repeated-header POSIX mixing was not rejected.'
    }
} else {
    $output = @(& $TinyCc -std=c89 -Wall -Werror `
        -I (Join-Path $root 'include') -c $selected `
        -o (Join-Path $build 'target-rejected.o') 2>&1)
    if ($LASTEXITCODE -eq 0 -or ($output -join "`n") -notmatch
        'WCRT_VC6_TIME32 requires an x86 consumer') {
        throw 'TC-0095 wrong-target selection did not fail as specified.'
    }
}
[PSCustomObject]@{ TestCase = 'TC-0095'; Status = 'Pass' }
