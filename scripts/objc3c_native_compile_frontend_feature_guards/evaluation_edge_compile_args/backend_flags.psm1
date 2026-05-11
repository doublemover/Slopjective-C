$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Add-FrontendEdgeBackendCompileArg {
  param(
    [string]$BackendValue,
    [hashtable]$AliasMap,
    [hashtable]$SingleValueFlags,
    [System.Collections.Generic.List[string]]$NormalizedArgs
  )

  Assert-FrontendFeatureCompileArgValueNotEmpty -FlagName "--objc3-ir-object-backend" -Value $BackendValue
  $backendKey = Normalize-FrontendFeatureBackendKey -Value $BackendValue
  if (-not $AliasMap.ContainsKey($backendKey)) {
    Stop-FrontendFeatureGuard "unsupported value '$BackendValue' for --objc3-ir-object-backend"
  }
  $SingleValueFlags["--objc3-ir-object-backend"] = [int]$SingleValueFlags["--objc3-ir-object-backend"] + 1
  $NormalizedArgs.Add("--objc3-ir-object-backend")
  $NormalizedArgs.Add([string]$AliasMap[$backendKey])
}
