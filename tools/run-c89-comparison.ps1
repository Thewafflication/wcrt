[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'

$repoRoot = Split-Path -Parent $PSScriptRoot
$presenceDirectory = Join-Path $repoRoot 'tests\c89\presence'
$evidenceRoot = Join-Path $repoRoot 'output\test-results'
$buildRoot = Join-Path $repoRoot 'build\tests\c89\comparison'
$jsonPath = Join-Path $evidenceRoot 'c89-comparison.json'

$testMap = @(
    @{ Id = '0001'; Name = 'assert'; Scope = 'assert.h' },
    @{ Id = '0002'; Name = 'ctype'; Scope = 'ctype.h' },
    @{ Id = '0003'; Name = 'errno'; Scope = 'errno.h' },
    @{ Id = '0004'; Name = 'float'; Scope = 'float.h' },
    @{ Id = '0005'; Name = 'limits'; Scope = 'limits.h' },
    @{ Id = '0006'; Name = 'locale'; Scope = 'locale.h' },
    @{ Id = '0007'; Name = 'math'; Scope = 'math.h' },
    @{ Id = '0008'; Name = 'setjmp'; Scope = 'setjmp.h' },
    @{ Id = '0009'; Name = 'signal'; Scope = 'signal.h' },
    @{ Id = '0010'; Name = 'stdarg'; Scope = 'stdarg.h' },
    @{ Id = '0011'; Name = 'stddef'; Scope = 'stddef.h' },
    @{ Id = '0012'; Name = 'stdio'; Scope = 'stdio.h' },
    @{ Id = '0013'; Name = 'stdlib'; Scope = 'stdlib.h' },
    @{ Id = '0014'; Name = 'string'; Scope = 'string.h' },
    @{ Id = '0015'; Name = 'time'; Scope = 'time.h' },
    @{ Id = '0016'; Name = 'source-documentation'; Scope = 'Source lint' }
)

function Find-TinyCc {
    $command = Get-Command tcc.exe -ErrorAction SilentlyContinue |
        Select-Object -First 1 -ExpandProperty Source
    if ($command) {
        return $command
    }

    $packaged = Join-Path (Split-Path -Parent $repoRoot) `
        'tcc_package\out\build\x64-debug\package\tcc.exe'
    if (Test-Path -LiteralPath $packaged) {
        return $packaged
    }

    throw 'TinyCC was not found.'
}

function Find-MsvcEnvironment {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} `
        'Microsoft Visual Studio\Installer\vswhere.exe'
    $installation = (& $vswhere -latest -products * `
        -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
        -property installationPath | Select-Object -First 1)
    if (-not $installation) {
        throw 'A Visual Studio C/C++ toolchain was not found.'
    }

    $versionFile = Join-Path $installation `
        'VC\Auxiliary\Build\Microsoft.VCToolsVersion.default.txt'
    $toolsVersion = (Get-Content -LiteralPath $versionFile -Raw).Trim()
    $toolsRoot = Join-Path $installation "VC\Tools\MSVC\$toolsVersion"
    $compiler = Join-Path $toolsRoot 'bin\Hostx64\x64\cl.exe'

    $kitsBase = Join-Path ${env:ProgramFiles(x86)} 'Windows Kits\10'
    $kitsRoot = Join-Path $kitsBase 'Include'
    $sdk = Get-ChildItem -LiteralPath $kitsRoot -Directory |
        Where-Object {
            Test-Path -LiteralPath (Join-Path $_.FullName 'ucrt')
        } |
        Sort-Object { [version]$_.Name } -Descending |
        Select-Object -First 1

    return [PSCustomObject]@{
        Compiler = $compiler
        CompilerVersion = (Get-Item $compiler).VersionInfo.FileVersion
        ToolsetVersion = $toolsVersion
        UcrtVersion = $sdk.Name
        IncludeDirectories = @(
            (Join-Path $toolsRoot 'include')
            (Join-Path $sdk.FullName 'ucrt')
            (Join-Path $sdk.FullName 'shared')
            (Join-Path $sdk.FullName 'um')
        )
        LibraryDirectories = @(
            (Join-Path $toolsRoot 'lib\x64')
            (Join-Path $kitsBase "Lib\$($sdk.Name)\ucrt\x64")
            (Join-Path $kitsBase "Lib\$($sdk.Name)\um\x64")
        )
    }
}

function Write-TestEvidence {
    param([object]$Result)

    $directory = Join-Path $evidenceRoot "c89-$($Result.TargetKey)"
    New-Item -ItemType Directory -Force -Path $directory | Out-Null
    $path = Join-Path $directory `
        "tc-$($Result.Id)-$($Result.Name)-evidence.txt"
    @(
        "Test Case: $($Result.TestCase)"
        "Requirement: $($Result.Requirement)"
        "Target: $($Result.Target)"
        "Status: $($Result.Status)"
        "Started: $($Result.Started)"
        "Finished: $($Result.Finished)"
        "DurationSeconds: $($Result.DurationSeconds)"
        "ExitCode: $($Result.ExitCode)"
        "Command: $($Result.Command)"
        'Output:'
        $Result.Output
    ) | Set-Content -LiteralPath $path -Encoding UTF8
    $Result | Add-Member -NotePropertyName EvidenceFile -NotePropertyValue `
        $path
}

function New-TestResult {
    param(
        [hashtable]$Test,
        [string]$Target,
        [string]$TargetKey,
        [string]$Status,
        [object]$ExitCode,
        [datetime]$Started,
        [datetime]$Finished,
        [double]$Duration,
        [string]$Command,
        [string]$Output
    )

    $result = [PSCustomObject]@{
        Id = $Test.Id
        TestCase = "TC-$($Test.Id)"
        Requirement = "REQ-$($Test.Id)"
        Name = $Test.Name
        Scope = $Test.Scope
        Target = $Target
        TargetKey = $TargetKey
        Status = $Status
        ExitCode = $ExitCode
        Started = $Started.ToString('o')
        Finished = $Finished.ToString('o')
        DurationSeconds = [Math]::Round($Duration, 3)
        Command = $Command
        Output = $Output
    }
    Write-TestEvidence $result
    return $result
}

function Invoke-WcrtTest {
    param([hashtable]$Test, [string]$TinyCc)

    $started = Get-Date
    $timer = [System.Diagnostics.Stopwatch]::StartNew()
    $exitCode = $null
    $status = 'Fail'
    $output = ''
    $command = ''

    if ($Test.Id -eq '0016') {
        $runner = Join-Path $repoRoot 'tests\c89\run-tc-0016.ps1'
        $command = 'tests/c89/run-tc-0016.ps1'
        if (Test-Path -LiteralPath $runner) {
            try {
                $runnerResult = & $runner
                $status = $runnerResult.Status
                $exitCode = if ($status -eq 'Pass') { 0 } else { 1 }
                $output = $runnerResult.Output
            } catch {
                $exitCode = 1
                $output = $_.Exception.Message
            }
        } else {
            $status = 'NotImplemented'
            $output = 'TC-0016 runner is not implemented.'
        }
    } else {
        $source = Join-Path $presenceDirectory "$($Test.Name).c"
        $relativeSource = [System.IO.Path]::GetRelativePath(
            $repoRoot, $source).Replace('\', '/')
        $sourceHash = (Get-FileHash -LiteralPath $source `
            -Algorithm SHA256).Hash
        $objectDirectory = Join-Path $buildRoot 'wcrt'
        New-Item -ItemType Directory -Force -Path $objectDirectory | Out-Null
        $object = Join-Path $objectDirectory "$($Test.Name).o"
        $tccInclude = Join-Path (Split-Path -Parent $TinyCc) 'include'
        $intrinsicDirectory = Join-Path $buildRoot 'tcc-intrinsic'
        New-Item -ItemType Directory -Force -Path $intrinsicDirectory |
            Out-Null
        Copy-Item -LiteralPath (Join-Path $tccInclude 'tccdefs.h') `
            -Destination $intrinsicDirectory -Force
        $arguments = @(
            '-std=c89', '-Werror', '-nostdinc',
            '-I', (Join-Path $repoRoot 'include'),
            '-I', $intrinsicDirectory, '-c', $source, '-o', $object
        )
        $command = "tcc $($arguments -join ' ')"
        $lines = & $TinyCc @arguments 2>&1
        $exitCode = $LASTEXITCODE
        $status = if ($exitCode -eq 0) { 'Pass' } else { 'Fail' }
        $compilerOutput = ($lines | ForEach-Object {
            $_.ToString()
        }) -join "`n"
        if ([string]::IsNullOrWhiteSpace($compilerOutput)) {
            $compilerOutput = '(no compiler diagnostics)'
        }
        $output = @(
            'Comparison phase: C89 API presence compile'
            "Test source: $relativeSource"
            "Test source SHA-256: $sourceHash"
            "Compiler: $TinyCc"
            "Compiler version: $((& $TinyCc -v 2>&1 | `
                Select-Object -First 1).ToString())"
            'Library include target: include/'
            'Host standard headers disabled: yes'
            "Command: $command"
            "Exit code: $exitCode"
            'Compiler output:'
            $compilerOutput
        ) -join "`n"

        $runner = Join-Path $repoRoot `
            "tests\c89\run-tc-$($Test.Id).ps1"
        if ((Test-Path -LiteralPath $runner) -and $exitCode -eq 0) {
            try {
                $runnerResult = & $runner -TinyCc $TinyCc
                $output += "`n`nSupplemental WCRT behavioral phase:`n"
                $output += ($runnerResult | Format-List | Out-String).Trim()
            } catch {
                $status = 'Fail'
                $exitCode = 1
                $output += "`n`nSupplemental WCRT behavioral phase failed:`n"
                $output += $_.Exception.Message
            }
        }
    }

    $timer.Stop()
    return New-TestResult $Test 'WCRT' 'wcrt' $status $exitCode `
        $started (Get-Date) $timer.Elapsed.TotalSeconds $command $output
}

function Invoke-UcrtTest {
    param([hashtable]$Test, [object]$Msvc)

    $started = Get-Date
    $timer = [System.Diagnostics.Stopwatch]::StartNew()
    if ($Test.Id -eq '0016') {
        $timer.Stop()
        return New-TestResult $Test 'Microsoft UCRT' 'mscrt' `
            'NotApplicable' $null $started (Get-Date) `
            $timer.Elapsed.TotalSeconds '(not applicable)' `
            'Project source lint rules do not apply to Microsoft UCRT.'
    }

    $source = Join-Path $presenceDirectory "$($Test.Name).c"
    $relativeSource = [System.IO.Path]::GetRelativePath(
        $repoRoot, $source).Replace('\', '/')
    $sourceHash = (Get-FileHash -LiteralPath $source `
        -Algorithm SHA256).Hash
    $arguments = [System.Collections.Generic.List[string]]::new()
    @('/nologo', '/TC', '/Zs', '/W3', '/WX',
        '/D_CRT_SECURE_NO_WARNINGS', '/X') |
        ForEach-Object { $arguments.Add($_) }
    $Msvc.IncludeDirectories |
        ForEach-Object { $arguments.Add("/I$_") }
    $arguments.Add($source)

    $command = "cl $($arguments -join ' ')"
    $lines = & $Msvc.Compiler @arguments 2>&1
    $exitCode = $LASTEXITCODE
    $status = if ($exitCode -eq 0) { 'Pass' } else { 'Fail' }
    $compilerOutput = ($lines | ForEach-Object {
        $_.ToString()
    }) -join "`n"
    if ([string]::IsNullOrWhiteSpace($compilerOutput)) {
        $compilerOutput = '(no compiler diagnostics)'
    }
    $output = @(
        'Comparison phase: C89 API presence compile'
        "Test source: $relativeSource"
        "Test source SHA-256: $sourceHash"
        "Compiler: $($Msvc.Compiler)"
        "Compiler version: $($Msvc.CompilerVersion)"
        "Library include target: Microsoft UCRT $($Msvc.UcrtVersion)"
        'WCRT and ambient host headers disabled: yes (/X)'
        "Command: $command"
        "Exit code: $exitCode"
        'Compiler output:'
        $compilerOutput
    ) -join "`n"

    $behaviorSource = Join-Path $repoRoot `
        "tests\c89\$($Test.Name).c"
    if ($Test.Id -eq '0001' -and $exitCode -eq 0) {
        $behaviorDirectory = Join-Path $buildRoot 'mscrt\tc-0001'
        New-Item -ItemType Directory -Force -Path $behaviorDirectory |
            Out-Null
        $executable = Join-Path $behaviorDirectory 'assert-test.exe'
        $behaviorArguments = [System.Collections.Generic.List[string]]::new()
        @('/nologo', '/TC', '/W3', '/WX',
            '/D_CRT_SECURE_NO_WARNINGS', '/X') |
            ForEach-Object { $behaviorArguments.Add($_) }
        $Msvc.IncludeDirectories |
            ForEach-Object { $behaviorArguments.Add("/I$_") }
        $behaviorArguments.Add((Join-Path $repoRoot 'tests\c89\assert.c'))
        $behaviorArguments.Add(
            (Join-Path $repoRoot 'tests\c89\assert_disabled.c')
        )
        $behaviorArguments.Add("/Fe:$executable")
        $behaviorArguments.Add('/link')
        $Msvc.LibraryDirectories |
            ForEach-Object { $behaviorArguments.Add("/LIBPATH:$_") }
        $behaviorCommand = "cl $($behaviorArguments -join ' ')"
        $buildLines = & $Msvc.Compiler @behaviorArguments 2>&1
        $buildExitCode = $LASTEXITCODE
        $buildOutput = ($buildLines | ForEach-Object {
            $_.ToString()
        }) -join "`n"

        $enabledStatus = 'Fail'
        $terminationStatus = 'Fail'
        $failureExitCode = 'not run'
        $diagnostic = $buildOutput
        if ($buildExitCode -eq 0) {
            & $executable
            $normalExitCode = $LASTEXITCODE
            $enabledStatus = if ($normalExitCode -eq 0) {
                'Pass'
            } else {
                'Fail'
            }
            $failureLines = & $executable fail 2>&1
            $failureExitCode = $LASTEXITCODE
            $diagnostic = ($failureLines | ForEach-Object {
                $_.ToString()
            }) -join "`n"
            $terminationStatus = if (
                $failureExitCode -ne 0 -and
                $diagnostic -match 'Assertion failed: 0' -and
                $diagnostic -match 'assert\.c' -and
                $diagnostic -match 'line [0-9]+'
            ) { 'Pass' } else { 'Fail' }
        }

        if ($buildExitCode -ne 0 -or $enabledStatus -ne 'Pass' -or
            $terminationStatus -ne 'Pass') {
            $status = 'Fail'
            $exitCode = 1
        }
        $output += @(
            ''
            'Supplemental Microsoft UCRT behavioral phase:'
            'TestCase : TC-0001'
            'Requirement : REQ-0001'
            "Compiler : Microsoft C/C++ $($Msvc.CompilerVersion)"
            'StandaloneHeader : Pass'
            "EnabledAndNDebug : $enabledStatus"
            "FailureTermination : $terminationStatus"
            "FailureExitCode : $failureExitCode"
            "Diagnostic : $diagnostic"
            "BuildCommand : $behaviorCommand"
            "BuildExitCode : $buildExitCode"
        ) -join "`n"
    } elseif ((Test-Path -LiteralPath $behaviorSource) -and
        $exitCode -eq 0) {
        $behaviorDirectory = Join-Path $buildRoot `
            "mscrt\tc-$($Test.Id)"
        New-Item -ItemType Directory -Force -Path $behaviorDirectory |
            Out-Null
        $executable = Join-Path $behaviorDirectory `
            "$($Test.Name)-test.exe"
        $behaviorArguments = [System.Collections.Generic.List[string]]::new()
        @('/nologo', '/TC', '/W3', '/WX',
            '/D_CRT_SECURE_NO_WARNINGS', '/X') |
            ForEach-Object { $behaviorArguments.Add($_) }
        $Msvc.IncludeDirectories |
            ForEach-Object { $behaviorArguments.Add("/I$_") }
        $behaviorArguments.Add($behaviorSource)
        $behaviorArguments.Add("/Fe:$executable")
        $behaviorArguments.Add('/link')
        $Msvc.LibraryDirectories |
            ForEach-Object { $behaviorArguments.Add("/LIBPATH:$_") }
        $behaviorCommand = "cl $($behaviorArguments -join ' ')"
        $buildLines = & $Msvc.Compiler @behaviorArguments 2>&1
        $buildExitCode = $LASTEXITCODE
        $buildOutput = ($buildLines | ForEach-Object {
            $_.ToString()
        }) -join "`n"
        $executionExitCode = 'not run'
        $behaviorStatus = 'Fail'
        if ($buildExitCode -eq 0) {
            & $executable
            $executionExitCode = $LASTEXITCODE
            $behaviorStatus = if ($executionExitCode -eq 0) {
                'Pass'
            } else {
                'Fail'
            }
        }
        if ($behaviorStatus -ne 'Pass') {
            $status = 'Fail'
            $exitCode = 1
        }
        $output += @(
            ''
            'Supplemental Microsoft UCRT behavioral phase:'
            "TestCase : TC-$($Test.Id)"
            "Requirement : REQ-$($Test.Id)"
            "Compiler : Microsoft C/C++ $($Msvc.CompilerVersion)"
            "BehaviorSource : tests/c89/$($Test.Name).c"
            "BuildExitCode : $buildExitCode"
            "ExecutionExitCode : $executionExitCode"
            "Result : $behaviorStatus"
            "BuildOutput : $buildOutput"
            "BuildCommand : $behaviorCommand"
        ) -join "`n"
    }

    $timer.Stop()
    return New-TestResult $Test 'Microsoft UCRT' 'mscrt' $status `
        $exitCode $started (Get-Date) $timer.Elapsed.TotalSeconds `
        $command $output
}

New-Item -ItemType Directory -Force -Path $evidenceRoot | Out-Null
New-Item -ItemType Directory -Force -Path $buildRoot | Out-Null

$tinyCc = Find-TinyCc
$msvc = Find-MsvcEnvironment
$started = Get-Date
$results = [System.Collections.Generic.List[object]]::new()
foreach ($test in $testMap) {
    $results.Add((Invoke-WcrtTest $test $tinyCc))
    $results.Add((Invoke-UcrtTest $test $msvc))
}
$finished = Get-Date

$record = [PSCustomObject]@{
    Started = $started.ToString('o')
    Finished = $finished.ToString('o')
    DurationSeconds = [Math]::Round(($finished - $started).TotalSeconds, 3)
    WcrtCompiler = (& $tinyCc -v 2>&1 | Select-Object -First 1).ToString()
    UcrtCompiler = "Microsoft C/C++ $($msvc.CompilerVersion)"
    UcrtVersion = $msvc.UcrtVersion
    Results = $results
}
$record | ConvertTo-Json -Depth 7 |
    Set-Content -LiteralPath $jsonPath -Encoding UTF8

Write-Output $jsonPath
