[CmdletBinding()]
param(
    [string]$TinyCc,
    [string]$MicrosoftCompiler,
    [string]$MicrosoftLibrarian,
    [string]$MicrosoftLinker,
    [string]$RuntimeDirectory
)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $PSScriptRoot 'test-lib.ps1')
$TinyCc = Resolve-WcrtTinyCc $TinyCc $root
$build = Join-Path $root 'build/tests/mscompat/tc-0079'
New-Item -ItemType Directory -Force -Path $build | Out-Null
$compiler = $TinyCc
if ((Split-Path -Leaf $TinyCc) -eq 'tcc-diagnostic-wrapper.cmd') {
    $compiler = (Resolve-Path -LiteralPath $env:WCRT_TEST_TINYCC).Path
}
$compilerInclude = Join-Path $build 'compiler-include'
New-Item -ItemType Directory -Force -Path $compilerInclude | Out-Null
Copy-Item -LiteralPath (Join-Path (Split-Path -Parent $compiler) `
    'include/tccdefs.h') -Destination $compilerInclude -Force
$headers = @('assert.h', 'ctype.h', 'errno.h', 'float.h', 'limits.h',
    'locale.h', 'math.h', 'setjmp.h', 'signal.h', 'stdarg.h', 'stddef.h',
    'stdio.h', 'stdlib.h', 'string.h', 'time.h', 'direct.h', 'fcntl.h',
    'io.h', 'share.h', 'sys/stat.h', 'sys/types.h', 'sys/utime.h',
    'wcrt/ms_types.h', 'sys/locking.h', 'memory.h')
foreach ($header in $headers) {
    $source = Join-Path $build ($header.Replace('/', '-').Replace('.', '-') + '.c')
    Set-Content -LiteralPath $source -Encoding ascii -Value @(
        "#include <$header>", "#include <$header>",
        'int main(void) { return 0; }'
    )
    & $TinyCc -std=c89 -DWCRT_C89=1 -Wall -Werror -nostdinc `
        -I (Join-Path $root 'include') -I $compilerInclude `
        -c $source -o "$source.o"
    if ($LASTEXITCODE -ne 0) { throw "TC-0079 header failed: $header" }
}
$qualification = 'Unknown (genuine VC6 not supplied)'
$evidencePath = $null
Invoke-WcrtCompatibilityTest -TestCase TC-0079 -RepositoryRoot $root `
    -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
    'macro-headers.c') -BehaviorSource (Join-Path $PSScriptRoot `
    'macro-headers.c') -AdditionalSources @(Join-Path $root `
    'src/platform/windows/setjmp.S') | Out-Null
