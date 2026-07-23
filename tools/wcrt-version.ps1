function Get-WcrtVersion {
    param(
        [Parameter(Mandatory = $true)]
        [string]$RepositoryRoot,

        [string]$SourceVersion = '0.0.0'
    )

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
    $packageVersion += "+$gitHash"
    if ($gitDescribe.EndsWith('-dirty')) {
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
