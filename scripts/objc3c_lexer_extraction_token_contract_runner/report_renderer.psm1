$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "../objc3c_lexer_extraction_token_contract_helpers.psm1") -DisableNameChecking

function Get-Objc3cLexerExtractionNativeExecutableSummary {
  param([Parameter(Mandatory = $true)]$Config)

  $nativeExeSummary = "$($Config.NativeExePath)"
  if (Test-Path -LiteralPath $Config.NativeExePath -PathType Leaf) {
    try {
      $nativeExeSummary = Get-RepoRelativePath -Path $Config.NativeExePath -Root $Config.RepoRoot
    }
    catch {
      $nativeExeSummary = "$($Config.NativeExePath)"
    }
  }
  $nativeExeSummary
}

function Write-Objc3cLexerExtractionTokenContractSummary {
  param(
    [Parameter(Mandatory = $true)]$Config,
    [Parameter(Mandatory = $true)]$Checks,
    [Parameter(Mandatory = $true)]$CaseResults,
    [Parameter(Mandatory = $true)][bool]$HadFatalError,
    [Parameter(Mandatory = $true)][string]$FatalErrorMessage
  )

  $checkArray = $Checks.ToArray()
  $caseResultArray = $CaseResults.ToArray()
  $total = $checkArray.Count
  $passed = @($checkArray | Where-Object { $_.passed }).Count
  $failed = $total - $passed
  $status = if (-not $HadFatalError -and $total -gt 0 -and $failed -eq 0) { "PASS" } else { "FAIL" }

  $summary = @{
    contract = "objc3c-lexer-extraction-token-contract-v1"
    run_id = $Config.RunId
    run_dir = $Config.RunDirRel
    summary_path = $Config.SummaryRel
    native_executable = Get-Objc3cLexerExtractionNativeExecutableSummary -Config $Config
    status = $status
    total = $total
    passed = $passed
    failed = $failed
    fatal_error = $FatalErrorMessage
    checks = $checkArray
    cases = $caseResultArray
  }
  $summary | ConvertTo-Json -Depth 9 | Set-Content -LiteralPath $Config.SummaryPath -Encoding utf8

  Write-Output ("summary_path: {0}" -f $Config.SummaryRel)
  Write-Output ("status: {0}" -f $status)

  if ($status -ne "PASS") {
    exit 1
  }
}

Export-ModuleMember -Function "Write-Objc3cLexerExtractionTokenContractSummary"
