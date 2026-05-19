$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$featureGuardNormalizationModule = Join-Path $PSScriptRoot "normalization.psm1"
if (!(Test-Path -LiteralPath $featureGuardNormalizationModule -PathType Leaf)) {
  Write-Error "native compile frontend feature guard normalization module missing at $featureGuardNormalizationModule"
  exit 2
}
$featureGuardNormalizationRootLiteral = (Split-Path -Parent $featureGuardNormalizationModule).Replace("'", "''")
$featureGuardNormalizationScript = [scriptblock]::Create(
  "`$PSScriptRoot = '$featureGuardNormalizationRootLiteral'`n" +
  (Get-Content -LiteralPath $featureGuardNormalizationModule -Raw)
)
. $featureGuardNormalizationScript

function Read-FrontendFeatureCompileArgs {
  param(
    [object]$ParsedArgs
  )

  if ($null -eq $ParsedArgs) {
    return @()
  }
  return @($ParsedArgs.compile_args)
}

function Read-FrontendCoreFeatureInvocationMetadata {
  param(
    [object]$Payload
  )

  return $Payload.invocation
}

function Read-FrontendCoreFeatureBackendRoutingMetadata {
  param(
    [object]$Payload
  )

  return $Payload.backend_routing
}

function Read-FrontendEdgeCompatibilityInvocationMetadata {
  param(
    [object]$Payload
  )

  return $Payload.invocation_edge_compat
}

function Read-FrontendEdgeCompatibilityBackendMetadata {
  param(
    [object]$Payload
  )

  return $Payload.backend_compat
}

function Test-FrontendFeatureArgumentPathHasParentSegment {
  param(
    [string]$Path
  )

  return Test-FrontendFeatureRelativePathHasParentSegment -Path $Path
}

Export-ModuleMember -Function @(
  "Read-FrontendFeatureCompileArgs",
  "Read-FrontendCoreFeatureInvocationMetadata",
  "Read-FrontendCoreFeatureBackendRoutingMetadata",
  "Read-FrontendEdgeCompatibilityInvocationMetadata",
  "Read-FrontendEdgeCompatibilityBackendMetadata",
  "Test-FrontendFeatureArgumentPathHasParentSegment"
)
