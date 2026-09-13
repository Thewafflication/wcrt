[CmdletBinding()]
param([string]$TinyCc)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $root 'tests\mscompat\test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$build = Join-Path $root 'build\tests\posix\tc-0070'
New-Item -ItemType Directory -Force -Path $build | Out-Null
$include = Join-Path $root 'include'
$tinyCcInclude = Join-Path (Split-Path -Parent $TinyCc) 'include'
foreach ($fixture in 'tests\mscompat\presence\popen.c',
    'tests\posix\presence\popen.c', 'tests\posix\absence\popen.c') {
    $object = Join-Path $build (($fixture -replace '\\', '-') + '.o')
    & $TinyCc -std=c89 -Wall -Werror -nostdinc -I $include -I $tinyCcInclude `
        -c (Join-Path $root $fixture) -o $object
    if ($LASTEXITCODE -ne 0) { throw 'TC-0070 header fixture failed.' }
}
$sources = Get-ChildItem (Join-Path $root 'src') -Recurse -Filter '*.c' |
    Where-Object { $_.Name -notin 'startup_console.c', 'startup_gui.c',
        'complex.c' } | Select-Object -ExpandProperty FullName
$executable = Join-Path $build 'test.exe'
& $TinyCc -std=c89 -Wall -Werror -I $include @sources `
    (Join-Path $PSScriptRoot 'popen.c') -o $executable
if ($LASTEXITCODE -ne 0) { throw 'TC-0070 behavior build failed.' }
& $executable (Join-Path $build 'input.txt')
if ($LASTEXITCODE -ne 0) { throw "TC-0070 failed with $LASTEXITCODE." }
[PSCustomObject]@{TestCase='TC-0070'; Requirement='REQ-0070'; Status='Pass'}
