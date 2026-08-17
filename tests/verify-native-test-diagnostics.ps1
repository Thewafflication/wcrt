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
. (Join-Path $PSScriptRoot 'native-test-diagnostics.ps1')
$buildDirectory = Join-Path $repoRoot (
    'build/tests/c99/diagnostic-self-test')
$resultDirectory = Join-Path $repoRoot "output/test-results/$Architecture"
$source = Join-Path $buildDirectory 'crash-probe.c'
$executable = Join-Path $buildDirectory 'crash-probe.exe'
$runner = Join-Path $buildDirectory 'crash-runner.ps1'
New-Item -ItemType Directory -Force $buildDirectory | Out-Null
New-Item -ItemType Directory -Force $resultDirectory | Out-Null
@(
    'static int crash_probe(void)'
    '{'
    '    volatile int *address = (int *)0;'
    '    return *address;'
    '}'
    ''
    'int main(void)'
    '{'
    '    return crash_probe();'
    '}'
) | Set-Content -LiteralPath $source -Encoding ascii
$escapedExecutable = $executable.Replace("'", "''")
@(
    '[CmdletBinding()]'
    'param([string]$TinyCc)'
    "& '$escapedExecutable' 2>&1"
    '$crashExitCode = $LASTEXITCODE'
    'if ($crashExitCode -eq 0) {'
    "    throw 'The diagnostic crash probe unexpectedly succeeded.'"
    '}'
    'throw "Diagnostic crash probe exited with code $crashExitCode."'
) | Set-Content -LiteralPath $runner -Encoding utf8NoBOM

$previousTinyCc = $env:WCRT_TEST_TINYCC
try {
    $wrapper = Get-WcrtDiagnosticTinyCc -TinyCc $TinyCc `
        -RepositoryRoot $repoRoot
    $buildOutput = @(& $wrapper -std=c99 -Wall -Werror $source `
        -o $executable 2>&1)
    if ($LASTEXITCODE -ne 0) {
        throw "The diagnostic crash probe did not build:`n" +
            ($buildOutput -join "`n")
    }
    $invocation = Invoke-WcrtTestRunnerWithDiagnostics `
        -TestCase 'DIAGNOSTIC-SELF-TEST' -Suite 'C99' `
        -Architecture $Architecture -RepositoryRoot $repoRoot `
        -Runner $runner -TinyCc $wrapper
    if ($invocation.Succeeded) {
        throw 'The diagnostic runner did not preserve the expected failure.'
    }
    $runtimeText = $invocation.FailureReport
    foreach ($requiredText in @('RUNTIME ERROR', 'crash_probe', 'main')) {
        if ($runtimeText -notmatch [regex]::Escape($requiredText)) {
            throw "The TinyCC backtrace omitted '$requiredText':`n$runtimeText"
        }
    }
    $runtimeExitCode = if ($runtimeText -match
        'Diagnostic crash probe exited with code (-?\d+)') {
        [int]$Matches[1]
    } else { $null }
    $debugger = $invocation.Diagnostics
    if ($debugger.Debugger -and $debugger.DebuggerStatus -ne 'Completed') {
        throw "GDB did not capture a backtrace: $($debugger.DebuggerOutput)"
    }
    if ($debugger.Debugger) {
        foreach ($requiredText in @('#0', 'crash_probe', 'main')) {
            if ($debugger.DebuggerOutput -notmatch
                [regex]::Escape($requiredText)) {
                throw "GDB omitted '$requiredText':`n" +
                    $debugger.DebuggerOutput
            }
        }
    }
    $result = [PSCustomObject]@{
        TestCase = 'DIAGNOSTIC-SELF-TEST'
        Architecture = $Architecture
        Status = 'Pass'
        BuiltInBacktrace = 'Pass'
        CrashExitCode = $runtimeExitCode
        Debugger = $debugger.Debugger
        DebuggerVersion = $debugger.DebuggerVersion
        DebuggerSha256 = $debugger.DebuggerSha256
        DebuggerStatus = $debugger.DebuggerStatus
        DiagnosticExecutableSha256 = $debugger.ExecutableSha256
    }
    $result | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath `
        (Join-Path $resultDirectory 'native-test-diagnostics.json') `
        -Encoding utf8NoBOM
    $result
} finally {
    if ($null -eq $previousTinyCc) {
        Remove-Item Env:WCRT_TEST_TINYCC -ErrorAction SilentlyContinue
    } else {
        $env:WCRT_TEST_TINYCC = $previousTinyCc
    }
}
