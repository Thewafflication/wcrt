function Resolve-WcrtTinyCc {
    param([string]$TinyCc, [string]$RepositoryRoot)
    if ([string]::IsNullOrWhiteSpace($TinyCc)) {
        $TinyCc = Get-Command tcc.exe -ErrorAction SilentlyContinue |
            Select-Object -First 1 -ExpandProperty Source
    }
    if (-not $TinyCc) {
        $TinyCc = Join-Path (Split-Path -Parent $RepositoryRoot) `
            'tcc_package\out\build\x64-debug\package\tcc.exe'
    }
    if (-not (Test-Path -LiteralPath $TinyCc -PathType Leaf)) {
        throw 'TinyCC was not found.'
    }
    (Resolve-Path -LiteralPath $TinyCc).Path
}

function Invoke-WcrtCompatibilityTest {
    param(
        [Parameter(Mandatory)][string]$TestCase,
        [Parameter(Mandatory)][string]$RepositoryRoot,
        [Parameter(Mandatory)][string]$TinyCc,
        [Parameter(Mandatory)][string]$PresenceSource,
        [Parameter(Mandatory)][string]$BehaviorSource,
        [string[]]$Arguments = @(),
        [string]$LanguageMode = 'c89',
        [string[]]$ExcludedSources = @(),
        [string[]]$AdditionalSources = @(),
        [switch]$CompileOnly
    )
    $buildDirectory = Join-Path $RepositoryRoot `
        "build\tests\mscompat\$($TestCase.ToLowerInvariant())"
    New-Item -ItemType Directory -Force -Path $buildDirectory | Out-Null
    $common = @("-std=$LanguageMode", '-Wall', '-Werror', '-I',
        (Join-Path $RepositoryRoot 'include'))
    $presenceObject = Join-Path $buildDirectory 'presence.o'
    $lines = @(& $TinyCc @common -c $PresenceSource -o $presenceObject 2>&1)
    if ($LASTEXITCODE -ne 0) {
        throw "$TestCase presence build failed:`n$($lines -join "`n")"
    }
    $sources = Get-ChildItem (Join-Path $RepositoryRoot 'src') -Recurse `
        -Filter '*.c' | Where-Object {
            $_.Name -notin 'startup_console.c', 'startup_gui.c', 'complex.c' -and
                $_.Name -notin $ExcludedSources
        } | Select-Object -ExpandProperty FullName
    $executable = Join-Path $buildDirectory 'test.exe'
    $lines = @(& $TinyCc @common @sources @AdditionalSources $BehaviorSource `
        -o $executable 2>&1)
    if ($LASTEXITCODE -ne 0) {
        throw "$TestCase behavior build failed:`n$($lines -join "`n")"
    }
    if (-not $CompileOnly) {
        & $executable @Arguments
        if ($LASTEXITCODE -ne 0) {
            throw "$TestCase behavior failed with code $LASTEXITCODE."
        }
    }
    [PSCustomObject]@{
        TestCase = $TestCase
        ExitCode = $(if ($CompileOnly) { $null } else { 0 })
        Execution = $(if ($CompileOnly) { 'Not run (compile/link only)' }
            else { 'Pass' })
    }
}

function Invoke-WcrtPackagedCompatibilityTest {
    param(
        [Parameter(Mandatory)][string]$TestCase,
        [Parameter(Mandatory)][string]$RepositoryRoot,
        [Parameter(Mandatory)][string]$TinyCc,
        [Parameter(Mandatory)][string]$RuntimeDirectory,
        [Parameter(Mandatory)][string]$BehaviorSource,
        [string[]]$Arguments = @(),
        [switch]$CompileOnly
    )
    . (Join-Path $RepositoryRoot 'tests/c89/startup-test-lib.ps1')
    $description = (& $TinyCc -v 2>&1 | Select-Object -First 1).ToString()
    $architecture = if ($description -match 'AArch64') { 'arm64' }
        elseif ($description -match 'x86_64') { 'x64' } else { 'x86' }
    $compiler = $TinyCc
    if ((Split-Path -Leaf $TinyCc) -eq 'tcc-diagnostic-wrapper.cmd') {
        $compiler = (Resolve-Path -LiteralPath $env:WCRT_TEST_TINYCC).Path
    }
    $support = Get-WcrtStartupToolchain $compiler $architecture
    foreach ($kind in 'static', 'dll') {
        $library = Join-Path $RuntimeDirectory $(if ($kind -eq 'static') {
            'libwcrt.a'
        } else { 'wcrt.def' })
        $options = @()
        if ($kind -eq 'dll') { $options += '-DWCRT_DLL=1' }
        $executable = Join-Path $RuntimeDirectory "$TestCase-$kind.exe"
        & $TinyCc -std=c89 -Wall -Werror -nostdlib '-Wl,-nostdlib' `
            '-Wl,-subsystem=console' @options `
            -I (Join-Path $RuntimeDirectory 'include') `
            (Join-Path $RuntimeDirectory 'wcrt-startup-console.o') `
            $BehaviorSource $library $support.CompilerSupport `
            $support.KernelDefinition -o $executable
        if ($LASTEXITCODE -ne 0) { throw "$TestCase $kind package link failed." }
        if ((Get-WcrtPeInformation $executable).HostCrtImport) {
            throw "$TestCase $kind package imported a host CRT."
        }
        if (-not $CompileOnly) {
            & $executable @Arguments
            if ($LASTEXITCODE -ne 0) {
                throw "$TestCase $kind package failed with $LASTEXITCODE."
            }
        }
    }
}
