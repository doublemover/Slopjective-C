$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Stop-FrontendConformanceGuard {
  param(
    [Parameter(Mandatory = $true)][string]$Message
  )

  Write-Error $Message
  exit 2
}

Export-ModuleMember -Function "Stop-FrontendConformanceGuard"
