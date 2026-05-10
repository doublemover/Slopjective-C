function Get-ParserAstBuilderNativeExecutableSummary {
  param(
    [Parameter(Mandatory = $true)]$Config
  )

  $nativeExeSummary = "$($Config.nativeExePath)"
  if (Test-Path -LiteralPath $Config.nativeExePath -PathType Leaf) {
    try {
      $nativeExeSummary = Get-RepoRelativePath -Path $Config.nativeExePath -Root $Config.repoRoot
    }
    catch {
      $nativeExeSummary = "$($Config.nativeExePath)"
    }
  }
  return $nativeExeSummary
}

function New-ParserAstBuilderContractSummary {
  param(
    [Parameter(Mandatory = $true)]$Config,
    [Parameter(Mandatory = $true)]$Checks,
    [Parameter(Mandatory = $true)]$CaseResults,
    [Parameter(Mandatory = $true)][bool]$HadFatalError,
    [Parameter(Mandatory = $true)][string]$FatalErrorMessage
  )

  $total = $Checks.Count
  $passed = @($Checks | Where-Object { $_.passed }).Count
  $failed = $total - $passed
  $status = if (-not $HadFatalError -and $total -gt 0 -and $failed -eq 0) { "PASS" } else { "FAIL" }

  return @{
    contract = "objc3c-parser-extraction-ast-builder-contract-v1"
    run_id = $Config.runId
    run_dir = $Config.runDirRel
    summary_path = $Config.summaryRel
    native_executable = (Get-ParserAstBuilderNativeExecutableSummary -Config $Config)
    status = $status
    total = $total
    passed = $passed
    failed = $failed
    fatal_error = $FatalErrorMessage
    checks = $Checks
    cases = $CaseResults
  }
}

function Write-ParserAstBuilderContractSummary {
  param(
    [Parameter(Mandatory = $true)]$Config,
    [Parameter(Mandatory = $true)]$Summary
  )

  $Summary | ConvertTo-Json -Depth 9 | Set-Content -LiteralPath $Config.summaryPath -Encoding utf8
  Write-Output ("summary_path: {0}" -f $Config.summaryRel)
  Write-Output ("status: {0}" -f $Summary.status)
}
