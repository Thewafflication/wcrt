[CmdletBinding()]
param(
    [string]$TinyCc
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$buildRoot = Join-Path $repoRoot 'build\tests\c89\arm64-cross'
$evidenceRoot = Join-Path $repoRoot 'output\test-results\c89-wcrt-arm64'

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

New-Item -ItemType Directory -Force -Path $buildRoot | Out-Null
New-Item -ItemType Directory -Force -Path $evidenceRoot | Out-Null

$tests = @(
    @{ Id = '0001'; Name = 'assert'; Sources = @(
        'src\assert.c', 'tests\c89\assert.c',
        'tests\c89\assert_disabled.c') },
    @{ Id = '0002'; Name = 'ctype'; Sources = @(
        'src\ctype.c', 'tests\c89\ctype.c') },
    @{ Id = '0003'; Name = 'errno'; Sources = @(
        'src\errno.c', 'tests\c89\errno.c') },
    @{ Id = '0004'; Name = 'float'; Sources = @(
        'tests\c89\float.c') },
    @{ Id = '0005'; Name = 'limits'; Sources = @(
        'tests\c89\limits.c') },
    @{ Id = '0006'; Name = 'locale'; Sources = @(
        'src\ctype.c', 'src\locale.c', 'tests\c89\locale.c') },
    @{ Id = '0007'; Name = 'math'; Sources = @(
        'src\errno.c', 'src\math.c', 'tests\c89\math.c') },
    @{ Id = '0008'; Name = 'setjmp'; Sources = @(
        'src\setjmp.c', 'src\platform\windows\setjmp.S',
        'tests\c89\setjmp.c') },
    @{ Id = '0009'; Name = 'signal'; Sources = @(
        'src\signal.c', 'src\platform\windows\signal.c',
        'tests\c89\signal.c') },
    @{ Id = '0010'; Name = 'stdarg'; Sources = @(
        'tests\c89\stdarg.c') },
    @{ Id = '0011'; Name = 'stddef'; Sources = @(
        'tests\c89\stddef.c') }
)

function Get-PeMachine {
    param([string]$Path)

    $stream = [System.IO.File]::OpenRead($Path)
    try {
        $reader = [System.IO.BinaryReader]::new($stream)
        $stream.Position = 0x3c
        $peOffset = $reader.ReadInt32()
        $stream.Position = $peOffset
        if ($reader.ReadUInt32() -ne 0x00004550) {
            throw "$Path is not a PE/COFF executable."
        }
        return $reader.ReadUInt16()
    } finally {
        $stream.Dispose()
    }
}

$results = foreach ($test in $tests) {
    $executable = Join-Path $buildRoot "$($test.Name)-test.exe"
    $arguments = [System.Collections.Generic.List[string]]::new()
    @('-std=c89', '-Wall', '-Werror', '-I',
        (Join-Path $repoRoot 'include')) |
        ForEach-Object { $arguments.Add($_) }
    $test.Sources | ForEach-Object {
        $arguments.Add((Join-Path $repoRoot $_))
    }
    $arguments.Add('-o')
    $arguments.Add($executable)

    $started = Get-Date
    $lines = & $TinyCc @arguments 2>&1
    $compilerExitCode = $LASTEXITCODE
    $machine = if ($compilerExitCode -eq 0) {
        Get-PeMachine $executable
    } else {
        0
    }
    $status = if ($compilerExitCode -eq 0 -and $machine -eq 0xaa64) {
        'Pass'
    } else {
        'Fail'
    }
    $output = ($lines | ForEach-Object { $_.ToString() }) -join "`n"
    if ([string]::IsNullOrWhiteSpace($output)) {
        $output = '(no compiler diagnostics)'
    }
    $result = [PSCustomObject]@{
        TestCase = "TC-$($test.Id)"
        Requirement = "REQ-$($test.Id)"
        Target = 'WCRT Windows ARM64'
        Compiler = $compilerVersion
        Status = $status
        CompilerExitCode = $compilerExitCode
        PeMachine = ('0x{0:X4}' -f $machine)
        Runtime = 'DeferredToArm64CI'
        Started = $started.ToString('o')
        Finished = (Get-Date).ToString('o')
        Output = $output
    }
    $evidencePath = Join-Path $evidenceRoot `
        "tc-$($test.Id)-$($test.Name)-evidence.txt"
    $result | Format-List | Out-String |
        Set-Content -LiteralPath $evidencePath -Encoding UTF8
    $result
}

$jsonPath = Join-Path $evidenceRoot 'c89-arm64-cross.json'
$results | ConvertTo-Json -Depth 4 |
    Set-Content -LiteralPath $jsonPath -Encoding UTF8
$results | Format-Table TestCase,Requirement,Status,PeMachine,Runtime

if ($results.Status -contains 'Fail') {
    throw 'One or more WCRT ARM64 cross-builds failed.'
}

$jsonPath
