$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$featureGuardNormalizationModule = Join-Path $PSScriptRoot "normalization.psm1"
$featureGuardReportingModule = Join-Path $PSScriptRoot "reporting.psm1"
foreach ($modulePath in @($featureGuardNormalizationModule, $featureGuardReportingModule)) {
  if (!(Test-Path -LiteralPath $modulePath -PathType Leaf)) {
    Write-Error "native compile frontend feature guard diagnostics dependency module missing at $modulePath"
    exit 2
  }
  Import-Module $modulePath -Force -DisableNameChecking
}

function Assert-FrontendFeatureDependencyContracts {
  param(
    [object[]]$PresentContracts,
    [string[]]$RequiredContracts,
    [string]$ArtifactName,
    [string]$ArtifactPath
  )

  $presentDependencyContracts = New-FrontendFeatureStringSet -Values @($PresentContracts)
  foreach ($requiredContractId in $RequiredContracts) {
    if (-not $presentDependencyContracts.ContainsKey($requiredContractId)) {
      Stop-FrontendFeatureGuard "$ArtifactName missing dependency contract '$requiredContractId' in $ArtifactPath"
    }
  }
}

function Assert-FrontendFeatureCompileArgHasNextValue {
  param(
    [object[]]$CompileArgs,
    [int]$Index,
    [string]$FlagName
  )

  if (($Index + 1) -ge $CompileArgs.Count) {
    Stop-FrontendFeatureGuard "missing value for $FlagName"
  }
}

function Assert-FrontendFeatureCompileArgValueNotEmpty {
  param(
    [string]$FlagName,
    [string]$Value
  )

  if ([string]::IsNullOrWhiteSpace($Value)) {
    Stop-FrontendFeatureGuard "empty value for $FlagName"
  }
}

function ConvertTo-FrontendFeatureBooleanFlagValue {
  param(
    [string]$FlagName,
    [string]$Value
  )

  $normalizedValue = $Value.Trim().ToLowerInvariant()
  if (@("1", "true", "yes", "on") -contains $normalizedValue) {
    return $true
  }
  if (@("0", "false", "no", "off") -contains $normalizedValue) {
    return $false
  }
  Stop-FrontendFeatureGuard "invalid boolean value '$normalizedValue' for $FlagName"
}

function Assert-FrontendFeatureSingleUseFlagCounts {
  param(
    [hashtable]$SingleValueFlags
  )

  foreach ($flag in $SingleValueFlags.Keys) {
    if ([int]$SingleValueFlags[$flag] -gt 1) {
      Stop-FrontendFeatureGuard "$flag can be provided at most once"
    }
  }
}

function Assert-FrontendFeatureCapabilityRoutingHasSummary {
  param(
    [bool]$UsesCapabilityRouting,
    [bool]$HasCapabilitySummary
  )

  if ($UsesCapabilityRouting -and -not $HasCapabilitySummary) {
    Stop-FrontendFeatureGuard "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary"
  }
}

Export-ModuleMember -Function @(
  "Assert-FrontendFeatureDependencyContracts",
  "Assert-FrontendFeatureCompileArgHasNextValue",
  "Assert-FrontendFeatureCompileArgValueNotEmpty",
  "ConvertTo-FrontendFeatureBooleanFlagValue",
  "Assert-FrontendFeatureSingleUseFlagCounts",
  "Assert-FrontendFeatureCapabilityRoutingHasSummary"
)
