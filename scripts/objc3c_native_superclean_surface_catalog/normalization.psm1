$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Convert-Objc3cNativeRepoSupercleanStringList {
  param(
    [AllowEmptyCollection()]
    [object[]]$Values
  )

  return @(
    foreach ($value in $Values) {
      [string]$value
    }
  )
}

function Convert-Objc3cNativeRepoSupercleanOrderedMap {
  param(
    [Parameter(Mandatory = $true)]
    [System.Collections.IDictionary]$Entries
  )

  $map = [ordered]@{}
  foreach ($key in $Entries.Keys) {
    $map[[string]$key] = $Entries[$key]
  }
  return $map
}

Export-ModuleMember -Function @(
  "Convert-Objc3cNativeRepoSupercleanStringList",
  "Convert-Objc3cNativeRepoSupercleanOrderedMap"
)
