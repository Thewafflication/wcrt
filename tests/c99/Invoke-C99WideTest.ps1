[CmdletBinding()]
param(
    [Parameter(Mandatory)][ValidatePattern('^TC-\d{4}$')][string]$TestCase,
    [Parameter(Mandatory)][ValidatePattern('^REQ-\d{4}$')][string]$Requirement,
    [Parameter(Mandatory)][string]$PublicHeader,
    [Parameter(Mandatory)][string]$PresenceSource,
    [Parameter(Mandatory)][string]$C89Source,
    [Parameter(Mandatory)][string]$BehaviorSource,
    [string]$TinyCc,
    [switch]$CompileOnly
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$buildDirectory = Join-Path $repoRoot "build/tests/c99/$($TestCase.ToLower())"
$include = Join-Path $repoRoot 'include'

if ([string]::IsNullOrWhiteSpace($TinyCc)) {
    $TinyCc = Get-Command tcc.exe -ErrorAction SilentlyContinue |
        Select-Object -First 1 -ExpandProperty Source
}
if (-not $TinyCc) {
    $TinyCc = Join-Path (Split-Path -Parent $repoRoot) `
        'tcc_package/out/build/x64-debug/package/tcc.exe'
}
if (-not (Test-Path -LiteralPath $TinyCc -PathType Leaf)) {
    throw 'TinyCC was not found.'
}
if (-not (Test-Path -LiteralPath (Join-Path $repoRoot $PublicHeader))) {
    throw "$TestCase required WCRT header is missing: $PublicHeader"
}

New-Item -ItemType Directory -Force -Path $buildDirectory | Out-Null
$common = @('-std=c99', '-Wall', '-Werror', '-I', $include)
$output = & $TinyCc @common -c (Join-Path $repoRoot $PresenceSource) `
    -o (Join-Path $buildDirectory 'presence.o') 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "$TestCase presence build failed:`n$($output | Out-String)"
}

$output = & $TinyCc -std=c89 -DWCRT_C89=1 -Wall -Werror -I $include -c `
    (Join-Path $repoRoot $C89Source) `
    -o (Join-Path $buildDirectory 'c89-isolation.o') 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "$TestCase C89 isolation build failed:`n$($output | Out-String)"
}

$sources = @(
    'src/ctype.c', 'src/errno.c', 'src/locale.c', 'src/stdio.c',
    'src/stdio_format.c', 'src/stdio_scan.c', 'src/stdlib.c', 'src/string.c',
    'src/time.c', 'src/platform/windows/clock.c',
    'src/platform/windows/file.c', 'src/platform/windows/heap.c',
    'src/platform/windows/process.c'
)
if ($TestCase -eq 'TC-0033') {
    $sources += @('src/wchar_convert.c', 'src/wchar_io.c')
} elseif ($TestCase -eq 'TC-0032') {
    $sources += 'src/wctype.c'
} else {
    $sources += @(
        'src/wchar_convert.c', 'src/wchar_string.c', 'src/wchar_io.c',
        'src/wchar_format.c', 'src/wchar_scan.c', 'src/wchar_time.c',
        'src/wctype.c'
    )
}
$sources += $BehaviorSource
$sources = $sources | ForEach-Object { Join-Path $repoRoot $_ }
$executable = Join-Path $buildDirectory 'behavior.exe'
$output = & $TinyCc @common @sources -o $executable 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "$TestCase behavior build failed:`n$($output | Out-String)"
}
if (-not $CompileOnly) {
    & $executable
    if ($LASTEXITCODE -ne 0) {
        throw "$TestCase behavior failed with code $LASTEXITCODE."
    }
}

[PSCustomObject]@{
    TestCase = $TestCase
    Requirement = $Requirement
    Presence = 'Pass'
    C89Isolation = 'Pass'
    Behavior = $(if ($CompileOnly) { 'Compile/link only' } else { 'Pass' })
    ExitCode = 0
}
