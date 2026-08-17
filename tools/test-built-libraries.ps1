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
        'typedef union {'
        '    double complex value;'
        '    double part[2];'
        '} wcrt_consumer_complex;'
        'static double complex wcrt_consumer_make(double real, double imaginary)'
        '{'
        '    wcrt_consumer_complex result;'
        '    result.part[0] = real;'
        '    result.part[1] = imaginary;'
        '    return result.value;'
        '}'
        'typedef double complex (*wcrt_complex_fn)(double complex);'
        'typedef double (*wcrt_component_fn)(double complex);'
        'static wcrt_complex_fn wcrt_conj_fn;'
        'static wcrt_component_fn wcrt_creal_fn;'
        'static wcrt_component_fn wcrt_cimag_fn;'
        'static wcrt_component_fn wcrt_cabs_fn;'
        '#if defined(WCRT_DLL_CONSUMER)'
        '#if defined(__TINYC__) || defined(__GNUC__)'
        '#define WCRT_WINAPI __attribute__((stdcall))'
        '#else'
        '#define WCRT_WINAPI __stdcall'
        '#endif'
        '__declspec(dllimport) void *WCRT_WINAPI LoadLibraryA(const char *name);'
        '__declspec(dllimport) void *WCRT_WINAPI GetProcAddress(void *module, const char *name);'
        '#endif'
        'static int wcrt_resolve_functions(void)'
        '{'
        '#if defined(WCRT_DLL_CONSUMER)'
        '    void *module = LoadLibraryA("wcrt.dll");'
        '    if (module == 0) return 0;'
        '    wcrt_conj_fn = (wcrt_complex_fn)GetProcAddress(module, "conj");'
        '    wcrt_creal_fn = (wcrt_component_fn)GetProcAddress(module, "creal");'
        '    wcrt_cimag_fn = (wcrt_component_fn)GetProcAddress(module, "cimag");'
        '    wcrt_cabs_fn = (wcrt_component_fn)GetProcAddress(module, "cabs");'
        '#else'
        '    wcrt_conj_fn = conj;'
        '    wcrt_creal_fn = creal;'
        '    wcrt_cimag_fn = cimag;'
        '    wcrt_cabs_fn = cabs;'
        '#endif'
        '    return wcrt_conj_fn != 0 && wcrt_creal_fn != 0 &&'
        '        wcrt_cimag_fn != 0 && wcrt_cabs_fn != 0;'
        '}'
        'int main(void)'
        '{'
        '    double complex value = wcrt_consumer_make(3.0, 4.0);'
        '    double complex mirror;'
        '    double complex right = wcrt_consumer_make(1.0, -2.0);'
        '    double complex product = value * right;'
        '    double complex quotient = product / right;'
        '    if (!wcrt_resolve_functions()) return 2;'
        '    mirror = wcrt_conj_fn(value);'
        '    return wcrt_creal_fn(mirror) == 3.0 &&'
        '        wcrt_cimag_fn(mirror) == -4.0 &&'
        '        wcrt_creal_fn(product) == 11.0 &&'
        '        wcrt_cimag_fn(product) == -2.0 &&'
        '        wcrt_creal_fn(quotient) == 3.0 &&'
        '        wcrt_cimag_fn(quotient) == 4.0 &&'
        '        wcrt_cabs_fn(value) == 5.0 ? 0 : 1;'
        '}'
    )
    & $TinyCc -std=c99 -Wall -Werror -I $include $complexSource `
        (Join-Path $buildDirectory 'libwcrt.a') -o $complexStaticExecutable
    if ($LASTEXITCODE -ne 0) {
        throw 'Complex static-library consumer failed to link.'
    }
    $complexDynamicInputs = @(
        (Join-Path $buildDirectory 'wcrt.def')
    )
    if ($Architecture -eq 'arm64') {
        # The DLL owns the C99 functions.  The archive contributes only the
        # compiler-private TinyCC operator ABI bridge after imports resolve.
        $complexDynamicInputs += Join-Path $buildDirectory `
            'libwcrt-tinycc-complex-abi.a'
    }
    & $TinyCc -std=c99 -DWCRT_DLL_CONSUMER=1 -Wall -Werror -I $include `
        $complexSource `
        @complexDynamicInputs -o $complexDynamicExecutable
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
