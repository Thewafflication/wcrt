[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('x86', 'x64', 'arm64')]
    [string]$Architecture,

    [Parameter(Mandatory = $true)]
    [string]$TinyCc,

    [string]$OutputRoot = 'output/test-results'
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
. (Join-Path $repoRoot 'tests\test-logging.ps1')
. (Join-Path $repoRoot 'tests\native-test-diagnostics.ps1')
$diagnosticTinyCc = Get-WcrtDiagnosticTinyCc -TinyCc $TinyCc `
    -RepositoryRoot $repoRoot
$outputDirectory = Join-Path $repoRoot "$OutputRoot/$Architecture"
$texPath = Join-Path $outputDirectory 'c89-test-results.tex'
$jsonPath = Join-Path $outputDirectory 'c89-test-results.json'
New-Item -ItemType Directory -Force -Path $outputDirectory | Out-Null

function ConvertTo-TexText {
    param([AllowNull()][string]$Value)
    if ($null -eq $Value) { return '' }
    $Value.Replace('\', '\textbackslash{}').Replace('&', '\&').
        Replace('%', '\%').Replace('$', '\$').Replace('#', '\#').
        Replace('_', '\_').Replace('{', '\{').Replace('}', '\}')
}

function Invoke-TestCase {
    param([string]$Id, [string]$Description)
    $runner = Join-Path $repoRoot "tests/c89/run-tc-$Id.ps1"
    Write-WspInfo "Running TC-$Id ($Description) on $Architecture."
    $started = Get-Date
    $invocation = Invoke-WcrtTestRunnerWithDiagnostics `
        -TestCase "TC-$Id" -Suite 'C89' -Architecture $Architecture `
        -RepositoryRoot $repoRoot -Runner $runner -TinyCc $diagnosticTinyCc
    if ($invocation.Succeeded) {
        $status = 'Pass'
        $result = @($invocation.RunnerOutput)
        $details = ($result | Format-List | Out-String).Trim()
        $diagnostics = $null
    } else {
        $status = 'Fail'
        $details = $invocation.FailureReport
        $diagnostics = $invocation.Diagnostics
    }
    Write-WcrtTestResult -Status $status `
        -Message "TC-$Id ($Description) on $Architecture."
    [PSCustomObject]@{
        TestCase = "TC-$Id"
        Description = $Description
        Status = $status
        Started = $started.ToString('o')
        Finished = (Get-Date).ToString('o')
        Details = $details
        Diagnostics = $diagnostics
    }
}

$tests = @(
    @('0001', 'Assertions'),
    @('0002', 'Character handling'),
    @('0003', 'Error numbers'),
    @('0004', 'Floating-point characteristics'),
    @('0005', 'Integer limits'),
    @('0006', 'Localization'),
    @('0007', 'Mathematics'),
    @('0008', 'Non-local jumps'),
    @('0009', 'Signals'),
    @('0010', 'Variadic arguments'),
    @('0011', 'Common definitions'),
    @('0012', 'Input and output'),
    @('0013', 'General utilities'),
    @('0014', 'String handling'),
    @('0015', 'Date and time')
)
$results = @($tests | ForEach-Object { Invoke-TestCase $_[0] $_[1] })
$results | ConvertTo-Json -Depth 4 |
    Set-Content -LiteralPath $jsonPath -Encoding utf8NoBOM

$rows = foreach ($result in $results) {
    "$(ConvertTo-TexText $result.TestCase) & $(ConvertTo-TexText $result.Description) & $(ConvertTo-TexText $result.Status) \\"
}
$document = @(
    '\documentclass{article}'
    '\usepackage[margin=1in]{geometry}'
    '\usepackage{longtable}'
    '\begin{document}'
    "\section*{WCRT C89 Test Results: $(ConvertTo-TexText $Architecture)}"
    "Generated: $(ConvertTo-TexText (Get-Date).ToString('o'))\\"
    '\begin{longtable}{lll}'
    '\textbf{Test case} & \textbf{Description} & \textbf{Result} \\'
    '\hline'
) + $rows + @(
    '\end{longtable}'
    '\end{document}'
)
Set-Content -LiteralPath $texPath -Value $document -Encoding utf8NoBOM

$results | Format-Table TestCase, Description, Status
if ($results.Status -contains 'Fail') {
    Write-WspError "C89 test suite failed on $Architecture. Results: $texPath"
    throw "One or more $Architecture test cases failed. Results: $texPath"
}

Write-WspPass "C89 test suite passed on $Architecture. Results: $texPath"
$texPath
