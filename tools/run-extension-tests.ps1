[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [ValidateSet('x86', 'x64', 'arm64')]
    [string]$Architecture,

    [Parameter(Mandatory)]
    [string]$TinyCc
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$outputDirectory = Join-Path $repoRoot `
    "output\test-results\$Architecture"
$tests = @(
    @('TC-0019', 'C99', 'Bounded formatted output',
        'tests\c99\run-tc-0019.ps1'),
    @('TC-0020', 'Microsoft compatibility',
        'Legacy bounded formatted output',
        'tests\mscompat\run-tc-0020.ps1')
)
$results = foreach ($test in $tests) {
    try {
        & (Join-Path $repoRoot $test[3]) -TinyCc $TinyCc | Out-Null
        [PSCustomObject]@{
            TestCase = $test[0]
            Suite = $test[1]
            Description = $test[2]
            Status = 'Pass'
            Output = ''
        }
    } catch {
        [PSCustomObject]@{
            TestCase = $test[0]
            Suite = $test[1]
            Description = $test[2]
            Status = 'Fail'
            Output = $_.Exception.Message
        }
    }
}
New-Item -ItemType Directory -Force $outputDirectory | Out-Null
$results | ConvertTo-Json -Depth 4 |
    Set-Content (Join-Path $outputDirectory 'extension-test-results.json') `
        -Encoding utf8NoBOM
$results | Format-Table TestCase, Suite, Description, Status
if (@($results | Where-Object Status -ne 'Pass').Count -ne 0) {
    exit 1
}
