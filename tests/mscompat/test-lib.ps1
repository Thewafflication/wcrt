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
        [string]$LanguageMode = 'c89'
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
            $_.Name -notin 'startup_console.c', 'startup_gui.c'
        } | Select-Object -ExpandProperty FullName
    $executable = Join-Path $buildDirectory 'test.exe'
    $lines = @(& $TinyCc @common @sources $BehaviorSource `
        -o $executable 2>&1)
    if ($LASTEXITCODE -ne 0) {
        throw "$TestCase behavior build failed:`n$($lines -join "`n")"
    }
    & $executable @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "$TestCase behavior failed with code $LASTEXITCODE."
    }
    [PSCustomObject]@{ TestCase = $TestCase; ExitCode = 0 }
}
