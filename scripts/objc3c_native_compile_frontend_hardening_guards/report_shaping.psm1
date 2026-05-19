$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function New-FrontendHardeningGuardReport {
  param(
    [string]$PropertyName,
    [string]$ArtifactPath
  )

  $report = [ordered]@{}
  $report[$PropertyName] = $ArtifactPath
  return [pscustomobject]$report
}

Export-ModuleMember -Function "New-FrontendHardeningGuardReport"
