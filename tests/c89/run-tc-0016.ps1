[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$sourcePaths = @('include', 'src', 'tests')
$sourceFiles = @($sourcePaths | ForEach-Object {
    Get-ChildItem -LiteralPath (Join-Path $repoRoot $_) -Recurse -File
} | Where-Object { $_.Extension -in '.c', '.h' })

$doxygen = Get-Command doxygen.exe -ErrorAction SilentlyContinue |
    Select-Object -First 1 -ExpandProperty Source
if (-not $doxygen) {
    $portableDoxygen = Join-Path $repoRoot 'build\tools\doxygen\doxygen.exe'
    if (Test-Path -LiteralPath $portableDoxygen) {
        $doxygen = $portableDoxygen
    }
}

$doxygenVersion = '(not available)'
$toolOutput = @()
$exitCode = 1
if (-not $doxygen) {
    $toolOutput = @('Doxygen executable was not found.')
} else {
    $doxygenVersion = (& $doxygen --version 2>&1 |
        Select-Object -First 1).ToString()
    $tool = Join-Path $repoRoot 'wsp/tools/Test-CSourceQuality.ps1'
    $powerShell = (Get-Process -Id $PID).Path
    $escapedTool = $tool.Replace("'", "''")
    $escapedRoot = $repoRoot.Replace("'", "''")
    $escapedDoxygen = $doxygen.Replace("'", "''")
    $command = "& '$escapedTool' -RepositoryRoot '$escapedRoot' " +
        "-SourcePath @('include', 'src', 'tests') " +
        "-Doxyfile 'Doxyfile' -Doxygen '$escapedDoxygen'"
    $encodedCommand = [Convert]::ToBase64String(
        [Text.Encoding]::Unicode.GetBytes($command))
    $arguments = @(
        '-NoProfile'
        '-EncodedCommand'
        $encodedCommand
    )
    $toolOutput = @(& $powerShell @arguments 2>&1)
    $exitCode = $LASTEXITCODE
}

$status = if ($exitCode -eq 0) { 'Pass' } else { 'Fail' }
$output = @(
    "Files scanned: $($sourceFiles.Count)"
    "Doxygen executable: $doxygen"
    "Doxygen version: $doxygenVersion"
    $toolOutput | ForEach-Object { $_.ToString() }
) -join "`n"

[PSCustomObject]@{
    TestCase = 'TC-0016'
    Requirement = 'REQ-0016'
    Status = $status
    FilesScanned = $sourceFiles.Count
    ViolationCount = if ($status -eq 'Pass') { 0 } else { 1 }
    Output = $output
}
