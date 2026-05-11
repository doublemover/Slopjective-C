$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

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
