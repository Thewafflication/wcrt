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
$startupObject = Join-Path $buildDirectory 'wcrt-startup-gui.o'
$testSource = Join-Path $PSScriptRoot 'startup_gui.c'
$testDirectory = Join-Path $buildDirectory 'startup-tests'
$executable = Join-Path $testDirectory 'gui-startup-test.exe'
$expectedMachine = @{ x86 = 0x014c; x64 = 0x8664; arm64 = 0xaa64 }
$started = Get-Date
$status = 'Fail'
$rationale = $null
$output = [Collections.Generic.List[string]]::new()

New-Item -ItemType Directory -Force -Path $testDirectory | Out-Null
try {
    $linkOutput = Invoke-WcrtStartupLink -TinyCc $TinyCc `
        -Architecture $Architecture -BuildDirectory $buildDirectory `
        -StartupObject $startupObject -TestSource $testSource `
        -Subsystem windows -OutputPath $executable
    $pe = Get-WcrtPeInformation $executable
    if ($pe.Machine -ne $expectedMachine[$Architecture] -or
        $pe.Subsystem -ne 2 -or $pe.EntryPoint -eq 0 -or
        $pe.HostCrtImport -or
        -not (Test-WcrtArm64PeVersions -Pe $pe `
            -Architecture $Architecture)) {
        throw "The GUI PE inspection failed: $($pe | Out-String)"
    }
    $output.Add("PE machine: 0x$('{0:X4}' -f $pe.Machine)")
    $output.Add("PE OS version: " +
        "$($pe.OperatingSystemMajor).$($pe.OperatingSystemMinor)")
    $output.Add("PE subsystem version: " +
        "$($pe.SubsystemMajor).$($pe.SubsystemMinor)")
    $output.Add("PE subsystem: $($pe.Subsystem)")
    $output.Add("PE entry RVA: 0x$('{0:X8}' -f $pe.EntryPoint)")
    $output.Add("Host CRT import: $($pe.HostCrtImport)")
    if (Test-WcrtNativeArchitecture $Architecture) {
        $returnStatus = Invoke-WcrtStartupProcess $executable '--return'
        $atexitStatus = Invoke-WcrtStartupProcess $executable '--atexit'
        $showStatus = Invoke-WcrtStartupProcess $executable '--show-hidden' `
            -Hidden
        $argumentStatus = Invoke-WcrtStartupProcess $executable `
            'alpha "two words" quote\"mark'
        $emptyStatus = Invoke-WcrtStartupProcess $executable 'empty "" tail'
        if ($returnStatus -ne 23 -or $atexitStatus -ne 29 -or
            $showStatus -ne 31 -or $argumentStatus -ne 0 -or
            $emptyStatus -ne 0) {
            throw "Runtime statuses were return=$returnStatus, " +
                "atexit=$atexitStatus, show=$showStatus, " +
                "arguments=$argumentStatus, empty=$emptyStatus."
        }
        $output.Add('Native runtime checks: Pass')
        $status = 'Pass'
    } else {
        $status = 'Blocked'
        $rationale = 'Native execution requires a matching host architecture.'
        $output.Add('Native runtime checks: Blocked')
    }
    if ($linkOutput) {
        $output.Add("Linker output:`n$linkOutput")
    }
} catch {
    $output.Add($_.Exception.Message)
}

[PSCustomObject]@{
    TestCase = 'TC-0018'
    Requirement = 'REQ-0018'
    Architecture = $Architecture
    Configuration = $Configuration
    Status = $status
    Rationale = $rationale
    Started = $started.ToString('o')
    Finished = (Get-Date).ToString('o')
    Output = $output -join "`n"
}
