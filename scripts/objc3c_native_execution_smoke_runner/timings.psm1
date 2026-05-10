Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$script:ScriptsRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $script:ScriptsRoot "objc3c_native_execution_smoke_helpers.psm1") -Force -DisableNameChecking

function New-ExecutionSmokeStageTimings {
  return [ordered]@{
    positive_compile_seconds = 0.0
    positive_link_seconds = 0.0
    positive_run_seconds = 0.0
    negative_compile_seconds = 0.0
    negative_link_seconds = 0.0
    negative_run_seconds = 0.0
    output_report_seconds = 0.0
  }
}

function Initialize-ExecutionSmokeStageTimings {
  $global:Objc3cExecutionSmokeStageTimings = New-ExecutionSmokeStageTimings
  return $global:Objc3cExecutionSmokeStageTimings
}

function Get-ExecutionSmokeStageTimings {
  $stageTimings = Get-Variable -Scope Global -Name Objc3cExecutionSmokeStageTimings -ValueOnly -ErrorAction SilentlyContinue
  if ($null -eq $stageTimings) {
    $global:Objc3cExecutionSmokeStageTimings = New-ExecutionSmokeStageTimings
    $stageTimings = $global:Objc3cExecutionSmokeStageTimings
  }
  return $stageTimings
}

function Add-StageDuration {
  param(
    [Parameter(Mandatory = $true)][string]$StageKey,
    [Parameter(Mandatory = $true)][double]$DurationSeconds
  )

  $stageTimings = Get-ExecutionSmokeStageTimings
  if (-not $stageTimings.Contains($StageKey)) {
    $stageTimings[$StageKey] = 0.0
  }
  $stageTimings[$StageKey] = [double]$stageTimings[$StageKey] + $DurationSeconds
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

Export-ModuleMember -Function @(
  "Add-StageDuration",
  "Get-ExecutionSmokeStageTimings",
  "Initialize-ExecutionSmokeStageTimings",
  "Invoke-TimedLoggedCommand",
  "New-ExecutionSmokeStageTimings"
)
