[CmdletBinding()]
param(
    [string]$WorkflowPath = (Join-Path (Split-Path -Parent `
        (Split-Path -Parent $PSScriptRoot)) '.github/workflows/build.yml')
)

$ErrorActionPreference = 'Stop'

if (-not (Test-Path -LiteralPath $WorkflowPath -PathType Leaf)) {
    throw "Release workflow not found: $WorkflowPath"
}

$workflow = Get-Content -LiteralPath $WorkflowPath -Raw
$repositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$tinyCcInstallerPath = Join-Path $repositoryRoot `
    'tools/install-latest-tinycc-wpm-package.ps1'
if (-not (Test-Path -LiteralPath $tinyCcInstallerPath -PathType Leaf)) {
    throw "Latest TinyCC WPM installer not found: $tinyCcInstallerPath"
}
$tinyCcInstaller = Get-Content -LiteralPath $tinyCcInstallerPath -Raw

function Get-WorkflowJobBlock {
    param([Parameter(Mandatory)][string]$Name)

    $escaped = [regex]::Escape($Name)
    $pattern = "(?ms)^  ${escaped}:\r?\n.*?(?=^  [A-Za-z0-9_-]+:\r?`$|\z)"
    $match = [regex]::Match($workflow, $pattern)
    if (-not $match.Success) {
        throw "Workflow job '$Name' was not found."
    }
    $match.Value
}

function Assert-JobContains {
    param(
        [Parameter(Mandatory)][string]$JobName,
        [Parameter(Mandatory)][string]$JobBlock,
        [Parameter(Mandatory)][string]$Text
    )

    if (-not $JobBlock.Contains($Text)) {
        throw "Workflow job '$JobName' does not contain required text: $Text"
    }
}

$build = Get-WorkflowJobBlock -Name 'build'
$debugPackage = Get-WorkflowJobBlock -Name 'debug-package'
$release = Get-WorkflowJobBlock -Name 'release'
$package = Get-WorkflowJobBlock -Name 'package'
$publish = Get-WorkflowJobBlock -Name 'publish'
$tagCondition = "if: startsWith(github.ref, 'refs/tags/')"

Assert-JobContains build $build 'run-c89-tests.ps1'
Assert-JobContains build $build 'run-extension-tests.ps1'
Assert-JobContains build $build 'test-built-libraries.ps1'
Assert-JobContains build $build 'test-startup-objects.ps1'
Assert-JobContains build $build 'install-latest-tinycc-wpm-package.ps1'
Assert-JobContains build $build 'Attach dependency provenance to verified debug build'

Assert-JobContains debug-package $debugPackage 'needs: build'
Assert-JobContains debug-package $debugPackage `
    'Debug targets selected different latest TinyCC packages'

Assert-JobContains release $release $tagCondition
Assert-JobContains release $release 'needs: build'
Assert-JobContains release $release 'arch: x86'
Assert-JobContains release $release 'arch: x64'
Assert-JobContains release $release 'arch: arm64'
Assert-JobContains release $release '-Configuration Release'
Assert-JobContains release $release 'test-built-libraries.ps1'
Assert-JobContains release $release 'test-startup-objects.ps1'
Assert-JobContains release $release 'test-windows-2000-imports.ps1'
Assert-JobContains release $release 'Enforce release Windows startup result'
Assert-JobContains release $release 'install-latest-tinycc-wpm-package.ps1'
Assert-JobContains release $release 'Record release dependency provenance'
Assert-JobContains release $release 'Verify TinyCC matches the tested Debug baseline'
if ($release.Contains('environment: release')) {
    throw "Workflow job 'release' must run before the protected package-signing environment gate."
}

if ([regex]::IsMatch($workflow, '(?m)^  sign:$')) {
    throw "Workflow must not require the deferred Authenticode job for 1.0.0."
}
foreach ($deferredInput in @(
    'Azure/login@',
    'Azure/artifact-signing-action@',
    'AUTHENTICODE_EXPECTED_SUBJECT',
    'needs: sign'
)) {
    if ($workflow.Contains($deferredInput)) {
        throw "Workflow still contains deferred Authenticode input: $deferredInput"
    }
}

if (-not $tinyCcInstaller.Contains(
    'https://github.com/Thewafflication/tcc_package/releases/latest/download')) {
    throw 'TinyCC installer does not use the WPM repository latest endpoint.'
}
if (-not [regex]::IsMatch($tinyCcInstaller,
    '(?m)^& wpm install tinycc --arch \$Architecture\s*$')) {
    throw 'TinyCC installer does not request the latest architecture package.'
}
if ([regex]::IsMatch($tinyCcInstaller,
    '(?m)^& wpm install tinycc[^\r\n]*--version')) {
    throw 'TinyCC installer must not pin a package version.'
}
if (-not $tinyCcInstaller.Contains('TINYCC_PACKAGE_VERSION')) {
    throw 'TinyCC installer does not export the selected package version.'
}

Assert-JobContains package $package $tagCondition
Assert-JobContains package $package 'needs: release'
Assert-JobContains package $package 'environment: release'
Assert-JobContains package $package 'wcrt-*-release-build'
Assert-JobContains package $package 'build-wpm-package.ps1'
Assert-JobContains package $package 'wpm verify'
Assert-JobContains package $package 'if ($verifyExitCode -ne 0)'
Assert-JobContains package $package 'test-wpm-package-authenticode.ps1'
Assert-JobContains package $package '-SkipAuthenticode'
Assert-JobContains package $package `
    'Release targets selected different TinyCC packages'

Assert-JobContains publish $publish $tagCondition
Assert-JobContains publish $publish 'needs: package'
Assert-JobContains publish $publish 'gh release create'
Assert-JobContains publish $publish 'gh release upload'

[PSCustomObject]@{
    Status = 'Pass'
    DebugDependency = 'complete build matrix'
    TinyCcSelection = 'latest eligible WPM package at job execution'
    ReleaseArchitectures = 3
    ReleaseDependency = 'build'
    AuthenticodeDisposition = 'Deferred from 1.0.0'
    PackageDependency = 'release'
    PublishDependency = 'package'
}
