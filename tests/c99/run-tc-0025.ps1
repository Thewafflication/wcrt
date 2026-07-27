& (Join-Path $PSScriptRoot 'Invoke-PlannedTest.ps1') -TestCase TC-0025 `
    -RequiredPath 'tests/c99/va-copy.c'
exit $LASTEXITCODE
