$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Get-BuildResultArtifactRelativePath {
  param(
    [object]$BuildResult,
    [string]$PropertyName
  )

  if ($null -eq $BuildResult) {
    return $null
  }

  $property = $BuildResult.PSObject.Properties[$PropertyName]
  if ($null -eq $property) {
    return $null
  }

  return [string]$property.Value
}
