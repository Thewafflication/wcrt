[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [ValidateSet('x86', 'x64', 'arm64')]
    [string]$Architecture,

    [Parameter(Mandatory)]
    [string]$TinyCc,

    [string]$OutputRoot = 'output/test-results'
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$tinyCcPath = (Resolve-Path -LiteralPath $TinyCc).Path
$buildDirectory = Join-Path $repoRoot `
    "build/tests/c99/capabilities/$Architecture"
$outputDirectory = Join-Path $repoRoot "$OutputRoot/$Architecture"
New-Item -ItemType Directory -Force $buildDirectory, $outputDirectory |
    Out-Null

$compiler = (& $tinyCcPath -v 2>&1 | Select-Object -First 1).ToString()
$expectedTarget = @{
    x86 = 'i386 Windows'
    x64 = 'x86_64 Windows'
    arm64 = 'AArch64 Windows'
}[$Architecture]
if ($compiler -notmatch [regex]::Escape($expectedTarget)) {
    throw "TinyCC target mismatch. Expected '$expectedTarget', got '$compiler'."
}

$probes = @(
    @('bool', '_Bool', $true, ''),
    @('long-long', 'long long', $true, ''),
    @('restrict', 'restrict', $true, ''),
    @('variadic-macros', 'variadic macros', $false, ''),
    @('complex-arithmetic', 'complex arithmetic', $true,
        "(?:_Complex is not yet supported|'\{' expected \(got ';'\))"),
    @('complex-constants', 'complex imaginary constants', $true,
        '(?:_Complex is not yet supported|invalid number)'),
    @('type-generic-macros', 'type-generic macros', $true, ''),
    @('fenv-access-pragma', 'FENV_ACCESS pragma', $false,
        '#pragma STDC ignored'),
    @('fp-contract-pragma', 'FP_CONTRACT pragma', $false,
        '#pragma STDC ignored'),
    @('cx-limited-range-pragma', 'CX_LIMITED_RANGE pragma', $false,
        '#pragma STDC ignored')
)

$probeResults = foreach ($probe in $probes) {
    $source = Join-Path $repoRoot "tests/c99/capabilities/$($probe[0]).c"
    $object = Join-Path $buildDirectory "$($probe[0]).o"
    if (Test-Path -LiteralPath $object) {
        Remove-Item -LiteralPath $object -Force
    }
    $diagnostic = @(& $tinyCcPath -std=c99 -Wall -Werror -c $source `
        -o $object 2>&1)
    $supported = $LASTEXITCODE -eq 0 -and
        (Test-Path -LiteralPath $object -PathType Leaf)
    $diagnosticText = ($diagnostic |
        ForEach-Object { $_.ToString() }) -join "`n"
    $diagnosticText = $diagnosticText.Replace(
        $repoRoot.Replace('\', '/') + '/', '')
    $expectedFailure = -not $supported -and
        -not [string]::IsNullOrWhiteSpace($probe[3]) -and
        $diagnostic.Count -eq 1 -and
        $diagnosticText -match ('(?s)^[^\r\n]*' +
            [regex]::Escape("$($probe[0]).c") + ':\d+: error: ' +
            $probe[3] + '\s*$')
    [PSCustomObject]@{
        Facility = $probe[1]
        Status = if ($supported) { 'Supported' }
            elseif ($expectedFailure) { 'ExpectedFail' }
            else { 'Unsupported' }
        RequiredForCurrentBaseline = [bool]$probe[2]
        ExpectedFailure = $expectedFailure
        Source = [IO.Path]::GetRelativePath($repoRoot, $source).
            Replace('\', '/')
        Diagnostic = $diagnosticText
    }
}

$pointerSize = if ($Architecture -eq 'x86') { 4 } else { 8 }
$dataModelSource = Join-Path $repoRoot `
    'tests/c99/capabilities/data-model.c'
$dataModelObject = Join-Path $buildDirectory 'data-model.o'
if (Test-Path -LiteralPath $dataModelObject) {
    Remove-Item -LiteralPath $dataModelObject -Force
}
$dataModelDiagnostic = @(
    & $tinyCcPath -std=c99 -Wall -Werror `
        "-DWCRT_EXPECT_POINTER_SIZE=$pointerSize" `
        -I (Join-Path $repoRoot 'include') -c $dataModelSource `
        -o $dataModelObject 2>&1
)
$dataModelPass = $LASTEXITCODE -eq 0 -and
    (Test-Path -LiteralPath $dataModelObject -PathType Leaf)
$dataModel = [PSCustomObject]@{
    Status = if ($dataModelPass) { 'Pass' } else { 'Fail' }
    Model = if ($Architecture -eq 'x86') { 'ILP32' } else { 'LLP64' }
    Char = 1
    Short = 2
    Int = 4
    Long = 4
    LongLong = 8
    Pointer = $pointerSize
    SizeT = $pointerSize
    PtrdiffT = $pointerSize
    WcharT = 2
    Bool = 1
    Float = 4
    Double = 8
    LongDouble = 8
    Diagnostic = ($dataModelDiagnostic |
        ForEach-Object { $_.ToString() }) -join "`n"
}

$record = [PSCustomObject]@{
    Architecture = $Architecture
    Compiler = $compiler
    LanguageMode = 'c99'
    DataModel = $dataModel
    Probes = @($probeResults)
}
$jsonPath = Join-Path $outputDirectory 'tinycc-c99-capabilities.json'
$record | ConvertTo-Json -Depth 6 |
    Set-Content -LiteralPath $jsonPath -Encoding utf8NoBOM
$record

$requiredFailures = @(
    $probeResults | Where-Object {
        $_.RequiredForCurrentBaseline -and
        $_.Status -notin 'Supported', 'ExpectedFail'
    }
)
if (-not $dataModelPass -or $requiredFailures.Count -ne 0) {
    throw "TinyCC capability baseline failed on $Architecture. Results: $jsonPath"
}
