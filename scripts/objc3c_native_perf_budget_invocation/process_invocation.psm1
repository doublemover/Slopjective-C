Set-StrictMode -Version Latest

$objc3cNativePerfBudgetScriptRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $objc3cNativePerfBudgetScriptRoot "objc3c_native_perf_budget_helpers.psm1") -Force -DisableNameChecking

function Invoke-Objc3cNativePerfNativeProcess {
  param(
    [string]$Command,
    [string[]]$Arguments,
    [string]$LogPath
  )

  return (Invoke-TimedNativeCommand -Command $Command -Arguments $Arguments -LogPath $LogPath)
}

function Invoke-Objc3cNativePerfWrapperProcess {
  param(
    [string]$ScriptPath,
    [string[]]$ScriptArguments,
    [string]$LogPath
  )

  return (Invoke-TimedWrapperCommand -ScriptPath $ScriptPath -ScriptArguments $ScriptArguments -LogPath $LogPath)
}

Export-ModuleMember -Function @(
  "Invoke-Objc3cNativePerfNativeProcess",
  "Invoke-Objc3cNativePerfWrapperProcess"
)
