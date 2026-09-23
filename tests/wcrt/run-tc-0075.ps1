[CmdletBinding()]
param([string]$TinyCc)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $root 'tests/mscompat/test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$build = Join-Path $root 'build/tests/wcrt/tc-0075'
New-Item -ItemType Directory -Force -Path $build | Out-Null
$sources = Get-ChildItem (Join-Path $root 'src') -Recurse -Filter '*.c' |
    Where-Object { $_.Name -notin 'startup_console.c', 'startup_gui.c',
        'complex.c', 'cpu.c' } | Select-Object -ExpandProperty FullName
foreach ($test in 'cpu', 'cpu_fallback') {
    $inputs = @($sources)
    if ($test -eq 'cpu') {
        $inputs += Join-Path $root 'src/platform/windows/cpu.c'
    }
    $executable = Join-Path $build "$test.exe"
    & $TinyCc -std=c89 -Wall -Werror -I (Join-Path $root 'include') `
        @inputs (Join-Path $PSScriptRoot "$test.c") -o $executable
    if ($LASTEXITCODE -ne 0) { throw "TC-0075 $test build failed." }
    & $executable
    if ($LASTEXITCODE -ne 0) {
        throw "TC-0075 $test failed with $LASTEXITCODE."
    }
}
. (Join-Path $root 'tests/c89/startup-test-lib.ps1')
$description = (& $TinyCc -v 2>&1 | Select-Object -First 1).ToString()
$architecture = if ($description -match 'AArch64') { 'arm64' }
    elseif ($description -match 'x86_64') { 'x64' } else { 'x86' }
$toolchainCompiler = $TinyCc
if ((Split-Path -Leaf $TinyCc) -eq 'tcc-diagnostic-wrapper.cmd') {
    $toolchainCompiler = (Resolve-Path -LiteralPath $env:WCRT_TEST_TINYCC).Path
}
$toolchain = Get-WcrtStartupToolchain $toolchainCompiler $architecture
foreach ($subsystem in 'console', 'gui') {
    $startup = Join-Path $build "$subsystem.o"
    & $TinyCc -std=c89 -Wall -Werror -I (Join-Path $root 'include') -c `
        (Join-Path $root "src/platform/windows/startup_$subsystem.c") `
        -o $startup
    if ($LASTEXITCODE -ne 0) { throw 'CPU startup-order compile failed.' }
    $executable = Join-Path $build "startup-$subsystem.exe"
    $selection = @()
    if ($subsystem -eq 'gui') { $selection += '-DTEST_GUI=1' }
    & $TinyCc -std=c89 -Wall -Werror -nostdlib '-Wl,-nostdlib' `
        "-Wl,-subsystem=$subsystem" -I (Join-Path $root 'include') `
        @selection $startup @sources (Join-Path $PSScriptRoot 'cpu_startup.c') `
        $toolchain.CompilerSupport $toolchain.KernelDefinition -o $executable
    if ($LASTEXITCODE -ne 0) { throw 'CPU startup-order link failed.' }
    if ((Invoke-WcrtStartupProcess $executable '' -Hidden) -ne 0) {
        throw "CPU $subsystem initialization did not precede entry."
    }
}
[PSCustomObject]@{TestCase='TC-0075'; Requirement='REQ-0075'; Status='Pass'}
