$ErrorActionPreference = "Stop"

function Write-Objc3cNativeBuildStep {
  param([Parameter(Mandatory = $true)][string]$Message)

  Write-Host ("[objc3c-native] " + $Message)
}

Export-ModuleMember -Function @(
  "Write-Objc3cNativeBuildStep"
)
