[CmdletBinding()]
param([string]$TinyCc, [string]$RuntimeDirectory)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$build = Join-Path $root 'build/tests/mscompat/tc-0091'
New-Item -ItemType Directory -Force -Path $build | Out-Null
$defaultSource = Join-Path $PSScriptRoot 'presence/utime-default.c'
& $TinyCc -std=c89 -Wall -Werror -I (Join-Path $root 'include') `
    -c $defaultSource -o (Join-Path $build 'default.o')
if ($LASTEXITCODE -ne 0) { throw 'TC-0091 default header isolation failed.' }
$selected = Join-Path $PSScriptRoot 'utime32-selected.c'
$description = (& $TinyCc -v 2>&1 | Out-String)
if ($description -match 'i386 Windows') {
    Invoke-WcrtCompatibilityTest -TestCase TC-0091 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource $selected -BehaviorSource $selected
    if ($RuntimeDirectory) {
        Invoke-WcrtPackagedCompatibilityTest -TestCase TC-0091 `
            -RepositoryRoot $root -TinyCc $TinyCc `
            -RuntimeDirectory $RuntimeDirectory -BehaviorSource $selected
    }
} else {
    $output = @(& $TinyCc -std=c89 -Wall -Werror `
        -I (Join-Path $root 'include') -c $selected `
        -o (Join-Path $build 'selected.o') 2>&1)
    if ($LASTEXITCODE -eq 0 -or ($output -join "`n") -notmatch
        'WCRT_VC6_UTIME32 requires an x86 consumer') {
        throw 'TC-0091 wrong-target selector did not fail as specified.'
    }
}
[PSCustomObject]@{
    TestCase = 'TC-0091'
    Requirement = 'REQ-0091'
    Status = 'Pass'
}
