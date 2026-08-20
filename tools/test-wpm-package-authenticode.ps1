[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$Package,

    [Parameter(Mandatory = $true)]
    [ValidateNotNullOrEmpty()]
    [string]$ExpectedSubject,

    [string]$ExpectedCompanyName = 'Jordan Waughtal',

    [string]$SourceRevision,

    [string]$BuildRoot = 'output/build',

    [string]$OutputPath =
        'output/test-results/release/package-authenticode-signatures.json',

    [string]$SignTool
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$architectures = @('x86', 'x64', 'arm64')

function Resolve-RepositoryPath {
    param([Parameter(Mandatory = $true)][string]$Candidate)

    if ([IO.Path]::IsPathRooted($Candidate)) {
        return [IO.Path]::GetFullPath($Candidate)
    }
    [IO.Path]::GetFullPath((Join-Path $repoRoot $Candidate))
}

$packagePath = (Resolve-Path -LiteralPath $Package).Path
$resolvedOutput = Resolve-RepositoryPath -Candidate $OutputPath
$tempBase = if (-not [string]::IsNullOrWhiteSpace($env:RUNNER_TEMP)) {
    $env:RUNNER_TEMP
} else {
    [IO.Path]::GetTempPath()
}
$extractRoot = Join-Path $tempBase (
    'wcrt-package-authenticode-' + [guid]::NewGuid().ToString('N'))
$signatureOutput = Join-Path $extractRoot 'signatures.json'
$failures = [Collections.Generic.List[string]]::new()
$comparisons = @()
$signatureRecord = $null

New-Item -ItemType Directory -Force $extractRoot | Out-Null
try {
    Add-Type -AssemblyName System.IO.Compression.FileSystem
    $archive = [IO.Compression.ZipFile]::OpenRead($packagePath)
    try {
        $extracted = @()
        foreach ($architecture in $architectures) {
            $entryName = "$architecture/bin/wcrt.dll"
            $entry = $archive.GetEntry($entryName)
            if ($null -eq $entry) {
                $failures.Add("Package entry is missing: $entryName")
                continue
            }

            $destinationDirectory = Join-Path $extractRoot $architecture
            New-Item -ItemType Directory -Force `
                $destinationDirectory | Out-Null
            $destination = Join-Path $destinationDirectory 'wcrt.dll'
            $inputStream = $entry.Open()
            $outputStream = [IO.File]::Create($destination)
            try {
                $inputStream.CopyTo($outputStream)
            } finally {
                $outputStream.Dispose()
                $inputStream.Dispose()
            }
            $extracted += $destination

            $source = Resolve-RepositoryPath -Candidate (
                "$BuildRoot/$architecture/Release/wcrt.dll")
            if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
                $failures.Add("Signed build input is missing: $source")
                continue
            }
            $sourceHash = (Get-FileHash -LiteralPath $source `
                -Algorithm SHA256).Hash.ToLowerInvariant()
            $packageHash = (Get-FileHash -LiteralPath $destination `
                -Algorithm SHA256).Hash.ToLowerInvariant()
            $match = $sourceHash -eq $packageHash
            if (-not $match) {
                $failures.Add(
                    "Packaged $architecture DLL differs from the signed build input.")
            }
            $comparisons += [ordered]@{
                architecture = $architecture
                packageEntry = $entryName
                signedBuildPath = $source
                signedBuildSha256 = $sourceHash
                packagedSha256 = $packageHash
                unchanged = $match
            }
        }
    } finally {
        $archive.Dispose()
    }

    if ($extracted.Count -eq $architectures.Count) {
        $arguments = @{
            Path = $extracted
            ExpectedSubject = $ExpectedSubject
            ExpectedCompanyName = $ExpectedCompanyName
            SourceRevision = $SourceRevision
            OutputPath = $signatureOutput
        }
        if (-not [string]::IsNullOrWhiteSpace($SignTool)) {
            $arguments.SignTool = $SignTool
        }
        try {
            & (Join-Path $PSScriptRoot 'test-authenticode-signatures.ps1') `
                @arguments | Out-Null
        } catch {
            $failures.Add($_.Exception.Message)
        }
        if (Test-Path -LiteralPath $signatureOutput -PathType Leaf) {
            $signatureRecord = Get-Content -LiteralPath $signatureOutput `
                -Raw | ConvertFrom-Json
        }
    }

    $packageItem = Get-Item -LiteralPath $packagePath
    $record = [ordered]@{
        schemaVersion = 1
        generatedAtUtc = [DateTime]::UtcNow.ToString('o')
        package = $packageItem.Name
        packageLength = $packageItem.Length
        packageSha256 = (Get-FileHash -LiteralPath $packagePath `
            -Algorithm SHA256).Hash.ToLowerInvariant()
        expectedSubject = $ExpectedSubject
        expectedCompanyName = $ExpectedCompanyName
        sourceRevision = $SourceRevision
        result = if ($failures.Count -eq 0) { 'Pass' } else { 'Fail' }
        unchangedDlls = $comparisons
        authenticodeVerification = $signatureRecord
        errors = @($failures)
    }
    New-Item -ItemType Directory -Force `
        (Split-Path -Parent $resolvedOutput) | Out-Null
    $record | ConvertTo-Json -Depth 10 |
        Set-Content -LiteralPath $resolvedOutput -Encoding utf8NoBOM
} finally {
    if (Test-Path -LiteralPath $extractRoot) {
        Remove-Item -LiteralPath $extractRoot -Recurse -Force
    }
}

if ($failures.Count -ne 0) {
    throw "Packaged Authenticode verification failed with " +
        "$($failures.Count) findings. Evidence: $resolvedOutput"
}

[PSCustomObject]$record
