$ErrorActionPreference = "Stop"

function New-Objc3cNativeFrontendEdgeCompatibilityPayload {
  param(
    [Parameter(Mandatory = $true)]
    [string[]]$AllowedBackends
  )

  $contracts = Get-Objc3cNativeFrontendCloseoutEdgeContractIds
  $backendCompat = Get-Objc3cNativeFrontendCloseoutEdgeBackendCompatibility
  $invocationCompat = Get-Objc3cNativeFrontendCloseoutEdgeInvocationCompatibility

  return [ordered]@{
    contract_id = $contracts.EdgeCompatCompletion
    schema_version = 1
    depends_on_contract_ids = @(
      $contracts.CoreFeatureExpansion,
      $contracts.ManifestGuard
    )
    backend_compat = [ordered]@{
      canonical_allowed_backends = $AllowedBackends
      alias_to_canonical = $backendCompat.AliasToCanonical
      single_value_flags = $backendCompat.SingleValueFlags
    }
    invocation_edge_compat = [ordered]@{
      supports_equals_form_flags = $invocationCompat.SupportsEqualsFormFlags
      supports_boolean_equals_flags = $invocationCompat.SupportsBooleanEqualsFlags
      route_flag = $invocationCompat.RouteFlag
      capability_summary_flag = $invocationCompat.CapabilitySummaryFlag
      fail_closed_exit_code = $invocationCompat.FailClosedExitCode
      disallow_relative_parent_segments = $invocationCompat.DisallowRelativeParentSegments
    }
  }
}
