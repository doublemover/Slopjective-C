Set-StrictMode -Version Latest

$objc3cNativePerfBudgetScriptRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $objc3cNativePerfBudgetScriptRoot "objc3c_native_perf_budget_helpers.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $objc3cNativePerfBudgetScriptRoot "objc3c_native_perf_budget_measurements.psm1") -Force -DisableNameChecking

function Read-Objc3cNativePerfInvocationCacheHitFlag {
  param(
    [string]$OutputText,
    [string]$RunLabel
  )

  return (Read-Objc3cNativePerfCacheHitFlag -OutputText $OutputText -RunLabel $RunLabel)
}

function Get-Objc3cNativePerfInvocationArtifactHashSet {
  param(
    [string]$Directory,
    [string[]]$ArtifactNames
  )

  return (Get-Objc3cNativePerfArtifactHashSet -Directory $Directory -ArtifactNames $ArtifactNames)
}

function Assert-Objc3cNativePerfCacheArtifactsMatch {
  param(
    [object]$MissHashes,
    [object]$HitHashes,
    [string[]]$ArtifactNames
  )

  foreach ($name in $ArtifactNames) {
    if ($MissHashes[$name] -ne $HitHashes[$name]) {
      throw "perf-budget FAIL: cache-proof artifact hash drift for $name"
    }
  }
}

function Assert-Objc3cNativePerfObjectArtifactNonEmpty {
  param([string]$ObjectPath)

  if (!(Test-Path -LiteralPath $ObjectPath -PathType Leaf)) {
    throw "perf-budget FAIL: cache-proof missing object artifact $ObjectPath"
  }

  $objSize = (Get-Item -LiteralPath $ObjectPath).Length
  if ($objSize -le 0) {
    throw "perf-budget FAIL: cache-proof produced empty object artifact"
  }
}

Export-ModuleMember -Function @(
  "Assert-Objc3cNativePerfCacheArtifactsMatch",
  "Assert-Objc3cNativePerfObjectArtifactNonEmpty",
  "Get-Objc3cNativePerfInvocationArtifactHashSet",
  "Read-Objc3cNativePerfInvocationCacheHitFlag"
)
