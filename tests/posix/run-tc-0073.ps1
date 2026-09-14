[CmdletBinding()]
param([string]$TinyCc)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $root 'tests\mscompat\test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$build = Join-Path $root 'build\tests\posix\tc-0073'
New-Item -ItemType Directory -Force -Path $build | Out-Null
$include = Join-Path $root 'include'
$presence = Join-Path $build 'presence.o'
& $TinyCc -std=c89 -Wall -Werror -I $include -c `
    (Join-Path $root 'tests\posix\presence\thread_pool.c') -o $presence
if ($LASTEXITCODE -ne 0) { throw 'TC-0073 presence build failed.' }
$sources = Get-ChildItem (Join-Path $root 'src') -Recurse -Filter '*.c' |
    Where-Object { $_.Name -notin 'startup_console.c', 'startup_gui.c',
        'complex.c' } | Select-Object -ExpandProperty FullName
$executable = Join-Path $build 'test.exe'
& $TinyCc -std=c89 -Wall -Werror -I $include @sources `
    (Join-Path $PSScriptRoot 'thread_pool.c') -o $executable
if ($LASTEXITCODE -ne 0) { throw 'TC-0073 behavior build failed.' }
& $executable
if ($LASTEXITCODE -ne 0) { throw "TC-0073 failed with $LASTEXITCODE." }
[PSCustomObject]@{TestCase='TC-0073'; Requirement='REQ-0073'; Status='Pass'}
