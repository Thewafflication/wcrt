[CmdletBinding()]
param(
    [ValidateSet('Msvc')]
    [string]$Target = 'Msvc',

    [string]$OutputPath
)

$ErrorActionPreference = 'Stop'

function ConvertTo-LatexText {
    param([AllowNull()][string]$Value)

    if ($null -eq $Value) {
        return ''
    }

    $escaped = [System.Text.StringBuilder]::new()
    foreach ($character in $Value.ToCharArray()) {
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
    param(
        [AllowNull()][string]$Value,
        [int]$Width = 88
    )

    if ([string]::IsNullOrEmpty($Value)) {
        return '(no compiler output)'
    }

    $lines = [System.Collections.Generic.List[string]]::new()
    foreach ($line in ($Value -split "`r?`n")) {
        $remaining = $line.Replace("`t", '    ')
        while ($remaining.Length -gt $Width) {
            $splitAt = $Width
            for ($index = $Width; $index -gt 0; $index--) {
                if (' /\-_' -like "*$($remaining[$index])*" ) {
                    $splitAt = $index + 1
                    break
                }
            }
            $lines.Add($remaining.Substring(0, $splitAt).TrimEnd())
            $remaining = '  ' + $remaining.Substring($splitAt).TrimStart()
        }
        $lines.Add($remaining)
    }
    return ($lines -join "`n")
}

function Find-PdfLaTeX {
    $command = Get-Command pdflatex.exe -ErrorAction SilentlyContinue |
        Select-Object -First 1 -ExpandProperty Source
    if ($command) {
        return $command
    }

    $userMiKTeX = Join-Path $env:LOCALAPPDATA 'Programs\MiKTeX\miktex\bin\x64\pdflatex.exe'
    if (Test-Path -LiteralPath $userMiKTeX) {
        return $userMiKTeX
    }

    throw 'pdflatex.exe was not found in PATH or the user-level MiKTeX installation.'
}

function Find-MsvcEnvironment {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (-not (Test-Path -LiteralPath $vswhere)) {
        throw "vswhere.exe was not found: $vswhere"
    }

    $installationPath = (& $vswhere -latest -products * `
        -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
        -property installationPath | Select-Object -First 1)
    if (-not $installationPath) {
        throw 'A Visual Studio C/C++ toolchain was not found.'
    }

    $versionFile = Join-Path $installationPath 'VC\Auxiliary\Build\Microsoft.VCToolsVersion.default.txt'
    $toolsVersion = (Get-Content -LiteralPath $versionFile -Raw).Trim()
    $toolsRoot = Join-Path $installationPath "VC\Tools\MSVC\$toolsVersion"
    $compiler = Join-Path $toolsRoot 'bin\Hostx64\x64\cl.exe'
    $compilerInclude = Join-Path $toolsRoot 'include'

    if (-not (Test-Path -LiteralPath $compiler)) {
        throw "MSVC compiler not found: $compiler"
    }

    $windowsKitsRoot = Join-Path ${env:ProgramFiles(x86)} 'Windows Kits\10\Include'
    $sdk = Get-ChildItem -LiteralPath $windowsKitsRoot -Directory |
        Where-Object { Test-Path -LiteralPath (Join-Path $_.FullName 'ucrt') } |
        Sort-Object { [version]$_.Name } -Descending |
        Select-Object -First 1
    if (-not $sdk) {
        throw "A Windows SDK UCRT include directory was not found under $windowsKitsRoot."
    }

    return [PSCustomObject]@{
        Compiler = $compiler
        CompilerVersion = (Get-Item -LiteralPath $compiler).VersionInfo.FileVersion
        ToolsetVersion = $toolsVersion
        UcrtVersion = $sdk.Name
        IncludeDirectories = @(
            $compilerInclude
            (Join-Path $sdk.FullName 'ucrt')
            (Join-Path $sdk.FullName 'shared')
            (Join-Path $sdk.FullName 'um')
        )
    }
}

$repoRoot = Split-Path -Parent $PSScriptRoot
$templatePath = Join-Path $repoRoot 'docs\c89-test-execution-report-template.tex'
$testDirectory = Join-Path $repoRoot 'tests\c89\presence'
$workDirectory = Join-Path $repoRoot 'tmp\pdfs\c89-test-execution-report'
$evidenceDirectory = Join-Path $repoRoot 'output\test-results\c89-mscrt'

if ([string]::IsNullOrWhiteSpace($OutputPath)) {
    $OutputPath = Join-Path $repoRoot 'output\pdf\c89-mscrt-test-execution-report.pdf'
} elseif (-not [System.IO.Path]::IsPathRooted($OutputPath)) {
    $OutputPath = Join-Path $repoRoot $OutputPath
}

$testMap = @(
    @{ Id = '0001'; Name = 'assert'; Header = 'assert.h' },
    @{ Id = '0002'; Name = 'ctype'; Header = 'ctype.h' },
    @{ Id = '0003'; Name = 'errno'; Header = 'errno.h' },
    @{ Id = '0004'; Name = 'float'; Header = 'float.h' },
    @{ Id = '0005'; Name = 'limits'; Header = 'limits.h' },
    @{ Id = '0006'; Name = 'locale'; Header = 'locale.h' },
    @{ Id = '0007'; Name = 'math'; Header = 'math.h' },
    @{ Id = '0008'; Name = 'setjmp'; Header = 'setjmp.h' },
    @{ Id = '0009'; Name = 'signal'; Header = 'signal.h' },
    @{ Id = '0010'; Name = 'stdarg'; Header = 'stdarg.h' },
    @{ Id = '0011'; Name = 'stddef'; Header = 'stddef.h' },
    @{ Id = '0012'; Name = 'stdio'; Header = 'stdio.h' },
    @{ Id = '0013'; Name = 'stdlib'; Header = 'stdlib.h' },
    @{ Id = '0014'; Name = 'string'; Header = 'string.h' },
    @{ Id = '0015'; Name = 'time'; Header = 'time.h' }
)

$msvc = Find-MsvcEnvironment
$pdfLaTeX = Find-PdfLaTeX
$ucrtDll = Join-Path $env:SystemRoot 'System32\ucrtbase.dll'
$ucrtDllVersion = if (Test-Path -LiteralPath $ucrtDll) {
    ((Get-Item -LiteralPath $ucrtDll).VersionInfo.FileVersion -split ' ')[0]
} else {
    'not found'
}

New-Item -ItemType Directory -Force -Path $workDirectory | Out-Null
New-Item -ItemType Directory -Force -Path $evidenceDirectory | Out-Null
New-Item -ItemType Directory -Force -Path (Split-Path -Parent $OutputPath) | Out-Null

$runStarted = Get-Date
$results = [System.Collections.Generic.List[object]]::new()

foreach ($test in $testMap) {
    $source = Join-Path $testDirectory "$($test.Name).c"
    $arguments = [System.Collections.Generic.List[string]]::new()
    foreach ($argument in @('/nologo', '/TC', '/Zs', '/W3', '/WX', '/D_CRT_SECURE_NO_WARNINGS', '/X')) {
        $arguments.Add($argument)
    }
    foreach ($includeDirectory in $msvc.IncludeDirectories) {
        $arguments.Add("/I$includeDirectory")
    }
    $arguments.Add($source)

    $testStarted = Get-Date
    $stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
    $outputLines = & $msvc.Compiler @arguments 2>&1
    $exitCode = $LASTEXITCODE
    $stopwatch.Stop()
    $testFinished = Get-Date
    $output = ($outputLines | ForEach-Object { $_.ToString() }) -join "`n"
    $status = if ($exitCode -eq 0) { 'Pass' } else { 'Fail' }
    $command = "cl $($arguments -join ' ')"

    $evidencePath = Join-Path $evidenceDirectory "tc-$($test.Id)-$($test.Name)-evidence.txt"
    $evidence = @(
        "Test Case: TC-$($test.Id)"
        "Requirement: REQ-$($test.Id)"
        "Status: $status"
        "Started: $($testStarted.ToString('o'))"
        "Finished: $($testFinished.ToString('o'))"
        "DurationSeconds: $($stopwatch.Elapsed.TotalSeconds.ToString('F3'))"
        "ExitCode: $exitCode"
        "Command: $command"
        'CompilerOutput:'
        $output
    ) -join "`r`n"
    Set-Content -LiteralPath $evidencePath -Value $evidence -Encoding UTF8

    $results.Add([PSCustomObject]@{
        TestCase = "TC-$($test.Id)"
        Requirement = "REQ-$($test.Id)"
        Name = $test.Name
        Header = $test.Header
        Source = $source
        Status = $status
        ExitCode = $exitCode
        Started = $testStarted.ToString('o')
        Finished = $testFinished.ToString('o')
        DurationSeconds = [Math]::Round($stopwatch.Elapsed.TotalSeconds, 3)
        Command = $command
        Output = $output
        EvidenceFile = $evidencePath
    })
}

$runFinished = Get-Date
$totalDuration = ($runFinished - $runStarted).TotalSeconds
$passed = @($results | Where-Object Status -eq 'Pass').Count
$failed = @($results | Where-Object Status -eq 'Fail').Count
$overallStatus = if ($failed -eq 0) { 'Pass' } else { 'Fail' }
$runId = 'C89-MSCRT-' + $runStarted.ToString('yyyyMMdd-HHmmss')

$compilerInformation = "Microsoft C/C++ $($msvc.CompilerVersion), toolset $($msvc.ToolsetVersion)"
$libraryInformation = "Microsoft UCRT headers $($msvc.UcrtVersion); ucrtbase.dll $ucrtDllVersion"
$operatingSystem = [System.Environment]::OSVersion.VersionString
$architecture = [System.Runtime.InteropServices.RuntimeInformation]::OSArchitecture.ToString()

$runRecord = [PSCustomObject]@{
    TestRunId = $runId
    Target = $Target
    Started = $runStarted.ToString('o')
    Finished = $runFinished.ToString('o')
    DurationSeconds = [Math]::Round($totalDuration, 3)
    OverallStatus = $overallStatus
    Passed = $passed
    Failed = $failed
    Total = $results.Count
    Compiler = [PSCustomObject]@{
        Path = $msvc.Compiler
        Version = $msvc.CompilerVersion
        ToolsetVersion = $msvc.ToolsetVersion
    }
    LibraryUnderTest = [PSCustomObject]@{
        Name = 'Microsoft Universal C Runtime headers'
        UcrtVersion = $msvc.UcrtVersion
        RuntimeDll = $ucrtDll
        RuntimeDllVersion = $ucrtDllVersion
        IncludeDirectories = $msvc.IncludeDirectories
        Scope = 'Compile-only public API presence; runtime DLL functionality was not executed.'
    }
    OperatingSystem = $operatingSystem
    Architecture = $architecture
    Results = $results
}

$jsonPath = Join-Path $evidenceDirectory 'test-run.json'
$runRecord | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $jsonPath -Encoding UTF8

$summaryRows = [System.Collections.Generic.List[string]]::new()
$detailSections = [System.Collections.Generic.List[string]]::new()
foreach ($result in $results) {
    $statusLatex = if ($result.Status -eq 'Pass') { '\PassResult' } else { '\FailResult' }
    $summaryRows.Add("$($result.TestCase) & $($result.Requirement) & \texttt{$($result.Header)} & $statusLatex & $($result.DurationSeconds.ToString('F3')) \\")

    $detailSections.Add("\section{$($result.TestCase) - \texttt{$($result.Header)}}")
    $detailSections.Add('\begin{description}')
    $detailSections.Add("\item[Requirement] $($result.Requirement)")
    $detailSections.Add("\item[Test Case Specification] \texttt{docs/tc-$($result.TestCase.Substring(3))-$($result.Name).tex}")
    $detailSections.Add("\item[Test Source] \texttt{tests/c89/presence/$($result.Name).c}")
    $detailSections.Add("\item[Result] $statusLatex")
    $detailSections.Add("\item[Exit Code] $($result.ExitCode)")
    $detailSections.Add("\item[Started] $(ConvertTo-LatexText $result.Started)")
    $detailSections.Add("\item[Finished] $(ConvertTo-LatexText $result.Finished)")
    $detailSections.Add("\item[Duration] $($result.DurationSeconds.ToString('F3')) seconds")
    $detailSections.Add("\item[Evidence File] \texttt{output/test-results/c89-mscrt/tc-$($result.TestCase.Substring(3))-$($result.Name)-evidence.txt}")
    $detailSections.Add('\end{description}')
    $detailSections.Add('\subsection*{Command}')
    $detailSections.Add('\begin{verbatim}')
    $detailSections.Add((Format-EvidenceText $result.Command))
    $detailSections.Add('\end{verbatim}')
    $detailSections.Add('\subsection*{Compiler output}')
    $detailSections.Add('\begin{verbatim}')
    $detailSections.Add((Format-EvidenceText $result.Output))
    $detailSections.Add('\end{verbatim}')
    $detailSections.Add('\clearpage')
}

$reportSource = Get-Content -LiteralPath $templatePath -Raw
$replacements = @{
    '@@REPORT_DATE@@' = ConvertTo-LatexText $runStarted.ToString('MMMM d, yyyy')
    '@@TEST_RUN_ID@@' = ConvertTo-LatexText $runId
    '@@OVERALL_RESULT@@' = if ($overallStatus -eq 'Pass') { '\PassResult' } else { '\FailResult' }
    '@@PASSED_COUNT@@' = [string]$passed
    '@@FAILED_COUNT@@' = [string]$failed
    '@@TOTAL_COUNT@@' = [string]$results.Count
    '@@TEST_DATE@@' = ConvertTo-LatexText $runStarted.ToString('yyyy-MM-dd HH:mm:ss zzz')
    '@@TOTAL_DURATION@@' = $totalDuration.ToString('F3')
    '@@COMPILER_INFORMATION@@' = ConvertTo-LatexText $compilerInformation
    '@@LIBRARY_INFORMATION@@' = ConvertTo-LatexText $libraryInformation
    '@@OPERATING_SYSTEM@@' = ConvertTo-LatexText $operatingSystem
    '@@ARCHITECTURE@@' = ConvertTo-LatexText $architecture
    '@@SUMMARY_ROWS@@' = $summaryRows -join "`r`n"
    '@@DETAIL_SECTIONS@@' = $detailSections -join "`r`n"
}
foreach ($entry in $replacements.GetEnumerator()) {
    $reportSource = $reportSource.Replace($entry.Key, $entry.Value)
}

$generatedTex = Join-Path $workDirectory 'c89-mscrt-test-execution-report.tex'
Set-Content -LiteralPath $generatedTex -Value $reportSource -Encoding UTF8

$latexArguments = @(
    '-interaction=nonstopmode'
    '-halt-on-error'
    '-file-line-error'
    "-output-directory=$workDirectory"
    '-job-name=c89-mscrt-test-execution-report'
    $generatedTex
)

foreach ($pass in 1..2) {
    & $pdfLaTeX @latexArguments
    if ($LASTEXITCODE -ne 0) {
        throw "pdflatex failed on pass $pass with exit code $LASTEXITCODE."
    }
}

$generatedPdf = Join-Path $workDirectory 'c89-mscrt-test-execution-report.pdf'
Copy-Item -LiteralPath $generatedPdf -Destination $OutputPath -Force

Write-Host "Test run: $runId"
Write-Host "Result: $overallStatus ($passed passed, $failed failed)"
Write-Host "Evidence: $evidenceDirectory"
Write-Host "Report: $OutputPath"
