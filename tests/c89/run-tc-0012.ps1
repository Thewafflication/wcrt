[CmdletBinding()]
param([string]$TinyCc)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$buildDirectory = Join-Path $repoRoot 'build\tests\c89\tc-0012'
$executable = Join-Path $buildDirectory 'stdio-test.exe'
$stdinExecutable = Join-Path $buildDirectory 'stdio-stdin-test.exe'
if ([string]::IsNullOrWhiteSpace($TinyCc)) {
    $TinyCc = Join-Path (Split-Path -Parent $repoRoot) `
        'tcc_package\out\build\x64-debug\package\tcc.exe'
}
New-Item -ItemType Directory -Force -Path $buildDirectory | Out-Null
$sources = @(
    'src\ctype.c', 'src\errno.c', 'src\string.c', 'src\stdlib.c',
    'src\stdio.c', 'src\stdio_format.c', 'src\stdio_scan.c',
    'src\platform\windows\heap.c', 'src\platform\windows\process.c',
    'src\platform\windows\file.c', 'tests\c89\stdio.c'
) | ForEach-Object { Join-Path $repoRoot $_ }
$arguments = @('-std=c89', '-Wall', '-Werror', '-I',
    (Join-Path $repoRoot 'include')) + $sources + @('-o', $executable)
$output = & $TinyCc @arguments 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "TC-0012 build failed:`n$($output | Out-String)"
}
$stdinSources = @(
    'src\errno.c', 'src\string.c', 'src\stdio.c',
    'src\platform\windows\file.c',
    'tests\c89\stdio_stdin.c'
) | ForEach-Object { Join-Path $repoRoot $_ }
$stdinArguments = @('-std=c89', '-Wall', '-Werror', '-I',
    (Join-Path $repoRoot 'include')) + $stdinSources + @('-o', $stdinExecutable)
$stdinOutput = & $TinyCc @stdinArguments 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "TC-0012 standard-input build failed:`n$($stdinOutput | Out-String)"
}
Push-Location $buildDirectory
try {
    & $executable
    $exitCode = $LASTEXITCODE
} finally {
    Pop-Location
}
if ($exitCode -ne 0) {
    throw "TC-0012 behavior failed with code $exitCode."
}
function Invoke-RedirectedInputCase([string]$Answer) {
    $inputPath = Join-Path $buildDirectory "stdin-$Answer.txt"
    [IO.File]::WriteAllBytes($inputPath,
        [Text.Encoding]::ASCII.GetBytes("$Answer`r`n"))
    $process = Start-Process -FilePath $stdinExecutable `
        -ArgumentList @('redirected', $Answer) `
        -RedirectStandardInput $inputPath -NoNewWindow -Wait -PassThru
    if ($process.ExitCode -ne 0) {
        throw "TC-0012 native redirected stdin '$Answer' failed with " +
            "code $($process.ExitCode)."
    }
}
Invoke-RedirectedInputCase 'n'
Invoke-RedirectedInputCase 'y'
& $stdinExecutable console
if ($LASTEXITCODE -ne 0) {
    throw "TC-0012 console standard input failed with code $LASTEXITCODE."
}
[PSCustomObject]@{
    TestCase = 'TC-0012'
    Requirement = 'REQ-0012'
    Compiler = (& $TinyCc -v 2>&1 | Select-Object -First 1).ToString()
    FunctionsCovered = 41
    FilesAndTemporaryFiles = 'Pass'
    CharacterAndDirectIo = 'Pass'
    PositionAndIndicators = 'Pass'
    FormattedOutput = 'Pass'
    FormattedInput = 'Pass'
    TextTranslation = 'Pass'
    RedirectedStandardInputNo = 'Pass'
    RedirectedStandardInputYes = 'Pass'
    RedirectedEndOfFile = 'Pass'
    ConsoleStandardInput = 'Pass'
    ExitCode = 0
}
