param(
  [string]$FixtureList = "",
  [string]$FixtureGlob = "",
  [int]$ShardIndex = -1,
  [int]$ShardCount = 0,
  [int]$Limit = 0
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "objc3c_native_execution_smoke_helpers.psm1") -Force -DisableNameChecking

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$positiveFixtureDir = Join-Path $repoRoot "tests/tooling/fixtures/native/execution/positive"
$negativeFixtureDir = Join-Path $repoRoot "tests/tooling/fixtures/native/execution/negative"
$defaultRuntimeLibrary = Join-Path $repoRoot "artifacts/lib/objc3_runtime.lib"
$buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
$suiteRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/execution-smoke"
# Suite ownership: this script is the authoritative owner for compile/link/run
# execution behavior. Recovery, replay/native-truth, runtime acceptance, and
# negative-fixture header enforcement must stay on their dedicated suites so the
# public runner can remove duplicate heavy recompilation.
# objc3c.execution.runnablesample.surface.v1 anchor: execution smoke remains the
# scalar/core corpus boundary rooted at tests/tooling/fixtures/native/execution.
# Broader object/property/import-module samples stay frozen as separate proof
# families until the canonical runnable sample surface widens the sample set.
# objc3c.execution.canonicalrunnablesamples.v1 anchor: the integrated object/property/category/protocol sample exists as a dedicated proof asset,
# but execution smoke still remains the scalar/core corpus gate in this issue.
# objc3c.execution.runnablecore.compatibilityguard.v1 anchor: advanced unsupported features must fail closed instead of counting as runnable smoke coverage.
# objc3c.execution.unsupportedfeature.diagnostics.v1 anchor: `O3S221` fail-closed diagnostics for accepted advanced surfaces
# must stay outside runnable smoke counts and never be treated as successful runtime coverage.
# objc3c.execution.replayinspection.freeze.v1 anchor: execution smoke remains the
# canonical runnable replay corpus boundary for scalar/core coverage. Broader
# object inspection is frozen onto the dedicated A002 sample instead of widening
# this smoke script.
# objc3c.execution.objectir.replayproof.v1 anchor: scalar/core smoke remains the base
# runtime replay corpus, while canonical object/IR replay plus metadata section
# inspection is delegated to execution replay proof over the A002 runnable
# sample.
# objc3c.execution.toolchainruntime.operations.v1 anchor: execution smoke remains one of
# the frozen runnable-core operations, and installer or cross-platform packaging claims remain outside this freeze until the workflow-package surface widens.
# objc3c.execution.workflowpackage.surface.v1 anchor: this script must run unchanged from a
# staged runnable toolchain bundle root that preserves the current repo-relative
# scripts/artifacts/tests layout under a local package root.
# objc3c.execution.platformbringup.surface.v1 anchor: supported repo-root/package-root execution
# must document the live override surface for `OBJC3C_NATIVE_EXECUTABLE`,
# `OBJC3C_NATIVE_EXECUTION_CLANG_PATH`, `OBJC3C_NATIVE_EXECUTION_LLC_PATH`, and
# `OBJC3C_NATIVE_EXECUTION_RUN_ID` on the supported Windows host baseline.
# objc3c.execution.releasegate.freeze.v1 anchor: execution smoke remains one preserved
# closeout-gate input alongside the canonical runnable sample, unsupported-feature diagnostics, and object replay proof surfaces,
# plus the platform-bringup surface; full matrix expansion remains deferred to the conformance-matrix surface.
# objc3c.execution.conformancematrix.surface.v1 anchor: execution smoke remains one concrete
# command-backed row in the runnable conformance matrix rather than an implicit
# blanket claim about unsupported runtime surfaces.
# objc3c.execution.closeoutsummary.surface.v1 anchor: execution smoke remains one preserved
# operator command in the final execution closeout runbook and sign-off summary.
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
. $runtimeLaunchContractScript

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

