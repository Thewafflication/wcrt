[CmdletBinding()]
param([string]$TinyCc, [string]$RuntimeDirectory, [switch]$CompileOnly)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$build = Join-Path $root 'build/tests/mscompat/tc-0092'
New-Item -ItemType Directory -Force -Path $build | Out-Null
$fixture = Join-Path $build ("query-$([guid]::NewGuid().ToString('N')).bin")
try {
    foreach ($mode in 'c89', 'c99') {
        & $TinyCc "-std=$mode" -Wall -Werror -DWCRT_POSIX=1 `
            -I (Join-Path $root 'include') -c (Join-Path $PSScriptRoot `
            'vc6-ms-errors.c') -o (Join-Path $build "combined-$mode.o")
        if ($LASTEXITCODE -ne 0) {
            throw 'TC-0092 combined selector compilation failed.'
        }
    }
    Invoke-WcrtCompatibilityTest -TestCase TC-0092 -RepositoryRoot $root `
        -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
        'vc6-ms-errors.c') -BehaviorSource (Join-Path `
        $PSScriptRoot 'ms-errors.c') -Arguments @($fixture) `
        -CompileOnly:$CompileOnly
    if ($RuntimeDirectory) {
        Invoke-WcrtPackagedCompatibilityTest -TestCase TC-0092 `
            -RepositoryRoot $root -TinyCc $TinyCc `
            -RuntimeDirectory $RuntimeDirectory `
            -BehaviorSource (Join-Path $PSScriptRoot `
            'ms-errors.c') -Arguments @($fixture) `
            -CompileOnly:$CompileOnly
    }
} finally {
    if (Test-Path -LiteralPath $fixture) {
        Remove-Item -LiteralPath $fixture -Force
    }
}
