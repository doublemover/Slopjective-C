$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

function Invoke-Objc3cDiagnosticsLoggedNativeCommand {
  param(
    [string]$Command,
    [string[]]$Arguments,
    [string]$LogPath
  )

  $previousErrorAction = $ErrorActionPreference
  try {
    $ErrorActionPreference = "Continue"
    & $Command @Arguments *> $LogPath
    return [int]$LASTEXITCODE
  } finally {
    $ErrorActionPreference = $previousErrorAction
  }
}

Export-ModuleMember -Function @(
  "Invoke-Objc3cDiagnosticsLoggedNativeCommand"
)
