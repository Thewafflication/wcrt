& (Join-Path $PSScriptRoot 'Invoke-PlannedTest.ps1') -TestCase TC-0024 `
    -RequiredPath 'tests/c99/float.c'
exit $LASTEXITCODE
