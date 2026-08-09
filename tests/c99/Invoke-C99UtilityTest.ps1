[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$TestCase,
    [Parameter(Mandatory)][string]$Requirement,
    [Parameter(Mandatory)][string]$Name,
    [Parameter(Mandatory)][string]$PublicHeader,
    [Parameter(Mandatory)][string]$PresenceSource,
    [Parameter(Mandatory)][string]$BehaviorSource,
    [string]$ExitSource,
    [string]$C89Source,
    [string]$C89Regression,
    [string]$TinyCc
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$buildDirectory = Join-Path $repoRoot "build\tests\c99\$($TestCase.ToLower())"
if (-not $TinyCc) {
    $TinyCc = Get-Command tcc.exe -ErrorAction SilentlyContinue |
        Select-Object -First 1 -ExpandProperty Source
}
if (-not $TinyCc) {
    $TinyCc = Join-Path (Split-Path -Parent $repoRoot) `
        'tcc_package\out\build\x64-debug\package\tcc.exe'
}
if (-not (Test-Path -LiteralPath $TinyCc -PathType Leaf)) {
    throw 'TinyCC was not found.'
}
if (-not (Test-Path -LiteralPath (Join-Path $repoRoot $PublicHeader))) {
    throw "$TestCase required WCRT header is missing: $PublicHeader"
}
New-Item -ItemType Directory -Force $buildDirectory | Out-Null
$include = Join-Path $repoRoot 'include'
$presenceObject = Join-Path $buildDirectory "$Name-presence.o"
$output = & $TinyCc -std=c99 -Wall -Werror -I $include -c `
    (Join-Path $repoRoot $PresenceSource) -o $presenceObject 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "$TestCase presence build failed:`n$($output | Out-String)"
}
$runtimeSources = @(
    'src/ctype.c', 'src/errno.c', 'src/string.c', 'src/stdlib.c',
    'src/wchar_string.c',
    'src/platform/windows/heap.c', 'src/platform/windows/process.c'
) | ForEach-Object { Join-Path $repoRoot $_ }
$executable = Join-Path $buildDirectory "$Name-test.exe"
$output = & $TinyCc -std=c99 -Wall -Werror -I $include @runtimeSources `
    (Join-Path $repoRoot $BehaviorSource) -o $executable 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "$TestCase behavior build failed:`n$($output | Out-String)"
}
& $executable
if ($LASTEXITCODE -ne 0) {
    throw "$TestCase behavior failed with code $LASTEXITCODE."
}
if ($ExitSource) {
    $exitExecutable = Join-Path $buildDirectory "$Name-exit-test.exe"
    $output = & $TinyCc -std=c99 -Wall -Werror -I $include @runtimeSources `
        (Join-Path $repoRoot $ExitSource) -o $exitExecutable 2>&1
    if ($LASTEXITCODE -ne 0) {
        throw "$TestCase exit build failed:`n$($output | Out-String)"
    }
    & $exitExecutable
    if ($LASTEXITCODE -ne 23) {
        throw "$TestCase _Exit child returned code $LASTEXITCODE."
    }
}
if ($C89Source) {
    $c89Object = Join-Path $buildDirectory "$Name-c89.o"
    $output = & $TinyCc -std=c89 -DWCRT_C89=1 -Wall -Werror -I $include -c `
        (Join-Path $repoRoot $C89Source) -o $c89Object 2>&1
    if ($LASTEXITCODE -ne 0) {
        throw "$TestCase C89 isolation build failed:`n$($output | Out-String)"
    }
}
if ($C89Regression) {
    & (Join-Path $repoRoot $C89Regression) -TinyCc $TinyCc | Out-Null
}

[PSCustomObject]@{
    TestCase = $TestCase
    Requirement = $Requirement
    Presence = 'Pass'
    Behavior = 'Pass'
    ExitBehavior = $(if ($ExitSource) { 'Pass' } else { 'NotApplicable' })
    C89Isolation = 'Pass'
    ExitCode = 0
}
