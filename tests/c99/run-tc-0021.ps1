& (Join-Path $PSScriptRoot 'Invoke-PlannedTest.ps1') -TestCase TC-0021 `
    -RequiredPath 'include/stdbool.h', 'tests/c99/presence/stdbool.c'
exit $LASTEXITCODE
