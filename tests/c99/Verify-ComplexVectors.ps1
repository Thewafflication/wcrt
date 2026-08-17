[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$python = Get-Command python.exe -ErrorAction SilentlyContinue |
    Select-Object -First 1 -ExpandProperty Source
if (-not $python) {
    throw 'Python is required to reproduce the controlled complex vectors.'
}
$generator = Join-Path $repoRoot 'tools\generate-complex-vectors.py'
$evidence = Join-Path $repoRoot 'tests\c99\data\complex-vectors.json'
$generated = (& $python $generator 2>&1) -join "`n"
if ($LASTEXITCODE -ne 0) {
    throw "Complex vector generation failed:`n$generated"
}
$retained = (Get-Content -LiteralPath $evidence -Raw).TrimEnd()
if ($generated.TrimEnd() -cne $retained) {
    throw 'Controlled complex vector evidence differs from its generator.'
}

[PSCustomObject]@{
    Status = 'Pass'
    Generator = [IO.Path]::GetRelativePath($repoRoot, $generator).
        Replace('\', '/')
    Evidence = [IO.Path]::GetRelativePath($repoRoot, $evidence).
        Replace('\', '/')
    PrecisionDigits = 100
    Rounding = 'ROUND_HALF_EVEN'
}
