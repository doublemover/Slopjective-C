Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "objc3c_native_execution_smoke_helpers.psm1") -Force -DisableNameChecking

$script:RuntimeLaunchContractScript = Join-Path $PSScriptRoot "objc3c_runtime_launch_contract.ps1"
if (!(Test-Path -LiteralPath $script:RuntimeLaunchContractScript -PathType Leaf)) {
  throw "execution smoke FAIL: runtime launch contract helper missing at $script:RuntimeLaunchContractScript"
}
. $script:RuntimeLaunchContractScript

function Resolve-Objc3cNativeExecutionSmokeConfig {
  param([Parameter(Mandatory = $true)][string]$ScriptRoot)

  $repoRoot = (Resolve-Path (Join-Path $ScriptRoot "..")).Path
  $positiveFixtureDir = Join-Path $repoRoot "tests/tooling/fixtures/native/execution/positive"
  $negativeFixtureDir = Join-Path $repoRoot "tests/tooling/fixtures/native/execution/negative"
  $defaultRuntimeLibrary = Join-Path $repoRoot "artifacts/lib/objc3_runtime.lib"
  $buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
  $suiteRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/execution-smoke"
  $configuredRunId = $env:OBJC3C_NATIVE_EXECUTION_RUN_ID
  $runId = if ([string]::IsNullOrWhiteSpace($configuredRunId)) { Get-Date -Format "yyyyMMdd_HHmmss_fff" } else { $configuredRunId }
  $runDir = Join-Path $suiteRoot $runId
  $summaryPath = Join-Path $runDir "summary.json"
  $runtimeLaunchContractScript = Join-Path $repoRoot "scripts/objc3c_runtime_launch_contract.ps1"
  $defaultNativeExe = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
  $configuredNativeExe = $env:OBJC3C_NATIVE_EXECUTABLE
  $nativeExe = if ([string]::IsNullOrWhiteSpace($configuredNativeExe)) { $defaultNativeExe } else { $configuredNativeExe }
  $nativeExeExplicit = -not [string]::IsNullOrWhiteSpace($configuredNativeExe)
  $configuredClangPath = $env:OBJC3C_NATIVE_EXECUTION_CLANG_PATH
  $clangCommand = if ([string]::IsNullOrWhiteSpace($configuredClangPath)) { "clang" } else { $configuredClangPath }
  $configuredLlcPath = $env:OBJC3C_NATIVE_EXECUTION_LLC_PATH
  $llcCommand = $configuredLlcPath
  $llcSourcePath = ""
  if ([string]::IsNullOrWhiteSpace($llcCommand)) {
    $llcCandidate = Get-Command llc -ErrorAction SilentlyContinue
    if ($null -ne $llcCandidate -and -not [string]::IsNullOrWhiteSpace($llcCandidate.Source)) {
      $llcCommand = $llcCandidate.Source
    } else {
      $llcCommand = "llc"
    }
  }
  if (-not [string]::IsNullOrWhiteSpace($llcCommand)) {
    $llcSourcePath = $llcCommand
    if ((Test-Path -LiteralPath $llcCommand -PathType Leaf) -and $llcCommand.Contains(" ")) {
      $quoted = $llcCommand.Replace('"', '""')
      $shortCandidate = cmd /d /c "for %I in (""$quoted"") do @echo %~sI" 2>$null
      if (-not [string]::IsNullOrWhiteSpace($shortCandidate)) {
        $llcCommand = $shortCandidate.Trim()
      }
    }
  }

  if (!(Test-Path -LiteralPath $runtimeLaunchContractScript -PathType Leaf)) {
    throw "execution smoke FAIL: runtime launch contract helper missing at $runtimeLaunchContractScript"
  }

  return [pscustomobject]@{
    repo_root = $repoRoot
    positive_fixture_dir = $positiveFixtureDir
    negative_fixture_dir = $negativeFixtureDir
    default_runtime_library = $defaultRuntimeLibrary
    build_script = $buildScript
    suite_root = $suiteRoot
    run_id = $runId
    run_dir = $runDir
    summary_path = $summaryPath
    runtime_launch_contract_script = $runtimeLaunchContractScript
    native_exe = $nativeExe
    native_exe_explicit = $nativeExeExplicit
    clang_command = $clangCommand
    llc_command = $llcCommand
    llc_source_path = $llcSourcePath
  }
}

function New-ExecutionSmokeStageTimings {
  return [ordered]@{
    positive_compile_seconds = 0.0
    positive_link_seconds = 0.0
    positive_run_seconds = 0.0
    negative_compile_seconds = 0.0
    negative_link_seconds = 0.0
    negative_run_seconds = 0.0
    output_report_seconds = 0.0
  }
}

