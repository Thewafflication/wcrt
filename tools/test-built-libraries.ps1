[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('x86', 'x64', 'arm64')]
    [string]$Architecture,

    [Parameter(Mandatory = $true)]
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration,

    [Parameter(Mandatory = $true)]
    [string]$TinyCc,

    [switch]$CompileOnly,

    [string]$BuildRoot = 'output/build'
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
. (Join-Path $repoRoot 'tests\test-logging.ps1')
$buildDirectory = Join-Path $repoRoot "$BuildRoot/$Architecture/$Configuration"
$smokeDirectory = Join-Path $buildDirectory 'link-smoke'
$source = Join-Path $smokeDirectory 'consumer.c'
$complexSource = Join-Path $smokeDirectory 'complex-consumer.c'
Write-WspInfo (
    "Testing $Architecture $Configuration static and DLL consumers.")
New-Item -ItemType Directory -Force -Path $smokeDirectory | Out-Null
Set-Content -LiteralPath $source -Encoding ascii -Value @(
    '#include <string.h>'
    'int main(void) { return strlen("wcrt") == 4 ? 0 : 1; }'
)

$include = Join-Path $buildDirectory 'include'
$staticExecutable = Join-Path $smokeDirectory 'static-consumer.exe'
$dynamicExecutable = Join-Path $smokeDirectory 'dll-consumer.exe'
$complexStaticExecutable = Join-Path $smokeDirectory `
    'complex-static-consumer.exe'
$complexDynamicExecutable = Join-Path $smokeDirectory `
    'complex-dll-consumer.exe'
& $TinyCc -std=c89 -Wall -Werror -I $include $source `
    (Join-Path $buildDirectory 'libwcrt.a') -o $staticExecutable
if ($LASTEXITCODE -ne 0) { throw 'Static-library consumer failed to link.' }
& $TinyCc -std=c89 -Wall -Werror -I $include $source `
    (Join-Path $buildDirectory 'wcrt.def') -o $dynamicExecutable
if ($LASTEXITCODE -ne 0) { throw 'DLL consumer failed to link.' }
Copy-Item -LiteralPath (Join-Path $buildDirectory 'wcrt.dll') `
    -Destination $smokeDirectory -Force

$capabilityPath = Join-Path $buildDirectory 'c99-complex-capability.json'
$complexCapability = Get-Content -LiteralPath $capabilityPath -Raw |
    ConvertFrom-Json
$complexSupported = $complexCapability.Status -eq 'Supported' -and
    [bool]$complexCapability.RuntimeIncluded
if ($complexSupported) {
    Set-Content -LiteralPath $complexSource -Encoding ascii -Value @(
        '#include <complex.h>'
        'int main(void)'
        '{'
        '    double complex value = 3.0 + 4.0 * I;'
        '    double complex mirror = conj(value);'
        '    return creal(mirror) == 3.0 && cimag(mirror) == -4.0 &&'
        '        cabs(value) == 5.0 ? 0 : 1;'
        '}'
    )
    & $TinyCc -std=c99 -Wall -Werror -I $include $complexSource `
        (Join-Path $buildDirectory 'libwcrt.a') -o $complexStaticExecutable
    if ($LASTEXITCODE -ne 0) {
        throw 'Complex static-library consumer failed to link.'
    }
    & $TinyCc -std=c99 -Wall -Werror -I $include $complexSource `
        (Join-Path $buildDirectory 'wcrt.def') -o $complexDynamicExecutable
    if ($LASTEXITCODE -ne 0) {
        throw 'Complex DLL consumer failed to link.'
    }
}

if (-not $CompileOnly) {
    & $staticExecutable
    if ($LASTEXITCODE -ne 0) { throw 'Static-library consumer failed at runtime.' }
    & $dynamicExecutable
    if ($LASTEXITCODE -ne 0) { throw 'DLL consumer failed at runtime.' }
    if ($complexSupported) {
        & $complexStaticExecutable
        if ($LASTEXITCODE -ne 0) {
            throw 'Complex static-library consumer failed at runtime.'
        }
        & $complexDynamicExecutable
        if ($LASTEXITCODE -ne 0) {
            throw 'Complex DLL consumer failed at runtime.'
        }
    }
}

Write-WspPass (
    "$Architecture $Configuration static and DLL consumer tests passed.")
[PSCustomObject]@{
    Architecture = $Architecture
    StaticLink = 'Pass'
    DllLink = 'Pass'
    Runtime = if ($CompileOnly) { 'Deferred' } else { 'Pass' }
    ComplexStaticLink = if ($complexSupported) { 'Pass' } else { 'Omitted' }
    ComplexDllLink = if ($complexSupported) { 'Pass' } else { 'Omitted' }
    ComplexRuntime = if (-not $complexSupported) { 'Omitted' }
        elseif ($CompileOnly) { 'Deferred' }
        else { 'Pass' }
}
