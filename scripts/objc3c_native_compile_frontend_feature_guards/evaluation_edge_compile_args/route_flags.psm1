$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Add-FrontendEdgeRouteCompileArg {
  param(
    [string]$RouteValue,
    [bool]$ImplicitTrue,
    [object]$State
  )

  $State.route_flag_occurrences = [int]$State.route_flag_occurrences + 1
  $routeEnabled = $ImplicitTrue
  if (-not $ImplicitTrue) {
    $routeEnabled = ConvertTo-FrontendFeatureBooleanFlagValue `
      -FlagName "--objc3-route-backend-from-capabilities" `
      -Value $RouteValue
  }
  if ($routeEnabled) {
    $State.uses_capability_routing = $true
    $State.normalized_args.Add("--objc3-route-backend-from-capabilities")
  }
}

function Assert-FrontendEdgeRouteCompileArgState {
  param(
    [Parameter(Mandatory = $true)]$State
  )

  if ([int]$State.route_flag_occurrences -gt 1) {
    Stop-FrontendFeatureGuard "--objc3-route-backend-from-capabilities can be provided at most once"
  }
  Assert-FrontendFeatureCapabilityRoutingHasSummary `
    -UsesCapabilityRouting $State.uses_capability_routing `
    -HasCapabilitySummary $State.has_capability_summary
}
