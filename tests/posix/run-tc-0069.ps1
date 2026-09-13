[CmdletBinding()]
param([string]$TinyCc)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $root 'tests\mscompat\test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$build = Join-Path $root 'build\tests\posix\tc-0069'
New-Item -ItemType Directory -Force -Path $build | Out-Null
$include = Join-Path $root 'include'
$tinyCcInclude = Join-Path (Split-Path -Parent $TinyCc) 'include'
foreach ($mode in 'c89', 'c99') {
    foreach ($fixture in 'presence\getopt.c', 'absence\getopt.c') {
        $object = Join-Path $build (($fixture -replace '\\', '-') + ".$mode.o")
        & $TinyCc "-std=$mode" -Wall -Werror -nostdinc -I $include `
            -I $tinyCcInclude -c (Join-Path $PSScriptRoot $fixture) -o $object
        if ($LASTEXITCODE -ne 0) { throw "TC-0069 header fixture failed." }
    }
}
$executable = Join-Path $build 'test.exe'
& $TinyCc -std=c89 -Wall -Werror -I $include `
    (Join-Path $root 'src\getopt.c') (Join-Path $PSScriptRoot 'getopt.c') `
    -o $executable
if ($LASTEXITCODE -ne 0) { throw 'TC-0069 behavior build failed.' }
& $executable
if ($LASTEXITCODE -ne 0) { throw "TC-0069 failed with $LASTEXITCODE." }
[PSCustomObject]@{TestCase='TC-0069'; Requirement='REQ-0069'; Status='Pass'}
