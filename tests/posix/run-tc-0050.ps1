[CmdletBinding()]
param([string]$TinyCc)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $root 'tests\mscompat\test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$build = Join-Path $root 'build\tests\posix\tc-0050'
New-Item -ItemType Directory -Force -Path $build | Out-Null
$include = Join-Path $root 'include'
$tinyCcInstallation = if ($TinyCc -like '*tcc-diagnostic-wrapper.cmd' -and
    $env:WCRT_TEST_TINYCC) {
    $env:WCRT_TEST_TINYCC
} else { $TinyCc }
$tinyCcInclude = Join-Path (Split-Path -Parent $tinyCcInstallation) 'include'
$fixtures = @(
    @{Name = 'selected'; Path = 'presence\profile.c'},
    @{Name = 'strict'; Path = 'absence\profile.c'}
)
foreach ($mode in 'c89', 'c99') {
    foreach ($fixture in $fixtures) {
        $source = Join-Path $PSScriptRoot $fixture.Path
        $object = Join-Path $build "$($fixture.Name)-$mode.o"
        $output = @(& $TinyCc "-std=$mode" -Wall -Werror -nostdinc `
            -I $include -I $tinyCcInclude -c $source -o $object 2>&1)
        if ($LASTEXITCODE -ne 0) {
            throw "TC-0050 $($fixture.Name) $mode compile failed:`n" +
                ($output -join "`n")
        }
    }
}
[PSCustomObject]@{TestCase = 'TC-0050'; Requirement = 'REQ-0050';
    Status = 'Pass'}
