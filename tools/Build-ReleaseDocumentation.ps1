[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Version,
    [string]$SourceRevision = '',
    [string]$EvidenceRoot = 'output/test-results',
    [string]$OutputDirectory = 'output/release-documentation',
    [string]$Doxygen = 'doxygen.exe',
    [string]$PdfLaTeX = 'pdflatex.exe',
    [string]$MakeIndex = 'makeindex.exe',
    [switch]$AllowIncompleteEvidence
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot

function Resolve-Tool([string]$Value) {
    if (Test-Path -LiteralPath $Value -PathType Leaf) {
        return (Resolve-Path -LiteralPath $Value).Path
    }
    $command = Get-Command $Value -ErrorAction SilentlyContinue
    if (-not $command) { throw "Required tool was not found: $Value" }
    $command.Source
}

function ConvertTo-LaTeX([AllowNull()][object]$Value) {
    if ($null -eq $Value) { return '' }
    $builder = [Text.StringBuilder]::new()
    foreach ($character in $Value.ToString().ToCharArray()) {
        $escaped = switch ($character) {
            '\' { '\textbackslash{}' }
            '&' { '\&' }
            '%' { '\%' }
            '$' { '\$' }
            '#' { '\#' }
            '_' { '\_' }
            '{' { '\{' }
            '}' { '\}' }
            '~' { '\textasciitilde{}' }
            '^' { '\textasciicircum{}' }
            default { $character }
        }
        [void]$builder.Append($escaped)
    }
    $builder.ToString()
}

function Read-Json([string]$Path, [switch]$Optional) {
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) {
        if ($Optional -or $AllowIncompleteEvidence) { return $null }
        throw "Required release evidence is missing: $Path"
    }
    Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json
}

function Invoke-Checked([string]$FilePath, [string[]]$Arguments) {
    $output = @(& $FilePath @Arguments 2>&1)
    if ($LASTEXITCODE -ne 0) {
        $output | Write-Output
        throw "$FilePath failed with exit code $LASTEXITCODE."
    }
}

$doxygenPath = Resolve-Tool $Doxygen
$pdfLaTeXPath = Resolve-Tool $PdfLaTeX
$makeIndexPath = Resolve-Tool $MakeIndex
if ([string]::IsNullOrWhiteSpace($SourceRevision)) {
    $SourceRevision = (git -C $repoRoot rev-parse HEAD).Trim()
}
$versionLabel = $Version.TrimStart('v')
$outputRoot = [IO.Path]::GetFullPath((Join-Path $repoRoot $OutputDirectory))
$workRoot = Join-Path $outputRoot 'work'
New-Item -ItemType Directory -Force $outputRoot | Out-Null
if (Test-Path -LiteralPath $workRoot) {
    Remove-Item -LiteralPath $workRoot -Recurse -Force
}
New-Item -ItemType Directory -Force $workRoot | Out-Null

