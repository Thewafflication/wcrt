[CmdletBinding()]
param([string]$TinyCc)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$buildDirectory = Join-Path $repoRoot 'build\tests\mscompat\tc-0043'
$presenceObject = Join-Path $buildDirectory 'ms-io-presence.o'
$header = Join-Path $repoRoot 'include\io.h'
if ([string]::IsNullOrWhiteSpace($TinyCc)) {
    $TinyCc = Get-Command tcc.exe -ErrorAction SilentlyContinue |
        Select-Object -First 1 -ExpandProperty Source
}
if (-not $TinyCc) {
    $TinyCc = Join-Path (Split-Path -Parent $repoRoot) `
        'tcc_package\out\build\x64-debug\package\tcc.exe'
}
if (-not (Test-Path -LiteralPath $TinyCc)) {
    throw 'TinyCC was not found.'
}
if (-not (Test-Path -LiteralPath $header)) {
    throw 'TC-0043 failed: include\io.h is missing.'
}
New-Item -ItemType Directory -Force -Path $buildDirectory | Out-Null
$common = @('-std=c89', '-Wall', '-Werror', '-I',
    (Join-Path $repoRoot 'include'))
$presence = & $TinyCc @common -c `
    (Join-Path $repoRoot 'tests\mscompat\presence\io.c') `
    -o $presenceObject 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "TC-0043 presence build failed:`n$($presence | Out-String)"
}
$controlled = @('_unlink', '_access', '_get_osfhandle', '_close', '_sopen',
    '_read', '_write', '_lseek', '_tell', '_commit', '_isatty', '_setmode',
    '_fdopen', '_dup', '_dup2', '_pipe', '_lseeki64', '_telli64',
    '_filelength', '_filelengthi64', '_eof', '_open_osfhandle', '_chsize',
    'wcrt_findfirst32', 'wcrt_findnext32', 'wcrt_findclose', '_locking', '_creat',
    'wcrt_findfirst32i64', 'wcrt_findnext32i64', '_umask')
$headerText = Get-Content -LiteralPath $header -Raw
$declarations = @([regex]::Matches($headerText,
    '(?m)^[^#\r\n;{}]*\b((?:_|wcrt_)[A-Za-z0-9_]+)\s*\(') |
    ForEach-Object { $_.Groups[1].Value })
foreach ($name in $declarations) {
    if ($name -notin $controlled) {
        throw "TC-0043 exposed uncontrolled interface $name."
    }
}
foreach ($name in $controlled) {
    if ($name -notin $declarations) {
        throw "TC-0043 controlled interface disappeared: $name."
    }
}
[PSCustomObject]@{
    TestCase = 'TC-0043'
    Requirement = 'REQ-0043'
    HeaderIsolation = 'Pass'
    ControlledSurface = 'Pass'
    ExitCode = 0
}
