[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [ValidateSet('x86', 'x64', 'arm64')]
    [string]$Architecture,

    [Parameter(Mandatory)]
    [string]$TinyCc
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$tinyCcPath = (Resolve-Path -LiteralPath $TinyCc).Path
$tinyCcInclude = Join-Path (Split-Path -Parent $tinyCcPath) 'include'
$buildDirectory = Join-Path $repoRoot `
    "build/tests/c99/header-matrix/$Architecture"
New-Item -ItemType Directory -Force $buildDirectory | Out-Null

$headers = @(
    'assert.h', 'complex.h', 'ctype.h', 'errno.h', 'fenv.h', 'float.h',
    'inttypes.h', 'iso646.h', 'limits.h', 'locale.h', 'math.h', 'setjmp.h',
    'signal.h', 'stdarg.h', 'stdbool.h', 'stddef.h', 'stdint.h', 'stdio.h',
    'stdlib.h', 'string.h', 'tgmath.h', 'time.h', 'wchar.h', 'wctype.h'
)
$c89Headers = @(
    'assert.h', 'ctype.h', 'errno.h', 'float.h', 'limits.h', 'locale.h',
    'math.h', 'setjmp.h', 'signal.h', 'stdarg.h', 'stddef.h', 'stdio.h',
    'stdlib.h', 'string.h', 'time.h'
)

function Invoke-HeaderCompile {
    param(
        [Parameter(Mandatory)][string]$Name,
        [Parameter(Mandatory)][string]$Mode,
        [Parameter(Mandatory)][string[]]$Lines,
        [switch]$C89
    )

    $source = Join-Path $buildDirectory "$Name.c"
    $object = Join-Path $buildDirectory "$Name.o"
    Set-Content -LiteralPath $source -Encoding ascii -Value $Lines
    $arguments = @(
        "-std=$Mode", '-Wall', '-Werror', '-nostdinc',
        '-I', (Join-Path $repoRoot 'include'), '-I', $tinyCcInclude,
        '-c', $source, '-o', $object
    )
    if ($C89) {
        $arguments = @('-DWCRT_C89=1') + $arguments
    }
    $diagnostic = @(& $tinyCcPath @arguments 2>&1)
    if ($LASTEXITCODE -ne 0) {
        throw "$Name failed:`n$($diagnostic -join "`n")"
    }
}

foreach ($header in $headers) {
    $stem = $header.Replace('.', '-')
    Invoke-HeaderCompile -Name "c99-$stem" -Mode c99 -Lines @(
        "#include <$header>",
        "#include <$header>",
        'int main(void) { return 0; }'
    )
}

foreach ($header in $c89Headers) {
    $stem = $header.Replace('.', '-')
    Invoke-HeaderCompile -Name "c89-$stem" -Mode c89 -C89 -Lines @(
        "#include <$header>",
        "#include <$header>",
        'int main(void) { return 0; }'
    )
}

$orders = @($headers, @($headers[($headers.Count - 1)..0]))
for ($index = 0; $index -lt $orders.Count; $index++) {
    $lines = @($orders[$index] | ForEach-Object { "#include <$_>" })
    $lines += 'int main(void) { return 0; }'
    Invoke-HeaderCompile -Name "c99-mixed-$index" -Mode c99 -Lines $lines
}

$c89Lines = @($c89Headers | ForEach-Object { "#include <$_>" })
$c89Lines += 'int main(void) { return 0; }'
Invoke-HeaderCompile -Name 'c89-mixed' -Mode c89 -C89 -Lines $c89Lines

$pointerSize = if ($Architecture -eq 'x86') { 4 } else { 8 }
$jumpSize = @{ x86 = 24; x64 = 240; arm64 = 168 }[$Architecture]
$abiSource = Join-Path $repoRoot 'tests/c99/compatibility-abi.c'
$abiObject = Join-Path $buildDirectory 'compatibility-abi.o'
$abiDiagnostic = @(& $tinyCcPath -std=c99 -Wall -Werror -nostdinc `
    -I (Join-Path $repoRoot 'include') -I $tinyCcInclude `
    "-DWCRT_EXPECT_POINTER_SIZE=$pointerSize" `
    "-DWCRT_EXPECT_JMP_SIZE=$jumpSize" -c $abiSource -o $abiObject 2>&1)
if ($LASTEXITCODE -ne 0) {
    throw "C99 ABI matrix failed:`n$($abiDiagnostic -join "`n")"
}

[PSCustomObject]@{
    Status = 'Pass'
    Architecture = $Architecture
    C99StandaloneHeaders = $headers.Count
    C89StandaloneHeaders = $c89Headers.Count
    MixedOrders = 3
    Abi = 'Pass'
}
