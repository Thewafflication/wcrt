[CmdletBinding()]
param([string]$TinyCc)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $root 'tests\mscompat\test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$build = Join-Path $root 'build\tests\posix\tc-0067'
New-Item -ItemType Directory -Force -Path $build | Out-Null
$include = Join-Path $root 'include'
$tinyCcInstallation = if ($TinyCc -like '*tcc-diagnostic-wrapper.cmd' -and
    $env:WCRT_TEST_TINYCC) { $env:WCRT_TEST_TINYCC } else { $TinyCc }
$tinyCcInclude = Join-Path (Split-Path -Parent $tinyCcInstallation) 'include'
$fixtures = @(
    @{Name='microsoft'; Path='tests\mscompat\presence\lowio.c'},
    @{Name='selected'; Path='tests\posix\presence\lowio.c'},
    @{Name='strict'; Path='tests\posix\absence\lowio.c'}
)
foreach ($mode in 'c89', 'c99') {
    foreach ($fixture in $fixtures) {
        $object = Join-Path $build "$($fixture.Name)-$mode.o"
        $output = @(& $TinyCc "-std=$mode" -Wall -Werror -nostdinc `
            -I $include -I $tinyCcInclude -c (Join-Path $root $fixture.Path) `
            -o $object 2>&1)
        if ($LASTEXITCODE -ne 0) {
            throw "TC-0067 $($fixture.Name) $mode failed:`n$($output -join "`n")"
        }
    }
}
$sources = Get-ChildItem (Join-Path $root 'src') -Recurse -Filter '*.c' |
    Where-Object { $_.Name -notin 'startup_console.c', 'startup_gui.c',
        'complex.c' } | Select-Object -ExpandProperty FullName
$executable = Join-Path $build 'test.exe'
$output = @(& $TinyCc -std=c89 -Wall -Werror -I $include @sources `
    (Join-Path $PSScriptRoot 'lowio.c') -o $executable 2>&1)
if ($LASTEXITCODE -ne 0) {
    throw "TC-0067 behavior build failed:`n$($output -join "`n")"
}
& $executable (Join-Path $build 'microsoft.bin') `
    (Join-Path $build 'posix.bin')
if ($LASTEXITCODE -ne 0) {
    throw "TC-0067 behavior failed with code $LASTEXITCODE."
}
[PSCustomObject]@{TestCase='TC-0067'; Requirement='REQ-0067'; Status='Pass'}