function Add-StageDuration {
  param(
    [Parameter(Mandatory = $true)][string]$StageKey,
    [Parameter(Mandatory = $true)][double]$DurationSeconds
  )

  if (-not $script:stageTimings.Contains($StageKey)) {
    $script:stageTimings[$StageKey] = 0.0
  }
  $script:stageTimings[$StageKey] = [double]$script:stageTimings[$StageKey] + $DurationSeconds
}

function Invoke-TimedLoggedCommand {
  param(
    [Parameter(Mandatory = $true)][string]$StageKey,
    [Parameter(Mandatory = $true)][string]$Command,
    [Parameter(Mandatory = $true)][AllowEmptyCollection()][string[]]$Arguments,
    [Parameter(Mandatory = $true)][string]$LogPath
  )

  $stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
  $exitCode = Invoke-LoggedCommand -Command $Command -Arguments $Arguments -LogPath $LogPath
  $stopwatch.Stop()
  $durationSeconds = [math]::Round($stopwatch.Elapsed.TotalSeconds, 6)
  Add-StageDuration -StageKey $StageKey -DurationSeconds $durationSeconds
  return [pscustomobject]@{
    exit_code = [int]$exitCode
    duration_seconds = $durationSeconds
  }
}

function Get-RuntimeLaunchLinkContract {
  param(
    [Parameter(Mandatory = $true)][string]$CompileDir,
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [string]$EmitPrefix = "module"
  )

  return Get-Objc3cRuntimeLaunchContract `
    -CompileDir $CompileDir `
    -RepoRoot $RepoRoot `
    -EmitPrefix $EmitPrefix `
    -DefaultRuntimeLibraryRelativePath "artifacts/lib/objc3_runtime.lib"
}

