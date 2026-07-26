[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'

[PSCustomObject]@{
    TestCase = 'TC-0017'
    Requirement = 'REQ-0017'
    Status = 'Not run'
    Rationale = 'The optional WCRT console startup object is not implemented.'
    Output = @(
        'Controlled specification: docs/tc-0017-optional-console-startup.tex'
        'Required artifact: wcrt-startup-console.o'
        'Execution was not attempted because the required artifact is absent.'
    ) -join "`n"
}
