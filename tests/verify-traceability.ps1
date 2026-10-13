[CmdletBinding()]
param(
    [string]$RepositoryRoot = (Split-Path -Parent $PSScriptRoot)
)

$ErrorActionPreference = 'Stop'
$tool = Join-Path $RepositoryRoot 'wsp/tools/Test-Traceability.ps1'

# Run in a fresh process so successful validation cannot inherit a prior
# native command's nonzero exit status from the calling test suite.
$escapedTool = $tool.Replace("'", "''")
$escapedRoot = $RepositoryRoot.Replace("'", "''")
$validation = @"
& '$escapedTool' -RepositoryRoot '$escapedRoot' -RequirementsPath 'docs' -TestSpecificationsPath 'docs' -TestImplementationsPath 'tests' -TraceabilityPath 'tests/c89/manifest.md','tests/c99/manifest.md','tests/mscompat/manifest.md','tests/posix/manifest.md','tests/wcrt/manifest.md'
"@
& (Get-Process -Id $PID).Path -NoProfile -Command $validation

if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}
