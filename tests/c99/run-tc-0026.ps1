& (Join-Path $PSScriptRoot 'Invoke-PlannedTest.ps1') -TestCase TC-0026 `
    -RequiredPath 'include/iso646.h', 'tests/c99/presence/iso646.c'
exit $LASTEXITCODE
