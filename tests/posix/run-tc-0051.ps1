[CmdletBinding()]
param([string]$TinyCc)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $root 'tests\mscompat\test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$build = Join-Path $root 'build\tests\posix\tc-0051'
New-Item -ItemType Directory -Force -Path $build | Out-Null
$include = Join-Path $root 'include'
$tinyCcInstallation = if ($TinyCc -like '*tcc-diagnostic-wrapper.cmd' -and
    $env:WCRT_TEST_TINYCC) {
    $env:WCRT_TEST_TINYCC
} else { $TinyCc }
$tinyCcInclude = Join-Path (Split-Path -Parent $tinyCcInstallation) 'include'
$fixtures = @(
    @{Name = 'selected'; Path = 'presence\files.c'},
    @{Name = 'strict'; Path = 'absence\files.c'}
)
foreach ($mode in 'c89', 'c99') {
    foreach ($fixture in $fixtures) {
        $source = Join-Path $PSScriptRoot $fixture.Path
        $object = Join-Path $build "$($fixture.Name)-$mode.o"
        $output = @(& $TinyCc "-std=$mode" -Wall -Werror -nostdinc `
            -I $include -I $tinyCcInclude -c $source -o $object 2>&1)
        if ($LASTEXITCODE -ne 0) {
            throw "TC-0051 $($fixture.Name) $mode compile failed:`n" +
                ($output -join "`n")
        }
    }
}
$sources = Get-ChildItem (Join-Path $root 'src') -Recurse -Filter '*.c' |
    Where-Object { $_.Name -notin 'startup_console.c', 'startup_gui.c',
        'complex.c' } | Select-Object -ExpandProperty FullName
$executable = Join-Path $build 'test.exe'
$behavior = Join-Path $PSScriptRoot 'files.c'
$output = @(& $TinyCc -std=c89 -Wall -Werror -I $include @sources `
    $behavior -o $executable 2>&1)
if ($LASTEXITCODE -ne 0) {
    throw "TC-0051 behavior build failed:`n$($output -join "`n")"
}
$path = Join-Path $build 'stat-utime.tmp'
& $executable $path
if ($LASTEXITCODE -ne 0) {
    throw "TC-0051 behavior failed with code $LASTEXITCODE."
}
[PSCustomObject]@{TestCase = 'TC-0051'; Requirement = 'REQ-0051';
    Status = 'Pass'}
