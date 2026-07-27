[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$expected = @{
    'include/stdio.h' = @{
        fopen = 2; freopen = 3; setbuf = 2; setvbuf = 2
        fprintf = 2; fscanf = 2; printf = 1; scanf = 1
        sprintf = 2; sscanf = 2; vfprintf = 2; vprintf = 1
        vsprintf = 2; snprintf = 2; vsnprintf = 2; fgets = 2
        fputs = 2; fread = 2; fwrite = 2; fgetpos = 2; fsetpos = 2
    }
    'include/stdlib.h' = @{
        strtod = 2; strtol = 2; strtoul = 2; mbtowc = 2
        mbstowcs = 2; wcstombs = 2
    }
    'include/string.h' = @{
        memcpy = 2; strcpy = 2; strncpy = 2; strcat = 2
        strncat = 2; strxfrm = 2; strtok = 2
    }
    'include/time.h' = @{ strftime = 3 }
}

foreach ($relativePath in $expected.Keys) {
    $contents = Get-Content (Join-Path $RepositoryRoot $relativePath) -Raw
    foreach ($function in $expected[$relativePath].Keys) {
        $pattern = "\b$function\s*\([^;]+;"
        $declaration = [regex]::Match(
            $contents, $pattern,
            [Text.RegularExpressions.RegexOptions]::Singleline)
        if (-not $declaration.Success) {
            throw "Missing declaration for $function in $relativePath."
        }
        $actual = [regex]::Matches(
            $declaration.Value, '\bWCRT_RESTRICT\b').Count
        $wanted = $expected[$relativePath][$function]
        if ($actual -ne $wanted) {
            throw "$function in $relativePath has $actual restrict markers; " +
                "expected $wanted."
        }
    }
}

[PSCustomObject]@{
    Headers = $expected.Count
    Declarations = ($expected.Values.Keys).Count
    Result = 'Pass'
}
