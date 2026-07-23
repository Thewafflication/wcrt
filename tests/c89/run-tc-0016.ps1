[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'

$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$violations = [System.Collections.Generic.List[string]]::new()
$sourceFiles = Get-ChildItem -LiteralPath @(
    (Join-Path $repoRoot 'include'),
    (Join-Path $repoRoot 'src'),
    (Join-Path $repoRoot 'tests')
) -Recurse -File -ErrorAction SilentlyContinue |
    Where-Object { $_.Extension -in @('.c', '.h') }

foreach ($file in $sourceFiles) {
    $relative = [System.IO.Path]::GetRelativePath(
        $repoRoot, $file.FullName).Replace('\', '/')
    $lines = Get-Content -LiteralPath $file.FullName
    for ($index = 0; $index -lt $lines.Count; $index++) {
        if ($lines[$index].Length -gt 80) {
            $violations.Add(
                "$relative`:$($index + 1): line contains " +
                "$($lines[$index].Length) characters; maximum is 80."
            )
        }
    }

    $content = $lines -join "`n"
    if ($content -notmatch '(?s)/\*\*.*?@file.*?\*/' -and
        $content -notmatch '(?s)/\*!.*?@file.*?\*/') {
        $violations.Add("$relative`: missing Doxygen @file comment.")
    }
}

$doxyfile = Join-Path $repoRoot 'Doxyfile'
$doxygen = Get-Command doxygen.exe -ErrorAction SilentlyContinue |
    Select-Object -First 1 -ExpandProperty Source
if (-not $doxygen) {
    $portableDoxygen = Join-Path $repoRoot `
        'build\tools\doxygen\doxygen.exe'
    if (Test-Path -LiteralPath $portableDoxygen) {
        $doxygen = $portableDoxygen
    }
}
$doxygenVersion = '(not available)'
if (-not (Test-Path -LiteralPath $doxyfile)) {
    $violations.Add('Doxyfile: required configuration file is missing.')
} elseif (-not $doxygen) {
    $violations.Add('Doxygen executable was not found.')
} else {
    $doxygenVersion = (& $doxygen --version 2>&1 |
        Select-Object -First 1).ToString()
    Push-Location $repoRoot
    try {
        $doxygenOutput = & $doxygen $doxyfile 2>&1
        if ($LASTEXITCODE -ne 0) {
            $warningLog = Join-Path $repoRoot `
                'output\doxygen-warnings.log'
            $warningText = if (Test-Path -LiteralPath $warningLog) {
                Get-Content -LiteralPath $warningLog -Raw
            } else {
                ($doxygenOutput | Out-String).Trim()
            }
            $violations.Add("Doxygen failed:`n$warningText")
        }
    } finally {
        Pop-Location
    }
}

$status = if ($violations.Count -eq 0) { 'Pass' } else { 'Fail' }
$output = if ($violations.Count -eq 0) {
    @(
        "Files scanned: $($sourceFiles.Count)"
        'Maximum physical line length: 80'
        'Line-length violations: 0'
        'Missing @file comments: 0'
        "Doxygen executable: $doxygen"
        "Doxygen version: $doxygenVersion"
        'Doxygen warnings-as-errors: Pass'
        'Result: no violations found.'
    ) -join "`n"
} else {
    $violations -join "`n"
}

[PSCustomObject]@{
    TestCase = 'TC-0016'
    Requirement = 'REQ-0016'
    Status = $status
    FilesScanned = $sourceFiles.Count
    ViolationCount = $violations.Count
    Output = $output
}
