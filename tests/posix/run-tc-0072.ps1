[CmdletBinding()]
param([string]$TinyCc)
$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $root 'tests\mscompat\test-lib.ps1')
$TinyCc=Resolve-WcrtTinyCc $TinyCc $root
$build=Join-Path $root 'build\tests\posix\tc-0072'
New-Item -ItemType Directory -Force -Path $build | Out-Null
$sources=Get-ChildItem (Join-Path $root 'src') -Recurse -Filter '*.c' |
 Where-Object {$_.Name -notin 'startup_console.c','startup_gui.c','complex.c'} |
 Select-Object -ExpandProperty FullName
$exe=Join-Path $build 'test.exe'
& $TinyCc -std=c89 -Wall -Werror -I (Join-Path $root 'include') @sources `
 (Join-Path $root 'tests\posix\mman.c') -o $exe
if($LASTEXITCODE -ne 0){throw 'TC-0072 build failed.'}
& $exe (Join-Path $build 'input.txt')
if($LASTEXITCODE -ne 0){throw "TC-0072 failed with $LASTEXITCODE."}
[PSCustomObject]@{TestCase='TC-0072';Requirement='REQ-0072';Status='Pass'}
