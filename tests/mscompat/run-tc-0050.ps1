[CmdletBinding()]
param([string]$TinyCc)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$result = Invoke-WcrtCompatibilityTest -TestCase TC-0050 `
    -RepositoryRoot $root -TinyCc $TinyCc -LanguageMode c99 `
    -PresenceSource (Join-Path $PSScriptRoot 'presence\wpm-snprintf.c') `
    -BehaviorSource (Join-Path $PSScriptRoot 'wpm-snprintf.c')
$wpmSource = Join-Path (Split-Path -Parent $root) `
    'wpm\wpm\tcc_compat\secure_crt.c'
if (Test-Path -LiteralPath $wpmSource) {
    $text = Get-Content -LiteralPath $wpmSource -Raw
    if ($text -match 'int\s+wpm_snprintf\s*\(') {
        throw 'WPM still defines the local wpm_snprintf compatibility bridge.'
    }
}
$result
