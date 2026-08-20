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
$release = Get-WorkflowJobBlock -Name 'release'
$sign = Get-WorkflowJobBlock -Name 'sign'
$package = Get-WorkflowJobBlock -Name 'package'
$publish = Get-WorkflowJobBlock -Name 'publish'
$tagCondition = "if: startsWith(github.ref, 'refs/tags/')"

Assert-JobContains build $build 'run-c89-tests.ps1'
Assert-JobContains build $build 'run-extension-tests.ps1'
Assert-JobContains build $build 'test-built-libraries.ps1'
Assert-JobContains build $build 'test-startup-objects.ps1'

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
if ($release.Contains('environment: release')) {
    throw "Workflow job 'release' must run before the protected signing environment gate."
}

Assert-JobContains sign $sign $tagCondition
Assert-JobContains sign $sign 'needs: release'
Assert-JobContains sign $sign 'environment: release'
Assert-JobContains sign $sign 'id-token: write'
Assert-JobContains sign $sign 'Azure/login@d54469830ea7d513b7371e02a077c3ee5cb7b112'
Assert-JobContains sign $sign 'Azure/artifact-signing-action@c0ae2c1d0c1847ab81ac0ab8521bee597cfedd30'
Assert-JobContains sign $sign 'file-digest: SHA256'
Assert-JobContains sign $sign 'timestamp-rfc3161: http://timestamp.acs.microsoft.com'
Assert-JobContains sign $sign 'timestamp-digest: SHA256'
Assert-JobContains sign $sign 'wcrt-x86-release-build\wcrt.dll'
Assert-JobContains sign $sign 'wcrt-x64-release-build\wcrt.dll'
Assert-JobContains sign $sign 'wcrt-arm64-release-build\wcrt.dll'
Assert-JobContains sign $sign 'test-authenticode-signatures.ps1'
Assert-JobContains sign $sign '-SourceRevision ''${{ github.sha }}'''
Assert-JobContains sign $sign 'test-windows-2000-imports.ps1'
Assert-JobContains sign $sign 'wcrt-x86-release-signed'
Assert-JobContains sign $sign 'wcrt-x64-release-signed'
Assert-JobContains sign $sign 'wcrt-arm64-release-signed'

Assert-JobContains package $package $tagCondition
Assert-JobContains package $package 'needs: sign'
Assert-JobContains package $package 'environment: release'
Assert-JobContains package $package 'wcrt-*-release-signed'
Assert-JobContains package $package 'build-wpm-package.ps1'
Assert-JobContains package $package 'wpm verify'
Assert-JobContains package $package 'if ($verifyExitCode -ne 0)'
Assert-JobContains package $package 'test-wpm-package-authenticode.ps1'

Assert-JobContains publish $publish $tagCondition
Assert-JobContains publish $publish 'needs: package'
Assert-JobContains publish $publish 'gh release create'
Assert-JobContains publish $publish 'gh release upload'

[PSCustomObject]@{
    Status = 'Pass'
    DebugDependency = 'complete build matrix'
    ReleaseArchitectures = 3
    ReleaseDependency = 'build'
    SigningDependency = 'release'
    PackageDependency = 'sign'
    PublishDependency = 'package'
}
