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
. (Join-Path $repoRoot 'tests\test-logging.ps1')
$outputDirectory = Join-Path $repoRoot `
    "output\test-results\$Architecture"
$tests = @(
    @('TC-0019', 'C99', 'Bounded formatted output',
        'tests\c99\run-tc-0019.ps1'),
    @('TC-0020', 'Microsoft compatibility',
        'Legacy bounded formatted output',
        'tests\mscompat\run-tc-0020.ps1'),
    @('TC-0043', 'Microsoft compatibility',
        'Low-level I/O header baseline',
        'tests\mscompat\run-tc-0043.ps1'),
    @('TC-0044', 'Microsoft compatibility',
        'Stream file descriptor',
        'tests\mscompat\run-tc-0044.ps1'),
    @('TC-0045', 'Microsoft compatibility',
        'Case-insensitive strings',
        'tests\mscompat\run-tc-0045.ps1'),
    @('TC-0046', 'Microsoft compatibility',
        'Secure narrow stdio',
        'tests\mscompat\run-tc-0046.ps1'),
    @('TC-0047', 'Microsoft compatibility',
        'Secure narrow string copy',
        'tests\mscompat\run-tc-0047.ps1'),
    @('TC-0048', 'Microsoft compatibility',
        'Explicit-width file-time update',
        'tests\mscompat\run-tc-0048.ps1'),
    @('TC-0049', 'Microsoft compatibility',
        '64-bit file status and x86 alias',
        'tests\mscompat\run-tc-0049.ps1'),
    @('TC-0050', 'WPM integration',
        'Bounded formatted-output integration',
        'tests\mscompat\run-tc-0050.ps1')
)
$results = foreach ($test in $tests) {
    Write-WspInfo "Running $($test[0]) ($($test[2])) on $Architecture."
    try {
        & (Join-Path $repoRoot $test[3]) -TinyCc $TinyCc | Out-Null
        $result = [PSCustomObject]@{
            TestCase = $test[0]
            Suite = $test[1]
            Description = $test[2]
            Status = 'Pass'
            Output = ''
        }
    } catch {
        $result = [PSCustomObject]@{
            TestCase = $test[0]
            Suite = $test[1]
            Description = $test[2]
            Status = 'Fail'
            Output = $_.Exception.Message
        }
    }
    Write-WcrtTestResult -Status $result.Status `
        -Message "$($test[0]) ($($test[2])) on $Architecture."
    $result
}
New-Item -ItemType Directory -Force $outputDirectory | Out-Null
$results | ConvertTo-Json -Depth 4 |
    Set-Content (Join-Path $outputDirectory 'extension-test-results.json') `
        -Encoding utf8NoBOM
$results | Format-Table TestCase, Suite, Description, Status
if (@($results | Where-Object Status -ne 'Pass').Count -ne 0) {
    Write-WspError "Extension test suite failed on $Architecture."
    exit 1
}
Write-WspPass "Extension test suite passed on $Architecture."
