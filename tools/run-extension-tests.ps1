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
. (Join-Path $repoRoot 'tests\native-test-diagnostics.ps1')
$diagnosticTinyCc = Get-WcrtDiagnosticTinyCc -TinyCc $TinyCc `
    -RepositoryRoot $repoRoot
$outputDirectory = Join-Path $repoRoot `
    "output\test-results\$Architecture"
$tests = @(
    @('TC-0019', 'REQ-0019', 'C99', 'Bounded formatted output',
        'tests\c99\run-tc-0019.ps1'),
    @('TC-0021', 'REQ-0021', 'C99', 'Boolean type and values',
        'tests\c99\run-tc-0021.ps1'),
    @('TC-0022', 'REQ-0022', 'C99', 'Fixed-width integer interface',
        'tests\c99\run-tc-0022.ps1'),
    @('TC-0023', 'REQ-0023', 'C99', 'Long-long limits',
        'tests\c99\run-tc-0023.ps1'),
    @('TC-0024', 'REQ-0024', 'C99', 'Floating-point characteristics',
        'tests\c99\run-tc-0024.ps1'),
    @('TC-0025', 'REQ-0025', 'C99',
        'Variable arguments and restrict contracts',
        'tests\c99\run-tc-0025.ps1'),
    @('TC-0026', 'REQ-0026', 'C99', 'Alternative operator spellings',
        'tests\c99\run-tc-0026.ps1'),
    @('TC-0027', 'REQ-0027', 'C99', 'Greatest-width integer interfaces',
        'tests\c99\run-tc-0027.ps1'),
    @('TC-0028', 'REQ-0028', 'C99', 'C99 general utilities',
        'tests\c99\run-tc-0028.ps1'),
    @('TC-0029', 'REQ-0029', 'C99', 'C99 narrow formatted output',
        'tests\c99\run-tc-0029.ps1'),
    @('TC-0030', 'REQ-0030', 'C99', 'C99 narrow formatted input',
        'tests\c99\run-tc-0030.ps1'),
    @('TC-0033', 'REQ-0033', 'C99',
        'Windows UTF-16 and wide-character ABI',
        'tests\c99\run-tc-0033.ps1'),
    @('TC-0031', 'REQ-0031', 'C99', 'C99 wide-character utilities',
        'tests\c99\run-tc-0031.ps1'),
    @('TC-0032', 'REQ-0032', 'C99',
        'C99 wide-character classification',
        'tests\c99\run-tc-0032.ps1'),
    @('TC-0035', 'REQ-0035', 'C99',
        'Real mathematics and classification',
        'tests\c99\run-tc-0035.ps1'),
    @('TC-0036', 'REQ-0036', 'C99',
        'Floating-point environment',
        'tests\c99\run-tc-0036.ps1'),
    @('TC-0037', 'REQ-0037', 'C99',
        'Complex arithmetic',
        'tests\c99\run-tc-0037.ps1'),
    @('TC-0038', 'REQ-0038', 'C99',
        'Type-generic mathematics',
        'tests\c99\run-tc-0038.ps1'),
    @('TC-0039', 'REQ-0039', 'C99', 'Narrow numeric text interchange',
        'tests\c99\run-tc-0039.ps1'),
    @('TC-0040', 'REQ-0040', 'C99',
        'Existing-header C99 integration audit',
        'tests\c99\run-tc-0040.ps1'),
    @('TC-0041', 'REQ-0041', 'C99', 'C99 conformance profile',
        'tests\c99\run-tc-0041.ps1'),
    @('TC-0042', 'REQ-0042', 'C99',
        'C99 compatibility and target validation',
        'tests\c99\run-tc-0042.ps1'),
    @('TC-0020', 'REQ-0020', 'Microsoft compatibility',
        'Legacy bounded formatted output',
        'tests\mscompat\run-tc-0020.ps1'),
    @('TC-0043', 'REQ-0043', 'Microsoft compatibility',
        'Low-level I/O header baseline',
        'tests\mscompat\run-tc-0043.ps1'),
    @('TC-0044', 'REQ-0044', 'Microsoft compatibility',
        'Stream file descriptor',
        'tests\mscompat\run-tc-0044.ps1'),
    @('TC-0045', 'REQ-0045', 'Microsoft compatibility',
        'Case-insensitive strings',
        'tests\mscompat\run-tc-0045.ps1'),
    @('TC-0046', 'REQ-0046', 'Microsoft compatibility',
        'Secure narrow stdio',
        'tests\mscompat\run-tc-0046.ps1'),
    @('TC-0047', 'REQ-0047', 'Microsoft compatibility',
        'Secure narrow string operations',
        'tests\mscompat\run-tc-0047.ps1'),
    @('TC-0048', 'REQ-0048', 'Microsoft compatibility',
        'Explicit-width file-time update',
        'tests\mscompat\run-tc-0048.ps1'),
    @('TC-0049', 'REQ-0049', 'Microsoft compatibility',
        '64-bit file status and x86 alias',
        'tests\mscompat\run-tc-0049.ps1'),
    @('TC-0050', 'REQ-0050', 'POSIX compatibility',
        'Compatibility selection and error names',
        'tests\posix\run-tc-0050.ps1'),
    @('TC-0051', 'REQ-0051', 'POSIX compatibility',
        'File status and timestamp adapters',
        'tests\posix\run-tc-0051.ps1'),
    @('TC-0052', 'REQ-0052', 'POSIX compatibility',
        'Directory stream adapters',
        'tests\posix\run-tc-0052.ps1'),
    @('TC-0053', 'REQ-0053', 'Microsoft compatibility',
        'In-place string transformations',
        'tests\mscompat\run-tc-0053.ps1'),
    @('TC-0054', 'REQ-0054', 'Microsoft compatibility',
        'Allocated string duplication',
        'tests\mscompat\run-tc-0054.ps1'),
    @('TC-0055', 'REQ-0055', 'Microsoft compatibility',
        'In-place string character replacement',
        'tests\mscompat\run-tc-0055.ps1'),
    @('TC-0056', 'REQ-0056', 'Microsoft compatibility',
        'Integer-to-string conversions',
        'tests\mscompat\run-tc-0056.ps1'),
    @('TC-0057', 'REQ-0057', 'Microsoft compatibility',
        'Case-insensitive memory comparison',
        'tests\mscompat\run-tc-0057.ps1'),
    @('TC-0058', 'REQ-0058', 'POSIX compatibility',
        'Case-insensitive string comparison',
        'tests\posix\run-tc-0058.ps1'),
    @('TC-0059', 'REQ-0059', 'POSIX compatibility',
        'Allocated string duplication',
        'tests\posix\run-tc-0059.ps1'),
    @('TC-0060', 'REQ-0060', 'POSIX compatibility',
        'Process identification',
        'tests\posix\run-tc-0060.ps1'),
    @('TC-0061', 'REQ-0061', 'POSIX compatibility',
        'Filesystem-name removal',
        'tests\posix\run-tc-0061.ps1'),
    @('TC-0062', 'REQ-0062', 'Microsoft compatibility',
        'Filesystem-name removal',
        'tests\mscompat\run-tc-0062.ps1')
)

