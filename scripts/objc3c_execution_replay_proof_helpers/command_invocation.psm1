Set-StrictMode -Version Latest

function Invoke-LoggedCommand {
  param(
    [Parameter(Mandatory = $true)][string]$Command,
    [Parameter(Mandatory = $true)][AllowEmptyCollection()][string[]]$Arguments,
    [Parameter(Mandatory = $true)][string]$LogPath
  )

  $previousErrorAction = $ErrorActionPreference
  try {
    $ErrorActionPreference = "Continue"
    & $Command @Arguments *> $LogPath
    return [int]$LASTEXITCODE
  }
  finally {
    $ErrorActionPreference = $previousErrorAction
  }
}

function Add-StageDuration {
  param(
    [Parameter(Mandatory = $true)][string]$StageKey,
    [Parameter(Mandatory = $true)][double]$DurationSeconds
  )

  if (-not $script:stageTimings.Contains($StageKey)) {
    $script:stageTimings[$StageKey] = 0.0
  }
  $script:stageTimings[$StageKey] = [double]$script:stageTimings[$StageKey] + $DurationSeconds
}

function Invoke-TimedLoggedCommand {
  param(
    [Parameter(Mandatory = $true)][string]$StageKey,
    [Parameter(Mandatory = $true)][string]$Command,
    [Parameter(Mandatory = $true)][AllowEmptyCollection()][string[]]$Arguments,
    [Parameter(Mandatory = $true)][string]$LogPath
  )

  $stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
  $exitCode = Invoke-LoggedCommand -Command $Command -Arguments $Arguments -LogPath $LogPath
  $stopwatch.Stop()
  $durationSeconds = [math]::Round($stopwatch.Elapsed.TotalSeconds, 6)
  Add-StageDuration -StageKey $StageKey -DurationSeconds $durationSeconds
  return [pscustomobject]@{
    exit_code = [int]$exitCode
    duration_seconds = $durationSeconds
  }
}

function Ensure-NativeCompilerExecutable {
  param(
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][bool]$NativeExeExplicit,
    [Parameter(Mandatory = $true)][string]$BuildScriptPath
  )

  if (Test-Path -LiteralPath $NativeExePath -PathType Leaf) {
    return
  }
  if ($NativeExeExplicit) {
    throw "execution replay proof FAIL: configured native compiler missing at $NativeExePath"
  }
  if (!(Test-Path -LiteralPath $BuildScriptPath -PathType Leaf)) {
    throw "execution replay proof FAIL: native build script missing at $BuildScriptPath"
  }

  & $BuildScriptPath -ExecutionMode binaries-only | Out-Null
  if ($LASTEXITCODE -ne 0) {
    throw "execution replay proof FAIL: native compiler build failed with exit code $LASTEXITCODE"
  }
  if (!(Test-Path -LiteralPath $NativeExePath -PathType Leaf)) {
    throw "execution replay proof FAIL: native compiler executable missing at $NativeExePath"
  }
}
