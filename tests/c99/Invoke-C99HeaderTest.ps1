[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [ValidatePattern('^TC-\d{4}$')]
    [string]$TestCase,

    [Parameter(Mandatory)]
    [ValidatePattern('^REQ-\d{4}$')]
    [string]$Requirement,

    [Parameter(Mandatory)]
    [string]$Name,

    [Parameter(Mandatory)]
    [string]$PublicHeader,

    [Parameter(Mandatory)]
    [string]$PresenceSource,

    [Parameter(Mandatory)]
    [string]$BehaviorSource,

    [string]$TinyCc,

    [string]$C89Source,

    [string[]]$C89Regression,

    [string[]]$RuntimeSource,

    [switch]$CompileOnly
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$buildDirectory = Join-Path $repoRoot "build\tests\c99\$($TestCase.ToLower())"
$presenceObject = Join-Path $buildDirectory "$Name-presence.o"
$executable = Join-Path $buildDirectory "$Name-test.exe"

if ([string]::IsNullOrWhiteSpace($TinyCc)) {
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

New-Item -ItemType Directory -Force -Path $buildDirectory | Out-Null
$headerPath = Join-Path $repoRoot $PublicHeader
if (-not (Test-Path -LiteralPath $headerPath -PathType Leaf)) {
    throw "$TestCase required WCRT header is missing: $PublicHeader"
}
$common = @('-std=c99', '-Wall', '-Werror', '-I',
    (Join-Path $repoRoot 'include'))
$output = & $TinyCc @common -c (Join-Path $repoRoot $PresenceSource) `
    -o $presenceObject 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "$TestCase presence build failed:`n$($output | Out-String)"
}

$runtimeSources = @(
    if ($RuntimeSource) {
        $RuntimeSource | ForEach-Object { Join-Path $repoRoot $_ }
    }
)
$output = & $TinyCc @common @runtimeSources `
    (Join-Path $repoRoot $BehaviorSource) `
    -o $executable 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "$TestCase behavior build failed:`n$($output | Out-String)"
}
if (-not $CompileOnly) {
    & $executable
    if ($LASTEXITCODE -ne 0) {
        throw "$TestCase behavior failed with code $LASTEXITCODE."
    }
}

if ($C89Source) {
    $c89Object = Join-Path $buildDirectory "$Name-c89.o"
    $c89Arguments = @('-std=c89', '-DWCRT_C89=1', '-Wall', '-Werror', '-I',
        (Join-Path $repoRoot 'include'))
    $output = & $TinyCc @c89Arguments -c `
        (Join-Path $repoRoot $C89Source) -o $c89Object 2>&1
    if ($LASTEXITCODE -ne 0) {
        throw "$TestCase C89 isolation build failed:`n" +
            ($output | Out-String)
    }
}

if (-not $CompileOnly) {
    foreach ($regression in $C89Regression) {
        & (Join-Path $repoRoot $regression) -TinyCc $TinyCc | Out-Null
        if ($LASTEXITCODE -ne 0) {
            throw "$TestCase C89 regression failed with code $LASTEXITCODE."
        }
    }
}

[PSCustomObject]@{
    TestCase = $TestCase
    Requirement = $Requirement
    Presence = 'Pass'
    Behavior = $(if ($CompileOnly) { 'CompilePass' } else { 'Pass' })
    C89Regression = $(if ($C89Regression -or $C89Source) {
        $(if ($CompileOnly) { 'CompilePass' } else { 'Pass' })
    } else {
        'NotApplicable'
    })
    ExitCode = 0
}
