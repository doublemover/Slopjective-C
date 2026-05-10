$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "../objc3c_driver_shell_split_contract_helpers.psm1") -DisableNameChecking

function Get-Objc3cDriverShellSplitNativeExecutableSummary {
  param([Parameter(Mandatory = $true)]$Config)

  $nativeExecutableSummary = "$($Config.NativeExePath)"
  if (Test-Path -LiteralPath $Config.NativeExePath -PathType Leaf) {
    try {
      $nativeExecutableSummary = Get-RepoRelativePath -Path $Config.NativeExePath -Root $Config.RepoRoot
    }
    catch {
      $nativeExecutableSummary = "$($Config.NativeExePath)"
    }
  }
  $nativeExecutableSummary
}

function Write-Objc3cDriverShellSplitContractSummary {
  param(
    [Parameter(Mandatory = $true)]$Config,
    [Parameter(Mandatory = $true)]$Checks,
    [Parameter(Mandatory = $true)][bool]$HadFatalError,
    [Parameter(Mandatory = $true)][string]$FatalErrorMessage,
    [Parameter()]$SmokeCompile = $null
  )

  $checkArray = $Checks.ToArray()
  $totalChecks = $checkArray.Count
  $passedChecks = @($checkArray | Where-Object { $_.passed }).Count
  $failedChecks = $totalChecks - $passedChecks
  $status = if (-not $HadFatalError -and $totalChecks -gt 0 -and $failedChecks -eq 0) { "PASS" } else { "FAIL" }

  $summary = @{
    contract = "objc3c-driver-shell-split-v1"
    run_id = $Config.RunId
    run_dir = $Config.RunDirRel
    summary_path = $Config.SummaryRel
    native_executable = Get-Objc3cDriverShellSplitNativeExecutableSummary -Config $Config
    status = $status
    total = $totalChecks
    passed = $passedChecks
    failed = $failedChecks
    fatal_error = $FatalErrorMessage
    checks = $checkArray
    smoke_compile = $SmokeCompile
  }
  $summary | ConvertTo-Json -Depth 9 | Set-Content -LiteralPath $Config.SummaryPath -Encoding utf8

  Write-Output ("summary_path: {0}" -f $Config.SummaryRel)
  Write-Output ("status: {0}" -f $status)

  if ($status -ne "PASS") {
    exit 1
  }
}

Export-ModuleMember -Function "Write-Objc3cDriverShellSplitContractSummary"
