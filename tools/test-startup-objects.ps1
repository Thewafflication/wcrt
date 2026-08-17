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
    & (Join-Path $repoRoot 'tests/c89/run-tc-0018.ps1') @arguments
)
$results

$failures = @($results | Where-Object Status -eq 'Fail')
if ($failures.Count -ne 0) {
    foreach ($failure in $failures) {
        Write-WspError (
            "$($failure.TestCase) startup verification failed on " +
            "$Architecture`: $($failure.Output)")
    }
    throw "$Architecture $Configuration startup-object tests failed."
}

$blocked = @($results | Where-Object Status -eq 'Blocked')
if ($blocked.Count -ne 0) {
    foreach ($result in $blocked) {
        Write-WspWarning (
            "$($result.TestCase) startup verification is blocked on " +
            "$Architecture`: $($result.Rationale)")
    }
    Write-WspWarning (
        "$Architecture $Configuration startup-object tests completed " +
        'with target execution blocked.')
} else {
    foreach ($result in $results) {
        Write-WspPass (
            "$($result.TestCase) startup verification passed on " +
            "$Architecture.")
    }
    Write-WspPass "$Architecture $Configuration startup-object tests passed."
}
