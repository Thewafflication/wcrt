[CmdletBinding()]
param(
    [string]$OutputPath
)

$ErrorActionPreference = 'Stop'

$repoRoot = Split-Path -Parent $PSScriptRoot
$docsDirectory = Join-Path $repoRoot 'docs'
$sourceFile = Join-Path $docsDirectory 'c89-test-report.tex'
$temporaryDirectory = Join-Path $repoRoot 'tmp\pdfs\c89-test-report'

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

$arguments = @(
    '-interaction=nonstopmode'
    '-halt-on-error'
    '-file-line-error'
    "-output-directory=$temporaryDirectory"
    '-job-name=c89-api-presence-test-specification'
    (Split-Path -Leaf $sourceFile)
)

Push-Location $docsDirectory
try {
    & $pdfLaTeX @arguments
    if ($LASTEXITCODE -ne 0) {
        throw "pdflatex failed with exit code $LASTEXITCODE."
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
