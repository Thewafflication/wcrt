[CmdletBinding()]
param([string]$TinyCc)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $root 'tests\mscompat\test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$build = Join-Path $root 'build\tests\posix\tc-0074'
New-Item -ItemType Directory -Force -Path $build | Out-Null
$include = Join-Path $root 'include'
$sources = Get-ChildItem (Join-Path $root 'src') -Recurse -Filter '*.c' |
    Where-Object { $_.Name -notin 'startup_console.c', 'startup_gui.c',
        'complex.c' } | Select-Object -ExpandProperty FullName
$executable = Join-Path $build 'test.exe'
& $TinyCc -std=c89 -Wall -Werror -I $include @sources `
    (Join-Path $PSScriptRoot 'stdio_threads.c') -o $executable
if ($LASTEXITCODE -ne 0) { throw 'TC-0074 behavior build failed.' }
Push-Location $build
try {
    & $executable
    if ($LASTEXITCODE -ne 0) { throw "TC-0074 failed with $LASTEXITCODE." }
} finally {
    Pop-Location
}
[PSCustomObject]@{TestCase='TC-0074'; Requirement='REQ-0074'; Status='Pass'}
