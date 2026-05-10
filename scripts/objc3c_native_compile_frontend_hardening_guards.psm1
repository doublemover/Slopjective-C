$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Assert-FrontendEdgeRobustness {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $robustnessPath = Resolve-FrontendEdgeRobustnessPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $robustnessPath -PathType Leaf)) {
    Write-Error "frontend edge robustness artifact missing at $robustnessPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $robustnessPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend edge robustness artifact is not valid JSON at $robustnessPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-edge-robustness/parser_build-edge-robustness-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend edge robustness contract id mismatch in $robustnessPath"
    exit 2
  }

  $expectedDependencies = @(
    "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1",
    "objc3c-frontend-build-invocation-core-feature-expansion/parser_build-core-feature-expansion-v1"
  )
  $dependencySet = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (-not [string]::IsNullOrWhiteSpace($contractIdText)) {
      $dependencySet[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencies) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "frontend edge robustness missing dependency contract '$requiredContractId' in $robustnessPath"
      exit 2
    }
  }

  $guardrails = $payload.wrapper_guardrails
  if ($null -eq $guardrails) {
    Write-Error "frontend edge robustness wrapper_guardrails metadata missing in $robustnessPath"
    exit 2
  }

  $requiredWrapperSingleFlags = @("--use-cache", "--out-dir")
  $wrapperSingleSet = @{}
  foreach ($flag in @($guardrails.wrapper_single_value_flags)) {
    $flagText = [string]$flag
    if (-not [string]::IsNullOrWhiteSpace($flagText)) {
      $wrapperSingleSet[$flagText] = $true
    }
  }
  foreach ($requiredFlag in $requiredWrapperSingleFlags) {
    if (-not $wrapperSingleSet.ContainsKey($requiredFlag)) {
      Write-Error "frontend edge robustness missing wrapper_single_value flag '$requiredFlag' in $robustnessPath"
      exit 2
    }
  }

  $requiredCompileSingleFlags = @(
    "--objc3-ir-object-backend",
    "--llvm-capabilities-summary",
    "--objc3-route-backend-from-capabilities"
  )
  $compileSingleSet = @{}
  foreach ($flag in @($guardrails.compile_single_value_flags)) {
    $flagText = [string]$flag
    if (-not [string]::IsNullOrWhiteSpace($flagText)) {
      $compileSingleSet[$flagText] = $true
    }
  }
  foreach ($requiredFlag in $requiredCompileSingleFlags) {
    if (-not $compileSingleSet.ContainsKey($requiredFlag)) {
      Write-Error "frontend edge robustness missing compile_single_value flag '$requiredFlag' in $robustnessPath"
      exit 2
    }
  }

  $requiredRejectEmptyFlags = @("--emit-prefix", "--clang", "--use-cache")
  $rejectEmptySet = @{}
  foreach ($flag in @($guardrails.reject_empty_equals_value_flags)) {
    $flagText = [string]$flag
    if (-not [string]::IsNullOrWhiteSpace($flagText)) {
      $rejectEmptySet[$flagText] = $true
    }
  }
  foreach ($requiredFlag in $requiredRejectEmptyFlags) {
    if (-not $rejectEmptySet.ContainsKey($requiredFlag)) {
      Write-Error "frontend edge robustness missing reject_empty_equals_value flag '$requiredFlag' in $robustnessPath"
      exit 2
    }
  }

  return [pscustomobject]@{
    edge_robustness_path = $robustnessPath
  }
}

function Assert-FrontendDiagnosticsHardening {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $diagnosticsPath = Resolve-FrontendDiagnosticsHardeningPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $diagnosticsPath -PathType Leaf)) {
    Write-Error "frontend diagnostics hardening artifact missing at $diagnosticsPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $diagnosticsPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend diagnostics hardening artifact is not valid JSON at $diagnosticsPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-diagnostics-hardening/parser_build-diagnostics-hardening-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend diagnostics hardening contract id mismatch in $diagnosticsPath"
    exit 2
  }

  $expectedDependencies = @(
    "objc3c-frontend-build-invocation-edge-robustness/parser_build-edge-robustness-v1",
    "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
  )
  $dependencySet = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (-not [string]::IsNullOrWhiteSpace($contractIdText)) {
      $dependencySet[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencies) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "frontend diagnostics hardening missing dependency contract '$requiredContractId' in $diagnosticsPath"
      exit 2
    }
  }

  $wrapperDiagnostics = $payload.wrapper_diagnostics
  if ($null -eq $wrapperDiagnostics) {
    Write-Error "frontend diagnostics hardening wrapper_diagnostics metadata missing in $diagnosticsPath"
    exit 2
  }
  if ([int]$wrapperDiagnostics.fail_closed_exit_code -ne 2) {
    Write-Error "frontend diagnostics hardening fail_closed_exit_code must be 2 in $diagnosticsPath"
    exit 2
  }

  $requiredMessages = @(
    "--use-cache can be provided at most once",
    "invalid --use-cache value",
    "--out-dir can be provided at most once",
    "missing value for --out-dir",
    "empty value for --out-dir",
    "missing value for --emit-prefix",
    "empty value for --emit-prefix",
    "missing value for --clang",
    "empty value for --clang"
  )
  $messageSet = @{}
  foreach ($message in @($wrapperDiagnostics.required_error_messages)) {
    $messageText = [string]$message
    if (-not [string]::IsNullOrWhiteSpace($messageText)) {
      $messageSet[$messageText] = $true
    }
  }
  foreach ($requiredMessage in $requiredMessages) {
    if (-not $messageSet.ContainsKey($requiredMessage)) {
      Write-Error "frontend diagnostics hardening missing required_error_messages entry '$requiredMessage' in $diagnosticsPath"
      exit 2
    }
  }

  return [pscustomobject]@{
    diagnostics_hardening_path = $diagnosticsPath
  }
}

