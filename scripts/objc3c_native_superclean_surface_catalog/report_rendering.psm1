$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "normalization.psm1") -Force -DisableNameChecking

function New-Objc3cNativeRepoSupercleanCatalogPayload {
  param(
    [Parameter(Mandatory = $true)]
    [System.Collections.IDictionary]$Entries
  )

  return Convert-Objc3cNativeRepoSupercleanOrderedMap -Entries $Entries
}

function New-Objc3cNativeRepoSupercleanPathList {
  param(
    [AllowEmptyCollection()]
    [object[]]$Paths
  )

  return Convert-Objc3cNativeRepoSupercleanStringList -Values $Paths
}

function New-Objc3cNativeRepoSupercleanActionList {
  param(
    [AllowEmptyCollection()]
    [object[]]$Actions
  )

  return Convert-Objc3cNativeRepoSupercleanStringList -Values $Actions
}

function New-Objc3cNativeRepoSupercleanStringList {
  param(
    [AllowEmptyCollection()]
    [object[]]$Values
  )

  return Convert-Objc3cNativeRepoSupercleanStringList -Values $Values
}

Export-ModuleMember -Function @(
  "New-Objc3cNativeRepoSupercleanCatalogPayload",
  "New-Objc3cNativeRepoSupercleanPathList",
  "New-Objc3cNativeRepoSupercleanActionList",
  "New-Objc3cNativeRepoSupercleanStringList"
)
