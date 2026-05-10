Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$script:ScriptsRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $script:ScriptsRoot "objc3c_native_execution_smoke_helpers.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "case_context.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "case_results.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "runtime_link.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "timings.psm1") -Force -DisableNameChecking

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
  $case = New-ExecutionSmokeCaseContext -Fixture $Fixture -Context $Context -Kind "positive"
  $fixtureRel = $case.fixture_rel
  $caseDir = $case.case_dir
  $compileDir = $case.compile_dir
  $exePath = $case.exe_path
  $compileLog = $case.compile_log
  $linkLog = $case.link_log
  Write-ExecutionSmokeProgressStart `
    -FixtureIndex $FixtureIndex `
    -TotalSelectedFixtures $TotalSelectedFixtures `
    -Kind "positive" `
    -FixtureRel $fixtureRel `
    -SuiteStopwatch $SuiteStopwatch `
    -LastCompletedFixture $LastCompletedFixture
  $expectation = Get-PositiveExpectation -FixturePath $Fixture.FullName

  $nativeArgs = Get-ExecutionSmokeNativeArgs `
    -Fixture $Fixture `
    -CompileArgs $expectation.compile_args `
    -Context $Context `
    -CompileDir $compileDir
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
  $runtimeLibrary = Get-ExecutionSmokeRuntimeLibrary -LaunchContract $launchContract
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

  $Results.Add((New-PositiveExecutionSmokeResult `
    -FixtureRel $fixtureRel `
    -Expectation $expectation `
    -LaunchContract $launchContract `
    -RuntimeLibrary $runtimeLibrary `
    -CompileExit $compileExit `
    -LinkExit $linkExit `
    -RunExit $runExit `
    -ExpectedExit $expectedExit `
    -CompileStep $compileStep `
    -LinkStep $linkStep `
    -RunStep $runStep `
    -CaseDir $caseDir `
    -RepoRoot $Context.repo_root))
  $caseStopwatch.Stop()
  $caseTiming = New-ExecutionSmokeCaseTiming `
    -Kind "positive" `
    -FixtureRel $fixtureRel `
    -DurationSeconds $caseStopwatch.Elapsed.TotalSeconds `
    -CompileStep $compileStep `
    -LinkStep $linkStep `
    -RunStep $runStep
  $CaseTimings.Add($caseTiming)
  $LastCompletedFixture.Value = $fixtureRel
  Write-Output "[PASS] positive $fixtureRel (run_exit=$runExit)"
  Write-ExecutionSmokeProgressDone `
    -FixtureIndex $FixtureIndex `
    -TotalSelectedFixtures $TotalSelectedFixtures `
    -Kind "positive" `
    -FixtureRel $fixtureRel `
    -DurationSeconds $caseTiming.duration_seconds `
    -SuiteStopwatch $SuiteStopwatch
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
  $case = New-ExecutionSmokeCaseContext -Fixture $Fixture -Context $Context -Kind "negative"
  $fixtureRel = $case.fixture_rel
  $caseDir = $case.case_dir
  $compileDir = $case.compile_dir
  $exePath = $case.exe_path
  $compileLog = $case.compile_log
  $linkLog = $case.link_log
  $runLog = $case.run_log
  $zeroStep = [pscustomobject]@{ duration_seconds = 0.0 }
  Write-ExecutionSmokeProgressStart `
    -FixtureIndex $FixtureIndex `
    -TotalSelectedFixtures $TotalSelectedFixtures `
    -Kind "negative" `
    -FixtureRel $fixtureRel `
    -SuiteStopwatch $SuiteStopwatch `
    -LastCompletedFixture $LastCompletedFixture
  $spec = Get-NegativeExpectation -FixturePath $Fixture.FullName

  $nativeArgs = Get-ExecutionSmokeNativeArgs `
    -Fixture $Fixture `
    -CompileArgs $spec.compile_args `
    -Context $Context `
    -CompileDir $compileDir
  $compileStep = Invoke-TimedLoggedCommand -StageKey "negative_compile_seconds" -Command $Context.native_exe -Arguments $nativeArgs -LogPath $compileLog
  $compileExit = [int]$compileStep.exit_code
  $compileText = Get-ExecutionSmokeCompileText -CompileDir $compileDir -CompileLog $compileLog

  if ($spec.stage -eq "compile") {
    if ($compileExit -eq 0) {
      throw "execution smoke FAIL: expected compile failure for negative fixture $fixtureRel"
    }
    $missingTokens = @(Get-MissingTokens -Text $compileText -Tokens $spec.required_link_tokens)
    if ($missingTokens.Count -gt 0) {
      throw "execution smoke FAIL: missing expected compile diagnostics for $fixtureRel (missing=$($missingTokens -join '|'))"
    }
    $Results.Add((New-CompileNegativeExecutionSmokeResult `
      -FixtureRel $fixtureRel `
      -Spec $spec `
      -CompileExit $compileExit `
      -CompileStep $compileStep `
      -CaseDir $caseDir `
      -RepoRoot $Context.repo_root))
    $caseStopwatch.Stop()
    $caseTiming = New-ExecutionSmokeCaseTiming `
      -Kind "negative" `
      -FixtureRel $fixtureRel `
      -Stage $spec.stage `
      -DurationSeconds $caseStopwatch.Elapsed.TotalSeconds `
      -CompileStep $compileStep `
      -LinkStep $zeroStep `
      -RunStep $zeroStep
    $CaseTimings.Add($caseTiming)
    $LastCompletedFixture.Value = $fixtureRel
    Write-Output "[PASS] negative $fixtureRel (stage=compile compile_exit=$compileExit)"
    Write-ExecutionSmokeProgressDone `
      -FixtureIndex $FixtureIndex `
      -TotalSelectedFixtures $TotalSelectedFixtures `
      -Kind "negative" `
      -FixtureRel $fixtureRel `
      -DurationSeconds $caseTiming.duration_seconds `
      -SuiteStopwatch $SuiteStopwatch
    return
  }

  if ($compileExit -ne 0) {
    throw "execution smoke FAIL: compile failed for negative fixture $fixtureRel (exit=$compileExit)"
  }

  $launchContract = Get-RuntimeLaunchLinkContract -CompileDir $compileDir -RepoRoot $Context.repo_root -EmitPrefix "module"
  $runtimeLibrary = Get-ExecutionSmokeRuntimeLibrary -LaunchContract $launchContract

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

    $Results.Add((New-LinkedNegativeExecutionSmokeResult `
      -FixtureRel $fixtureRel `
      -Spec $spec `
      -LaunchContract $launchContract `
      -RuntimeLibrary $runtimeLibrary `
      -CompileExit $compileExit `
      -LinkExit $linkExit `
      -RunExit -1 `
      -CompileStep $compileStep `
      -LinkStep $linkStep `
      -RunStep $zeroStep `
      -CaseDir $caseDir `
      -RepoRoot $Context.repo_root `
      -LinkDiagnosticsPath $linkDiagnosticsPath))
    $caseStopwatch.Stop()
    $caseTiming = New-ExecutionSmokeCaseTiming `
      -Kind "negative" `
      -FixtureRel $fixtureRel `
      -Stage $spec.stage `
      -DurationSeconds $caseStopwatch.Elapsed.TotalSeconds `
      -CompileStep $compileStep `
      -LinkStep $linkStep `
      -RunStep $zeroStep
    $CaseTimings.Add($caseTiming)
    $LastCompletedFixture.Value = $fixtureRel
    Write-Output "[PASS] negative $fixtureRel (stage=link link_exit=$linkExit)"
    Write-ExecutionSmokeProgressDone `
      -FixtureIndex $FixtureIndex `
      -TotalSelectedFixtures $TotalSelectedFixtures `
      -Kind "negative" `
      -FixtureRel $fixtureRel `
      -DurationSeconds $caseTiming.duration_seconds `
      -SuiteStopwatch $SuiteStopwatch
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

    $Results.Add((New-LinkedNegativeExecutionSmokeResult `
      -FixtureRel $fixtureRel `
      -Spec $spec `
      -LaunchContract $launchContract `
      -RuntimeLibrary $runtimeLibrary `
      -CompileExit $compileExit `
      -LinkExit $linkExit `
      -RunExit $runExit `
      -CompileStep $compileStep `
      -LinkStep $linkStep `
      -RunStep $runStep `
      -CaseDir $caseDir `
      -RepoRoot $Context.repo_root))
    $caseStopwatch.Stop()
    $caseTiming = New-ExecutionSmokeCaseTiming `
      -Kind "negative" `
      -FixtureRel $fixtureRel `
      -Stage $spec.stage `
      -DurationSeconds $caseStopwatch.Elapsed.TotalSeconds `
      -CompileStep $compileStep `
      -LinkStep $linkStep `
      -RunStep $runStep
    $CaseTimings.Add($caseTiming)
    $LastCompletedFixture.Value = $fixtureRel
    Write-Output "[PASS] negative $fixtureRel (stage=run run_exit=$runExit)"
    Write-ExecutionSmokeProgressDone `
      -FixtureIndex $FixtureIndex `
      -TotalSelectedFixtures $TotalSelectedFixtures `
      -Kind "negative" `
      -FixtureRel $fixtureRel `
      -DurationSeconds $caseTiming.duration_seconds `
      -SuiteStopwatch $SuiteStopwatch
    return
  }

  throw "execution smoke FAIL: unsupported negative stage '$($spec.stage)' for $fixtureRel"
}

Export-ModuleMember -Function @(
  "Invoke-NegativeExecutionSmokeFixture",
  "Invoke-PositiveExecutionSmokeFixture"
)
