[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [string]$TinyCc,

    [string]$BuildDirectory
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$compiler = (& $TinyCc -v 2>&1 | Select-Object -First 1).ToString()
$target = if ($compiler -match 'i386 Windows') { 'x86' }
    elseif ($compiler -match 'x86_64 Windows') { 'x64' }
    elseif ($compiler -match 'AArch64 Windows') { 'arm64' }
    else { 'unknown' }
if ([string]::IsNullOrWhiteSpace($BuildDirectory)) {
    $BuildDirectory = Join-Path $repoRoot `
        "build\tests\c99\complex-capability\$target"
}
New-Item -ItemType Directory -Force -Path $BuildDirectory | Out-Null
$probes = @(
    [PSCustomObject]@{
        Name = 'complex-arithmetic'
        Expected = "(?:_Complex is not yet supported|'\{' expected " +
            "\(got ';'\))"
    }
    [PSCustomObject]@{
        Name = 'complex-constants'
        Expected = '(?:_Complex is not yet supported|invalid number)'
    }
)
$results = foreach ($probe in $probes) {
    $source = Join-Path $repoRoot `
        "tests\c99\capabilities\$($probe.Name).c"
    $object = Join-Path $BuildDirectory "$($probe.Name).o"
    if (Test-Path -LiteralPath $object) {
        Remove-Item -LiteralPath $object -Force
    }
    $diagnosticLines = @(& $TinyCc -std=c99 -Wall -Werror -c $source `
        -o $object 2>&1)
    $diagnostic = ($diagnosticLines |
        ForEach-Object { $_.ToString() }) -join "`n"
    $diagnostic = $diagnostic.Replace(
        $repoRoot.Replace('\', '/') + '/', '')
    $supported = $LASTEXITCODE -eq 0 -and
        (Test-Path -LiteralPath $object -PathType Leaf)
    $expected = -not $supported -and
        $diagnostic -match ('(?s)^[^\r\n]*' +
            [regex]::Escape("$($probe.Name).c") + ':\d+: error: ' +
            $probe.Expected + '\s*$')
    [PSCustomObject]@{
        Probe = $probe.Name
        Supported = $supported
        ExpectedFailure = $expected
        Diagnostic = $diagnostic
    }
}
$supported = @($results | Where-Object { -not $_.Supported }).Count -eq 0
$expected = -not $supported -and
    @($results | Where-Object {
        -not $_.Supported -and -not $_.ExpectedFailure
    }).Count -eq 0
$diagnostic = ($results | Where-Object { -not $_.Supported } |
    ForEach-Object { "$($_.Probe): $($_.Diagnostic)" }) -join "`n"

[PSCustomObject]@{
    Compiler = $compiler
    Target = $target
    Supported = $supported
    ExpectedFailure = $expected
    Diagnostic = $diagnostic
    Probes = @($results)
}
