$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function ConvertTo-Objc3cNativeCompileBooleanFlagValue {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RawValue,
    [Parameter(Mandatory = $true)]
    [string]$FlagName
  )

  $normalizedValue = $RawValue.Trim().ToLowerInvariant()
  if (@("1", "true", "yes", "on") -contains $normalizedValue) {
    return $true
  }
  if (@("0", "false", "no", "off") -contains $normalizedValue) {
    return $false
  }

  Stop-Objc3cNativeCompileArgumentError -Message "invalid $FlagName value '$normalizedValue' (expected true/false style token)"
}
