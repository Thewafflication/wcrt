[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [ValidateSet('x86', 'x64', 'arm64')]
    [string]$Architecture,

    [string]$GitHubEnvironmentFile = $env:GITHUB_ENV,
    [string]$GitHubPathFile = $env:GITHUB_PATH
)

$ErrorActionPreference = 'Stop'

$repository = 'https://github.com/Thewafflication/tcc_package/releases/latest/download'
$releaseKeySha256 = 'e141a5f383b8d2e7096080beb2920b37a007de727d1c8ff47a520453460f23a6'
$temporaryDirectory = if ([string]::IsNullOrWhiteSpace($env:RUNNER_TEMP)) {
    [IO.Path]::GetTempPath()
} else {
    $env:RUNNER_TEMP
}
$key = Join-Path $temporaryDirectory 'tcc-wpm-release.public'

Invoke-WebRequest -UseBasicParsing "$repository/wpm-release.public" -OutFile $key
$actualKeySha256 = (Get-FileHash -LiteralPath $key -Algorithm SHA256).Hash.ToLowerInvariant()
if ($actualKeySha256 -ne $releaseKeySha256) {
    throw "TinyCC release-key digest mismatch: $actualKeySha256"
}

& wpm trust add $key
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$repositories = @(& wpm repo list 2>&1)
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
if (-not ($repositories | Where-Object { $_.ToString().Contains($repository) })) {
    & wpm repo add $repository
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}

& wpm config set prerelease true --package tinycc
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& wpm update
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

# Deliberately omit --version: WPM resolves the newest eligible entry from the
# tcc_package repository's current releases/latest/download index.
& wpm install tinycc --arch $Architecture
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$tccHome = [Environment]::GetEnvironmentVariable('TCC_HOME', 'Machine')
if ([string]::IsNullOrWhiteSpace($tccHome)) {
    throw 'WPM did not define TCC_HOME after installing TinyCC.'
}
$tcc = Join-Path $tccHome 'tcc.exe'
if (-not (Test-Path -LiteralPath $tcc -PathType Leaf)) {
    throw "TinyCC was not installed at $tcc."
}

if ([string]::IsNullOrWhiteSpace($env:WPM_DATA_DIR)) {
    throw 'WPM_DATA_DIR must identify the isolated WPM data directory.'
}
$packageDirectory = Join-Path $env:WPM_DATA_DIR 'packages'
$packages = @(Get-ChildItem -LiteralPath $packageDirectory -File `
    -Filter "tinycc-$Architecture-*.zip")
if ($packages.Count -ne 1) {
    throw "Expected one installed TinyCC $Architecture package archive, found $($packages.Count)."
}
$package = $packages[0]

Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive = [IO.Compression.ZipFile]::OpenRead($package.FullName)
try {
    $entry = $archive.GetEntry('.wpm/package.txt')
    if ($null -eq $entry) {
        throw "TinyCC package metadata is missing from $($package.FullName)."
    }
    $reader = [IO.StreamReader]::new($entry.Open())
    try {
        $packageText = $reader.ReadToEnd()
    } finally {
        $reader.Dispose()
    }
} finally {
    $archive.Dispose()
}

$metadata = @{}
foreach ($line in ($packageText -split '\r?\n')) {
    if ($line -match '^([^=]+)=(.*)$') {
        $metadata[$Matches[1]] = $Matches[2]
    }
}
foreach ($required in @('name', 'version', 'arch', 'source-revision')) {
    if ([string]::IsNullOrWhiteSpace($metadata[$required])) {
        throw "TinyCC package metadata does not define '$required'."
    }
}
if ($metadata['name'] -ne 'tinycc' -or $metadata['arch'] -ne $Architecture) {
    throw "WPM selected an unexpected package identity: $($metadata['name'])/$($metadata['arch'])."
}

$packageSha256 = (Get-FileHash -LiteralPath $package.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
$values = [ordered]@{
    TINYCC = $tcc
    TINYCC_PACKAGE_VERSION = $metadata['version']
    TINYCC_SOURCE_REVISION = $metadata['source-revision']
    TINYCC_PACKAGE_SHA256 = $packageSha256
    TINYCC_RELEASE_KEY_SHA256 = $actualKeySha256
    TINYCC_WPM_REPOSITORY = $repository
}
foreach ($item in $values.GetEnumerator()) {
    [Environment]::SetEnvironmentVariable($item.Key, $item.Value, 'Process')
    if (-not [string]::IsNullOrWhiteSpace($GitHubEnvironmentFile)) {
        "$($item.Key)=$($item.Value)" | Add-Content -LiteralPath $GitHubEnvironmentFile
    }
}
if (-not [string]::IsNullOrWhiteSpace($GitHubPathFile)) {
    $tccHome | Add-Content -LiteralPath $GitHubPathFile
}

[PSCustomObject]@{
    Architecture = $Architecture
    PackageName = $metadata['name']
    PackageVersion = $metadata['version']
    SourceRevision = $metadata['source-revision']
    Repository = $repository
    PackageSha256 = $packageSha256
    ExecutableSha256 = (Get-FileHash -LiteralPath $tcc -Algorithm SHA256).Hash.ToLowerInvariant()
}
