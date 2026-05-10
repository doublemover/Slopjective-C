$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "objc3c_diagnostics_regression_suite_catalog.psm1") -Force -DisableNameChecking

function Write-Objc3cDiagnosticsCaseResult {
  param(
    [bool]$Passed,
    [string]$FixtureRelativePath,
    [string[]]$Errors
  )

  $statusToken = if ($Passed) { "PASS" } else { "FAIL" }
  Write-Output "[$statusToken] $FixtureRelativePath"

  if (-not $Passed) {
    foreach ($errorMessage in $Errors) {
      Write-Output "  - $errorMessage"
    }
  }
}

function New-Objc3cDiagnosticsRegressionSummary {
  param(
    [string]$RunId,
    [string]$RunDir,
    [string]$RepoRoot,
    [int]$Total,
    [int]$PassedCount,
    [int]$FailedCount,
    [string]$Status,
    [string]$FatalErrorMessage,
    [object[]]$Results
  )

  return [ordered]@{
    run_id = $RunId
    run_dir = (Get-Objc3cDiagnosticsRepoRelativePath -Path $RunDir -Root $RepoRoot)
    generated_at_utc = (Get-Date).ToUniversalTime().ToString("o")
    total = $Total
    passed = $PassedCount
    failed = $FailedCount
    status = $Status
    fatal_error = $FatalErrorMessage
    results = $Results
  }
}

function Write-Objc3cDiagnosticsRegressionSummary {
  param(
    [System.Collections.Specialized.OrderedDictionary]$Summary,
    [string]$SummaryPath,
    [string]$RepoRoot
  )

  $Summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $SummaryPath -Encoding utf8

  Write-Output "summary_path: $(Get-Objc3cDiagnosticsRepoRelativePath -Path $SummaryPath -Root $RepoRoot)"
  Write-Output "overall: $($Summary.status) ($($Summary.passed)/$($Summary.total) passed)"
}

Export-ModuleMember -Function @(
  "Write-Objc3cDiagnosticsCaseResult",
  "New-Objc3cDiagnosticsRegressionSummary",
  "Write-Objc3cDiagnosticsRegressionSummary"
)
