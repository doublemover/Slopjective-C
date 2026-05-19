$ErrorActionPreference = "Stop"

function Write-Objc3cNativeFrontendArtifactStep {
  param([Parameter(Mandatory = $true)][string]$Message)

  Write-Host ("[build:objc3c-native] " + $Message)
}
