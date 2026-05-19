Set-StrictMode -Version Latest

function Invoke-TimedNativeCommand {
  param(
    [string]$Command,
    [string[]]$Arguments,
    [string]$LogPath
  )

  $previousErrorAction = $ErrorActionPreference
  $stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
  try {
    $ErrorActionPreference = "Continue"
    & $Command @Arguments *> $LogPath
    $exitCode = $LASTEXITCODE
  } finally {
    $stopwatch.Stop()
    $ErrorActionPreference = $previousErrorAction
  }

  return [pscustomobject]@{
    exit_code = $exitCode
    elapsed_ms = [Math]::Round($stopwatch.Elapsed.TotalMilliseconds, 3)
  }
}

function Invoke-TimedWrapperCommand {
  param(
    [string]$ScriptPath,
    [string[]]$ScriptArguments,
    [string]$LogPath
  )

  $previousErrorAction = $ErrorActionPreference
  $outputLines = @()
  $stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
  try {
    $ErrorActionPreference = "Continue"
    $outputLines = & $ScriptPath @ScriptArguments 2>&1
    $exitCode = $LASTEXITCODE
  } finally {
    $stopwatch.Stop()
    $ErrorActionPreference = $previousErrorAction
  }

  $outputText = ""
  if ($null -ne $outputLines) {
    $outputText = (($outputLines | ForEach-Object { $_.ToString() }) -join [Environment]::NewLine)
  }
  Set-Content -LiteralPath $LogPath -Value $outputText -Encoding utf8

  return [pscustomobject]@{
    exit_code = $exitCode
    elapsed_ms = [Math]::Round($stopwatch.Elapsed.TotalMilliseconds, 3)
    output_text = $outputText
  }
}

Export-ModuleMember -Function @(
  "Invoke-TimedNativeCommand",
  "Invoke-TimedWrapperCommand"
)
