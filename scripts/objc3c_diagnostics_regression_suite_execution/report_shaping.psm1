$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$script:Objc3cDiagnosticsScriptsRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $script:Objc3cDiagnosticsScriptsRoot "objc3c_diagnostics_regression_suite_reporting.psm1") -Force -DisableNameChecking

function New-Objc3cDiagnosticsRegressionSuiteReport {
  param(
    [string]$RunId,
    [string]$RunDir,
    [string]$RepoRoot,
    [bool]$HadFatalError,
    [string]$FatalErrorMessage,
    [object[]]$Results
  )

  $resultsArray = @()
  if ($null -ne $Results) {
    $resultsArray = @($Results)
  }

  $total = $resultsArray.Count
  $passedCount = @($resultsArray | Where-Object { $_.passed }).Count
  $failedCount = $total - $passedCount
  $status = if (-not $HadFatalError -and $total -gt 0 -and $failedCount -eq 0) { "PASS" } else { "FAIL" }

  $summary = New-Objc3cDiagnosticsRegressionSummary `
    -RunId $RunId `
    -RunDir $RunDir `
    -RepoRoot $RepoRoot `
    -Total $total `
    -PassedCount $passedCount `
    -FailedCount $failedCount `
    -Status $status `
    -FatalErrorMessage $FatalErrorMessage `
    -Results $resultsArray

  return [pscustomobject]@{
    total = $total
    passed = $passedCount
    failed = $failedCount
    status = $status
    summary = $summary
  }
}

Export-ModuleMember -Function @(
  "New-Objc3cDiagnosticsRegressionSuiteReport"
)
