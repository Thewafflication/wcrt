[CmdletBinding()]
param(
    [string]$OutputPath
)

$ErrorActionPreference = 'Stop'

$repoRoot = Split-Path -Parent $PSScriptRoot
$docsDirectory = Join-Path $repoRoot 'docs'
$sourceFile = Join-Path $docsDirectory 'c89-test-report.tex'
$temporaryDirectory = Join-Path $repoRoot 'tmp\pdfs\c89-test-report'
$comparisonRunner = Join-Path $repoRoot 'tools\run-c89-comparison.ps1'

if ([string]::IsNullOrWhiteSpace($OutputPath)) {
    $OutputPath = Join-Path $repoRoot 'output\pdf\c89-api-presence-test-specification.pdf'
} elseif (-not [System.IO.Path]::IsPathRooted($OutputPath)) {
    $OutputPath = Join-Path $repoRoot $OutputPath
}

$pdfLaTeX = Get-Command pdflatex.exe -ErrorAction SilentlyContinue |
    Select-Object -First 1 -ExpandProperty Source

if (-not $pdfLaTeX) {
    $userMiKTeX = Join-Path $env:LOCALAPPDATA 'Programs\MiKTeX\miktex\bin\x64\pdflatex.exe'
    if (Test-Path -LiteralPath $userMiKTeX) {
        $pdfLaTeX = $userMiKTeX
    }
}

if (-not $pdfLaTeX) {
    throw 'pdflatex.exe was not found in PATH or the user-level MiKTeX installation.'
}

if (-not (Test-Path -LiteralPath $sourceFile)) {
    throw "Report source not found: $sourceFile"
}

New-Item -ItemType Directory -Force -Path $temporaryDirectory | Out-Null
New-Item -ItemType Directory -Force -Path (Split-Path -Parent $OutputPath) | Out-Null

if (-not (Test-Path -LiteralPath $comparisonRunner)) {
    throw "Comparison runner not found: $comparisonRunner"
}

function ConvertTo-LaTeXText {
    param([AllowNull()][object]$Value)

    if ($null -eq $Value) {
        return ''
    }
    $escaped = [System.Text.StringBuilder]::new()
    foreach ($character in ([string]$Value).ToCharArray()) {
        $null = switch ($character) {
            '\' { $escaped.Append('\textbackslash{}'); break }
            '{' { $escaped.Append('\{'); break }
            '}' { $escaped.Append('\}'); break }
            '&' { $escaped.Append('\&'); break }
            '%' { $escaped.Append('\%'); break }
            '$' { $escaped.Append('\$'); break }
            '#' { $escaped.Append('\#'); break }
            '_' { $escaped.Append('\_'); break }
            '~' { $escaped.Append('\textasciitilde{}'); break }
            '^' { $escaped.Append('\textasciicircum{}'); break }
            default { $escaped.Append($character) }
        }
    }
    return $escaped.ToString()
}

