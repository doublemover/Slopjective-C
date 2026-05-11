$ErrorActionPreference = "Stop"

function New-Objc3cNativeFrontendEdgeRobustnessPayload {
  $contracts = Get-Objc3cNativeFrontendCloseoutEdgeContractIds
  $guardrails = Get-Objc3cNativeFrontendCloseoutEdgeWrapperGuardrails

  return [ordered]@{
    contract_id = $contracts.EdgeRobustness
    schema_version = 1
    depends_on_contract_ids = @(
      $contracts.EdgeCompatCompletion,
      $contracts.CoreFeatureExpansion
    )
    wrapper_guardrails = [ordered]@{
      wrapper_single_value_flags = $guardrails.WrapperSingleValueFlags
      compile_single_value_flags = $guardrails.CompileSingleValueFlags
      reject_empty_equals_value_flags = $guardrails.RejectEmptyEqualsValueFlags
    }
  }
}
