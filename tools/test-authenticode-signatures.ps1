[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateNotNullOrEmpty()]
    [string[]]$Path,

    [Parameter(Mandatory = $true)]
    [ValidateNotNullOrEmpty()]
    [string]$ExpectedSubject,

    [string]$ExpectedCompanyName = 'Jordan Waughtal',

    [string]$SourceRevision,

    [string]$OutputPath =
        'output/test-results/release/authenticode-signatures.json',

    [string]$SignTool
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$codeSigningOid = '1.3.6.1.5.5.7.3.3'

function Resolve-OutputPath {
    param([Parameter(Mandatory = $true)][string]$Candidate)

    if ([IO.Path]::IsPathRooted($Candidate)) {
        return [IO.Path]::GetFullPath($Candidate)
    }
    [IO.Path]::GetFullPath((Join-Path $repoRoot $Candidate))
}

function Resolve-SignToolPath {
    if (-not [string]::IsNullOrWhiteSpace($SignTool)) {
        return (Resolve-Path -LiteralPath $SignTool).Path
    }

    $command = Get-Command signtool.exe -ErrorAction SilentlyContinue
    if ($null -ne $command) {
        return $command.Source
    }

    $programFilesX86 = [Environment]::GetFolderPath(
        [Environment+SpecialFolder]::ProgramFilesX86)
    $sdkBin = Join-Path $programFilesX86 'Windows Kits/10/bin'
    $candidates = @(
        Get-ChildItem -Path (Join-Path $sdkBin '*/x64/signtool.exe') `
            -File -ErrorAction SilentlyContinue |
            Sort-Object { [version]$_.Directory.Parent.Name } -Descending
    )
    if ($candidates.Count -eq 0) {
        throw 'SignTool was not found in PATH or the Windows 10 SDK.'
    }
    $candidates[0].FullName
}

function Get-CertificateRecord {
    param($Certificate)

    if ($null -eq $Certificate) {
        return $null
    }

    [ordered]@{
        subject = $Certificate.Subject
        issuer = $Certificate.Issuer
        thumbprint = $Certificate.Thumbprint
        serialNumber = $Certificate.SerialNumber
        notBeforeUtc = $Certificate.NotBefore.ToUniversalTime().ToString('o')
        notAfterUtc = $Certificate.NotAfter.ToUniversalTime().ToString('o')
    }
}

function Get-EnhancedKeyUsageOids {
    param($Certificate)

    if ($null -eq $Certificate) {
        return @()
    }

    @(
        foreach ($extension in $Certificate.Extensions) {
            if ($extension.Oid.Value -ne '2.5.29.37') {
                continue
            }
            foreach ($usage in $extension.EnhancedKeyUsages) {
                $usage.Value
            }
        }
    )
}

function Get-ArchitectureFromPath {
    param([string]$Candidate)

    if ($Candidate -match '(?i)(?:^|[\\/\-])(x86|x64|arm64)(?:[\\/\-])') {
        return $Matches[1].ToLowerInvariant()
    }
    $null
}

$signToolPath = $null
$signToolError = $null
try {
    $signToolPath = Resolve-SignToolPath
} catch {
    $signToolError = $_.Exception.Message
}

$results = @()
foreach ($candidate in $Path) {
    $errors = [Collections.Generic.List[string]]::new()
    $resolvedPath = $null
    $signature = $null
    $fileVersion = $null
    $sha256 = $null
    $verifyOutput = $null
    $verifyExitCode = $null

    try {
        $resolvedPath = (Resolve-Path -LiteralPath $candidate).Path
        if (-not (Test-Path -LiteralPath $resolvedPath -PathType Leaf)) {
            throw "Authenticode input is not a file: $candidate"
        }
        $sha256 = (Get-FileHash -LiteralPath $resolvedPath `
            -Algorithm SHA256).Hash.ToLowerInvariant()
        $fileVersion = [Diagnostics.FileVersionInfo]::GetVersionInfo(
            $resolvedPath)
        $signature = Get-AuthenticodeSignature -LiteralPath $resolvedPath
    } catch {
        $errors.Add($_.Exception.Message)
    }

    if ($null -ne $signature) {
        if ($signature.Status.ToString() -ne 'Valid') {
            $errors.Add("Authenticode status is $($signature.Status): " +
                $signature.StatusMessage)
        }
        if ($null -eq $signature.SignerCertificate) {
            $errors.Add('The Authenticode signer certificate is missing.')
        } else {
            if (-not [string]::Equals($signature.SignerCertificate.Subject,
                    $ExpectedSubject, [StringComparison]::Ordinal)) {
                $errors.Add("Unexpected signer subject: " +
                    $signature.SignerCertificate.Subject)
            }
            $ekuOids = @(Get-EnhancedKeyUsageOids `
                -Certificate $signature.SignerCertificate)
            if ($codeSigningOid -notin $ekuOids) {
                $errors.Add('The signer certificate lacks the Code Signing EKU.')
            }
        }
        if ($null -eq $signature.TimeStamperCertificate) {
            $errors.Add('A trusted Authenticode timestamp is missing.')
        }
    }

    if ($null -ne $fileVersion -and
        -not [string]::Equals($fileVersion.CompanyName,
            $ExpectedCompanyName, [StringComparison]::Ordinal)) {
        $errors.Add("Unexpected VERSIONINFO CompanyName: " +
            $fileVersion.CompanyName)
    }

    if ($null -ne $signToolError) {
        $errors.Add($signToolError)
    } elseif ($null -ne $resolvedPath) {
        $verifyOutput = (& $signToolPath verify /pa /all /v `
            $resolvedPath 2>&1 | Out-String).Trim()
        $verifyExitCode = $LASTEXITCODE
        if ($verifyExitCode -ne 0) {
            $errors.Add("SignTool verification exited $verifyExitCode.")
        }
        if ($verifyOutput -notmatch
            '(?im)(Hash of file \(sha256\)|Digest Algorithm:\s*sha256|File Digest:\s*sha256)') {
            $errors.Add('SignTool output did not confirm a SHA-256 file digest.')
        }
    }

    $results += [ordered]@{
        path = $resolvedPath
        architecture = Get-ArchitectureFromPath -Candidate $resolvedPath
        sha256 = $sha256
        length = if ($null -ne $resolvedPath) {
            (Get-Item -LiteralPath $resolvedPath).Length
        } else { $null }
        productVersion = if ($null -ne $fileVersion) {
            $fileVersion.ProductVersion
        } else { $null }
        companyName = if ($null -ne $fileVersion) {
            $fileVersion.CompanyName
        } else { $null }
        signatureType = if ($null -ne $signature) {
            $signature.SignatureType.ToString()
        } else { $null }
        authenticodeStatus = if ($null -ne $signature) {
            $signature.Status.ToString()
        } else { $null }
        authenticodeStatusMessage = if ($null -ne $signature) {
            $signature.StatusMessage
        } else { $null }
        signerCertificate = if ($null -ne $signature) {
            Get-CertificateRecord -Certificate $signature.SignerCertificate
        } else { $null }
        signerEnhancedKeyUsageOids = if ($null -ne $signature) {
            @(Get-EnhancedKeyUsageOids `
                -Certificate $signature.SignerCertificate)
        } else { @() }
        timestampCertificate = if ($null -ne $signature) {
            Get-CertificateRecord `
                -Certificate $signature.TimeStamperCertificate
        } else { $null }
        signToolExitCode = $verifyExitCode
        signToolOutput = $verifyOutput
        signToolTimestampLines = @(
            if ($null -ne $verifyOutput) {
                $verifyOutput -split "`r?`n" |
                    Where-Object { $_ -match '(?i)timestamp' }
            }
        )
        status = if ($errors.Count -eq 0) { 'Pass' } else { 'Fail' }
        errors = @($errors)
    }
}

$failed = @($results | Where-Object status -ne 'Pass')
$record = [ordered]@{
    schemaVersion = 1
    generatedAtUtc = [DateTime]::UtcNow.ToString('o')
    expectedSubject = $ExpectedSubject
    expectedCompanyName = $ExpectedCompanyName
    sourceRevision = $SourceRevision
    codeSigningEnhancedKeyUsageOid = $codeSigningOid
    signTool = $signToolPath
    result = if ($failed.Count -eq 0) { 'Pass' } else { 'Fail' }
    signatures = $results
}

$resolvedOutput = Resolve-OutputPath -Candidate $OutputPath
New-Item -ItemType Directory -Force `
    (Split-Path -Parent $resolvedOutput) | Out-Null
$record | ConvertTo-Json -Depth 8 |
    Set-Content -LiteralPath $resolvedOutput -Encoding utf8NoBOM

if ($failed.Count -ne 0) {
    throw "Authenticode verification failed for $($failed.Count) of " +
        "$($results.Count) files. Evidence: $resolvedOutput"
}

[PSCustomObject]$record
