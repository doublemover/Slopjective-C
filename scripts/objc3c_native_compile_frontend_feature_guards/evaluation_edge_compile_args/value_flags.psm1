$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Add-FrontendEdgeCapabilitySummaryCompileArg {
  param(
    [string]$SummaryPath,
    [hashtable]$SingleValueFlags,
    [object]$State
  )

  Assert-FrontendFeatureCompileArgValueNotEmpty -FlagName "--llvm-capabilities-summary" -Value $SummaryPath
  if (Test-FrontendFeatureArgumentPathHasParentSegment -Path $SummaryPath) {
    Stop-FrontendFeatureGuard "--llvm-capabilities-summary must not contain '..' relative segments"
  }
  $SingleValueFlags["--llvm-capabilities-summary"] = [int]$SingleValueFlags["--llvm-capabilities-summary"] + 1
  $State.has_capability_summary = $true
  $State.normalized_args.Add("--llvm-capabilities-summary")
  $State.normalized_args.Add($SummaryPath)
}

function Add-FrontendEdgeInlineValueCompileArg {
  param(
    [string]$FlagName,
    [string]$Token,
    [string]$Value,
    [System.Collections.Generic.List[string]]$NormalizedArgs
  )

  Assert-FrontendFeatureCompileArgValueNotEmpty -FlagName $FlagName -Value $Value
  $NormalizedArgs.Add($Token)
}

function Add-FrontendEdgeSplitValueCompileArg {
  param(
    [string]$FlagName,
    [string]$Value,
    [System.Collections.Generic.List[string]]$NormalizedArgs
  )

  Assert-FrontendFeatureCompileArgValueNotEmpty -FlagName $FlagName -Value $Value
  $NormalizedArgs.Add($FlagName)
  $NormalizedArgs.Add($Value)
}
