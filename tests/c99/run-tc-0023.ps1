& (Join-Path $PSScriptRoot 'Invoke-PlannedTest.ps1') -TestCase TC-0023 `
    -RequiredPath 'tests/c99/presence/limits.c'
exit $LASTEXITCODE
