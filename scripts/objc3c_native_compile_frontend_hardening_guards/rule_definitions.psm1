$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Get-FrontendEdgeRobustnessGuardRules {
  [pscustomobject]@{
    artifact_name = "frontend edge robustness"
    contract_id = "objc3c-frontend-build-invocation-edge-robustness/parser_build-edge-robustness-v1"
    dependency_contract_ids = @(
      "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1",
      "objc3c-frontend-build-invocation-core-feature-expansion/parser_build-core-feature-expansion-v1"
    )
    wrapper_single_value_flags = @("--use-cache", "--out-dir")
    compile_single_value_flags = @(
      "--objc3-ir-object-backend",
      "--llvm-capabilities-summary",
      "--objc3-route-backend-from-capabilities"
    )
    reject_empty_equals_value_flags = @("--emit-prefix", "--clang", "--use-cache")
    report_key = "edge_robustness_path"
  }
}

function Get-FrontendDiagnosticsHardeningGuardRules {
  [pscustomobject]@{
    artifact_name = "frontend diagnostics hardening"
    contract_id = "objc3c-frontend-build-invocation-diagnostics-hardening/parser_build-diagnostics-hardening-v1"
    dependency_contract_ids = @(
      "objc3c-frontend-build-invocation-edge-robustness/parser_build-edge-robustness-v1",
      "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
    )
    fail_closed_exit_code = 2
    required_error_messages = @(
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
    report_key = "diagnostics_hardening_path"
  }
}

function Get-FrontendRecoveryDeterminismHardeningGuardRules {
  [pscustomobject]@{
    artifact_name = "frontend recovery determinism hardening"
    contract_id = "objc3c-frontend-build-invocation-recovery-determinism-hardening/parser_build-recovery-determinism-hardening-v1"
    dependency_contract_ids = @(
      "objc3c-frontend-build-invocation-diagnostics-hardening/parser_build-diagnostics-hardening-v1",
      "objc3c-frontend-build-invocation-edge-robustness/parser_build-edge-robustness-v1"
    )
    fail_closed_exit_code = 2
    entry_contract_id = "objc3c-native-cache-entry/parser_build-recovery-determinism-hardening-v1"
    cache_status_tokens = @("cache_hit=true", "cache_hit=false")
    required_entry_files = @("files", "exit_code.txt", "ready.marker", "metadata.json")
    recovery_signals = @(
      "cache_recovery=metadata_missing",
      "cache_recovery=metadata_invalid",
      "cache_recovery=metadata_contract_mismatch",
      "cache_recovery=metadata_cache_key_mismatch",
      "cache_recovery=metadata_exit_code_mismatch",
      "cache_recovery=metadata_digest_mismatch",
      "cache_recovery=restore_failed"
    )
    report_key = "recovery_determinism_hardening_path"
  }
}

Export-ModuleMember -Function @(
  "Get-FrontendEdgeRobustnessGuardRules",
  "Get-FrontendDiagnosticsHardeningGuardRules",
  "Get-FrontendRecoveryDeterminismHardeningGuardRules"
)
