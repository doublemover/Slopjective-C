$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Stop-FrontendFeatureGuard {
  param(
    [Parameter(Mandatory = $true)][string]$Message
  )

  Write-Error $Message
  exit 2
}

Export-ModuleMember -Function "Stop-FrontendFeatureGuard"
