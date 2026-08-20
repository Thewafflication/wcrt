[CmdletBinding()]
param([string]$TinyCc, [switch]$CompileOnly)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
if ([string]::IsNullOrWhiteSpace($TinyCc)) {
    $TinyCc = Get-Command tcc.exe -ErrorAction SilentlyContinue |
        Select-Object -First 1 -ExpandProperty Source
}
if (-not $TinyCc) {
    $TinyCc = Join-Path (Split-Path -Parent $repoRoot) `
        'tcc_package\out\build\x64-debug\package\tcc.exe'
}
$compiler = (& $TinyCc -v 2>&1 | Select-Object -First 1).ToString()
$architecture = if ($compiler -match 'i386 Windows') { 'x86' }
    elseif ($compiler -match 'x86_64 Windows') { 'x64' }
    elseif ($compiler -match 'AArch64 Windows') { 'arm64' }
    else { throw "Unsupported TinyCC target: $compiler" }

$result = & (Join-Path $PSScriptRoot 'Verify-C99HeaderMatrix.ps1') `
    -Architecture $architecture -TinyCc $TinyCc
$releaseGate = & (Join-Path $PSScriptRoot `
    'Verify-ReleaseWorkflowGate.ps1')
[PSCustomObject]@{
    TestCase = 'TC-0042'
    Requirement = 'REQ-0042'
    Status = 'Pass'
    Scope = 'Header/ABI compile and release/sign/package workflow gate matrix'
    Architecture = $architecture
    C99StandaloneHeaders = $result.C99StandaloneHeaders
    C89StandaloneHeaders = $result.C89StandaloneHeaders
    MixedOrders = $result.MixedOrders
    DebugDependency = $releaseGate.DebugDependency
    ReleaseArchitectures = $releaseGate.ReleaseArchitectures
    ReleaseDependency = $releaseGate.ReleaseDependency
    SigningDependency = $releaseGate.SigningDependency
    PackageDependency = $releaseGate.PackageDependency
    PublishDependency = $releaseGate.PublishDependency
}
