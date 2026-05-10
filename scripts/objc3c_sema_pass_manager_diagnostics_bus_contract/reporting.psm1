function Get-SemaPassManagerDiagnosticsBusNativeExeSummary {
  param(
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  if (Test-Path -LiteralPath $NativeExePath -PathType Leaf) {
    try {
      return Get-RepoRelativePath -Path $NativeExePath -Root $RepoRoot
    }
    catch {
      return "$NativeExePath"
    }
  }
  return "$NativeExePath"
}

function New-SemaPassManagerDiagnosticsBusSummary {
  param(
    [Parameter(Mandatory = $true)][object]$Config,
    [Parameter(Mandatory = $true)]$Checks,
    [Parameter(Mandatory = $true)]$CaseResults,
    [Parameter(Mandatory = $true)][bool]$HadFatalError,
    [Parameter(Mandatory = $true)][AllowEmptyString()][string]$FatalErrorMessage
  )

  $checkArray = @($Checks.ToArray())
  $caseResultArray = @($CaseResults.ToArray())
  $total = $checkArray.Count
  $passed = @($checkArray | Where-Object { $_.passed }).Count
  $failed = $total - $passed
  $status = if (-not $HadFatalError -and $total -gt 0 -and $failed -eq 0) { "PASS" } else { "FAIL" }
  $nativeExeSummary = Get-SemaPassManagerDiagnosticsBusNativeExeSummary `
    -NativeExePath $Config.native_exe_path `
    -RepoRoot $Config.repo_root

  return [pscustomobject]([ordered]@{
      contract = "objc3c-sema-pass-manager-diagnostics-bus-contract-v1"
      run_id = $Config.run_id
      run_dir = $Config.run_dir_rel
      summary_path = $Config.summary_rel
      native_executable = $nativeExeSummary
      status = $status
      total = $total
      passed = $passed
      failed = $failed
      fatal_error = $FatalErrorMessage
      checks = $checkArray
      cases = $caseResultArray
    })
}

function Write-SemaPassManagerDiagnosticsBusSummary {
  param(
    [Parameter(Mandatory = $true)][object]$Summary,
    [Parameter(Mandatory = $true)][string]$SummaryPath
  )

  $Summary | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath $SummaryPath -Encoding utf8
}
