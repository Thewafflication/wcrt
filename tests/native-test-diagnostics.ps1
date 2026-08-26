function Get-WcrtDiagnosticTinyCc {
    param(
        [Parameter(Mandatory)][string]$TinyCc,
        [Parameter(Mandatory)][string]$RepositoryRoot
    )

    $resolved = (Resolve-Path -LiteralPath $TinyCc).Path
    $wrapper = Join-Path $RepositoryRoot 'tests/tcc-diagnostic-wrapper.cmd'
    if (-not (Test-Path -LiteralPath $wrapper -PathType Leaf)) {
        throw "The TinyCC diagnostic wrapper was not found: $wrapper"
    }
    $env:WCRT_TEST_TINYCC = $resolved
    $wrapper
}

function Invoke-WcrtDebuggerProcess {
    param(
        [Parameter(Mandatory)][string]$Debugger,
        [Parameter(Mandatory)][string]$Executable,
        [int]$TimeoutSeconds = 30
    )

    $startInfo = [Diagnostics.ProcessStartInfo]::new()
    $startInfo.FileName = $Debugger
    $startInfo.UseShellExecute = $false
    $startInfo.RedirectStandardOutput = $true
    $startInfo.RedirectStandardError = $true
    foreach ($argument in @(
        '--nx', '--quiet', '--batch',
        '-ex', 'set pagination off',
        '-ex', 'set confirm off',
        '-ex', 'run',
        '-ex', 'bt',
        '--args', $Executable
    )) {
        $startInfo.ArgumentList.Add($argument)
    }

    $process = [Diagnostics.Process]::new()
    $process.StartInfo = $startInfo
    if (-not $process.Start()) {
        throw "The debugger process could not be started: $Debugger"
    }
    $standardOutput = $process.StandardOutput.ReadToEndAsync()
    $standardError = $process.StandardError.ReadToEndAsync()
    if (-not $process.WaitForExit($TimeoutSeconds * 1000)) {
        $process.Kill($true)
        $process.WaitForExit()
        return [PSCustomObject]@{
            Status = 'TimedOut'
            ExitCode = $null
            Output = "GDB exceeded the $TimeoutSeconds-second limit."
        }
    }
    [PSCustomObject]@{
        Status = 'Completed'
        ExitCode = $process.ExitCode
        Output = @($standardOutput.Result, $standardError.Result).
            Where({ -not [string]::IsNullOrWhiteSpace($_) }) -join "`n"
    }
}

