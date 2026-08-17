[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)

$families = @(
    @('assert.h', 'src/assert.c', 'tests/c89/run-tc-0001.ps1'),
    @('ctype.h', 'src/ctype.c', 'tests/c89/run-tc-0002.ps1'),
    @('errno.h', 'src/errno.c', 'tests/c89/run-tc-0003.ps1'),
    @('float.h', 'include/float.h', 'tests/c99/run-tc-0024.ps1'),
    @('limits.h', 'include/limits.h', 'tests/c99/run-tc-0023.ps1'),
    @('locale.h', 'src/locale.c', 'tests/c89/run-tc-0006.ps1'),
    @('math.h', 'src/math.c', 'tests/c99/run-tc-0035.ps1'),
    @('setjmp.h', 'src/setjmp.c', 'tests/c89/run-tc-0008.ps1'),
    @('signal.h', 'src/signal.c', 'tests/c89/run-tc-0009.ps1'),
    @('stdarg.h', 'include/stdarg.h', 'tests/c99/run-tc-0025.ps1'),
    @('stddef.h', 'include/stddef.h', 'tests/c89/run-tc-0011.ps1'),
    @('stdio.h', 'src/stdio.c', 'tests/c99/run-tc-0030.ps1'),
    @('stdlib.h', 'src/stdlib.c', 'tests/c99/run-tc-0039.ps1'),
    @('string.h', 'src/string.c', 'tests/c89/run-tc-0014.ps1'),
    @('time.h', 'src/time.c', 'tests/c89/run-tc-0015.ps1')
)

foreach ($family in $families) {
    $header = Join-Path $repoRoot "include/$($family[0])"
    $implementation = Join-Path $repoRoot $family[1]
    $test = Join-Path $repoRoot $family[2]
    foreach ($path in @($header, $implementation, $test)) {
        if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
            throw "REQ-0040 inventory reference is missing: $path"
        }
    }
}

[PSCustomObject]@{
    Status = 'Pass'
    HeaderFamilies = $families.Count
    References = 3 * $families.Count
}
