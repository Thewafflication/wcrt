[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$Version,

    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Release',

    [string]$PackageName = 'wcrt',

    [string]$Wpm = 'wpm.exe',
    [string]$SigningKey,
    [string]$BuildRoot = 'output/build',
    [string]$PackageRoot = 'output/packages'
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
. (Join-Path $PSScriptRoot 'wcrt-version.ps1')
$versionInfo = Get-WcrtVersion -RepositoryRoot $repoRoot -SourceVersion $Version
$sourceVersion = $versionInfo.SourceVersion
$packageVersion = $versionInfo.PackageVersion
$gitHash = $versionInfo.GitHash

$architectures = @('x86', 'x64', 'arm64')
foreach ($architecture in $architectures) {
    $buildDirectory = Join-Path $repoRoot "$BuildRoot/$architecture/$Configuration"
    $dll = Join-Path $buildDirectory 'wcrt.dll'
    if (-not (Test-Path -LiteralPath $dll -PathType Leaf)) {
        throw "$Configuration DLL was not found: $dll"
    }
}

$staging = Join-Path $repoRoot "output/wpm-staging/$PackageName-any"
$packageOutput = Join-Path $repoRoot $PackageRoot
if (Test-Path -LiteralPath $staging) {
    Remove-Item -LiteralPath $staging -Recurse -Force
}
New-Item -ItemType Directory -Force -Path $staging, (Join-Path $staging '.wpm'),
    (Join-Path $staging 'include'), $packageOutput |
    Out-Null
foreach ($architecture in $architectures) {
    $buildDirectory = Join-Path $repoRoot "$BuildRoot/$architecture/$Configuration"
    $architectureRoot = Join-Path $staging $architecture
    $binDirectory = Join-Path $architectureRoot 'bin'
    $libDirectory = Join-Path $architectureRoot 'lib'
    New-Item -ItemType Directory -Force -Path $binDirectory, $libDirectory | Out-Null
    Copy-Item -LiteralPath (Join-Path $buildDirectory 'wcrt.dll') -Destination $binDirectory
    foreach ($file in @('libwcrt.a', 'wcrt-startup-console.o', 'wcrt-startup-gui.o', 'wcrt.def')) {
        Copy-Item -LiteralPath (Join-Path $buildDirectory $file) -Destination $libDirectory
    }
    Copy-Item -LiteralPath (Join-Path $buildDirectory `
        'c99-complex-capability.json') -Destination $architectureRoot
}
Copy-Item -Path (Join-Path $repoRoot 'include/*') -Destination (Join-Path $staging 'include') -Recurse
Copy-Item -LiteralPath (Join-Path $repoRoot 'LICENSE.txt') -Destination $staging
Copy-Item -LiteralPath (Join-Path $repoRoot 'README.md') -Destination $staging
$complexProfiles = foreach ($architecture in $architectures) {
    $profilePath = Join-Path $staging `
        "$architecture/c99-complex-capability.json"
    $profile = Get-Content -LiteralPath $profilePath -Raw | ConvertFrom-Json
    if ($profile.Status -notin 'Supported', 'ExpectedFail') {
        throw "Invalid complex capability status for $architecture."
    }
    if ([bool]$profile.RuntimeIncluded -ne
        ($profile.Status -eq 'Supported')) {
        throw "Complex runtime/profile mismatch for $architecture."
    }
    if (@($profile.Probes).Count -ne 2) {
        throw "Complex capability evidence is incomplete for $architecture."
    }
    if (@($profile.Probes | Where-Object {
            -not $_.Supported -and -not $_.ExpectedFailure
        }).Count -ne 0) {
        throw "Unexpected complex diagnostic for $architecture."
    }
    if ($profile.Status -eq 'Supported' -and
        @($profile.Probes | Where-Object { -not $_.Supported }).Count -ne 0) {
        throw "Supported complex profile is inconsistent for $architecture."
    }
    if ($profile.Status -eq 'ExpectedFail' -and
        @($profile.Probes | Where-Object { $_.ExpectedFailure }).Count -eq 0) {
        throw "ExpectedFail complex profile has no expected failure for $architecture."
    }
    "$architecture=$($profile.Status);runtime-included=" +
        $profile.RuntimeIncluded.ToString().ToLowerInvariant()
}
@(
    'C99 complex and type-generic mathematics release profile'
    'ExpectedFail means TinyCC emitted ADR-0005 controlled complex diagnostics.'
    'Such a package contains the headers but omits complex runtime symbols.'
    $complexProfiles
) | Set-Content -LiteralPath (Join-Path $staging `
    'C99-COMPLEX-PROFILE.txt') -Encoding ascii

$metadata = @(
    "name=$PackageName"
    "version=$packageVersion"
    'arch=any'
    # wcrt-debug is a separate package identity. Keep WPM's filename flavor
    # field clear so its archive name is wcrt-debug-any-<version>.zip.
    'debug=false'
    'description=Waughtal C Run Time for Windows'
    'maintainer=Jordan Waughtal'
    'homepage=https://github.com/Thewafflication/wcrt'
    'repository=https://github.com/Thewafflication/wcrt'
    'license=GPL-3.0-or-later'
    "source-version=$sourceVersion"
    "source-revision=$gitHash"
)
Set-Content -LiteralPath (Join-Path $staging '.wpm/package.txt') -Value $metadata -Encoding ascii

$productDirectory = if ($Configuration -eq 'Debug') { 'WCRT\Debug' } else { 'WCRT' }
$homeVariable = if ($Configuration -eq 'Debug') { 'WCRT_DEBUG_HOME' } else { 'WCRT_HOME' }
$installDirectory = "%ProgramFiles%\$productDirectory\$packageVersion"
$installScript = @(
    '@echo off'
    'setlocal'
    ('set "WCRT_DEST={0}"' -f $installDirectory)
    'if not exist "%WCRT_DEST%" mkdir "%WCRT_DEST%" || exit /b 1'
    'xcopy "%~dp0..\*" "%WCRT_DEST%\" /E /I /Q /Y >nul || exit /b 1'
    'if exist "%WCRT_DEST%\.wpm" rmdir /S /Q "%WCRT_DEST%\.wpm"'
    ('reg add "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v {0} /t REG_EXPAND_SZ /d "%WCRT_DEST%" /f >nul || exit /b 1' -f $homeVariable)
    'exit /b 0'
)
$removeScript = @(
    '@echo off'
    'setlocal'
    ('set "WCRT_DEST={0}"' -f $installDirectory)
    'if exist "%WCRT_DEST%" rmdir /S /Q "%WCRT_DEST%" || exit /b 1'
    'set "WCRT_CURRENT="'
    ('for /f "tokens=2,*" %%A in (''reg query "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v {0} 2^>nul'') do set "WCRT_CURRENT=%%B"' -f $homeVariable)
    ('if /I "%WCRT_CURRENT%"=="%WCRT_DEST%" reg delete "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v {0} /f >nul 2>&1' -f $homeVariable)
    'exit /b 0'
)
Set-Content -LiteralPath (Join-Path $staging '.wpm/install.cmd') -Value $installScript -Encoding ascii
Set-Content -LiteralPath (Join-Path $staging '.wpm/remove.cmd') -Value $removeScript -Encoding ascii

$wpmArguments = @('build', $staging, $packageOutput)
if (-not [string]::IsNullOrWhiteSpace($SigningKey)) {
    $resolvedSigningKey = (Resolve-Path -LiteralPath $SigningKey).Path
    $wpmArguments += @('--sign', $resolvedSigningKey)
}
& $Wpm @wpmArguments
if ($LASTEXITCODE -ne 0) {
    throw 'WPM failed to create the WCRT package.'
}
$package = Get-ChildItem -LiteralPath $packageOutput -Filter "$PackageName-any-$packageVersion.zip" -File
if (@($package).Count -ne 1) {
    throw "Expected one multi-architecture WCRT package for $packageVersion."
}

# Keep the shared and static distributions atomic: a release package is not
# valid unless consumers receive both libraries and the files needed to use
# them.
Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive = [IO.Compression.ZipFile]::OpenRead($package.FullName)
try {
    $entries = @{}
    foreach ($entry in $archive.Entries) {
        $entries[$entry.FullName.Replace('\', '/')] = $entry.Length
    }
    $requiredEntries = @(
        'include/stdio.h'
        'include/complex.h'
        'include/tgmath.h'
        'C99-COMPLEX-PROFILE.txt'
    )
    foreach ($architecture in $architectures) {
        $requiredEntries += @(
            "$architecture/bin/wcrt.dll"
            "$architecture/lib/libwcrt.a"
            "$architecture/lib/wcrt-startup-console.o"
            "$architecture/lib/wcrt-startup-gui.o"
            "$architecture/lib/wcrt.def"
            "$architecture/c99-complex-capability.json"
        )
    }
    foreach ($entry in $requiredEntries) {
        if (-not $entries.ContainsKey($entry) -or $entries[$entry] -eq 0) {
            throw "WCRT package is missing required non-empty entry: $entry"
        }
    }
} finally {
    $archive.Dispose()
}
$package.FullName
