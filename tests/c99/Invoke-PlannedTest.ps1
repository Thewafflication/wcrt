[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [ValidatePattern('^TC-\d{4}$')]
    [string]$TestCase,

    [Parameter(Mandatory)]
    [string[]]$RequiredPath
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$missing = @($RequiredPath | Where-Object {
    -not (Test-Path -LiteralPath (Join-Path $repoRoot $_) -PathType Leaf)
})

if ($missing.Count -gt 0) {
    Write-Error (
        "$TestCase is planned but not implemented. Missing: " +
        ($missing -join ', '))
    exit 1
}

Write-Error (
    "$TestCase implementation inputs exist, but its conformance checks have " +
    'not yet replaced the planning guard.')
exit 1
