[CmdletBinding()]
param(
    [string]$RepositoryRoot = (Split-Path -Parent $PSScriptRoot)
)

$ErrorActionPreference = 'Stop'
$tool = Join-Path $RepositoryRoot 'tools/wsp/tools/Test-Traceability.ps1'

& $tool `
    -RepositoryRoot $RepositoryRoot `
    -RequirementsPath 'docs' `
    -TestSpecificationsPath 'docs' `
    -TestImplementationsPath 'tests/c89' `
    -TraceabilityPath 'tests/c89/manifest.md'

if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}
