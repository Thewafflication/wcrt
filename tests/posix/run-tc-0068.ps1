[CmdletBinding()]
param([string]$TinyCc)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $root 'tests\mscompat\test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$build = Join-Path $root 'build\tests\posix\tc-0068'
New-Item -ItemType Directory -Force -Path $build | Out-Null
$include = Join-Path $root 'include'
$tinyCcInstallation = if ($TinyCc -like '*tcc-diagnostic-wrapper.cmd' -and
    $env:WCRT_TEST_TINYCC) { $env:WCRT_TEST_TINYCC } else { $TinyCc }
$tinyCcInclude = Join-Path (Split-Path -Parent $tinyCcInstallation) 'include'
foreach ($mode in 'c89', 'c99') {
    foreach ($fixture in 'presence\fnmatch.c', 'absence\fnmatch.c') {
        $object = Join-Path $build (($fixture -replace '\\', '-') + ".$mode.o")
        & $TinyCc "-std=$mode" -Wall -Werror -nostdinc -I $include `
            -I $tinyCcInclude -c (Join-Path $PSScriptRoot $fixture) -o $object
        if ($LASTEXITCODE -ne 0) { throw "TC-0068 header fixture failed." }
    }
}
$executable = Join-Path $build 'test.exe'
& $TinyCc -std=c89 -Wall -Werror -I $include `
    (Join-Path $root 'src\ctype.c') (Join-Path $root 'src\fnmatch.c') `
    (Join-Path $PSScriptRoot 'fnmatch.c') -o $executable
if ($LASTEXITCODE -ne 0) { throw 'TC-0068 behavior build failed.' }
& $executable
if ($LASTEXITCODE -ne 0) { throw "TC-0068 failed with $LASTEXITCODE." }
[PSCustomObject]@{TestCase='TC-0068'; Requirement='REQ-0068'; Status='Pass'}
