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
if (-not (Test-Path -LiteralPath $TinyCc -PathType Leaf)) {
    throw 'TinyCC was not found.'
}

& (Join-Path $PSScriptRoot 'Verify-T6ExistingHeaders.ps1') | Out-Null
$buildDirectory = Join-Path $repoRoot 'build/tests/c99/tc-0040'
New-Item -ItemType Directory -Force $buildDirectory | Out-Null
$include = Join-Path $repoRoot 'include'
$behavior = Join-Path $buildDirectory 'existing-headers.exe'
$behaviorArguments = @(
    '-std=c99', '-Wall', '-Werror', '-I', $include,
    (Join-Path $repoRoot 'src/ctype.c'),
    (Join-Path $repoRoot 'src/locale.c'),
    (Join-Path $repoRoot 'src/assert.c'),
    (Join-Path $repoRoot 'tests/c99/existing-headers.c'),
    '-o', $behavior
)
& $TinyCc @behaviorArguments
if ($LASTEXITCODE -ne 0) {
    throw 'TC-0040 focused existing-header build failed.'
}

$diagnostic = Join-Path $buildDirectory 'assert-diagnostic.exe'
& $TinyCc -std=c99 -Wall -Werror -I $include `
    (Join-Path $repoRoot 'src/assert.c') `
    (Join-Path $repoRoot 'tests/c99/assert-diagnostic.c') `
    -o $diagnostic
if ($LASTEXITCODE -ne 0) {
    throw 'TC-0040 assertion-diagnostic build failed.'
}

if (-not $CompileOnly) {
    & $behavior
    if ($LASTEXITCODE -ne 0) {
        throw "TC-0040 focused behavior failed with $LASTEXITCODE."
    }
    $output = @(& $diagnostic 2>&1)
    $exitCode = $LASTEXITCODE
    $text = ($output | ForEach-Object { $_.ToString() }) -join "`n"
    if ($exitCode -eq 0 -or
        $text -notmatch 'Assertion failed: 0' -or
        $text -notmatch 'assert-diagnostic\.c' -or
        $text -notmatch 'line [0-9]+' -or
        $text -notmatch 'function main') {
        throw "C99 assertion diagnostic is incomplete: $text"
    }
}

[PSCustomObject]@{
    TestCase = 'TC-0040'
    Requirement = 'REQ-0040'
    Status = 'Pass'
    Runtime = if ($CompileOnly) { 'Blocked' } else { 'Pass' }
    HeaderFamilies = 15
}
