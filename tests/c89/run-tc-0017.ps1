[CmdletBinding()]
param(
    [Parameter(Mandatory)][ValidateSet('x86', 'x64', 'arm64')]
    [string]$Architecture,
    [Parameter(Mandatory)][ValidateSet('Debug', 'Release')]
    [string]$Configuration,
    [Parameter(Mandatory)][string]$TinyCc,
    [string]$BuildRoot = 'output/build'
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'startup-test-lib.ps1')
$buildDirectory = Join-Path $repoRoot `
    "$BuildRoot/$Architecture/$Configuration"
$startupObject = Join-Path $buildDirectory 'wcrt-startup-console.o'
$testSource = Join-Path $PSScriptRoot 'startup_console.c'
$ordinarySource = Join-Path $PSScriptRoot 'startup_ordinary.c'
$testDirectory = Join-Path $buildDirectory 'startup-tests'
$executable = Join-Path $testDirectory 'console-startup-test.exe'
$ordinaryExecutable = Join-Path $testDirectory 'ordinary-startup-test.exe'
$expectedMachine = @{ x86 = 0x014c; x64 = 0x8664; arm64 = 0xaa64 }
$started = Get-Date
$status = 'Fail'
$rationale = $null
$output = [Collections.Generic.List[string]]::new()

New-Item -ItemType Directory -Force -Path $testDirectory | Out-Null
try {
    $ordinaryOutput = Invoke-WcrtOrdinaryLink -TinyCc $TinyCc `
        -BuildDirectory $buildDirectory -TestSource $ordinarySource `
        -OutputPath $ordinaryExecutable
    $linkOutput = Invoke-WcrtStartupLink -TinyCc $TinyCc `
        -Architecture $Architecture -BuildDirectory $buildDirectory `
        -StartupObject $startupObject -TestSource $testSource `
        -Subsystem console -OutputPath $executable
    $pe = Get-WcrtPeInformation $executable
    if ($pe.Machine -ne $expectedMachine[$Architecture] -or
        $pe.Subsystem -ne 3 -or $pe.EntryPoint -eq 0 -or
        $pe.HostCrtImport) {
        throw "The console PE inspection failed: $($pe | Out-String)"
    }
    $archiveText = [Text.Encoding]::ASCII.GetString(
        [IO.File]::ReadAllBytes((Join-Path $buildDirectory 'libwcrt.a')))
    if ($archiveText -match '(?m)^_start\x00') {
        throw 'libwcrt.a unexpectedly defines the startup entry symbol.'
    }
    $output.Add("PE machine: 0x$('{0:X4}' -f $pe.Machine)")
    $output.Add("PE subsystem: $($pe.Subsystem)")
    $output.Add("PE entry RVA: 0x$('{0:X8}' -f $pe.EntryPoint)")
    $output.Add("Host CRT import: $($pe.HostCrtImport)")
    if (Test-WcrtNativeArchitecture $Architecture) {
        $ordinaryStatus = Invoke-WcrtStartupProcess $ordinaryExecutable ''
        $returnStatus = Invoke-WcrtStartupProcess $executable '--return'
        $atexitStatus = Invoke-WcrtStartupProcess $executable '--atexit'
        $argumentStatus = Invoke-WcrtStartupProcess $executable `
            'alpha "two words" quote\"mark'
        $emptyStatus = Invoke-WcrtStartupProcess $executable 'empty "" tail'
        $doubleStatus = Invoke-WcrtStartupProcess $executable `
            'doubled "double""quote"'
        $stdinStatuses = foreach ($answer in 'n', 'y') {
            $inputPath = Join-Path $testDirectory "startup-stdin-$answer.txt"
            [IO.File]::WriteAllBytes($inputPath,
                [Text.Encoding]::ASCII.GetBytes("$answer`r`n"))
            Invoke-WcrtStartupProcess $executable "--stdin $answer" `
                -RedirectStandardInput $inputPath
        }
        if ($ordinaryStatus -ne 0 -or $returnStatus -ne 23 -or
            $atexitStatus -ne 29 -or $argumentStatus -ne 0 -or
            $emptyStatus -ne 0 -or $doubleStatus -ne 0) {
            throw "Runtime statuses were ordinary=$ordinaryStatus, " +
                "return=$returnStatus, atexit=$atexitStatus, " +
                "arguments=$argumentStatus, empty=$emptyStatus, " +
                "doubled=$doubleStatus."
        }
        if ($stdinStatuses[0] -ne 0 -or $stdinStatuses[1] -ne 0) {
            throw "WCRT-startup redirected stdin statuses were " +
                "n=$($stdinStatuses[0]), y=$($stdinStatuses[1])."
        }
        $output.Add('Native runtime checks: Pass')
        $output.Add('WCRT-startup redirected stdin checks: Pass')
        $status = 'Pass'
    } else {
        $status = 'Blocked'
        $rationale = 'Native execution requires a matching host architecture.'
        $output.Add('Native runtime checks: Blocked')
    }
    if ($linkOutput) {
        $output.Add("Linker output:`n$linkOutput")
    }
    if ($ordinaryOutput) {
        $output.Add("Ordinary linker output:`n$ordinaryOutput")
    }
} catch {
    $output.Add($_.Exception.Message)
}

[PSCustomObject]@{
    TestCase = 'TC-0017'
    Requirement = 'REQ-0017'
    Architecture = $Architecture
    Configuration = $Configuration
    Status = $status
    Rationale = $rationale
    Started = $started.ToString('o')
    Finished = (Get-Date).ToString('o')
    Output = $output -join "`n"
}
