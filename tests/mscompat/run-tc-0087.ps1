[CmdletBinding()]
param([string]$TinyCc, [string]$RuntimeDirectory, [switch]$CompileOnly)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$build = Join-Path $root 'build/tests/mscompat/tc-0087'
New-Item -ItemType Directory -Force -Path $build | Out-Null
foreach ($profile in 'Microsoft', 'POSIX') {
    foreach ($order in 'Types', 'Stat') {
        $options = @()
        if ($profile -eq 'POSIX') { $options += '-DWCRT_POSIX=1' }
        if ($order -eq 'Stat') { $options += '-DWCRT_TYPES_STAT_FIRST=1' }
        $executable = Join-Path $build "$profile-$order.exe"
        & $TinyCc -std=c89 -Wall -Werror @options `
            -I (Join-Path $root 'include') `
            (Join-Path $PSScriptRoot 'system-types.c') -o $executable
        if ($LASTEXITCODE -ne 0) { throw "TC-0087 compile: $profile/$order" }
        if (-not $CompileOnly) {
            & $executable
            if ($LASTEXITCODE -ne 0) { throw 'TC-0087 type behavior failed.' }
        }
    }
}
if ($RuntimeDirectory) {
    Invoke-WcrtPackagedCompatibilityTest -TestCase TC-0087 `
        -RepositoryRoot $root -TinyCc $TinyCc `
        -RuntimeDirectory $RuntimeDirectory `
        -BehaviorSource (Join-Path $PSScriptRoot 'system-types.c') `
        -CompileOnly:$CompileOnly
}
[PSCustomObject]@{
    TestCase = 'TC-0087'
    Requirement = 'REQ-0087'
    Status = 'Pass'
    Execution = $(if ($CompileOnly) { 'Not run (compile/link only)' }
        else { 'Pass' })
}
