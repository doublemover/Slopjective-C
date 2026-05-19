$ErrorActionPreference = "Stop"

function New-Objc3cNativeFrontendRecoveryDeterminismHardeningPayload {
  $contracts = Get-Objc3cNativeFrontendCloseoutEdgeContractIds
  $cacheDeterminism = Get-Objc3cNativeFrontendCloseoutEdgeCacheDeterminism

  return [ordered]@{
    contract_id = $contracts.RecoveryDeterminismHardening
    schema_version = 1
    depends_on_contract_ids = @(
      $contracts.DiagnosticsHardening,
      $contracts.EdgeRobustness
    )
    cache_determinism = [ordered]@{
      fail_closed_exit_code = $cacheDeterminism.FailClosedExitCode
      entry_contract_id = $cacheDeterminism.EntryContractId
      cache_status_tokens = $cacheDeterminism.CacheStatusTokens
      required_entry_files = $cacheDeterminism.RequiredEntryFiles
      recovery_signals = $cacheDeterminism.RecoverySignals
    }
  }
}
