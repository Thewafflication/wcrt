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

Assert-JobContains package $package $tagCondition
Assert-JobContains package $package 'needs: release'
Assert-JobContains package $package 'build-wpm-package.ps1'
Assert-JobContains package $package 'wpm verify'

Assert-JobContains publish $publish $tagCondition
Assert-JobContains publish $publish 'needs: package'
Assert-JobContains publish $publish 'gh release create'
Assert-JobContains publish $publish 'gh release upload'

[PSCustomObject]@{
    Status = 'Pass'
    DebugDependency = 'complete build matrix'
    ReleaseArchitectures = 3
    ReleaseDependency = 'build'
    PackageDependency = 'release'
    PublishDependency = 'package'
}