$manifest = Join-Path $repoRoot 'tests\c99\manifest.md'
$controlledC99 = @(
    Select-String -LiteralPath $manifest -Pattern '\[TC-(\d{4})\]' -AllMatches |
        ForEach-Object { $_.Matches } |
        ForEach-Object { "TC-$($_.Groups[1].Value)" } |
        Sort-Object -Unique
)
$aggregateC99 = @(
    $tests | Where-Object { $_[2] -eq 'C99' } |
        ForEach-Object { $_[0] } | Sort-Object -Unique
)
$inventoryDifference = @(Compare-Object $controlledC99 $aggregateC99)
if ($inventoryDifference.Count -ne 0) {
    $detail = $inventoryDifference | ForEach-Object {
        "$($_.InputObject) $($_.SideIndicator)"
    }
    throw "Aggregate C99 inventory differs from the controlled manifest: " +
        ($detail -join ', ')
}
$posixManifest = Join-Path $repoRoot 'tests\posix\manifest.md'
$controlledPosix = @(
    Select-String -LiteralPath $posixManifest -Pattern '\[TC-(\d{4})\]' `
        -AllMatches |
        ForEach-Object { $_.Matches } |
        ForEach-Object { "TC-$($_.Groups[1].Value)" } |
        Sort-Object -Unique
)
$aggregatePosix = @(
    $tests | Where-Object { $_[2] -eq 'POSIX compatibility' } |
        ForEach-Object { $_[0] } | Sort-Object -Unique
)
$posixDifference = @(Compare-Object $controlledPosix $aggregatePosix)
if ($posixDifference.Count -ne 0) {
    $detail = $posixDifference | ForEach-Object {
        "$($_.InputObject) $($_.SideIndicator)"
    }
    throw "Aggregate POSIX inventory differs from the controlled manifest: " +
        ($detail -join ', ')
}

$results = foreach ($test in $tests) {
    Write-WspInfo "Running $($test[0]) ($($test[3])) on $Architecture."
    $invocation = Invoke-WcrtTestRunnerWithDiagnostics `
        -TestCase $test[0] -Suite $test[2] -Architecture $Architecture `
        -RepositoryRoot $repoRoot -Runner (Join-Path $repoRoot $test[4]) `
        -TinyCc $diagnosticTinyCc
    if ($invocation.Succeeded) {
        try {
            $testOutput = @($invocation.RunnerOutput)
            $reported = @($testOutput | Where-Object {
                $_ -is [PSCustomObject] -and $_.PSObject.Properties['Status']
            } | Select-Object -Last 1)
            $reportedStatus = if ($reported.Count -eq 1) {
                [string]$reported[0].Status
            } else { 'Pass' }
            if ($reportedStatus -notin 'Pass', 'ExpectedFail') {
                throw "$($test[0]) runner reported $reportedStatus."
            }
            $result = [PSCustomObject]@{
                TestCase = $test[0]
                Requirement = $test[1]
                Suite = $test[2]
                Description = $test[3]
                Status = $reportedStatus
                Output = if ($reportedStatus -eq 'ExpectedFail') {
                    [string]$reported[0].Output
                } else { '' }
                Diagnostics = $null
            }
        } catch {
            $result = [PSCustomObject]@{
                TestCase = $test[0]
                Requirement = $test[1]
                Suite = $test[2]
                Description = $test[3]
                Status = 'Fail'
                Output = $_.Exception.Message
                Diagnostics = $null
            }
        }
    } else {
        $result = [PSCustomObject]@{
            TestCase = $test[0]
            Requirement = $test[1]
            Suite = $test[2]
            Description = $test[3]
            Status = 'Fail'
            Output = $invocation.FailureReport
            Diagnostics = $invocation.Diagnostics
        }
    }
    Write-WcrtTestResult -Status $result.Status `
        -Message "$($test[0]) ($($test[3])) on $Architecture."
    $result
}
New-Item -ItemType Directory -Force $outputDirectory | Out-Null
$results | ConvertTo-Json -Depth 4 |
    Set-Content (Join-Path $outputDirectory 'extension-test-results.json') `
        -Encoding utf8NoBOM
$results | Format-Table TestCase, Requirement, Suite, Description, Status
if (@($results | Where-Object Status -notin 'Pass', 'ExpectedFail').Count `
    -ne 0) {
    Write-WspError "Extension test suite failed on $Architecture."
    exit 1
}
$expectedFailures = @($results | Where-Object Status -eq 'ExpectedFail').Count
if ($expectedFailures -ne 0) {
    Write-WspWarning ("Extension test suite completed on $Architecture " +
        "with $expectedFailures expected compiler failure(s).")
} else {
    Write-WspPass "Extension test suite passed on $Architecture."
}
