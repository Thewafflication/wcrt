$repositoryRoot = Split-Path -Parent $PSScriptRoot
$loggingModule = Join-Path $repositoryRoot `
    'wsp\tools\logging\Wsp.Logging.psm1'

if (-not (Test-Path -LiteralPath $loggingModule)) {
    throw "The pinned WSP logging module was not found: $loggingModule"
}

Import-Module $loggingModule -Force
Set-WspLogConfiguration -ConsoleLevel Info -FileLevel Off -Color Auto

function Write-WcrtTestResult {
    param(
        [Parameter(Mandatory)]
        [ValidateSet('Pass', 'Fail', 'ExpectedFail')]
        [string]$Status,

        [Parameter(Mandatory)]
        [string]$Message
    )

    if ($Status -eq 'Pass') {
        Write-WspPass $Message
    }
    elseif ($Status -eq 'ExpectedFail') {
        Write-WspWarning $Message
    }
    else {
        Write-WspError $Message
    }
}
