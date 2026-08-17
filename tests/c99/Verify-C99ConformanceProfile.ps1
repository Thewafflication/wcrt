[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$profilePath = Join-Path $repoRoot 'docs/c99-conformance-profile.md'
if (-not (Test-Path -LiteralPath $profilePath -PathType Leaf)) {
    throw "The C99 conformance profile is missing: $profilePath"
}
$profile = Get-Content -LiteralPath $profilePath -Raw

foreach ($number in 1..25) {
    if ($profile -notmatch "(?m)^\| F-[0-9]+ \| 7\.$number(?:\D|$)") {
        throw "The profile has no facility row for clause 7.$number."
    }
}

$headers = @(
    'assert.h', 'complex.h', 'ctype.h', 'errno.h', 'fenv.h', 'float.h',
    'inttypes.h', 'iso646.h', 'limits.h', 'locale.h', 'math.h', 'setjmp.h',
    'signal.h', 'stdarg.h', 'stdbool.h', 'stddef.h', 'stdint.h', 'stdio.h',
    'stdlib.h', 'string.h', 'tgmath.h', 'time.h', 'wchar.h', 'wctype.h'
)
foreach ($header in $headers) {
    if ($profile -notmatch [regex]::Escape("<$header>")) {
        throw "The profile does not classify <$header>."
    }
}

$normative = @(
    'required', 'optional', 'recommended-practice',
    'implementation-defined', 'unspecified', 'out-of-profile'
)
$dispositions = @(
    'implemented', 'optional-omitted', 'compiler-blocked',
    'deviating', 'not-applicable'
)
$targetStates = @('Pass', 'Fail', 'Unknown', 'Blocked', 'N/A')
$rows = @(Get-Content -LiteralPath $profilePath | Where-Object {
    $_ -match '^\| F-[0-9]+ \|'
})
if ($rows.Count -lt 25) {
    throw 'The profile has fewer facility rows than library clauses.'
}
foreach ($row in $rows) {
    $cells = @($row.Trim('|').Split('|') | ForEach-Object { $_.Trim() })
    if ($cells.Count -ne 12) {
        throw "Profile row has $($cells.Count) columns instead of 12: $row"
    }
    if ($cells[3] -notin $normative) {
        throw "Invalid normative class in profile row: $row"
    }
    if ($cells[4] -notin $dispositions) {
        throw "Invalid disposition in profile row: $row"
    }
    foreach ($state in $cells[5..7]) {
        if ($state -notin $targetStates) {
            throw "Invalid target evidence state in profile row: $row"
        }
    }
    foreach ($reference in @($cells[8..10])) {
        if ([string]::IsNullOrWhiteSpace($reference) -or
            $reference -eq '--') {
            throw "Profile row lacks a direct reference: $row"
        }
    }
}

[PSCustomObject]@{
    Status = 'Pass'
    Clauses = 25
    Headers = $headers.Count
    FacilityRows = $rows.Count
}
