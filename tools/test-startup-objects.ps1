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
$arguments = @{
    Architecture = $Architecture
    Configuration = $Configuration
    TinyCc = $TinyCc
    BuildRoot = $BuildRoot
}

@(
    & (Join-Path $repoRoot 'tests/c89/run-tc-0017.ps1') @arguments
    & (Join-Path $repoRoot 'tests/c89/run-tc-0018.ps1') @arguments
)
