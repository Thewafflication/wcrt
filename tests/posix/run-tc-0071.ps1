[CmdletBinding()]
param([string]$TinyCc)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $root 'tests\mscompat\test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$build = Join-Path $root 'build\tests\posix\tc-0071'
New-Item -ItemType Directory -Force -Path $build | Out-Null
$include = Join-Path $root 'include'
$tinyCcInstallation = if ($TinyCc -like '*tcc-diagnostic-wrapper.cmd' -and
    $env:WCRT_TEST_TINYCC) { $env:WCRT_TEST_TINYCC } else { $TinyCc }
$tinyCcInclude = Join-Path (Split-Path -Parent $tinyCcInstallation) 'include'
foreach ($fixture in 'tests\posix\presence\pthread.c',
    'tests\posix\absence\pthread.c') {
    $object = Join-Path $build (($fixture -replace '\\', '-') + '.o')
    & $TinyCc -std=c89 -Wall -Werror -nostdinc -I $include -I $tinyCcInclude `
        -c (Join-Path $root $fixture) -o $object
    if ($LASTEXITCODE -ne 0) { throw 'TC-0071 header fixture failed.' }
}
$sources = Get-ChildItem (Join-Path $root 'src') -Recurse -Filter '*.c' |
    Where-Object { $_.Name -notin 'startup_console.c', 'startup_gui.c',
        'complex.c' } | Select-Object -ExpandProperty FullName
$executable = Join-Path $build 'test.exe'
& $TinyCc -std=c89 -Wall -Werror -I $include @sources `
    (Join-Path $PSScriptRoot 'pthread.c') -o $executable
if ($LASTEXITCODE -ne 0) { throw 'TC-0071 behavior build failed.' }
& $executable
if ($LASTEXITCODE -ne 0) { throw "TC-0071 failed with $LASTEXITCODE." }
[PSCustomObject]@{TestCase='TC-0071'; Requirement='REQ-0071'; Status='Pass'}
