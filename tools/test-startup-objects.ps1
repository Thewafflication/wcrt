[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('x86', 'x64', 'arm64')]
    [string]$Architecture,

    [Parameter(Mandatory = $true)]
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration,

    [Parameter(Mandatory = $true)]
    [string]$TinyCc,

    [string]$BuildRoot = 'output/build'
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
. (Join-Path $repoRoot 'tests\test-logging.ps1')
$arguments = @{
    Architecture = $Architecture
    Configuration = $Configuration
    TinyCc = $TinyCc
    BuildRoot = $BuildRoot
}

Write-WspInfo "Testing $Architecture $Configuration startup objects."
$results = @(
    & (Join-Path $repoRoot 'tests/c89/run-tc-0017.ps1') @arguments
    Write-WspPass "TC-0017 console startup passed on $Architecture."
    & (Join-Path $repoRoot 'tests/c89/run-tc-0018.ps1') @arguments
    Write-WspPass "TC-0018 GUI startup passed on $Architecture."
)
Write-WspPass "$Architecture $Configuration startup-object tests passed."
$results
