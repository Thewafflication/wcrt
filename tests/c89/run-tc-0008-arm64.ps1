[CmdletBinding()]
param(
    [string]$TinyCc
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$buildDirectory = Join-Path $repoRoot 'build\tests\c89\tc-0008-arm64'
$executable = Join-Path $buildDirectory 'setjmp-test.exe'

if ([string]::IsNullOrWhiteSpace($TinyCc)) {
    $TinyCc = Join-Path (Split-Path -Parent $repoRoot) `
        'tcc_package\out\build\arm64-debug\package\tcc.exe'
}
if (-not (Test-Path -LiteralPath $TinyCc)) {
    throw 'The AArch64 Windows TinyCC toolchain was not found.'
}

$compilerVersion = (& $TinyCc -v 2>&1 | Select-Object -First 1).ToString()
if ($compilerVersion -notmatch 'AArch64 Windows') {
    throw "TinyCC is not an AArch64 Windows compiler: $compilerVersion"
}

New-Item -ItemType Directory -Force -Path $buildDirectory | Out-Null
$arguments = @(
    '-std=c89', '-Wall', '-Werror',
    '-I', (Join-Path $repoRoot 'include'),
    (Join-Path $repoRoot 'src\setjmp.c'),
    (Join-Path $repoRoot 'src\platform\windows\setjmp.S'),
    (Join-Path $repoRoot 'tests\c89\setjmp.c'),
    '-o', $executable
)
$buildOutput = & $TinyCc @arguments 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "TC-0008 ARM64 build failed:`n$($buildOutput | Out-String)"
}

$hostArchitecture = [System.Runtime.InteropServices.RuntimeInformation]::`
    OSArchitecture.ToString()
$runtimeStatus = 'NotRunOnNonArm64Host'
$exitCode = $null
if ($hostArchitecture -eq 'Arm64') {
    & $executable
    $exitCode = $LASTEXITCODE
    if ($exitCode -ne 0) {
        throw "TC-0008 ARM64 behavior failed with code $exitCode."
    }
    $runtimeStatus = 'Pass'
}

[PSCustomObject]@{
    TestCase = 'TC-0008-ARM64'
    Requirement = 'REQ-0008'
    Compiler = $compilerVersion
    Target = 'Windows ARM64'
    ContextSize = 168
    NonvolatileIntegerState = 'X19-X30 and SP compiled'
    NonvolatileVectorState = 'D8-D15 compiled'
    CrossCompile = 'Pass'
    HostArchitecture = $hostArchitecture
    Runtime = $runtimeStatus
    ExitCode = $exitCode
}
