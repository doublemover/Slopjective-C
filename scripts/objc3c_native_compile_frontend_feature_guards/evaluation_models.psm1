$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function New-FrontendCoreCompileArgGuardState {
  [pscustomobject]@{
    requested_backend = $null
    uses_capability_routing = $false
    has_capability_summary = $false
  }
}

function New-FrontendEdgeCompileArgNormalizationState {
  [pscustomobject]@{
    normalized_args = New-Object System.Collections.Generic.List[string]
    uses_capability_routing = $false
    has_capability_summary = $false
    route_flag_occurrences = 0
  }
}

function New-FrontendEdgeCompatibilityPayloadEvaluation {
  param(
    [hashtable]$AliasMap,
    [hashtable]$SingleValueFlags
  )

  [pscustomobject]@{
    alias_map = $AliasMap
    single_value_flags = $SingleValueFlags
  }
}

Export-ModuleMember -Function @(
  "New-FrontendCoreCompileArgGuardState",
  "New-FrontendEdgeCompileArgNormalizationState",
  "New-FrontendEdgeCompatibilityPayloadEvaluation"
)