function Invoke-WcrtFailureDebugger {
    param(
        [Parameter(Mandatory)][string]$TestCase,
        [Parameter(Mandatory)][string]$Architecture,
        [Parameter(Mandatory)][string]$RepositoryRoot,
        [Parameter(Mandatory)][string]$BuildDirectory,
        [string]$TranscriptPath
    )

    $diagnosticDirectory = Join-Path $RepositoryRoot (
        "output/test-results/$Architecture/diagnostics/" +
        $TestCase.ToLowerInvariant())
    New-Item -ItemType Directory -Force $diagnosticDirectory | Out-Null
    $retainedTranscript = $null
    if ($TranscriptPath -and
        (Test-Path -LiteralPath $TranscriptPath -PathType Leaf)) {
        $retainedTranscript = Join-Path $diagnosticDirectory `
            'test-transcript.txt'
        if ([IO.Path]::GetFullPath($TranscriptPath) -ne
            [IO.Path]::GetFullPath($retainedTranscript)) {
            Copy-Item -LiteralPath $TranscriptPath `
                -Destination $retainedTranscript -Force
        }
    }
    $executable = Get-ChildItem -LiteralPath $BuildDirectory -Filter '*.exe' `
        -File -Recurse -ErrorAction SilentlyContinue |
        Sort-Object LastWriteTimeUtc -Descending | Select-Object -First 1
    if ($null -eq $executable) {
        $record = [PSCustomObject]@{
            TestCase = $TestCase
            Architecture = $Architecture
            Status = 'ExecutableUnavailable'
            Executable = $null
            ExecutableSha256 = $null
            Debugger = $null
            DebuggerVersion = $null
            DebuggerSha256 = $null
            DebuggerStatus = 'NotRun'
            DebuggerExitCode = $null
            DebuggerOutput = 'No test executable was available to debug.'
            Transcript = $retainedTranscript
        }
    } else {
        $retainedExecutable = Join-Path $diagnosticDirectory `
            $executable.Name
        Copy-Item -LiteralPath $executable.FullName `
            -Destination $retainedExecutable -Force
        $pdb = [IO.Path]::ChangeExtension($executable.FullName, '.pdb')
        if (Test-Path -LiteralPath $pdb -PathType Leaf) {
            Copy-Item -LiteralPath $pdb -Destination $diagnosticDirectory `
                -Force
        }
        $gdb = Get-Command gdb.exe -ErrorAction SilentlyContinue |
            Select-Object -First 1 -ExpandProperty Source
        if (-not $gdb) {
            $gdb = Get-Command gdb -ErrorAction SilentlyContinue |
                Select-Object -First 1 -ExpandProperty Source
        }
        if (-not $gdb) {
            $projectTools = Join-Path $RepositoryRoot 'output/tools'
            if (Test-Path -LiteralPath $projectTools -PathType Container) {
                $gdb = Get-ChildItem -LiteralPath $projectTools `
                    -Filter gdb.exe -File -Recurse -ErrorAction SilentlyContinue |
                    Select-Object -First 1 -ExpandProperty FullName
            }
        }
        if ($gdb) {
            try {
                $debuggerVersion = (& $gdb --version 2>&1 |
                    Select-Object -First 1).ToString()
                $debuggerSha256 = (Get-FileHash -LiteralPath $gdb `
                    -Algorithm SHA256).Hash.ToLowerInvariant()
                $debuggerResult = Invoke-WcrtDebuggerProcess `
                    -Debugger $gdb -Executable $executable.FullName
                $debuggerStatus = $debuggerResult.Status
                $debuggerExitCode = $debuggerResult.ExitCode
                $debuggerOutput = $debuggerResult.Output
                if ($debuggerStatus -eq 'Completed' -and
                    $debuggerOutput -notmatch '(?m)^#0\s') {
                    $debuggerStatus = if ($debuggerOutput -match
                        'not in executable format: file format not recognized') {
                        'UnsupportedTarget'
                    } else { 'NoBacktrace' }
                }
            } catch {
                $debuggerStatus = 'Failed'
                $debuggerExitCode = $null
                $debuggerOutput = $_.Exception.Message
            }
        } else {
            $debuggerVersion = $null
            $debuggerSha256 = $null
            $debuggerStatus = 'Unavailable'
            $debuggerExitCode = $null
            $debuggerOutput = (
                'GDB was unavailable; the executable was built with ' +
                'TinyCC -g -bt30 for an in-process crash backtrace.')
        }
        $record = [PSCustomObject]@{
            TestCase = $TestCase
            Architecture = $Architecture
            Status = 'Retained'
            Executable = $retainedExecutable
            ExecutableSha256 = (Get-FileHash -LiteralPath `
                $retainedExecutable -Algorithm SHA256).Hash.ToLowerInvariant()
            Debugger = $gdb
            DebuggerVersion = $debuggerVersion
            DebuggerSha256 = $debuggerSha256
            DebuggerStatus = $debuggerStatus
            DebuggerExitCode = $debuggerExitCode
            DebuggerOutput = $debuggerOutput
            Transcript = $retainedTranscript
        }
        $debuggerOutput | Set-Content -LiteralPath `
            (Join-Path $diagnosticDirectory 'gdb.txt') -Encoding utf8NoBOM
    }
    $record | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath `
        (Join-Path $diagnosticDirectory 'diagnostic.json') `
        -Encoding utf8NoBOM
    $record
}

function Invoke-WcrtTestRunnerWithDiagnostics {
    param(
        [Parameter(Mandatory)][string]$TestCase,
        [Parameter(Mandatory)][string]$Suite,
        [Parameter(Mandatory)][string]$Architecture,
        [Parameter(Mandatory)][string]$RepositoryRoot,
        [Parameter(Mandatory)][string]$Runner,
        [Parameter(Mandatory)][string]$TinyCc
    )

    $category = if ($Suite -eq 'C89') { 'c89' } `
        elseif ($Suite -eq 'Microsoft compatibility') { 'mscompat' } `
        elseif ($Suite -eq 'POSIX compatibility') { 'posix' } `
        else { 'c99' }
    $buildDirectory = Join-Path $RepositoryRoot (
        "build/tests/$category/" + $TestCase.ToLowerInvariant())
    $diagnosticDirectory = Join-Path $RepositoryRoot (
        "output/test-results/$Architecture/diagnostics/" +
        $TestCase.ToLowerInvariant())
    New-Item -ItemType Directory -Force $diagnosticDirectory | Out-Null
    $transcriptPath = Join-Path $diagnosticDirectory 'test-transcript.txt'
    $transcriptStarted = $false
    $transcriptError = $null
    try {
        Start-Transcript -LiteralPath $transcriptPath -Force | Out-Null
        $transcriptStarted = $true
    } catch {
        $transcriptError = $_.Exception.Message
    }

    $capturedOutput = [Collections.Generic.List[object]]::new()
    $failure = $null
    try {
        & $Runner -TinyCc $TinyCc *>&1 | ForEach-Object {
            $capturedOutput.Add($_)
        }
    } catch {
        $failure = $_
    } finally {
        if ($transcriptStarted) {
            Stop-Transcript | Out-Null
        }
    }

    if ($null -eq $failure) {
        if (Test-Path -LiteralPath $transcriptPath) {
            Remove-Item -LiteralPath $transcriptPath -Force
        }
        return [PSCustomObject]@{
            Succeeded = $true
            RunnerOutput = @($capturedOutput)
            FailureReport = $null
            Diagnostics = $null
        }
    }

    try {
        $diagnostics = Invoke-WcrtFailureDebugger -TestCase $TestCase `
            -Architecture $Architecture -RepositoryRoot $RepositoryRoot `
            -BuildDirectory $buildDirectory -TranscriptPath $transcriptPath
    } catch {
        $diagnostics = [PSCustomObject]@{
            TestCase = $TestCase
            Architecture = $Architecture
            Status = 'DiagnosticCaptureFailed'
            Executable = $null
            ExecutableSha256 = $null
            Debugger = $null
            DebuggerVersion = $null
            DebuggerSha256 = $null
            DebuggerStatus = 'Failed'
            DebuggerExitCode = $null
            DebuggerOutput = $_.Exception.Message
            Transcript = $transcriptPath
        }
        $diagnostics | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath `
            (Join-Path $diagnosticDirectory 'diagnostic.json') `
            -Encoding utf8NoBOM
    }
    $capturedText = @($capturedOutput | ForEach-Object {
        $_.ToString()
    }) -join "`n"
    if (-not [string]::IsNullOrWhiteSpace($capturedText)) {
        $capturedText | Set-Content -LiteralPath `
            (Join-Path $diagnosticDirectory 'captured-output.txt') `
            -Encoding utf8NoBOM
    }
    $transcript = if (Test-Path -LiteralPath $transcriptPath) {
        Get-Content -LiteralPath $transcriptPath -Raw
    } elseif ($transcriptError) {
        "Transcript unavailable: $transcriptError"
    } else {
        'Transcript unavailable.'
    }
    $failureReport = @(
        $failure.Exception.Message
        ''
        '--- captured runner output ---'
        $capturedText
        ''
        '--- retained test transcript ---'
        $transcript.Trim()
        ''
        "--- GDB status: $($diagnostics.DebuggerStatus) ---"
        $diagnostics.DebuggerOutput
    ) -join "`n"
    [PSCustomObject]@{
        Succeeded = $false
        RunnerOutput = @($capturedOutput)
        FailureReport = $failureReport
        Diagnostics = $diagnostics
    }
}
