[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$complexHeader = Get-Content -LiteralPath (Join-Path $repoRoot `
    'include\complex.h') -Raw
$complexSource = Get-Content -LiteralPath (Join-Path $repoRoot `
    'src\complex.c') -Raw
$tgmathHeader = Get-Content -LiteralPath (Join-Path $repoRoot `
    'include\tgmath.h') -Raw
$complexAbiAssembly = Get-Content -LiteralPath (Join-Path $repoRoot `
    'src\platform\windows\tinycc_complex_abi.S') -Raw

$complexNames = @(
    'cacos', 'casin', 'catan', 'ccos', 'csin', 'ctan',
    'cacosh', 'casinh', 'catanh', 'ccosh', 'csinh', 'ctanh',
    'cexp', 'clog', 'cabs', 'cpow', 'csqrt', 'carg', 'cimag',
    'conj', 'cproj', 'creal'
)
$macroDeclared = @(
    'cacos', 'casin', 'catan', 'ccos', 'csin', 'ctan',
    'cacosh', 'casinh', 'catanh', 'ccosh', 'csinh', 'ctanh',
    'cexp', 'clog', 'csqrt', 'conj', 'cproj'
)
$wrapperImplemented = @(
    'cacos', 'casin', 'catan', 'ccos', 'csin', 'ctan',
    'cacosh', 'casinh', 'catanh', 'ccosh', 'csinh', 'ctanh',
    'cexp', 'clog', 'csqrt'
)
foreach ($name in $complexNames) {
    if ($name -in $macroDeclared) {
        if ($complexHeader -notmatch
            [regex]::Escape("__WCRT_COMPLEX_UNARY($name);")) {
            throw "Complex header inventory is missing $name."
        }
    } elseif ($complexHeader -notmatch "(?m)\b$name(?:f|l)?\(") {
        throw "Complex header inventory is missing $name."
    }
    if ($complexSource -notmatch "(?m)^double (?:complex )?$name\(") {
        throw "Complex source inventory is missing $name."
    }
    if ($name -in $wrapperImplemented) {
        if ($complexSource -notmatch
            [regex]::Escape("WCRT_COMPLEX_WRAPPERS($name)")) {
            throw "Complex wrapper inventory is missing $name."
        }
    } elseif ($complexSource -notmatch "(?m)^float (?:complex )?$($name)f\(" -or
        $complexSource -notmatch
            "(?m)^long double (?:complex )?$($name)l\(") {
        throw "Complex precision inventory is missing $name."
    }
}

$genericNames = @(
    'acos', 'asin', 'atan', 'acosh', 'asinh', 'atanh', 'cos', 'sin',
    'tan', 'cosh', 'sinh', 'tanh', 'exp', 'log', 'sqrt', 'fabs', 'pow',
    'atan2', 'cbrt', 'ceil', 'copysign', 'erf', 'erfc', 'exp2', 'expm1',
    'fdim', 'floor', 'fma', 'fmax', 'fmin', 'fmod', 'frexp', 'hypot',
    'ilogb', 'ldexp', 'lgamma', 'llrint', 'llround', 'log10', 'log1p',
    'log2', 'logb', 'lrint', 'lround', 'nearbyint', 'nextafter',
    'nexttoward', 'remainder', 'remquo', 'rint', 'round', 'scalbln',
    'scalbn', 'tgamma', 'trunc', 'carg', 'cimag', 'conj', 'cproj', 'creal'
)
$definedGenericNames = @(
    [regex]::Matches($tgmathHeader, '(?m)^#define ([a-z][a-z0-9]*)\(') |
        ForEach-Object { $_.Groups[1].Value }
)
$difference = @(Compare-Object ($genericNames | Sort-Object) `
    ($definedGenericNames | Sort-Object))
if ($difference.Count -ne 0 -or $definedGenericNames.Count -ne 60) {
    throw 'The tgmath macro inventory differs from the controlled 60 names.'
}
if ($definedGenericNames -contains 'modf' -or
    $definedGenericNames -contains 'nan') {
    throw 'modf and nan must not be generic macros.'
}

foreach ($bridge in @(
    @('__tcc_muldc3', '__wcrt_tinycc_muldc3'),
    @('__tcc_divdc3', '__wcrt_tinycc_divdc3'),
    @('__tcc_mulxc3', '__tcc_muldc3'),
    @('__tcc_divxc3', '__tcc_divdc3')
)) {
    $pattern = '(?ms)^' + [regex]::Escape($bridge[0]) + ':\s*' +
        'b\s+' + [regex]::Escape($bridge[1]) + '\s*$'
    if ($complexAbiAssembly -notmatch $pattern) {
        throw "TinyCC ARM64 ABI bridge $($bridge[0]) does not preserve the current helper ABI."
    }
}
if ($complexAbiAssembly -match '(?m)\bfmov\s+d[0-3],\s*x[0-3]\b' -or
    $complexAbiAssembly -match '(?m)\bmov\s+x0,\s*x4\b') {
    throw 'TinyCC ARM64 ABI bridge still remaps the obsolete 1442 helper ABI.'
}

[PSCustomObject]@{
    Status = 'Pass'
    ComplexBaseNames = $complexNames.Count
    ComplexFunctions = 3 * $complexNames.Count
    GenericNames = $definedGenericNames.Count
    Arm64HelperBridges = 4
}
