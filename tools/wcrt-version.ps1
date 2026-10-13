function Get-WcrtNearestReleaseTag {
    param(
        [Parameter(Mandatory = $true)]
        [string]$RepositoryRoot
    )

    # Only version-shaped tags are considered, so an unrelated tag closer to
    # HEAD cannot mask the release it was cut from.
    $tagLines = @(& git -C $RepositoryRoot describe --tags --abbrev=0 `
        --match 'v[0-9]*.[0-9]*.[0-9]*' --match '[0-9]*.[0-9]*.[0-9]*' `
        2>$null)
    $tagExitCode = $LASTEXITCODE
    if ($tagExitCode -ne 0 -or $tagLines.Count -eq 0) {
        return '0.0.0'
    }
    $tagLines[0].ToString().Trim()
}

function Get-WcrtVersion {
    param(
        [Parameter(Mandatory = $true)]
        [string]$RepositoryRoot,

        [string]$SourceVersion
    )

    # An explicit version always wins, so release builds stay deterministic.
    # With no version supplied, fall back to the nearest release tag rather
    # than stamping 0.0.0 over a tagged revision.
    if ([string]::IsNullOrWhiteSpace($SourceVersion)) {
        $SourceVersion = Get-WcrtNearestReleaseTag `
            -RepositoryRoot $RepositoryRoot
    }

    $sourceVersion = $SourceVersion -replace '^v', ''
    if ($sourceVersion -notmatch '^([0-9]+)\.([0-9]+)\.([0-9]+)(.*)$') {
        throw "WCRT version '$SourceVersion' is not a supported semantic version."
    }
    $major = [int]$Matches[1]
    $minor = [int]$Matches[2]
    $patch = [int]$Matches[3]
    $suffix = $Matches[4]

    $gitDescribe = (& git -C $RepositoryRoot describe --tags --long --always --dirty 2>&1)
    if ($LASTEXITCODE -ne 0) {
        throw "Could not describe the WCRT Git revision: $gitDescribe"
    }
    $gitHash = (& git -C $RepositoryRoot rev-parse --short=8 HEAD 2>&1)
    if ($LASTEXITCODE -ne 0) {
        throw "Could not read the WCRT Git revision: $gitHash"
    }
    $gitDescribe = ($gitDescribe | Select-Object -First 1).ToString().Trim()
    $gitHash = ($gitHash | Select-Object -First 1).ToString().Trim()
    $exactTagLines = @(& git -C $RepositoryRoot describe --tags `
        --exact-match HEAD 2>$null)
    $exactTagResult = $LASTEXITCODE
    $exactTagText = if ($exactTagLines.Count -eq 0) {
        ''
    } else {
        $exactTagLines[0].ToString().Trim()
    }
    # WSP-WINRES-0004 requires the string version to preserve the source
    # revision needed for traceability. A modified tree is therefore not
    # the release it was tagged from, and keeps its commit identifier.
    $isDirty = $gitDescribe.EndsWith('-dirty')
    $isExactReleaseTag = $exactTagResult -eq 0 -and
        (($exactTagText -replace '^v', '') -eq $sourceVersion) -and
        -not $isDirty

    $distance = 0
    if ($gitDescribe -match '-([0-9]+)-g[0-9A-Fa-f]+(?:-dirty)?$') {
        $distance = [int]$Matches[1]
    }
    $packageVersion = "$major.$minor.$patch"
    if (-not [string]::IsNullOrEmpty($suffix)) {
        $suffix = $suffix -replace '^[._-]+', ''
        $suffix = $suffix -replace '[^0-9A-Za-z-]+', '.'
        $packageVersion += "-$suffix"
        if ($distance -gt 0) {
            $packageVersion += ".$distance"
        }
    } elseif ($distance -gt 0) {
        $packageVersion += "-dev.$distance"
    }
    if (-not $isExactReleaseTag) {
        $packageVersion += "+$gitHash"
    }
    if ($isDirty) {
        $packageVersion += '.dirty'
    }

    [PSCustomObject]@{
        SourceVersion = $sourceVersion
        PackageVersion = $packageVersion
        GitHash = $gitHash
        GitDescribe = $gitDescribe
        Major = [Math]::Min($major, 65535)
        Minor = [Math]::Min($minor, 65535)
        Patch = [Math]::Min($patch, 65535)
        Build = [Math]::Min($distance, 65535)
    }
}
