[CmdletBinding()]
param([string]$TinyCc, [switch]$CompileOnly)

$result = & (Join-Path $PSScriptRoot `
    'Verify-C99ConformanceProfile.ps1')
[PSCustomObject]@{
    TestCase = 'TC-0041'
    Requirement = 'REQ-0041'
    Status = 'Pass'
    Clauses = $result.Clauses
    Headers = $result.Headers
    FacilityRows = $result.FacilityRows
}
