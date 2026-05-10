Set-StrictMode -Version Latest

function Assert-RequiredPackageSurfaceKeys {
  param(
    [Parameter(Mandatory = $true)][hashtable]$Payload,
    [Parameter(Mandatory = $true)][string]$RelativePath,
    [Parameter(Mandatory = $true)][string[]]$RequiredKeys
  )

  foreach ($requiredKey in $RequiredKeys) {
    if (-not $Payload.ContainsKey($requiredKey)) {
      throw "runnable toolchain package FAIL: missing $requiredKey in $RelativePath"
    }
  }
}

Export-ModuleMember -Function @("Assert-RequiredPackageSurfaceKeys")