function Assert-FrontendRecoveryDeterminismHardening {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $recoveryPath = Resolve-FrontendRecoveryDeterminismHardeningPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $recoveryPath -PathType Leaf)) {
    Write-Error "frontend recovery determinism hardening artifact missing at $recoveryPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $recoveryPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend recovery determinism hardening artifact is not valid JSON at $recoveryPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-recovery-determinism-hardening/parser_build-recovery-determinism-hardening-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend recovery determinism hardening contract id mismatch in $recoveryPath"
    exit 2
  }

  $expectedDependencies = @(
    "objc3c-frontend-build-invocation-diagnostics-hardening/parser_build-diagnostics-hardening-v1",
    "objc3c-frontend-build-invocation-edge-robustness/parser_build-edge-robustness-v1"
  )
  $dependencySet = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (-not [string]::IsNullOrWhiteSpace($contractIdText)) {
      $dependencySet[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencies) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "frontend recovery determinism hardening missing dependency contract '$requiredContractId' in $recoveryPath"
      exit 2
    }
  }

  $cacheDeterminism = $payload.cache_determinism
  if ($null -eq $cacheDeterminism) {
    Write-Error "frontend recovery determinism hardening cache_determinism metadata missing in $recoveryPath"
    exit 2
  }
  if ([int]$cacheDeterminism.fail_closed_exit_code -ne 2) {
    Write-Error "frontend recovery determinism hardening fail_closed_exit_code must be 2 in $recoveryPath"
    exit 2
  }
  if ([string]$cacheDeterminism.entry_contract_id -ne "objc3c-native-cache-entry/parser_build-recovery-determinism-hardening-v1") {
    Write-Error "frontend recovery determinism hardening entry_contract_id mismatch in $recoveryPath"
    exit 2
  }

  $requiredStatusTokens = @("cache_hit=true", "cache_hit=false")
  $statusTokenSet = @{}
  foreach ($token in @($cacheDeterminism.cache_status_tokens)) {
    $tokenText = [string]$token
    if (-not [string]::IsNullOrWhiteSpace($tokenText)) {
      $statusTokenSet[$tokenText] = $true
    }
  }
  foreach ($requiredToken in $requiredStatusTokens) {
    if (-not $statusTokenSet.ContainsKey($requiredToken)) {
      Write-Error "frontend recovery determinism hardening missing cache_status_tokens entry '$requiredToken' in $recoveryPath"
      exit 2
    }
  }

  $requiredEntryFiles = @("files", "exit_code.txt", "ready.marker", "metadata.json")
  $entryFileSet = @{}
  foreach ($entryFile in @($cacheDeterminism.required_entry_files)) {
    $entryFileText = [string]$entryFile
    if (-not [string]::IsNullOrWhiteSpace($entryFileText)) {
      $entryFileSet[$entryFileText] = $true
    }
  }
  foreach ($requiredEntryFile in $requiredEntryFiles) {
    if (-not $entryFileSet.ContainsKey($requiredEntryFile)) {
      Write-Error "frontend recovery determinism hardening missing required_entry_files entry '$requiredEntryFile' in $recoveryPath"
      exit 2
    }
  }

  $requiredRecoverySignals = @(
    "cache_recovery=metadata_missing",
    "cache_recovery=metadata_invalid",
    "cache_recovery=metadata_contract_mismatch",
    "cache_recovery=metadata_cache_key_mismatch",
    "cache_recovery=metadata_exit_code_mismatch",
    "cache_recovery=metadata_digest_mismatch",
    "cache_recovery=restore_failed"
  )
  $recoverySignalSet = @{}
  foreach ($signal in @($cacheDeterminism.recovery_signals)) {
    $signalText = [string]$signal
    if (-not [string]::IsNullOrWhiteSpace($signalText)) {
      $recoverySignalSet[$signalText] = $true
    }
  }
  foreach ($requiredSignal in $requiredRecoverySignals) {
    if (-not $recoverySignalSet.ContainsKey($requiredSignal)) {
      Write-Error "frontend recovery determinism hardening missing recovery_signals entry '$requiredSignal' in $recoveryPath"
      exit 2
    }
  }

  return [pscustomobject]@{
    recovery_determinism_hardening_path = $recoveryPath
  }
}

Export-ModuleMember -Function @("Assert-FrontendEdgeRobustness", "Assert-FrontendDiagnosticsHardening", "Assert-FrontendRecoveryDeterminismHardening")