New-Item -ItemType Directory -Force -Path $runDir | Out-Null
Push-Location $repoRoot
try {
  $suiteStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
  $script:stageTimings = [ordered]@{
    positive_compile_seconds = 0.0
    positive_link_seconds = 0.0
    positive_run_seconds = 0.0
    negative_compile_seconds = 0.0
    negative_link_seconds = 0.0
    negative_run_seconds = 0.0
    output_report_seconds = 0.0
  }

  Ensure-NativeCompilerExecutable -NativeExePath $nativeExe -NativeExeExplicit $nativeExeExplicit -BuildScriptPath $buildScript

  $clangCheckExit = Invoke-LoggedCommand -Command $clangCommand -Arguments @("--version") -LogPath (Join-Path $runDir "clang-version.log")
  if ($clangCheckExit -ne 0) {
    throw "execution smoke FAIL: clang command is unavailable: $clangCommand"
  }

  $positiveFixtures = Get-Fixtures -Directory $positiveFixtureDir -FixtureKind "positive execution"
  $negativeFixtures = Get-Fixtures -Directory $negativeFixtureDir -FixtureKind "negative execution"
  $executionFixtureEntries = @(
    $positiveFixtures | ForEach-Object {
      [pscustomobject]@{
        kind = "positive"
        file = $_
        relative_path = Get-RepoRelativePath -Path $_.FullName -Root $repoRoot
      }
    }
  ) + @(
    $negativeFixtures | ForEach-Object {
      [pscustomobject]@{
        kind = "negative"
        file = $_
        relative_path = Get-RepoRelativePath -Path $_.FullName -Root $repoRoot
      }
    }
  )
  $selectedExecutionEntries = Select-ExecutionFixtureEntries `
    -Entries $executionFixtureEntries `
    -FixtureListPath $FixtureList `
    -FixtureGlobPattern $FixtureGlob `
    -ShardIndexValue $ShardIndex `
    -ShardCountValue $ShardCount `
    -LimitValue $Limit `
    -RepoRoot $repoRoot
  $selectedPositiveFixtures = @($selectedExecutionEntries | Where-Object { $_.kind -eq "positive" } | ForEach-Object { $_.file })
  $selectedNegativeFixtures = @($selectedExecutionEntries | Where-Object { $_.kind -eq "negative" } | ForEach-Object { $_.file })
  Write-Output ("selection: positive={0} negative={1}" -f $selectedPositiveFixtures.Count, $selectedNegativeFixtures.Count)

  $results = @()
  $caseTimings = @()
  $totalSelectedFixtures = $selectedPositiveFixtures.Count + $selectedNegativeFixtures.Count
  $fixtureIndex = 0
  $lastCompletedFixture = "none"

  foreach ($fixture in $selectedPositiveFixtures) {
    $fixtureIndex += 1
    $caseStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
    $fixtureRel = Get-RepoRelativePath -Path $fixture.FullName -Root $repoRoot
    Write-Output ("execution-smoke-progress: [{0}/{1}] START kind=positive fixture={2} elapsed={3:n3}s last={4}" -f $fixtureIndex, $totalSelectedFixtures, $fixtureRel, $suiteStopwatch.Elapsed.TotalSeconds, $lastCompletedFixture)
    $expectation = Get-PositiveExpectation -FixturePath $fixture.FullName
    $caseDirName = Get-CaseDirectoryName `
      -RunDir $runDir `
      -Kind "positive" `
      -FixtureRelativePath $fixtureRel `
      -FixtureBaseName $fixture.BaseName
    $caseDir = Join-Path $runDir $caseDirName
    $compileDir = Join-Path $caseDir "compile"
    $exePath = Join-Path $caseDir "module.exe"
    $compileLog = Join-Path $caseDir "compile.log"
    $linkLog = Join-Path $caseDir "link.log"
    New-Item -ItemType Directory -Force -Path $compileDir | Out-Null

    $nativeArgs = @($fixture.FullName, "--out-dir", $compileDir, "--emit-prefix", "module", "--llc", $llcCommand)
    if ($expectation.compile_args.Count -gt 0) {
      $nativeArgs += @($expectation.compile_args)
    }
    $compileStep = Invoke-TimedLoggedCommand -StageKey "positive_compile_seconds" -Command $nativeExe -Arguments $nativeArgs -LogPath $compileLog
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

    $launchContract = Get-RuntimeLaunchLinkContract -CompileDir $compileDir -RepoRoot $repoRoot -EmitPrefix "module"
    $runtimeLibrary = [pscustomobject]@{
      path = $launchContract.runtime_library_path
      relative_path = $launchContract.runtime_library_relative_path
      source = $launchContract.runtime_library_source
    }
    $linkArgs = @($objPath, $runtimeLibrary.path) + @($launchContract.driver_linker_flags)
    $linkArgs += @("-o", $exePath, "-fno-color-diagnostics")
    $linkStep = Invoke-TimedLoggedCommand -StageKey "positive_link_seconds" -Command $clangCommand -Arguments $linkArgs -LogPath $linkLog
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

    $results += [pscustomobject]@{
      kind = "positive"
      fixture = $fixtureRel
      expectation = Get-RepoRelativePath -Path $expectation.expected_path -Root $repoRoot
      meta = if (Test-Path -LiteralPath $expectation.meta_path -PathType Leaf) { Get-RepoRelativePath -Path $expectation.meta_path -Root $repoRoot } else { "" }
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
      out_dir = Get-RepoRelativePath -Path $caseDir -Root $repoRoot
    }
    $caseStopwatch.Stop()
    $caseTiming = [ordered]@{
      kind = "positive"
      fixture = $fixtureRel
      duration_seconds = [math]::Round($caseStopwatch.Elapsed.TotalSeconds, 6)
      compile_seconds = [double]$compileStep.duration_seconds
      link_seconds = [double]$linkStep.duration_seconds
      run_seconds = [double]$runStep.duration_seconds
    }
    $caseTimings += $caseTiming
    $lastCompletedFixture = $fixtureRel
    Write-Output "[PASS] positive $fixtureRel (run_exit=$runExit)"
    Write-Output ("execution-smoke-progress: [{0}/{1}] DONE kind=positive fixture={2} duration={3:n3}s elapsed={4:n3}s" -f $fixtureIndex, $totalSelectedFixtures, $fixtureRel, $caseTiming.duration_seconds, $suiteStopwatch.Elapsed.TotalSeconds)
  }

  foreach ($fixture in $selectedNegativeFixtures) {
    $fixtureIndex += 1
    $caseStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
    $fixtureRel = Get-RepoRelativePath -Path $fixture.FullName -Root $repoRoot
    Write-Output ("execution-smoke-progress: [{0}/{1}] START kind=negative fixture={2} elapsed={3:n3}s last={4}" -f $fixtureIndex, $totalSelectedFixtures, $fixtureRel, $suiteStopwatch.Elapsed.TotalSeconds, $lastCompletedFixture)
    $spec = Get-NegativeExpectation -FixturePath $fixture.FullName
    $caseDirName = Get-CaseDirectoryName `
      -RunDir $runDir `
      -Kind "negative" `
      -FixtureRelativePath $fixtureRel `
      -FixtureBaseName $fixture.BaseName
    $caseDir = Join-Path $runDir $caseDirName
    $compileDir = Join-Path $caseDir "compile"
    $exePath = Join-Path $caseDir "module.exe"
    $compileLog = Join-Path $caseDir "compile.log"
    $linkLog = Join-Path $caseDir "link.log"
    $runLog = Join-Path $caseDir "run.log"
    New-Item -ItemType Directory -Force -Path $compileDir | Out-Null

    $nativeArgs = @($fixture.FullName, "--out-dir", $compileDir, "--emit-prefix", "module", "--llc", $llcCommand)
    if ($spec.compile_args.Count -gt 0) {
      $nativeArgs += @($spec.compile_args)
    }
    $compileStep = Invoke-TimedLoggedCommand -StageKey "negative_compile_seconds" -Command $nativeExe -Arguments $nativeArgs -LogPath $compileLog
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
      $results += [pscustomobject]@{
        kind = "negative"
        fixture = $fixtureRel
        expectation = Get-RepoRelativePath -Path $spec.expectation_path -Root $repoRoot
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
        out_dir = Get-RepoRelativePath -Path $caseDir -Root $repoRoot
      }
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
      $caseTimings += $caseTiming
      $lastCompletedFixture = $fixtureRel
      Write-Output "[PASS] negative $fixtureRel (stage=compile compile_exit=$compileExit)"
      Write-Output ("execution-smoke-progress: [{0}/{1}] DONE kind=negative fixture={2} duration={3:n3}s elapsed={4:n3}s" -f $fixtureIndex, $totalSelectedFixtures, $fixtureRel, $caseTiming.duration_seconds, $suiteStopwatch.Elapsed.TotalSeconds)
      continue
    }

    if ($compileExit -ne 0) {
      throw "execution smoke FAIL: compile failed for negative fixture $fixtureRel (exit=$compileExit)"
    }

    $launchContract = Get-RuntimeLaunchLinkContract -CompileDir $compileDir -RepoRoot $repoRoot -EmitPrefix "module"
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
      $linkStep = Invoke-TimedLoggedCommand -StageKey "negative_link_seconds" -Command $clangCommand -Arguments $linkArgs -LogPath $linkLog
      $linkExit = [int]$linkStep.exit_code
      if ($linkExit -eq 0) {
        throw "execution smoke FAIL: expected link failure for negative fixture $fixtureRel"
      }

      $linkText = if (Test-Path -LiteralPath $linkLog -PathType Leaf) { Get-Content -LiteralPath $linkLog -Raw } else { "" }
      $linkDiagnosticsPath = Join-Path $caseDir "link.diagnostics.txt"
      $canonicalLinkText = Get-CanonicalLinkDiagnosticsText -RawText $linkText -ObjectPath $objPath -RepoRoot $repoRoot
      Set-Content -LiteralPath $linkDiagnosticsPath -Value $canonicalLinkText -Encoding utf8
      $missingTokens = @(Get-MissingTokens -Text $canonicalLinkText -Tokens $spec.required_link_tokens)
      if ($missingTokens.Count -gt 0) {
        $linkDiagnosticsRel = Get-RepoRelativePath -Path $linkDiagnosticsPath -Root $repoRoot
        throw "execution smoke FAIL: missing expected link diagnostics for $fixtureRel (missing=$($missingTokens -join '|') diagnostics=$linkDiagnosticsRel)"
      }

      $results += [pscustomobject]@{
        kind = "negative"
        fixture = $fixtureRel
        expectation = Get-RepoRelativePath -Path $spec.expectation_path -Root $repoRoot
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
        link_diagnostics = Get-RepoRelativePath -Path $linkDiagnosticsPath -Root $repoRoot
        passed = $true
        timing = [ordered]@{
          compile_seconds = [double]$compileStep.duration_seconds
          link_seconds = [double]$linkStep.duration_seconds
          run_seconds = 0.0
        }
        out_dir = Get-RepoRelativePath -Path $caseDir -Root $repoRoot
      }
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
      $caseTimings += $caseTiming
      $lastCompletedFixture = $fixtureRel
      Write-Output "[PASS] negative $fixtureRel (stage=link link_exit=$linkExit)"
      Write-Output ("execution-smoke-progress: [{0}/{1}] DONE kind=negative fixture={2} duration={3:n3}s elapsed={4:n3}s" -f $fixtureIndex, $totalSelectedFixtures, $fixtureRel, $caseTiming.duration_seconds, $suiteStopwatch.Elapsed.TotalSeconds)
      continue
    }

    if ($spec.stage -eq "run") {
      $linkArgs = @($objPath, $runtimeLibrary.path) + @($launchContract.driver_linker_flags) + @("-o", $exePath, "-fno-color-diagnostics")
      $linkStep = Invoke-TimedLoggedCommand -StageKey "negative_link_seconds" -Command $clangCommand -Arguments $linkArgs -LogPath $linkLog
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

      $results += [pscustomobject]@{
        kind = "negative"
        fixture = $fixtureRel
        expectation = Get-RepoRelativePath -Path $spec.expectation_path -Root $repoRoot
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
        out_dir = Get-RepoRelativePath -Path $caseDir -Root $repoRoot
      }
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
      $caseTimings += $caseTiming
      $lastCompletedFixture = $fixtureRel
      Write-Output "[PASS] negative $fixtureRel (stage=run run_exit=$runExit)"
      Write-Output ("execution-smoke-progress: [{0}/{1}] DONE kind=negative fixture={2} duration={3:n3}s elapsed={4:n3}s" -f $fixtureIndex, $totalSelectedFixtures, $fixtureRel, $caseTiming.duration_seconds, $suiteStopwatch.Elapsed.TotalSeconds)
      continue
    }

    throw "execution smoke FAIL: unsupported negative stage '$($spec.stage)' for $fixtureRel"
  }

  $total = $results.Count
  $passedCount = @($results | Where-Object { $_.passed }).Count
  $failedCount = $total - $passedCount
  $reportStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
  $slowestFixtures = @($caseTimings | Sort-Object -Property duration_seconds -Descending | Select-Object -First 10)
  $summary = [ordered]@{
    run_dir = Get-RepoRelativePath -Path $runDir -Root $repoRoot
    compile_command = if (Test-Path -LiteralPath $nativeExe -PathType Leaf) { Get-RepoRelativePath -Path $nativeExe -Root $repoRoot } else { $nativeExe }
    runtime_launch_contract_script = Get-RepoRelativePath -Path $runtimeLaunchContractScript -Root $repoRoot
    native_exe = if (Test-Path -LiteralPath $nativeExe -PathType Leaf) { Get-RepoRelativePath -Path $nativeExe -Root $repoRoot } else { $nativeExe }
    runtime_library = if (Test-Path -LiteralPath $defaultRuntimeLibrary -PathType Leaf) { Get-RepoRelativePath -Path $defaultRuntimeLibrary -Root $repoRoot } else { "" }
    live_runtime_dispatch_default_symbol = "objc3_runtime_dispatch_i32"
    clang = $clangCommand
    llc = $llcCommand
    llc_source = $llcSourcePath
    selection = [ordered]@{
      fixture_list = $FixtureList
      fixture_glob = $FixtureGlob
      shard_index = $ShardIndex
      shard_count = $ShardCount
      limit = $Limit
      selected_positive = $selectedPositiveFixtures.Count
      selected_negative = $selectedNegativeFixtures.Count
    }
    total = $total
    passed = $passedCount
    failed = $failedCount
    status = if ($failedCount -eq 0) { "PASS" } else { "FAIL" }
    timing = [ordered]@{
      elapsed_seconds = [math]::Round($suiteStopwatch.Elapsed.TotalSeconds, 6)
      stage_totals = $script:stageTimings
      slowest_fixtures = @($slowestFixtures)
      fixture_timings = @($caseTimings)
    }
    results = $results
  }
  $reportStopwatch.Stop()
  Add-StageDuration -StageKey "output_report_seconds" -DurationSeconds ([math]::Round($reportStopwatch.Elapsed.TotalSeconds, 6))
  $summary.timing.stage_totals = $script:stageTimings
  $summary.timing.elapsed_seconds = [math]::Round($suiteStopwatch.Elapsed.TotalSeconds, 6)
  $summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $summaryPath -Encoding utf8
  Write-Output "summary_path: $(Get-RepoRelativePath -Path $summaryPath -Root $repoRoot)"
  Write-Output "status: PASS"
  $global:LASTEXITCODE = 0
}
finally {
  Pop-Location
}
