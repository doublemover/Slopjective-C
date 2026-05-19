$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Test-FrontendFeatureRelativePathHasParentSegment {
  param(
    [string]$Path
  )

  return (-not [System.IO.Path]::IsPathRooted($Path) -and $Path.Replace('\', '/').Split('/') -contains "..")
}
