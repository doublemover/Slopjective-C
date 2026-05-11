$ErrorActionPreference = "Stop"

function New-Objc3cNativeFrontendIntegrationCloseoutPayload {
  param(
    [int]$AcceptanceCount,
    [int]$RejectionCount
  )

  $contracts = Get-Objc3cNativeFrontendCloseoutConformanceContractIds

  return [ordered]@{
    contract_id = $contracts.IntegrationCloseout
    schema_version = 1
    depends_on_contract_ids = @(
      $contracts.ConformanceCorpus
      $contracts.ConformanceMatrix
      $contracts.RecoveryDeterminismHardening
    )
    closeout_gate = [ordered]@{
      build_integration_gate_signoff = $true
      invocation_profile_gate_signoff = $true
      corpus_coverage_gate_signoff = $true
      deterministic_fail_closed_exit_code = 2
      acceptance_corpus_count = $AcceptanceCount
      rejection_corpus_count = $RejectionCount
    }
  }
}