$template = Get-Content (Join-Path $repoRoot 'docs/release-test-execution-report-template.tex') -Raw
$pdfs = @()
foreach ($architecture in @('x86', 'x64', 'arm64')) {
    $evidence = [IO.Path]::GetFullPath((Join-Path $repoRoot (Join-Path $EvidenceRoot $architecture)))
    $c89 = @(Read-Json (Join-Path $evidence 'c89-test-results.json') |
        Where-Object { $null -ne $_ })
    $extensions = @(Read-Json (Join-Path $evidence 'extension-test-results.json') |
        Where-Object { $null -ne $_ })
    $capabilities = Read-Json (Join-Path $evidence 'tinycc-c99-capabilities.json')
    $diagnostics = Read-Json (Join-Path $evidence 'native-test-diagnostics.json')
    $startup = @(Read-Json (Join-Path $evidence 'startup-test-results.json') |
        Where-Object { $null -ne $_ })
    $provenance = Read-Json (Join-Path $evidence 'dependency-provenance.json')
    $imports = if ($architecture -eq 'x86') {
        Read-Json (Join-Path $evidence 'windows-2000-imports.json')
    } else { $null }

    $detail = @()
    foreach ($item in $c89) {
        $number = [int]($item.TestCase -replace '^TC-', '')
        $detail += [PSCustomObject]@{
            TestCase = $item.TestCase
            Requirement = ('REQ-{0:D4}' -f $number)
            Suite = 'C89'
            Description = $item.Description
            Status = $item.Status
        }
    }
    $detail += $extensions
    $summaryRows = @()
    foreach ($suite in @('C89', 'C99', 'Microsoft', 'POSIX', 'WCRT')) {
        $rows = @($detail | Where-Object Suite -eq $suite)
        if ($rows.Count -eq 0) { continue }
        $passed = @($rows | Where-Object Status -eq 'Pass').Count
        $other = $rows.Count - $passed
        $summaryRows += "$(ConvertTo-LaTeX $suite) & $($rows.Count) & $passed & $other\\"
    }
    $failed = @($detail | Where-Object Status -notin @('Pass', 'ExpectedFail'))
    $expected = @($detail | Where-Object Status -eq 'ExpectedFail')
    $overall = if ($failed.Count -eq 0) { 'PASS' } else { 'FAIL' }
    $color = if ($failed.Count -eq 0) { 'passgreen' } else { 'failred' }
    $qualification = if ($expected.Count) {
        " The inventory includes $($expected.Count) controlled expected-failure result(s)."
    } else { '' }
    $detailRows = foreach ($item in ($detail | Sort-Object TestCase)) {
        $status = ConvertTo-LaTeX $item.Status
        "$(ConvertTo-LaTeX $item.TestCase) & $(ConvertTo-LaTeX $item.Requirement) & $(ConvertTo-LaTeX $item.Suite) & $(ConvertTo-LaTeX $item.Description) & $status\\"
    }
    $compiler = if ($capabilities) { $capabilities.Compiler } else { 'Evidence unavailable' }
    $dataModel = if ($capabilities) { "$($capabilities.DataModel.Model) ($($capabilities.DataModel.Status))" } else { 'Evidence unavailable' }
    $dependencyStatus = if ($provenance) { "TinyCC $($provenance.TinyCcVersion); retained" } else { 'Evidence unavailable' }
    $diagnosticStatus = if ($diagnostics) { $diagnostics.Status } else { 'Evidence unavailable' }
    $startupStatus = if ($startup.Count) {
        if (@($startup | Where-Object Status -ne 'Pass').Count) { 'Fail' } else { 'Pass' }
    } else { 'Evidence unavailable' }
    $importStatus = if ($architecture -ne 'x86') { 'Not applicable' } elseif ($imports) { $imports.Status } else { 'Evidence unavailable' }

    $tex = $template
    $replacements = [ordered]@{
        '@@ARCHITECTURE@@' = $architecture.ToUpperInvariant()
        '@@VERSION@@' = ConvertTo-LaTeX $versionLabel
        '@@SOURCE_REVISION@@' = ConvertTo-LaTeX $SourceRevision
        '@@GENERATED_AT@@' = ConvertTo-LaTeX ([DateTime]::UtcNow.ToString('yyyy-MM-dd HH:mm:ss'))
        '@@SUMMARY_ROWS@@' = $summaryRows -join "`r`n"
        '@@OVERALL_COLOR@@' = $color
        '@@OVERALL_STATUS@@' = $overall
        '@@QUALIFICATION@@' = ConvertTo-LaTeX $qualification
        '@@COMPILER@@' = ConvertTo-LaTeX $compiler
        '@@DATA_MODEL@@' = ConvertTo-LaTeX $dataModel
        '@@DEPENDENCY_STATUS@@' = ConvertTo-LaTeX $dependencyStatus
        '@@DIAGNOSTICS_STATUS@@' = ConvertTo-LaTeX $diagnosticStatus
        '@@STARTUP_STATUS@@' = ConvertTo-LaTeX $startupStatus
        '@@IMPORT_STATUS@@' = ConvertTo-LaTeX $importStatus
        '@@DETAIL_ROWS@@' = $detailRows -join "`r`n"
    }
    foreach ($entry in $replacements.GetEnumerator()) { $tex = $tex.Replace($entry.Key, $entry.Value) }
    $archWork = Join-Path $workRoot "test-$architecture"
    New-Item -ItemType Directory -Force $archWork | Out-Null
    $texPath = Join-Path $archWork 'report.tex'
    Set-Content -LiteralPath $texPath -Value $tex -Encoding utf8NoBOM
    for ($pass = 1; $pass -le 2; $pass++) {
        Invoke-Checked $pdfLaTeXPath @('-halt-on-error', '-interaction=nonstopmode', '-output-directory', $archWork, $texPath)
    }
    $pdf = Join-Path $outputRoot "wcrt-test-execution-$versionLabel-$architecture.pdf"
    Copy-Item (Join-Path $archWork 'report.pdf') $pdf -Force
    $pdfs += $pdf
}

