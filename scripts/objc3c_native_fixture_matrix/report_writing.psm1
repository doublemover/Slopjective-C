$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "paths.psm1") -Force -DisableNameChecking

function Write-Objc3cNativeFixtureMatrixReport {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RunDir,
    [Parameter(Mandatory = $true)][string]$SummaryPath,
    [object[]]$Results = @(),
    [string]$FixtureList = "",
    [string]$FixtureGlob = "",
    [int]$ShardIndex = -1,
    [int]$ShardCount = 0,
    [int]$Limit = 0,
    [int]$SelectedPositiveCount = 0,
    [bool]$HadFatalError = $false,
    [string]$FatalErrorMessage = ""
  )

  $total = $Results.Count
  $passedCount = @($Results | Where-Object { $_.passed }).Count
  $failedCount = $total - $passedCount
  $status = if (!$HadFatalError -and $total -gt 0 -and $failedCount -eq 0) { "PASS" } else { "FAIL" }

  $summary = [ordered]@{
    run_dir = (Get-Objc3cNativeFixtureMatrixRepoRelativePath -Path $RunDir -Root $RepoRoot)
    selection = [ordered]@{
      fixture_list = $FixtureList
      fixture_glob = $FixtureGlob
      shard_index = $ShardIndex
      shard_count = $ShardCount
      limit = $Limit
      selected_positive = $SelectedPositiveCount
    }
    total = $total
    passed = $passedCount
    failed = $failedCount
    status = $status
    fatal_error = $FatalErrorMessage
    results = $Results
  }
  $summary | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $SummaryPath -Encoding utf8

  return [pscustomobject]@{
    total = $total
    passed = $passedCount
    failed = $failedCount
    status = $status
    summary_path = (Get-Objc3cNativeFixtureMatrixRepoRelativePath -Path $SummaryPath -Root $RepoRoot)
  }
}

Export-ModuleMember -Function "Write-Objc3cNativeFixtureMatrixReport"
