[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('x86', 'x64', 'arm64')]
    [string]$Architecture,

    [Parameter(Mandatory = $true)]
    [string]$Version,

    [string]$Wpm = 'wpm.exe',
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

$releaseDirectory = Join-Path $repoRoot "$BuildRoot/$Architecture/Release"
$dll = Join-Path $releaseDirectory 'wcrt.dll'
if (-not (Test-Path -LiteralPath $dll -PathType Leaf)) {
    throw "Release DLL was not found: $dll"
}

$staging = Join-Path $repoRoot "output/wpm-staging/$Architecture"
$packageOutput = Join-Path $repoRoot $PackageRoot
if (Test-Path -LiteralPath $staging) {
    Remove-Item -LiteralPath $staging -Recurse -Force
}
New-Item -ItemType Directory -Force -Path $staging, (Join-Path $staging '.wpm'),
    (Join-Path $staging 'bin'), (Join-Path $staging 'include'), $packageOutput |
    Out-Null
Copy-Item -LiteralPath $dll -Destination (Join-Path $staging 'bin/wcrt.dll')
Copy-Item -Path (Join-Path $repoRoot 'include/*') -Destination (Join-Path $staging 'include') -Recurse
Copy-Item -LiteralPath (Join-Path $repoRoot 'LICENSE.txt') -Destination $staging
Copy-Item -LiteralPath (Join-Path $repoRoot 'README.md') -Destination $staging

$metadata = @(
    'name=wcrt'
    "version=$packageVersion"
    "arch=$Architecture"
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

$installDirectory = "%ProgramFiles%\WCRT\$packageVersion"
$installScript = @(
    '@echo off'
    'setlocal'
    ('set "WCRT_DEST={0}"' -f $installDirectory)
    'if not exist "%WCRT_DEST%" mkdir "%WCRT_DEST%" || exit /b 1'
    'xcopy "%~dp0..\*" "%WCRT_DEST%\" /E /I /Q /Y >nul || exit /b 1'
    'if exist "%WCRT_DEST%\.wpm" rmdir /S /Q "%WCRT_DEST%\.wpm"'
    'reg add "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v WCRT_HOME /t REG_EXPAND_SZ /d "%WCRT_DEST%" /f >nul || exit /b 1'
    'exit /b 0'
)
$removeScript = @(
    '@echo off'
    'setlocal'
    ('set "WCRT_DEST={0}"' -f $installDirectory)
    'if exist "%WCRT_DEST%" rmdir /S /Q "%WCRT_DEST%" || exit /b 1'
    'set "WCRT_CURRENT="'
    'for /f "tokens=2,*" %%A in (''reg query "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v WCRT_HOME 2^>nul'') do set "WCRT_CURRENT=%%B"'
    'if /I "%WCRT_CURRENT%"=="%WCRT_DEST%" reg delete "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v WCRT_HOME /f >nul 2>&1'
    'exit /b 0'
)
Set-Content -LiteralPath (Join-Path $staging '.wpm/install.cmd') -Value $installScript -Encoding ascii
Set-Content -LiteralPath (Join-Path $staging '.wpm/remove.cmd') -Value $removeScript -Encoding ascii

& $Wpm build $staging $packageOutput
if ($LASTEXITCODE -ne 0) {
    throw 'WPM failed to create the WCRT package.'
}
$package = Get-ChildItem -LiteralPath $packageOutput -Filter "wcrt-$Architecture-$packageVersion.zip" -File
if (@($package).Count -ne 1) {
    throw "Expected one WCRT package for $Architecture $packageVersion."
}
$package.FullName
