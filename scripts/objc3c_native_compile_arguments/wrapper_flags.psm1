$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function New-Objc3cNativeCompileWrapperFlagCounts {
  return @{
    "--use-cache" = 0
    "--out-dir" = 0
  }
}

function Add-Objc3cNativeCompileWrapperFlagUse {
  param(
    [Parameter(Mandatory = $true)]
    [hashtable]$FlagCounts,
    [Parameter(Mandatory = $true)]
    [string]$FlagName
  )

  $FlagCounts[$FlagName] = [int]$FlagCounts[$FlagName] + 1
  if ([int]$FlagCounts[$FlagName] -gt 1) {
    Stop-Objc3cNativeCompileArgumentError -Message "$FlagName can be provided at most once"
  }
}
