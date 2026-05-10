Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$script:ScriptsRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $script:ScriptsRoot "objc3c_native_execution_smoke_helpers.psm1") -Force -DisableNameChecking
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

Export-ModuleMember -Function @(
  "Invoke-NegativeExecutionSmokeFixture",
  "Invoke-PositiveExecutionSmokeFixture"
)