function Format-EvidenceText {
    param([AllowNull()][string]$Value, [int]$Width = 78)

    if ([string]::IsNullOrWhiteSpace($Value)) {
        return '(no output)'
    }
    $lines = [System.Collections.Generic.List[string]]::new()
    foreach ($line in ($Value -split "`r?`n")) {
        $remaining = $line.Replace("`t", '    ')
        while ($remaining.Length -gt $Width) {
            $splitAt = $Width
            for ($index = $Width; $index -gt 0; $index--) {
                if ($remaining[$index] -in @(' ', '/', '\', '-', '_')) {
                    $splitAt = $index + 1
                    break
                }
            }
            $lines.Add($remaining.Substring(0, $splitAt).TrimEnd())
            $remaining = '  ' + $remaining.Substring($splitAt).TrimStart()
        }
        $lines.Add($remaining)
    }
    return $lines -join "`n"
}

function Get-StatusCommand([string]$Status) {
    switch ($Status) {
        'Pass' { '\TestPass' }
        'Fail' { '\TestFail' }
        'NotImplemented' { '\TestNotImplemented' }
        'NotApplicable' { '\TestNotApplicable' }
        default { '\TestNotRun' }
    }
}

$jsonPath = (& $comparisonRunner | Select-Object -Last 1)
if (-not (Test-Path -LiteralPath $jsonPath)) {
    throw "Comparison result was not generated: $jsonPath"
}
$run = Get-Content -LiteralPath $jsonPath -Raw | ConvertFrom-Json

$summaryRows = [System.Collections.Generic.List[string]]::new()
foreach ($id in 1..16) {
    $number = $id.ToString('0000')
    $wcrt = $run.Results | Where-Object {
        $_.Id -eq $number -and $_.TargetKey -eq 'wcrt'
    }
    $ucrt = $run.Results | Where-Object {
        $_.Id -eq $number -and $_.TargetKey -eq 'mscrt'
    }
    $scope = ConvertTo-LaTeXText $wcrt.Scope
    $summaryRows.Add(
        "\hyperlink{tc-$number}{TC-$number} & REQ-$number & " +
        "$scope & $(Get-StatusCommand $wcrt.Status) & " +
        "$(Get-StatusCommand $ucrt.Status) \\"
    )

    $sections = [System.Collections.Generic.List[string]]::new()
    $sections.Add("\subsection*{TC-$number comparison result}")
    if ($id -le 15) {
        $wcrtHash = if ($wcrt.Output -match `
            'Test source SHA-256: ([A-F0-9]+)') { $Matches[1] }
        $ucrtHash = if ($ucrt.Output -match `
            'Test source SHA-256: ([A-F0-9]+)') { $Matches[1] }
        if ($wcrtHash -and $wcrtHash -eq $ucrtHash) {
            $sections.Add('\noindent\textbf{Comparison Input:} \TestIdentical')
            $sections.Add('\par\smallskip')
        }
    }
    foreach ($result in @($wcrt, $ucrt)) {
        $sections.Add("\subsubsection*{$($result.Target)}")
        $sections.Add('\begin{description}')
        $sections.Add("\item[Result] $(Get-StatusCommand $result.Status)")
        $sections.Add("\item[Duration] $($result.DurationSeconds) seconds")
        $exitText = if ($null -eq $result.ExitCode) {
            'N/A'
        } else {
            [string]$result.ExitCode
        }
        $sections.Add("\item[Exit Code] \texttt{$exitText}")
        $relativeEvidence = [System.IO.Path]::GetRelativePath(
            $repoRoot, $result.EvidenceFile).Replace('\', '/')
        $sections.Add(
            "\item[Evidence] \path{$relativeEvidence}"
        )
        $sections.Add('\end{description}')
        $sections.Add('\paragraph{Output}')
        $sections.Add('\begin{verbatim}')
        $sections.Add((Format-EvidenceText $result.Output))
        $sections.Add('\end{verbatim}')
    }
    $fragmentPath = Join-Path $temporaryDirectory `
        "tc-$number-results.tex"
    Set-Content -LiteralPath $fragmentPath -Value ($sections -join "`n") `
        -Encoding UTF8
}

$summary = @"
\begin{description}
\item[Test Date] $(ConvertTo-LaTeXText ([datetime]$run.Started).ToString('yyyy-MM-dd HH:mm:ss zzz'))
\item[Total Duration] $($run.DurationSeconds) seconds
\item[WCRT Compiler] $(ConvertTo-LaTeXText $run.WcrtCompiler)
\item[MS UCRT Compiler] $(ConvertTo-LaTeXText $run.UcrtCompiler)
\item[MS UCRT Version] $(ConvertTo-LaTeXText $run.UcrtVersion)
\end{description}
\begin{center}
\small
\begin{tabular}{lllll}
\textbf{Test Case} & \textbf{Requirement} & \textbf{Scope} &
\textbf{WCRT} & \textbf{MS UCRT} \\
\hline
$($summaryRows -join "`n")
\end{tabular}
\end{center}
"@
Set-Content -LiteralPath (Join-Path $temporaryDirectory 'test-summary.tex') `
    -Value $summary -Encoding UTF8

$arguments = @(
    '-interaction=nonstopmode'
    '-halt-on-error'
    '-file-line-error'
    "-output-directory=$temporaryDirectory"
    "-include-directory=$temporaryDirectory"
    '-job-name=c89-api-presence-test-specification'
    (Split-Path -Leaf $sourceFile)
)

Push-Location $docsDirectory
try {
    foreach ($pass in 1..2) {
        & $pdfLaTeX @arguments
        if ($LASTEXITCODE -ne 0) {
            throw "pdflatex failed on pass $pass with exit code $LASTEXITCODE."
        }
    }
} finally {
    Pop-Location
}

$generatedPdf = Join-Path $temporaryDirectory 'c89-api-presence-test-specification.pdf'
if (-not (Test-Path -LiteralPath $generatedPdf)) {
    throw "Expected PDF was not generated: $generatedPdf"
}

Copy-Item -LiteralPath $generatedPdf -Destination $OutputPath -Force
Write-Host "Generated test report: $OutputPath"
