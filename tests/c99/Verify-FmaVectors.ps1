[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$TinyCc,
    [switch]$CompileOnly
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$python = Get-Command python.exe -ErrorAction SilentlyContinue |
    Select-Object -First 1 -ExpandProperty Source
if (-not $python) {
    throw 'Python is required to reproduce the controlled fma vectors.'
}
$generator = Join-Path $repoRoot 'tools/generate-fma-vectors.py'
$evidence = Join-Path $repoRoot 'tests/c99/data/fma-vectors.json'
$generated = (& $python $generator 2>&1) -join "`n"
if ($LASTEXITCODE -ne 0) {
    throw "Fma vector generation failed:`n$generated"
}
$retained = (Get-Content -LiteralPath $evidence -Raw).TrimEnd()
if ($generated.TrimEnd() -cne $retained) {
    throw 'Controlled fma vector evidence differs from its generator.'
}
$record = $retained | ConvertFrom-Json
if ($record.vectorCount -ne @($record.vectors).Count) {
    throw 'Controlled fma vector count is inconsistent.'
}

$buildDirectory = Join-Path $repoRoot 'build/tests/c99/fma-vectors'
New-Item -ItemType Directory -Force $buildDirectory | Out-Null
$source = Join-Path $buildDirectory 'fma-vectors.c'
$executable = Join-Path $buildDirectory 'fma-vectors.exe'
$lines = [Collections.Generic.List[string]]::new()
$lines.Add('#include <math.h>')
$lines.Add('union shape { double value; unsigned long long bits; };')
$lines.Add('struct vector { unsigned long long a, b, c, expected; };')
$lines.Add('static const struct vector vectors[] = {')
foreach ($item in $record.vectors) {
    $lines.Add("    {$($item.lhs)ULL, $($item.middle)ULL, " +
        "$($item.rhs)ULL, $($item.expected)ULL},")
}
$lines.Add('};')
$lines.Add('int main(void) {')
$lines.Add('    unsigned int i;')
$lines.Add('    union shape a, b, c, result;')
$lines.Add('    for (i = 0; i < sizeof(vectors) / sizeof(vectors[0]); ++i) {')
$lines.Add('        a.bits = vectors[i].a; b.bits = vectors[i].b;')
$lines.Add('        c.bits = vectors[i].c;')
$lines.Add('        result.value = fma(a.value, b.value, c.value);')
$lines.Add('        if (result.bits != vectors[i].expected) return 1;')
$lines.Add('    }')
$lines.Add('    return 0;')
$lines.Add('}')
Set-Content -LiteralPath $source -Encoding ascii -Value $lines

& $TinyCc -std=c99 -Wall -Werror -I (Join-Path $repoRoot 'include') `
    (Join-Path $repoRoot 'src/errno.c') `
    (Join-Path $repoRoot 'src/fenv.c') `
    (Join-Path $repoRoot 'src/math.c') $source -o $executable
if ($LASTEXITCODE -ne 0) {
    throw 'Controlled fma vector build failed.'
}
if (-not $CompileOnly) {
    & $executable
    if ($LASTEXITCODE -ne 0) {
        throw 'Controlled exact-rational fma vectors failed.'
    }
}

[PSCustomObject]@{
    Status = 'Pass'
    Vectors = $record.vectorCount
    Runtime = if ($CompileOnly) { 'Blocked' } else { 'Pass' }
    Generator = 'tools/generate-fma-vectors.py'
    Evidence = 'tests/c99/data/fma-vectors.json'
}
