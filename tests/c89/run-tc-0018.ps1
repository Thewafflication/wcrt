[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'

[PSCustomObject]@{
    TestCase = 'TC-0018'
    Requirement = 'REQ-0018'
    Status = 'Not run'
    Rationale = 'The optional WCRT GUI startup object is not implemented.'
    Output = @(
        'Controlled specification: docs/tc-0018-optional-gui-startup.tex'
        'Required artifact: wcrt-startup-gui.o'
        'Execution was not attempted because the required artifact is absent.'
    ) -join "`n"
}
