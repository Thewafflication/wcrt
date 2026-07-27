& (Join-Path $PSScriptRoot 'Invoke-PlannedTest.ps1') -TestCase TC-0022 `
    -RequiredPath 'include/stdint.h', 'tests/c99/presence/stdint.c'
exit $LASTEXITCODE
