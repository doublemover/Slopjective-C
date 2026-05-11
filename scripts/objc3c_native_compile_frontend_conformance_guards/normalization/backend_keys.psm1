$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Normalize-FrontendConformanceBackendKey {
  param(
    [string]$Value
  )

  if ([string]::IsNullOrWhiteSpace($Value)) {
    return ""
  }
  return $Value.Trim().ToLowerInvariant().Replace("_", "-")
}
