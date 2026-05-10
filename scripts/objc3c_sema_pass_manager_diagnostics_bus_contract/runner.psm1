function Invoke-SemaPassManagerDiagnosticsBusContract {
  param(
    [Parameter(Mandatory = $true)][string]$ScriptRoot,
    [Parameter()][string]$ConfiguredRunId,
    [Parameter()][string]$ConfiguredNativeExe
  )

  if ($PSVersionTable.PSVersion.Major -ge 7) {
    $PSNativeCommandUseErrorActionPreference = $false
  }

  $config = New-SemaPassManagerDiagnosticsBusContractConfig `
    -ScriptRoot $ScriptRoot `
    -ConfiguredRunId $ConfiguredRunId `
    -ConfiguredNativeExe $ConfiguredNativeExe

  $checks = New-Object 'System.Collections.Generic.List[object]'
  $caseResults = New-Object 'System.Collections.Generic.List[object]'
  $hadFatalError = $false
  $fatalErrorMessage = ""

  Set-SemaPassManagerDiagnosticsBusContractContext -Checks $checks -RepoRoot $config.repo_root
  New-Item -ItemType Directory -Force -Path $config.run_dir | Out-Null

  Push-Location $config.repo_root
  try {
    Assert-SemaPassManagerDiagnosticsBusSourceFiles -Config $config
    $sourceSnapshot = Read-SemaPassManagerDiagnosticsBusSourceSnapshot -Config $config
    Assert-SemaPassManagerDiagnosticsBusStaticContracts -Snapshot $sourceSnapshot

    Invoke-SemaPassManagerDiagnosticsBusRuntimeCases `
      -RepoRoot $config.repo_root `
      -RunDir $config.run_dir `
      -BuildScriptPath $config.build_script_path `
      -NativeExePath $config.native_exe_path `
      -NativeExeExplicit $config.native_exe_explicit `
      -PositiveFixturePath $config.fixtures.positive `
      -NegativeFixturePaths $config.fixtures.negative `
      -CaseResults $caseResults
  }
  catch {
    $hadFatalError = $true
    $fatalErrorMessage = $_.Exception.Message
  }
  finally {
    Pop-Location
  }

  $summary = New-SemaPassManagerDiagnosticsBusSummary `
    -Config $config `
    -Checks $checks `
    -CaseResults $caseResults `
    -HadFatalError $hadFatalError `
    -FatalErrorMessage $fatalErrorMessage
  Write-SemaPassManagerDiagnosticsBusSummary -Summary $summary -SummaryPath $config.summary_path
  return $summary
}
