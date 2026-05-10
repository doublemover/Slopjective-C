Set-StrictMode -Version Latest

$script:ScriptsRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $script:ScriptsRoot "objc3c_native_execution_smoke_helpers.psm1") -Force -DisableNameChecking

function New-PositiveExecutionSmokeResult {
  param(
    [Parameter(Mandatory = $true)][string]$FixtureRel,
    [Parameter(Mandatory = $true)]$Expectation,
    [Parameter(Mandatory = $true)]$LaunchContract,
    [Parameter(Mandatory = $true)]$RuntimeLibrary,
    [Parameter(Mandatory = $true)][int]$CompileExit,
    [Parameter(Mandatory = $true)][int]$LinkExit,
    [Parameter(Mandatory = $true)][int]$RunExit,
    [Parameter(Mandatory = $true)][int]$ExpectedExit,
    [Parameter(Mandatory = $true)]$CompileStep,
    [Parameter(Mandatory = $true)]$LinkStep,
    [Parameter(Mandatory = $true)]$RunStep,
    [Parameter(Mandatory = $true)][string]$CaseDir,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  return [pscustomobject]@{
    kind = "positive"
    fixture = $FixtureRel
    expectation = Get-RepoRelativePath -Path $Expectation.expected_path -Root $RepoRoot
    meta = if (Test-Path -LiteralPath $Expectation.meta_path -PathType Leaf) { Get-RepoRelativePath -Path $Expectation.meta_path -Root $RepoRoot } else { "" }
    native_compile_args = @($Expectation.compile_args)
    requires_live_runtime_dispatch = $Expectation.requires_live_runtime_dispatch
    requires_live_runtime_dispatch_explicit = $Expectation.requires_live_runtime_dispatch_explicit
    runtime_dispatch_symbol = $Expectation.runtime_dispatch_symbol
    launch_integration_contract_id = $LaunchContract.launch_integration_contract_id
    registration_manifest = $LaunchContract.registration_manifest_relative_path
    runtime_library = $RuntimeLibrary.relative_path
    runtime_library_source = $RuntimeLibrary.source
    driver_linker_flags = @($LaunchContract.driver_linker_flags)
    compile_exit = $CompileExit
    link_exit = $LinkExit
    run_exit = $RunExit
    expected_exit = $ExpectedExit
    passed = $true
    timing = [ordered]@{
      compile_seconds = [double]$CompileStep.duration_seconds
      link_seconds = [double]$LinkStep.duration_seconds
      run_seconds = [double]$RunStep.duration_seconds
    }
    out_dir = Get-RepoRelativePath -Path $CaseDir -Root $RepoRoot
  }
}

function New-CompileNegativeExecutionSmokeResult {
  param(
    [Parameter(Mandatory = $true)][string]$FixtureRel,
    [Parameter(Mandatory = $true)]$Spec,
    [Parameter(Mandatory = $true)][int]$CompileExit,
    [Parameter(Mandatory = $true)]$CompileStep,
    [Parameter(Mandatory = $true)][string]$CaseDir,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  return [pscustomobject]@{
    kind = "negative"
    fixture = $FixtureRel
    expectation = Get-RepoRelativePath -Path $Spec.expectation_path -Root $RepoRoot
    stage = $Spec.stage
    native_compile_args = @($Spec.compile_args)
    requires_live_runtime_dispatch = $Spec.requires_live_runtime_dispatch
    runtime_dispatch_symbol = $Spec.runtime_dispatch_symbol
    compile_exit = $CompileExit
    link_exit = -1
    run_exit = -1
    required_link_tokens = $Spec.required_link_tokens
    missing_link_tokens = @()
    passed = $true
    timing = [ordered]@{
      compile_seconds = [double]$CompileStep.duration_seconds
      link_seconds = 0.0
      run_seconds = 0.0
    }
    out_dir = Get-RepoRelativePath -Path $CaseDir -Root $RepoRoot
  }
}

function New-LinkedNegativeExecutionSmokeResult {
  param(
    [Parameter(Mandatory = $true)][string]$FixtureRel,
    [Parameter(Mandatory = $true)]$Spec,
    [Parameter(Mandatory = $true)]$LaunchContract,
    [Parameter(Mandatory = $true)]$RuntimeLibrary,
    [Parameter(Mandatory = $true)][int]$CompileExit,
    [Parameter(Mandatory = $true)][int]$LinkExit,
    [Parameter(Mandatory = $true)][int]$RunExit,
    [Parameter(Mandatory = $true)]$CompileStep,
    [Parameter(Mandatory = $true)]$LinkStep,
    [Parameter(Mandatory = $true)]$RunStep,
    [Parameter(Mandatory = $true)][string]$CaseDir,
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [string]$LinkDiagnosticsPath = ""
  )

  $payload = [ordered]@{
    kind = "negative"
    fixture = $FixtureRel
    expectation = Get-RepoRelativePath -Path $Spec.expectation_path -Root $RepoRoot
    stage = $Spec.stage
    native_compile_args = @($Spec.compile_args)
    requires_live_runtime_dispatch = $Spec.requires_live_runtime_dispatch
    runtime_dispatch_symbol = $Spec.runtime_dispatch_symbol
    launch_integration_contract_id = $LaunchContract.launch_integration_contract_id
    registration_manifest = $LaunchContract.registration_manifest_relative_path
    runtime_library = $RuntimeLibrary.relative_path
    runtime_library_source = $RuntimeLibrary.source
    driver_linker_flags = @($LaunchContract.driver_linker_flags)
    compile_exit = $CompileExit
    link_exit = $LinkExit
    run_exit = $RunExit
    required_link_tokens = $Spec.required_link_tokens
    missing_link_tokens = @()
  }
  if ($LinkDiagnosticsPath -ne "") {
    $payload["link_diagnostics"] = Get-RepoRelativePath -Path $LinkDiagnosticsPath -Root $RepoRoot
  }
  $payload["passed"] = $true
  $payload["timing"] = [ordered]@{
    compile_seconds = [double]$CompileStep.duration_seconds
    link_seconds = [double]$LinkStep.duration_seconds
    run_seconds = [double]$RunStep.duration_seconds
  }
  $payload["out_dir"] = Get-RepoRelativePath -Path $CaseDir -Root $RepoRoot
  return [pscustomobject]$payload
}

function New-ExecutionSmokeCaseTiming {
  param(
    [Parameter(Mandatory = $true)][string]$Kind,
    [Parameter(Mandatory = $true)][string]$FixtureRel,
    [Parameter(Mandatory = $true)][double]$DurationSeconds,
    [Parameter(Mandatory = $true)]$CompileStep,
    [Parameter(Mandatory = $true)]$LinkStep,
    [Parameter(Mandatory = $true)]$RunStep,
    [string]$Stage = ""
  )

  $payload = [ordered]@{
    kind = $Kind
    fixture = $FixtureRel
  }
  if ($Stage -ne "") {
    $payload["stage"] = $Stage
  }
  $payload["duration_seconds"] = [math]::Round($DurationSeconds, 6)
  $payload["compile_seconds"] = [double]$CompileStep.duration_seconds
  $payload["link_seconds"] = [double]$LinkStep.duration_seconds
  $payload["run_seconds"] = [double]$RunStep.duration_seconds
  return $payload
}

Export-ModuleMember -Function @(
  "New-CompileNegativeExecutionSmokeResult",
  "New-ExecutionSmokeCaseTiming",
  "New-LinkedNegativeExecutionSmokeResult",
  "New-PositiveExecutionSmokeResult"
)
