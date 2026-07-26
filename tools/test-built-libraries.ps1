[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('x86', 'x64', 'arm64')]
    [string]$Architecture,

    [Parameter(Mandatory = $true)]
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration,

    [Parameter(Mandatory = $true)]
    [string]$TinyCc,

    [switch]$CompileOnly,

    [string]$BuildRoot = 'output/build'
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$buildDirectory = Join-Path $repoRoot "$BuildRoot/$Architecture/$Configuration"
$smokeDirectory = Join-Path $buildDirectory 'link-smoke'
$source = Join-Path $smokeDirectory 'consumer.c'
New-Item -ItemType Directory -Force -Path $smokeDirectory | Out-Null
Set-Content -LiteralPath $source -Encoding ascii -Value @(
    '#include <string.h>'
    'int main(void) { return strlen("wcrt") == 4 ? 0 : 1; }'
)

$include = Join-Path $buildDirectory 'include'
$staticExecutable = Join-Path $smokeDirectory 'static-consumer.exe'
$dynamicExecutable = Join-Path $smokeDirectory 'dll-consumer.exe'
& $TinyCc -std=c89 -Wall -Werror -I $include $source `
    (Join-Path $buildDirectory 'libwcrt.a') -o $staticExecutable
if ($LASTEXITCODE -ne 0) { throw 'Static-library consumer failed to link.' }
& $TinyCc -std=c89 -Wall -Werror -I $include $source `
    (Join-Path $buildDirectory 'wcrt.def') -o $dynamicExecutable
if ($LASTEXITCODE -ne 0) { throw 'DLL consumer failed to link.' }
Copy-Item -LiteralPath (Join-Path $buildDirectory 'wcrt.dll') `
    -Destination $smokeDirectory -Force

if (-not $CompileOnly) {
    & $staticExecutable
    if ($LASTEXITCODE -ne 0) { throw 'Static-library consumer failed at runtime.' }
    & $dynamicExecutable
    if ($LASTEXITCODE -ne 0) { throw 'DLL consumer failed at runtime.' }
}

[PSCustomObject]@{
    Architecture = $Architecture
    StaticLink = 'Pass'
    DllLink = 'Pass'
    Runtime = if ($CompileOnly) { 'Deferred' } else { 'Pass' }
}
