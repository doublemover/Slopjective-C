Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$script:LoweringSuiteScriptRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $script:LoweringSuiteScriptRoot "objc3c_lowering_regression_suite_core.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "layout.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "diagnostics_assertions.psm1") -Force -DisableNameChecking

function New-LoweringCaseCatalogEntry {
  param(
    [Parameter(Mandatory = $true)][System.IO.FileInfo]$Fixture,
    [Parameter(Mandatory = $true)][ValidateSet("positive", "negative")][string]$FixtureKind,
    [Parameter(Mandatory = $true)][pscustomobject]$SuiteContext
  )

  return [pscustomobject]@{
    Fixture = $Fixture
    FixtureKind = $FixtureKind
    Layout = New-LoweringCaseLayout -Fixture $Fixture -FixtureKind $FixtureKind -SuiteContext $SuiteContext
  }
}

function New-LoweringCaseResult {
  param(
    [Parameter(Mandatory = $true)][pscustomobject]$Case,
    [Parameter(Mandatory = $true)][pscustomobject]$NativeRuns,
    [Parameter(Mandatory = $true)][pscustomobject]$Artifacts,
    [Parameter(Mandatory = $true)][pscustomobject]$Expectations,
    [Parameter(Mandatory = $true)][pscustomobject]$DispatchIrCheck,
    [Parameter(Mandatory = $true)][pscustomobject]$Objc3IrCheck,
    [Parameter(Mandatory = $true)][System.Collections.IDictionary]$Checks,
    [Parameter(Mandatory = $true)][System.Collections.Generic.List[string]]$FailedChecks
  )

  $layout = $Case.Layout
  $passed = $FailedChecks.Count -eq 0

  return [pscustomobject]@{
    kind = $Case.FixtureKind
    fixture = $layout.FixtureRel
    passed = $passed
    detail = New-LoweringCaseDetail `
      -Passed $passed `
      -FailedChecks $FailedChecks `
      -DispatchExpectation $Expectations.Dispatch `
      -DispatchIrCheck $DispatchIrCheck `
      -Objc3Expectation $Expectations.Objc3 `
      -Objc3IrCheck $Objc3IrCheck
    case_dir = (Get-RepoRelativePath -Path $layout.CaseDir -Root $layout.RepoRoot)
    run1_exit_code = $NativeRuns.Run1Exit
    run2_exit_code = $NativeRuns.Run2Exit
    run1_obj_bytes = $Artifacts.Run1ObjBytes
    run2_obj_bytes = $Artifacts.Run2ObjBytes
    dispatch_ir_expectation = $Expectations.Dispatch.PathRel
    dispatch_ir_run1 = $DispatchIrCheck.Run1PathRel
    dispatch_ir_run2 = $DispatchIrCheck.Run2PathRel
    dispatch_ir_expected_tokens = $Expectations.Dispatch.Tokens
    dispatch_ir_run1_missing_tokens = $DispatchIrCheck.Run1Missing
    dispatch_ir_run2_missing_tokens = $DispatchIrCheck.Run2Missing
    objc3_ir_expectation = $Expectations.Objc3.PathRel
    objc3_ir_expected_tokens = $Expectations.Objc3.Tokens
    objc3_ir_run1_missing_tokens = $Objc3IrCheck.Run1Missing
    objc3_ir_run2_missing_tokens = $Objc3IrCheck.Run2Missing
    checks = $Checks
  }
}

Export-ModuleMember -Function @(
  "New-LoweringCaseCatalogEntry",
  "New-LoweringCaseResult"
)
