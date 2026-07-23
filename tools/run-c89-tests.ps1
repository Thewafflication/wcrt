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
    param([string]$Id)
    $runner = Join-Path $repoRoot "tests/c89/run-tc-$Id.ps1"
    $started = Get-Date
    $status = 'Pass'
    $details = ''
    try {
        if ($Id -eq '0016') {
            $result = & $runner
            if ($result.Status -ne 'Pass') {
                throw $result.Output
            }
        } else {
            $result = & $runner -TinyCc $TinyCc
        }
        $details = ($result | Format-List | Out-String).Trim()
    } catch {
        $status = 'Fail'
        $details = $_.Exception.Message
    }
    [PSCustomObject]@{
        TestCase = "TC-$Id"
        Status = $status
        Started = $started.ToString('o')
        Finished = (Get-Date).ToString('o')
        Details = $details
    }
}

# The source/documentation gate deliberately precedes every compiled test.
$ids = @('0016') + (1..15 | ForEach-Object { $_.ToString('0000') })
$results = @($ids | ForEach-Object { Invoke-TestCase $_ })
$results | ConvertTo-Json -Depth 4 |
    Set-Content -LiteralPath $jsonPath -Encoding utf8NoBOM

$rows = foreach ($result in $results) {
    "$(ConvertTo-TexText $result.TestCase) & $(ConvertTo-TexText $result.Status) \\"
}
$document = @(
    '\documentclass{article}'
    '\usepackage[margin=1in]{geometry}'
    '\usepackage{longtable}'
    '\begin{document}'
    "\section*{WCRT C89 Test Results: $(ConvertTo-TexText $Architecture)}"
    "Generated: $(ConvertTo-TexText (Get-Date).ToString('o'))\\"
    '\begin{longtable}{ll}'
    '\textbf{Test case} & \textbf{Result} \\'
    '\hline'
) + $rows + @(
    '\end{longtable}'
    '\end{document}'
)
Set-Content -LiteralPath $texPath -Value $document -Encoding utf8NoBOM

$results | Format-Table TestCase, Status
if ($results.Status -contains 'Fail') {
    throw "One or more $Architecture test cases failed. Results: $texPath"
}

$texPath