$doxygenRoot = Join-Path $workRoot 'doxygen'
$config = Join-Path $workRoot 'Doxyfile.release'
$doxygenRootConfig = $doxygenRoot.Replace('\', '/')
@"
@INCLUDE = $($repoRoot.Replace('\', '/'))/Doxyfile
PROJECT_NUMBER = $versionLabel
OUTPUT_DIRECTORY = $doxygenRootConfig
INPUT = $($repoRoot.Replace('\', '/'))/README.md $($repoRoot.Replace('\', '/'))/include
FILE_PATTERNS = *.h *.md
USE_MDFILE_AS_MAINPAGE = $($repoRoot.Replace('\', '/'))/README.md
EXTRACT_PRIVATE = NO
EXTRACT_STATIC = NO
INTERNAL_DOCS = NO
SOURCE_BROWSER = NO
REFERENCED_BY_RELATION = NO
REFERENCES_RELATION = NO
GENERATE_HTML = YES
HTML_OUTPUT = html
GENERATE_LATEX = YES
LATEX_OUTPUT = latex
GENERATE_XML = NO
GENERATE_TREEVIEW = YES
SEARCHENGINE = YES
WARN_LOGFILE = $doxygenRootConfig/warnings.log
WARN_AS_ERROR = NO
"@ | Set-Content -LiteralPath $config -Encoding utf8NoBOM
Invoke-Checked $doxygenPath @($config)

$latexDirectory = Join-Path $doxygenRoot 'latex'
$htmlDirectory = Join-Path $doxygenRoot 'html'
$latexSourceZip = Join-Path $workRoot 'latex-source.zip'
Compress-Archive -Path (Join-Path $latexDirectory '*') -DestinationPath $latexSourceZip -Force
Push-Location $latexDirectory
try {
    Invoke-Checked $pdfLaTeXPath @('-halt-on-error', '-interaction=nonstopmode', 'refman.tex')
    if (Test-Path 'refman.idx') { Invoke-Checked $makeIndexPath @('refman.idx') }
    Invoke-Checked $pdfLaTeXPath @('-halt-on-error', '-interaction=nonstopmode', 'refman.tex')
    Invoke-Checked $pdfLaTeXPath @('-halt-on-error', '-interaction=nonstopmode', 'refman.tex')
} finally { Pop-Location }
$apiPdf = Join-Path $outputRoot "wcrt-api-reference-$versionLabel.pdf"
Copy-Item (Join-Path $latexDirectory 'refman.pdf') $apiPdf -Force
$pdfs += $apiPdf

$bundleRoot = Join-Path $workRoot 'bundle'
New-Item -ItemType Directory -Force (Join-Path $bundleRoot 'pdf'), `
    (Join-Path $bundleRoot 'html'), (Join-Path $bundleRoot 'latex') | Out-Null
Copy-Item $pdfs (Join-Path $bundleRoot 'pdf')
Copy-Item (Join-Path $htmlDirectory '*') (Join-Path $bundleRoot 'html') -Recurse
Expand-Archive -LiteralPath $latexSourceZip -DestinationPath (Join-Path $bundleRoot 'latex')
Copy-Item (Join-Path $repoRoot 'README.md') $bundleRoot
$bundle = Join-Path $outputRoot "wcrt-documentation-$versionLabel.zip"
Compress-Archive -Path (Join-Path $bundleRoot '*') -DestinationPath $bundle -Force

$assets = @($pdfs) + $bundle
if ($pdfs.Count -ne 4) { throw "Expected four PDFs, generated $($pdfs.Count)." }
foreach ($asset in $assets) {
    if (-not (Test-Path $asset -PathType Leaf) -or (Get-Item $asset).Length -eq 0) {
        throw "Documentation asset is missing or empty: $asset"
    }
}
[PSCustomObject]@{ Pdfs = $pdfs; Bundle = $bundle; Html = $htmlDirectory }
