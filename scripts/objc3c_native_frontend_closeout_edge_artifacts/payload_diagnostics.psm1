$ErrorActionPreference = "Stop"

function New-Objc3cNativeFrontendDiagnosticsHardeningPayload {
  $contracts = Get-Objc3cNativeFrontendCloseoutEdgeContractIds
  $diagnostics = Get-Objc3cNativeFrontendCloseoutEdgeDiagnostics

  return [ordered]@{
    contract_id = $contracts.DiagnosticsHardening
    schema_version = 1
    depends_on_contract_ids = @(
      $contracts.EdgeRobustness,
      $contracts.EdgeCompatCompletion
    )
    wrapper_diagnostics = [ordered]@{
      fail_closed_exit_code = $diagnostics.FailClosedExitCode
      required_error_messages = $diagnostics.RequiredErrorMessages
    }
  }
}
