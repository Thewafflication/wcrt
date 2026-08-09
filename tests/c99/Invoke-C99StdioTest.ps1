[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [string]$TestCase,

    [Parameter(Mandatory)]
    [string]$Requirement,

    [Parameter(Mandatory)]
    [string]$PresenceSource,

    [Parameter(Mandatory)]
    [string]$C89Source,

    [Parameter(Mandatory)]
    [string]$BehaviorSource,

    [string]$TinyCc,

    [switch]$CompileOnly
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$testName = $TestCase.ToLowerInvariant()
$buildDirectory = Join-Path $repoRoot "build/tests/c99/$testName"
$presenceObject = Join-Path $buildDirectory 'presence.o'
$c89Object = Join-Path $buildDirectory 'c89-isolation.o'
$executable = Join-Path $buildDirectory 'behavior.exe'

if ([string]::IsNullOrWhiteSpace($TinyCc)) {
    $TinyCc = Get-Command tcc.exe -ErrorAction SilentlyContinue |
        Select-Object -First 1 -ExpandProperty Source
}
if (-not $TinyCc) {
    $TinyCc = Join-Path (Split-Path -Parent $repoRoot) `
        'tcc_package/out/build/x64-debug/package/tcc.exe'
}
if (-not (Test-Path -LiteralPath $TinyCc)) {
    throw 'TinyCC was not found.'
}

New-Item -ItemType Directory -Force -Path $buildDirectory | Out-Null
$include = Join-Path $repoRoot 'include'
$c99Arguments = @('-std=c99', '-Wall', '-Werror', '-I', $include)
$presenceOutput = & $TinyCc @c99Arguments -c `
    (Join-Path $repoRoot $PresenceSource) -o $presenceObject 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "$TestCase presence build failed:`n" +
        ($presenceOutput | Out-String)
}

$c89Output = & $TinyCc -std=c89 -DWCRT_C89=1 -Wall -Werror -I $include `
    -c (Join-Path $repoRoot $C89Source) -o $c89Object 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "$TestCase C89 isolation build failed:`n" +
        ($c89Output | Out-String)
}

$sources = @(
    'src/ctype.c', 'src/errno.c', 'src/string.c', 'src/stdlib.c',
    'src/stdio.c', 'src/stdio_format.c', 'src/stdio_scan.c',
    'src/platform/windows/heap.c', 'src/platform/windows/process.c',
    'src/platform/windows/file.c', $BehaviorSource
) | ForEach-Object { Join-Path $repoRoot $_ }
$behaviorOutput = & $TinyCc @c99Arguments @sources -o $executable 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "$TestCase behavior build failed:`n" +
        ($behaviorOutput | Out-String)
}

if ($CompileOnly) {
    return [PSCustomObject]@{
        TestCase = $TestCase
        Requirement = $Requirement
        Interface = 'C99 narrow formatted I/O'
        C89Isolation = 'Pass'
        Execution = 'Not run (compile/link only)'
        ExitCode = $null
    }
}

& $executable
if ($LASTEXITCODE -ne 0) {
    throw "$TestCase behavior failed with code $LASTEXITCODE."
}

[PSCustomObject]@{
    TestCase = $TestCase
    Requirement = $Requirement
    Interface = 'C99 narrow formatted I/O'
    C89Isolation = 'Pass'
    ExitCode = 0
}