function New-ExecutionFixtureEntries {
  param(
    [Parameter(Mandatory = $true)][object[]]$PositiveFixtures,
    [Parameter(Mandatory = $true)][object[]]$NegativeFixtures,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  return @(
    $PositiveFixtures | ForEach-Object {
      [pscustomobject]@{
        kind = "positive"
        file = $_
        relative_path = Get-RepoRelativePath -Path $_.FullName -Root $RepoRoot
      }
    }
  ) + @(
    $NegativeFixtures | ForEach-Object {
      [pscustomobject]@{
        kind = "negative"
        file = $_
        relative_path = Get-RepoRelativePath -Path $_.FullName -Root $RepoRoot
      }
    }
  )
}

function Invoke-PositiveExecutionSmokeFixture {
  param(
    [Parameter(Mandatory = $true)][object]$Fixture,
    [Parameter(Mandatory = $true)][object]$Context,
    [Parameter(Mandatory = $true)][int]$FixtureIndex,
    [Parameter(Mandatory = $true)][int]$TotalSelectedFixtures,
    [Parameter(Mandatory = $true)][System.Diagnostics.Stopwatch]$SuiteStopwatch,
    [Parameter(Mandatory = $true)][System.Collections.Generic.List[object]]$Results,
    [Parameter(Mandatory = $true)][System.Collections.Generic.List[object]]$CaseTimings,
    [Parameter(Mandatory = $true)][ref]$LastCompletedFixture
  )

  $caseStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
  $fixtureRel = Get-RepoRelativePath -Path $Fixture.FullName -Root $Context.repo_root
  Write-Output ("execution-smoke-progress: [{0}/{1}] START kind=positive fixture={2} elapsed={3:n3}s last={4}" -f $FixtureIndex, $TotalSelectedFixtures, $fixtureRel, $SuiteStopwatch.Elapsed.TotalSeconds, $LastCompletedFixture.Value)
  $expectation = Get-PositiveExpectation -FixturePath $Fixture.FullName
  $caseDirName = Get-CaseDirectoryName `
    -RunDir $Context.run_dir `
    -Kind "positive" `
    -FixtureRelativePath $fixtureRel `
    -FixtureBaseName $Fixture.BaseName
  $caseDir = Join-Path $Context.run_dir $caseDirName
  $compileDir = Join-Path $caseDir "compile"
  $exePath = Join-Path $caseDir "module.exe"
  $compileLog = Join-Path $caseDir "compile.log"
  $linkLog = Join-Path $caseDir "link.log"
  New-Item -ItemType Directory -Force -Path $compileDir | Out-Null

  $nativeArgs = @($Fixture.FullName, "--out-dir", $compileDir, "--emit-prefix", "module", "--llc", $Context.llc_command)
  if ($expectation.compile_args.Count -gt 0) {
    $nativeArgs += @($expectation.compile_args)
  }
  $compileStep = Invoke-TimedLoggedCommand -StageKey "positive_compile_seconds" -Command $Context.native_exe -Arguments $nativeArgs -LogPath $compileLog
  $compileExit = [int]$compileStep.exit_code
  if ($compileExit -ne 0) {
    throw "execution smoke FAIL: compile failed for $fixtureRel (exit=$compileExit)"
  }

  if ($expectation.requires_live_runtime_dispatch_explicit) {
    $llPath = Join-Path $compileDir "module.ll"
    Assert-RuntimeDispatchParityFromLl `
      -LlPath $llPath `
      -FixtureRel $fixtureRel `
      -RequiresLiveRuntimeDispatch $expectation.requires_live_runtime_dispatch `
      -RuntimeDispatchSymbol $expectation.runtime_dispatch_symbol
  }

  $objPath = Resolve-NativeObjectPath -CompileDir $compileDir -FixtureRel $fixtureRel

  $launchContract = Get-RuntimeLaunchLinkContract -CompileDir $compileDir -RepoRoot $Context.repo_root -EmitPrefix "module"
  $runtimeLibrary = [pscustomobject]@{
    path = $launchContract.runtime_library_path
    relative_path = $launchContract.runtime_library_relative_path
    source = $launchContract.runtime_library_source
  }
  $linkArgs = @($objPath, $runtimeLibrary.path) + @($launchContract.driver_linker_flags)
  $linkArgs += @("-o", $exePath, "-fno-color-diagnostics")
  $linkStep = Invoke-TimedLoggedCommand -StageKey "positive_link_seconds" -Command $Context.clang_command -Arguments $linkArgs -LogPath $linkLog
  $linkExit = [int]$linkStep.exit_code
  if ($linkExit -ne 0) {
    throw "execution smoke FAIL: link failed for $fixtureRel (exit=$linkExit)"
  }
  if (!(Test-Path -LiteralPath $exePath -PathType Leaf)) {
    throw "execution smoke FAIL: missing module.exe for $fixtureRel"
  }

  $runLog = Join-Path $caseDir "run.log"
  $runStep = Invoke-TimedLoggedCommand -StageKey "positive_run_seconds" -Command $exePath -Arguments @() -LogPath $runLog
  $runExit = [int]$runStep.exit_code
  $expectedExit = [int]$expectation.expected_exit
  $passed = ($runExit -eq $expectedExit)
  if (-not $passed) {
    throw "execution smoke FAIL: unexpected run exit for $fixtureRel (expected=$expectedExit actual=$runExit)"
  }

  $Results.Add([pscustomobject]@{
    kind = "positive"
    fixture = $fixtureRel
    expectation = Get-RepoRelativePath -Path $expectation.expected_path -Root $Context.repo_root
    meta = if (Test-Path -LiteralPath $expectation.meta_path -PathType Leaf) { Get-RepoRelativePath -Path $expectation.meta_path -Root $Context.repo_root } else { "" }
    native_compile_args = @($expectation.compile_args)
    requires_live_runtime_dispatch = $expectation.requires_live_runtime_dispatch
    requires_live_runtime_dispatch_explicit = $expectation.requires_live_runtime_dispatch_explicit
    runtime_dispatch_symbol = $expectation.runtime_dispatch_symbol
    launch_integration_contract_id = $launchContract.launch_integration_contract_id
    registration_manifest = $launchContract.registration_manifest_relative_path
    runtime_library = $runtimeLibrary.relative_path
    runtime_library_source = $runtimeLibrary.source
    driver_linker_flags = @($launchContract.driver_linker_flags)
    compile_exit = $compileExit
    link_exit = $linkExit
    run_exit = $runExit
    expected_exit = $expectedExit
    passed = $true
    timing = [ordered]@{
      compile_seconds = [double]$compileStep.duration_seconds
      link_seconds = [double]$linkStep.duration_seconds
      run_seconds = [double]$runStep.duration_seconds
    }
    out_dir = Get-RepoRelativePath -Path $caseDir -Root $Context.repo_root
  })
  $caseStopwatch.Stop()
  $caseTiming = [ordered]@{
    kind = "positive"
    fixture = $fixtureRel
    duration_seconds = [math]::Round($caseStopwatch.Elapsed.TotalSeconds, 6)
    compile_seconds = [double]$compileStep.duration_seconds
    link_seconds = [double]$linkStep.duration_seconds
    run_seconds = [double]$runStep.duration_seconds
  }
  $CaseTimings.Add($caseTiming)
  $LastCompletedFixture.Value = $fixtureRel
  Write-Output "[PASS] positive $fixtureRel (run_exit=$runExit)"
  Write-Output ("execution-smoke-progress: [{0}/{1}] DONE kind=positive fixture={2} duration={3:n3}s elapsed={4:n3}s" -f $FixtureIndex, $TotalSelectedFixtures, $fixtureRel, $caseTiming.duration_seconds, $SuiteStopwatch.Elapsed.TotalSeconds)
}

function Invoke-NegativeExecutionSmokeFixture {
  param(
    [Parameter(Mandatory = $true)][object]$Fixture,
    [Parameter(Mandatory = $true)][object]$Context,
    [Parameter(Mandatory = $true)][int]$FixtureIndex,
    [Parameter(Mandatory = $true)][int]$TotalSelectedFixtures,
    [Parameter(Mandatory = $true)][System.Diagnostics.Stopwatch]$SuiteStopwatch,
    [Parameter(Mandatory = $true)][System.Collections.Generic.List[object]]$Results,
    [Parameter(Mandatory = $true)][System.Collections.Generic.List[object]]$CaseTimings,
    [Parameter(Mandatory = $true)][ref]$LastCompletedFixture
  )

  $caseStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
  $fixtureRel = Get-RepoRelativePath -Path $Fixture.FullName -Root $Context.repo_root
  Write-Output ("execution-smoke-progress: [{0}/{1}] START kind=negative fixture={2} elapsed={3:n3}s last={4}" -f $FixtureIndex, $TotalSelectedFixtures, $fixtureRel, $SuiteStopwatch.Elapsed.TotalSeconds, $LastCompletedFixture.Value)
  $spec = Get-NegativeExpectation -FixturePath $Fixture.FullName
  $caseDirName = Get-CaseDirectoryName `
    -RunDir $Context.run_dir `
    -Kind "negative" `
    -FixtureRelativePath $fixtureRel `
    -FixtureBaseName $Fixture.BaseName
  $caseDir = Join-Path $Context.run_dir $caseDirName
  $compileDir = Join-Path $caseDir "compile"
  $exePath = Join-Path $caseDir "module.exe"
  $compileLog = Join-Path $caseDir "compile.log"
  $linkLog = Join-Path $caseDir "link.log"
  $runLog = Join-Path $caseDir "run.log"
  New-Item -ItemType Directory -Force -Path $compileDir | Out-Null

  $nativeArgs = @($Fixture.FullName, "--out-dir", $compileDir, "--emit-prefix", "module", "--llc", $Context.llc_command)
  if ($spec.compile_args.Count -gt 0) {
    $nativeArgs += @($spec.compile_args)
  }
  $compileStep = Invoke-TimedLoggedCommand -StageKey "negative_compile_seconds" -Command $Context.native_exe -Arguments $nativeArgs -LogPath $compileLog
  $compileExit = [int]$compileStep.exit_code
  $compileDiagPath = Join-Path $compileDir "module.diagnostics.txt"
  $compileText = if (Test-Path -LiteralPath $compileDiagPath -PathType Leaf) {
    Get-Content -LiteralPath $compileDiagPath -Raw
  } elseif (Test-Path -LiteralPath $compileLog -PathType Leaf) {
    Get-Content -LiteralPath $compileLog -Raw
  } else {
    ""
  }

  if ($spec.stage -eq "compile") {
    if ($compileExit -eq 0) {
      throw "execution smoke FAIL: expected compile failure for negative fixture $fixtureRel"
    }
    $missingTokens = @(Get-MissingTokens -Text $compileText -Tokens $spec.required_link_tokens)
    if ($missingTokens.Count -gt 0) {
      throw "execution smoke FAIL: missing expected compile diagnostics for $fixtureRel (missing=$($missingTokens -join '|'))"
    }
    $Results.Add([pscustomobject]@{
      kind = "negative"
      fixture = $fixtureRel
      expectation = Get-RepoRelativePath -Path $spec.expectation_path -Root $Context.repo_root
      stage = $spec.stage
      native_compile_args = @($spec.compile_args)
      requires_live_runtime_dispatch = $spec.requires_live_runtime_dispatch
      runtime_dispatch_symbol = $spec.runtime_dispatch_symbol
      compile_exit = $compileExit
      link_exit = -1
      run_exit = -1
      required_link_tokens = $spec.required_link_tokens
      missing_link_tokens = @()
      passed = $true
      timing = [ordered]@{
        compile_seconds = [double]$compileStep.duration_seconds
        link_seconds = 0.0
        run_seconds = 0.0
      }
      out_dir = Get-RepoRelativePath -Path $caseDir -Root $Context.repo_root
    })
    $caseStopwatch.Stop()
    $caseTiming = [ordered]@{
      kind = "negative"
      fixture = $fixtureRel
      stage = $spec.stage
      duration_seconds = [math]::Round($caseStopwatch.Elapsed.TotalSeconds, 6)
      compile_seconds = [double]$compileStep.duration_seconds
      link_seconds = 0.0
      run_seconds = 0.0
    }
    $CaseTimings.Add($caseTiming)
    $LastCompletedFixture.Value = $fixtureRel
    Write-Output "[PASS] negative $fixtureRel (stage=compile compile_exit=$compileExit)"
    Write-Output ("execution-smoke-progress: [{0}/{1}] DONE kind=negative fixture={2} duration={3:n3}s elapsed={4:n3}s" -f $FixtureIndex, $TotalSelectedFixtures, $fixtureRel, $caseTiming.duration_seconds, $SuiteStopwatch.Elapsed.TotalSeconds)
    return
  }

  if ($compileExit -ne 0) {
    throw "execution smoke FAIL: compile failed for negative fixture $fixtureRel (exit=$compileExit)"
  }

  $launchContract = Get-RuntimeLaunchLinkContract -CompileDir $compileDir -RepoRoot $Context.repo_root -EmitPrefix "module"
  $runtimeLibrary = [pscustomobject]@{
    path = $launchContract.runtime_library_path
    relative_path = $launchContract.runtime_library_relative_path
    source = $launchContract.runtime_library_source
  }

  if ($spec.requires_live_runtime_dispatch_explicit) {
    $llPath = Join-Path $compileDir "module.ll"
    Assert-RuntimeDispatchParityFromLl `
      -LlPath $llPath `
      -FixtureRel $fixtureRel `
      -RequiresLiveRuntimeDispatch $spec.requires_live_runtime_dispatch `
      -RuntimeDispatchSymbol $spec.runtime_dispatch_symbol
  }

  $objPath = Resolve-NativeObjectPath -CompileDir $compileDir -FixtureRel $fixtureRel
  $linkArgs = @($objPath, "-o", $exePath, "-fno-color-diagnostics")
  if ($spec.stage -eq "link") {
    $linkStep = Invoke-TimedLoggedCommand -StageKey "negative_link_seconds" -Command $Context.clang_command -Arguments $linkArgs -LogPath $linkLog
    $linkExit = [int]$linkStep.exit_code
    if ($linkExit -eq 0) {
      throw "execution smoke FAIL: expected link failure for negative fixture $fixtureRel"
    }

    $linkText = if (Test-Path -LiteralPath $linkLog -PathType Leaf) { Get-Content -LiteralPath $linkLog -Raw } else { "" }
    $linkDiagnosticsPath = Join-Path $caseDir "link.diagnostics.txt"
    $canonicalLinkText = Get-CanonicalLinkDiagnosticsText -RawText $linkText -ObjectPath $objPath -RepoRoot $Context.repo_root
    Set-Content -LiteralPath $linkDiagnosticsPath -Value $canonicalLinkText -Encoding utf8
    $missingTokens = @(Get-MissingTokens -Text $canonicalLinkText -Tokens $spec.required_link_tokens)
    if ($missingTokens.Count -gt 0) {
      $linkDiagnosticsRel = Get-RepoRelativePath -Path $linkDiagnosticsPath -Root $Context.repo_root
      throw "execution smoke FAIL: missing expected link diagnostics for $fixtureRel (missing=$($missingTokens -join '|') diagnostics=$linkDiagnosticsRel)"
    }

    $Results.Add([pscustomobject]@{
      kind = "negative"
      fixture = $fixtureRel
      expectation = Get-RepoRelativePath -Path $spec.expectation_path -Root $Context.repo_root
      stage = $spec.stage
      native_compile_args = @($spec.compile_args)
      requires_live_runtime_dispatch = $spec.requires_live_runtime_dispatch
      runtime_dispatch_symbol = $spec.runtime_dispatch_symbol
      launch_integration_contract_id = $launchContract.launch_integration_contract_id
      registration_manifest = $launchContract.registration_manifest_relative_path
      runtime_library = $runtimeLibrary.relative_path
      runtime_library_source = $runtimeLibrary.source
      driver_linker_flags = @($launchContract.driver_linker_flags)
      compile_exit = $compileExit
      link_exit = $linkExit
      run_exit = -1
      required_link_tokens = $spec.required_link_tokens
      missing_link_tokens = @()
      link_diagnostics = Get-RepoRelativePath -Path $linkDiagnosticsPath -Root $Context.repo_root
      passed = $true
      timing = [ordered]@{
        compile_seconds = [double]$compileStep.duration_seconds
        link_seconds = [double]$linkStep.duration_seconds
        run_seconds = 0.0
      }
      out_dir = Get-RepoRelativePath -Path $caseDir -Root $Context.repo_root
    })
    $caseStopwatch.Stop()
    $caseTiming = [ordered]@{
      kind = "negative"
      fixture = $fixtureRel
      stage = $spec.stage
      duration_seconds = [math]::Round($caseStopwatch.Elapsed.TotalSeconds, 6)
      compile_seconds = [double]$compileStep.duration_seconds
      link_seconds = [double]$linkStep.duration_seconds
      run_seconds = 0.0
    }
    $CaseTimings.Add($caseTiming)
    $LastCompletedFixture.Value = $fixtureRel
    Write-Output "[PASS] negative $fixtureRel (stage=link link_exit=$linkExit)"
    Write-Output ("execution-smoke-progress: [{0}/{1}] DONE kind=negative fixture={2} duration={3:n3}s elapsed={4:n3}s" -f $FixtureIndex, $TotalSelectedFixtures, $fixtureRel, $caseTiming.duration_seconds, $SuiteStopwatch.Elapsed.TotalSeconds)
    return
  }

  if ($spec.stage -eq "run") {
    $linkArgs = @($objPath, $runtimeLibrary.path) + @($launchContract.driver_linker_flags) + @("-o", $exePath, "-fno-color-diagnostics")
    $linkStep = Invoke-TimedLoggedCommand -StageKey "negative_link_seconds" -Command $Context.clang_command -Arguments $linkArgs -LogPath $linkLog
    $linkExit = [int]$linkStep.exit_code
    if ($linkExit -ne 0) {
      throw "execution smoke FAIL: expected successful link for run-stage negative fixture $fixtureRel (exit=$linkExit)"
    }

    $runStep = Invoke-TimedLoggedCommand -StageKey "negative_run_seconds" -Command $exePath -Arguments @() -LogPath $runLog
    $runExit = [int]$runStep.exit_code
    if ($runExit -eq 0) {
      throw "execution smoke FAIL: expected non-zero run exit for negative fixture $fixtureRel"
    }
    $runText = if (Test-Path -LiteralPath $runLog -PathType Leaf) { Get-Content -LiteralPath $runLog -Raw } else { "" }
    $missingTokens = @(Get-MissingTokens -Text $runText -Tokens $spec.required_link_tokens)
    if ($missingTokens.Count -gt 0) {
      throw "execution smoke FAIL: missing expected run diagnostics for $fixtureRel (missing=$($missingTokens -join '|'))"
    }

    $Results.Add([pscustomobject]@{
      kind = "negative"
      fixture = $fixtureRel
      expectation = Get-RepoRelativePath -Path $spec.expectation_path -Root $Context.repo_root
      stage = $spec.stage
      native_compile_args = @($spec.compile_args)
      requires_live_runtime_dispatch = $spec.requires_live_runtime_dispatch
      runtime_dispatch_symbol = $spec.runtime_dispatch_symbol
      launch_integration_contract_id = $launchContract.launch_integration_contract_id
      registration_manifest = $launchContract.registration_manifest_relative_path
      runtime_library = $runtimeLibrary.relative_path
      runtime_library_source = $runtimeLibrary.source
      driver_linker_flags = @($launchContract.driver_linker_flags)
      compile_exit = $compileExit
      link_exit = $linkExit
      run_exit = $runExit
      required_link_tokens = $spec.required_link_tokens
      missing_link_tokens = @()
      passed = $true
      timing = [ordered]@{
        compile_seconds = [double]$compileStep.duration_seconds
        link_seconds = [double]$linkStep.duration_seconds
        run_seconds = [double]$runStep.duration_seconds
      }
      out_dir = Get-RepoRelativePath -Path $caseDir -Root $Context.repo_root
    })
    $caseStopwatch.Stop()
    $caseTiming = [ordered]@{
      kind = "negative"
      fixture = $fixtureRel
      stage = $spec.stage
      duration_seconds = [math]::Round($caseStopwatch.Elapsed.TotalSeconds, 6)
      compile_seconds = [double]$compileStep.duration_seconds
      link_seconds = [double]$linkStep.duration_seconds
      run_seconds = [double]$runStep.duration_seconds
    }
    $CaseTimings.Add($caseTiming)
    $LastCompletedFixture.Value = $fixtureRel
    Write-Output "[PASS] negative $fixtureRel (stage=run run_exit=$runExit)"
    Write-Output ("execution-smoke-progress: [{0}/{1}] DONE kind=negative fixture={2} duration={3:n3}s elapsed={4:n3}s" -f $FixtureIndex, $TotalSelectedFixtures, $fixtureRel, $caseTiming.duration_seconds, $SuiteStopwatch.Elapsed.TotalSeconds)
    return
  }

  throw "execution smoke FAIL: unsupported negative stage '$($spec.stage)' for $fixtureRel"
}

function Write-ExecutionSmokeSummary {
  param(
    [Parameter(Mandatory = $true)][object]$Context,
    [Parameter(Mandatory = $true)][System.Diagnostics.Stopwatch]$SuiteStopwatch,
    [Parameter(Mandatory = $true)][System.Collections.Generic.List[object]]$Results,
    [Parameter(Mandatory = $true)][System.Collections.Generic.List[object]]$CaseTimings,
    [string]$FixtureList,
    [string]$FixtureGlob,
    [Parameter(Mandatory = $true)][int]$ShardIndex,
    [Parameter(Mandatory = $true)][int]$ShardCount,
    [Parameter(Mandatory = $true)][int]$Limit,
    [Parameter(Mandatory = $true)][int]$SelectedPositiveCount,
    [Parameter(Mandatory = $true)][int]$SelectedNegativeCount
  )

  $resultItems = @($Results.ToArray())
  $caseTimingItems = @($CaseTimings.ToArray())
  $total = $resultItems.Count
  $passedCount = @($resultItems | Where-Object { $_.passed }).Count
  $failedCount = $total - $passedCount
  $reportStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
  $slowestFixtures = @($caseTimingItems | Sort-Object -Property duration_seconds -Descending | Select-Object -First 10)
  $summary = [ordered]@{
    run_dir = Get-RepoRelativePath -Path $Context.run_dir -Root $Context.repo_root
    compile_command = if (Test-Path -LiteralPath $Context.native_exe -PathType Leaf) { Get-RepoRelativePath -Path $Context.native_exe -Root $Context.repo_root } else { $Context.native_exe }
    runtime_launch_contract_script = Get-RepoRelativePath -Path $Context.runtime_launch_contract_script -Root $Context.repo_root
    native_exe = if (Test-Path -LiteralPath $Context.native_exe -PathType Leaf) { Get-RepoRelativePath -Path $Context.native_exe -Root $Context.repo_root } else { $Context.native_exe }
    runtime_library = if (Test-Path -LiteralPath $Context.default_runtime_library -PathType Leaf) { Get-RepoRelativePath -Path $Context.default_runtime_library -Root $Context.repo_root } else { "" }
    live_runtime_dispatch_default_symbol = "objc3_runtime_dispatch_i32"
    clang = $Context.clang_command
    llc = $Context.llc_command
    llc_source = $Context.llc_source_path
    selection = [ordered]@{
      fixture_list = $FixtureList
      fixture_glob = $FixtureGlob
      shard_index = $ShardIndex
      shard_count = $ShardCount
      limit = $Limit
      selected_positive = $SelectedPositiveCount
      selected_negative = $SelectedNegativeCount
    }
    total = $total
    passed = $passedCount
    failed = $failedCount
    status = if ($failedCount -eq 0) { "PASS" } else { "FAIL" }
    timing = [ordered]@{
      elapsed_seconds = [math]::Round($SuiteStopwatch.Elapsed.TotalSeconds, 6)
      stage_totals = $script:stageTimings
      slowest_fixtures = @($slowestFixtures)
      fixture_timings = @($caseTimingItems)
    }
    results = @($resultItems)
  }
  $reportStopwatch.Stop()
  Add-StageDuration -StageKey "output_report_seconds" -DurationSeconds ([math]::Round($reportStopwatch.Elapsed.TotalSeconds, 6))
  $summary.timing.stage_totals = $script:stageTimings
  $summary.timing.elapsed_seconds = [math]::Round($SuiteStopwatch.Elapsed.TotalSeconds, 6)
  $summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $Context.summary_path -Encoding utf8
  Write-Output "summary_path: $(Get-RepoRelativePath -Path $Context.summary_path -Root $Context.repo_root)"
  Write-Output "status: PASS"
  $global:LASTEXITCODE = 0
}

function Invoke-Objc3cNativeExecutionSmoke {
  param(
    [Parameter(Mandatory = $true)][string]$ScriptRoot,
    [string]$FixtureList = "",
    [string]$FixtureGlob = "",
    [int]$ShardIndex = -1,
    [int]$ShardCount = 0,
    [int]$Limit = 0
  )

  $context = Resolve-Objc3cNativeExecutionSmokeConfig -ScriptRoot $ScriptRoot
  New-Item -ItemType Directory -Force -Path $context.run_dir | Out-Null
  Push-Location $context.repo_root
  try {
    $suiteStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
    $script:stageTimings = New-ExecutionSmokeStageTimings

    Ensure-NativeCompilerExecutable `
      -NativeExePath $context.native_exe `
      -NativeExeExplicit $context.native_exe_explicit `
      -BuildScriptPath $context.build_script

    $clangCheckExit = Invoke-LoggedCommand -Command $context.clang_command -Arguments @("--version") -LogPath (Join-Path $context.run_dir "clang-version.log")
    if ($clangCheckExit -ne 0) {
      throw "execution smoke FAIL: clang command is unavailable: $($context.clang_command)"
    }

    $positiveFixtures = Get-Fixtures -Directory $context.positive_fixture_dir -FixtureKind "positive execution"
    $negativeFixtures = Get-Fixtures -Directory $context.negative_fixture_dir -FixtureKind "negative execution"
    $executionFixtureEntries = New-ExecutionFixtureEntries `
      -PositiveFixtures $positiveFixtures `
      -NegativeFixtures $negativeFixtures `
      -RepoRoot $context.repo_root
    $selectedExecutionEntries = Select-ExecutionFixtureEntries `
      -Entries $executionFixtureEntries `
      -FixtureListPath $FixtureList `
      -FixtureGlobPattern $FixtureGlob `
      -ShardIndexValue $ShardIndex `
      -ShardCountValue $ShardCount `
      -LimitValue $Limit `
      -RepoRoot $context.repo_root
    $selectedPositiveFixtures = @($selectedExecutionEntries | Where-Object { $_.kind -eq "positive" } | ForEach-Object { $_.file })
    $selectedNegativeFixtures = @($selectedExecutionEntries | Where-Object { $_.kind -eq "negative" } | ForEach-Object { $_.file })
    Write-Output ("selection: positive={0} negative={1}" -f $selectedPositiveFixtures.Count, $selectedNegativeFixtures.Count)

    $results = [System.Collections.Generic.List[object]]::new()
    $caseTimings = [System.Collections.Generic.List[object]]::new()
    $totalSelectedFixtures = $selectedPositiveFixtures.Count + $selectedNegativeFixtures.Count
    $fixtureIndex = 0
    $lastCompletedFixture = "none"

    foreach ($fixture in $selectedPositiveFixtures) {
      $fixtureIndex += 1
      Invoke-PositiveExecutionSmokeFixture `
        -Fixture $fixture `
        -Context $context `
        -FixtureIndex $fixtureIndex `
        -TotalSelectedFixtures $totalSelectedFixtures `
        -SuiteStopwatch $suiteStopwatch `
        -Results $results `
        -CaseTimings $caseTimings `
        -LastCompletedFixture ([ref]$lastCompletedFixture)
    }

    foreach ($fixture in $selectedNegativeFixtures) {
      $fixtureIndex += 1
      Invoke-NegativeExecutionSmokeFixture `
        -Fixture $fixture `
        -Context $context `
        -FixtureIndex $fixtureIndex `
        -TotalSelectedFixtures $totalSelectedFixtures `
        -SuiteStopwatch $suiteStopwatch `
        -Results $results `
        -CaseTimings $caseTimings `
        -LastCompletedFixture ([ref]$lastCompletedFixture)
    }

    Write-ExecutionSmokeSummary `
      -Context $context `
      -SuiteStopwatch $suiteStopwatch `
      -Results $results `
      -CaseTimings $caseTimings `
      -FixtureList $FixtureList `
      -FixtureGlob $FixtureGlob `
      -ShardIndex $ShardIndex `
      -ShardCount $ShardCount `
      -Limit $Limit `
      -SelectedPositiveCount $selectedPositiveFixtures.Count `
      -SelectedNegativeCount $selectedNegativeFixtures.Count
  }
  finally {
    Pop-Location
  }
}

Export-ModuleMember -Function "Invoke-Objc3cNativeExecutionSmoke"
