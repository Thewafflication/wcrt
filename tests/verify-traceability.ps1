[CmdletBinding()]
param(
    [string]$RepositoryRoot = (Split-Path -Parent $PSScriptRoot)
)

$ErrorActionPreference = 'Stop'
$tool = Join-Path $RepositoryRoot 'wsp/tools/Test-Traceability.ps1'

& $tool `
    -RepositoryRoot $RepositoryRoot `
    -RequirementsPath 'docs' `
    -TestSpecificationsPath 'docs' `
    -TestImplementationsPath 'tests' `
    -TraceabilityPath 'tests/c89/manifest.md', 'tests/c99/manifest.md', `
        'tests/mscompat/manifest.md'

if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}