Invoke-WcrtCompatibilityTest -TestCase TC-0079 -RepositoryRoot $root `
    -TinyCc $TinyCc -PresenceSource (Join-Path $PSScriptRoot `
    'c89-calls.c') -BehaviorSource (Join-Path $PSScriptRoot `
    'c89-calls.c') | Out-Null
if ($MicrosoftCompiler) {
    # Microsoft branch qualification is reported separately below.
    if (-not $MicrosoftLibrarian -or -not $MicrosoftLinker -or
        -not $RuntimeDirectory) {
        throw 'Microsoft consumer needs compiler, librarian, linker and runtime.'
    }
    $banner = @(& $MicrosoftCompiler 2>&1) -join "`n"
    if ($banner -notmatch 'Version (\d+)\.\d+') {
        throw 'Could not identify the Microsoft compiler version.'
    }
    $major = [int]$Matches[1]
    $compilerVersion = if ($banner -match
        'Version ([0-9]+(?:\.[0-9]+){1,3})') { $Matches[1] } else { 'Unknown' }
    $selection = @()
    if ($major -ne 12) {
        $selection = @('/U_MSC_VER', '/D_MSC_VER=1200')
    }
    $common = @('/nologo', '/TC', '/c', '/X', '/W4', '/WX',
        '/DWCRT_C89=1', '/DWCRT_DLL=1', "/I$(Join-Path $root 'include')")
    if ($major -gt 12) { $common += @('/GS-', '/Zl') }
    $consumerNames = @('vc6-headers', 'vc6-stat32', 'vc6-macros',
        'vc6-utime32', 'vc6-c89-calls', 'vc6-ms-errors', 'vc6-calendar-time32',
        'vc6-find32', 'byte-swap', 'vc6-default-mode', 'sentinel-copy',
        'presence/system-types', 'vc6-startup')
    $inputFiles = @(Get-ChildItem (Join-Path $root 'include') -Recurse `
        -Filter '*.h' | Select-Object -ExpandProperty FullName)
    $inputFiles += @($consumerNames | ForEach-Object {
        Join-Path $PSScriptRoot "$_.c"
    })
    $inputFiles += @(Join-Path $PSScriptRoot 'calendar-time32.c')
    $inputFiles += @(Join-Path $RuntimeDirectory 'wcrt.dll')
    $inputFiles += @(Join-Path $RuntimeDirectory 'wcrt.def')
    $inputFiles += @($PSCommandPath, (Join-Path $PSScriptRoot 'test-lib.ps1'),
        (Join-Path $root 'tests/c89/startup-test-lib.ps1'))
    $inputFiles += @($headers | ForEach-Object {
        Join-Path $build ($_.Replace('/', '-').Replace('.', '-') + '.c')
    })
    $inputHashes = @($inputFiles | ForEach-Object {
        [PSCustomObject]@{
            Path = (Resolve-Path -LiteralPath $_).Path
            Sha256 = (Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash
        }
    })
    $toolsUsed = @($MicrosoftCompiler, $MicrosoftLibrarian, $MicrosoftLinker) |
        ForEach-Object {
            $entryPath = (Get-Command $_ -ErrorAction Stop).Source
            [PSCustomObject]@{
                EntryPoint = $entryPath
                FileVersion = [Diagnostics.FileVersionInfo]::GetVersionInfo(
                    $entryPath).FileVersion
                Sha256 = (Get-FileHash -LiteralPath $entryPath `
                    -Algorithm SHA256).Hash
            }
        }
    foreach ($header in $headers) {
        $source = Join-Path $build ($header.Replace('/', '-').Replace('.', '-') + '.c')
        & $MicrosoftCompiler @common @selection $source "/Fo$source.obj"
        if ($LASTEXITCODE -ne 0) { throw "Microsoft header failed: $header" }
    }
    $library = Join-Path $build 'wcrt.lib'
    & $MicrosoftLibrarian /nologo /machine:x86 `
        "/def:$(Join-Path $RuntimeDirectory 'wcrt.def')" "/out:$library"
    if ($LASTEXITCODE -ne 0) { throw 'Microsoft import library failed.' }
    . (Join-Path $root 'tests/c89/startup-test-lib.ps1')
    $configurations = foreach ($optimization in 'Od', 'O2') {
        $objects = foreach ($name in $consumerNames) {
            $object = Join-Path $build `
                "$($name.Replace('/', '-'))-$optimization.obj"
            & $MicrosoftCompiler @common @selection "/$optimization" `
                (Join-Path $PSScriptRoot "$name.c") "/Fo$object" | Out-Host
            if ($LASTEXITCODE -ne 0) {
                throw "Microsoft consumer compile: $name /$optimization"
            }
            $object
        }
        $executable = Join-Path $RuntimeDirectory `
            "vc6-header-consumer-$optimization.exe"
        & $MicrosoftLinker /nologo /machine:x86 /nodefaultlib `
            /subsystem:console /entry:mainCRTStartup @objects $library `
            "/out:$executable" | Out-Host
        if ($LASTEXITCODE -ne 0) { throw 'Microsoft consumer link failed.' }
        if ((Get-WcrtPeInformation $executable).HostCrtImport) {
            throw 'Microsoft consumer imports a host CRT.'
        }
        & $executable | Out-Host
        if ($LASTEXITCODE -ne 0) {
            throw "Microsoft consumer /$optimization returned $LASTEXITCODE."
        }
        [PSCustomObject]@{
            Optimization = "/$optimization"
            Executable = $executable
            Sha256 = (Get-FileHash -LiteralPath $executable `
                -Algorithm SHA256).Hash
            HostCrtImport = $false
            ExitCode = 0
            Status = 'Pass'
        }
    }
    foreach ($inputHash in $inputHashes) {
        if ((Get-FileHash -LiteralPath $inputHash.Path -Algorithm SHA256).Hash `
            -ne $inputHash.Sha256) {
            throw "Consumer input changed during verification: $($inputHash.Path)"
        }
    }
    foreach ($toolEntry in $toolsUsed) {
        if ((Get-FileHash -LiteralPath $toolEntry.EntryPoint `
            -Algorithm SHA256).Hash -ne $toolEntry.Sha256) {
            throw "Consumer tool changed during verification: $($toolEntry.EntryPoint)"
        }
    }
    $qualification = if ($major -eq 12) { 'Pass (genuine VC6 consumer)' }
        else { "Unknown (supplementary MSVC $major branch test passed)" }
    $evidenceDirectory = Join-Path $root 'output/test-results/x86'
    New-Item -ItemType Directory -Force -Path $evidenceDirectory | Out-Null
    $evidencePath = Join-Path $evidenceDirectory 'vc6-consumer-results.json'
    [PSCustomObject]@{
        TestCase = 'TC-0079'
        Requirement = 'REQ-0079'
        Status = 'Pass'
        Vc6Qualification = $qualification
        ReferenceRuntimeQualification = 'Unknown (reference not supplied)'
        RecordedAtUtc = [DateTime]::UtcNow.ToString('o')
        CompilerVersion = $compilerVersion
        CompilerMajor = $major
        ToolEntries = @($toolsUsed)
        CompilerOptions = @($common + $selection)
        StandaloneHeaders = $headers
        ConsumerSources = $consumerNames
        InputHashes = $inputHashes
        Configurations = @($configurations)
    } | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $evidencePath `
        -Encoding utf8NoBOM
}
[PSCustomObject]@{
    TestCase = 'TC-0079'
    Requirement = 'REQ-0079'
    Status = 'Pass'
    Vc6Qualification = $qualification
    MicrosoftEvidence = $evidencePath
}
