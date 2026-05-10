Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$script:LoweringCaseModuleRoot = Split-Path -Parent $PSScriptRoot
$script:LoweringSuiteScriptRoot = Split-Path -Parent $script:LoweringCaseModuleRoot
Import-Module (Join-Path $script:LoweringSuiteScriptRoot "objc3c_lowering_regression_suite_core.psm1") -Force -DisableNameChecking

function Add-LoweringPositiveArtifactFailures {
  param(
    [Parameter(Mandatory = $true)][pscustomobject]$NativeRuns,
    [Parameter(Mandatory = $true)][pscustomobject]$Artifacts,
    [Parameter(Mandatory = $true)][System.Collections.IDictionary]$Checks,
    [Parameter(Mandatory = $true)][System.Collections.Generic.List[string]]$FailedChecks,
    [Parameter(Mandatory = $true)][pscustomobject]$DispatchExpectation,
    [Parameter(Mandatory = $true)][pscustomobject]$DispatchIrCheck,
    [Parameter(Mandatory = $true)][pscustomobject]$Objc3Expectation,
    [Parameter(Mandatory = $true)][pscustomobject]$Objc3IrCheck
  )

  $compileSuccess = ($NativeRuns.Run1Exit -eq 0) -and ($NativeRuns.Run2Exit -eq 0)
  $objNonEmpty = $Artifacts.Run1ObjExists -and $Artifacts.Run2ObjExists -and ($Artifacts.Run1ObjBytes -gt 0) -and ($Artifacts.Run2ObjBytes -gt 0)
  $manifestDeterministic =
    ($null -ne $Artifacts.Manifest1Bytes) -and
    ($null -ne $Artifacts.Manifest2Bytes) -and
    (Test-ByteArrayEqual -Left $Artifacts.Manifest1Bytes -Right $Artifacts.Manifest2Bytes)

  $Checks["compile_success"] = $compileSuccess
  $Checks["obj_non_empty"] = $objNonEmpty
  $Checks["manifest_deterministic"] = $manifestDeterministic
  $Checks["ll_present"] = $Artifacts.LlPresent
  $Checks["ll_deterministic"] = $Artifacts.LlDeterministic

  if (-not $compileSuccess) {
    $FailedChecks.Add("compile_success")
  }
  if (-not $objNonEmpty) {
    $FailedChecks.Add("obj_non_empty")
  }
  if (-not $manifestDeterministic) {
    $FailedChecks.Add("manifest_deterministic")
  }
  if ($Artifacts.LlPresent -and -not $Artifacts.LlDeterministic) {
    $FailedChecks.Add("ll_deterministic")
  }

  if ($DispatchExpectation.Enabled -and -not $DispatchIrCheck.CompileSuccess) {
    $FailedChecks.Add("dispatch_ir_compile_success")
  }
  if ($DispatchExpectation.Enabled -and -not $DispatchIrCheck.Present) {
    $FailedChecks.Add("dispatch_ir_present")
  }
  if ($DispatchExpectation.Enabled -and -not $DispatchIrCheck.Deterministic) {
    $FailedChecks.Add("dispatch_ir_deterministic")
  }
  if ($DispatchExpectation.Enabled -and -not $DispatchIrCheck.ExpectationsMatch) {
    $FailedChecks.Add("dispatch_ir_expectations_match")
  }
  if ($Objc3Expectation.Enabled -and -not $Objc3IrCheck.Present) {
    $FailedChecks.Add("objc3_ir_present")
  }
  if ($Objc3Expectation.Enabled -and -not $Objc3IrCheck.Deterministic) {
    $FailedChecks.Add("objc3_ir_deterministic")
  }
  if ($Objc3Expectation.Enabled -and -not $Objc3IrCheck.ExpectationsMatch) {
    $FailedChecks.Add("objc3_ir_expectations_match")
  }
}

function Add-LoweringNegativeArtifactFailures {
  param(
    [Parameter(Mandatory = $true)][pscustomobject]$NativeRuns,
    [Parameter(Mandatory = $true)][pscustomobject]$Layout,
    [Parameter(Mandatory = $true)][pscustomobject]$Artifacts,
    [Parameter(Mandatory = $true)][System.Collections.IDictionary]$Checks,
    [Parameter(Mandatory = $true)][System.Collections.Generic.List[string]]$FailedChecks
  )

  $compileFails = ($NativeRuns.Run1Exit -ne 0) -and ($NativeRuns.Run2Exit -ne 0)
  $diag1Text = Get-FileTextOrEmpty -Path $Layout.Run1DiagnosticsPath
  $diag2Text = Get-FileTextOrEmpty -Path $Layout.Run2DiagnosticsPath
  $diagnosticsPopulated =
    (-not [string]::IsNullOrWhiteSpace($diag1Text)) -and
    (-not [string]::IsNullOrWhiteSpace($diag2Text))
  $manifestAbsent = ($null -eq $Artifacts.Manifest1Bytes) -and ($null -eq $Artifacts.Manifest2Bytes)
  $objAbsent = (-not $Artifacts.Run1ObjExists) -and (-not $Artifacts.Run2ObjExists)
  $llAbsent = ($null -eq $Artifacts.Ll1Bytes) -and ($null -eq $Artifacts.Ll2Bytes)

  $Checks["compile_fails"] = $compileFails
  $Checks["diagnostics_populated"] = $diagnosticsPopulated
  $Checks["manifest_absent"] = $manifestAbsent
  $Checks["obj_absent"] = $objAbsent
  $Checks["ll_absent"] = $llAbsent

  if (-not $compileFails) {
    $FailedChecks.Add("compile_fails")
  }
  if (-not $diagnosticsPopulated) {
    $FailedChecks.Add("diagnostics_populated")
  }
  if (-not $manifestAbsent) {
    $FailedChecks.Add("manifest_absent")
  }
  if (-not $objAbsent) {
    $FailedChecks.Add("obj_absent")
  }
  if (-not $llAbsent) {
    $FailedChecks.Add("ll_absent")
  }
}

function Add-LoweringExpectedArtifactFailures {
  param(
    [Parameter(Mandatory = $true)][ValidateSet("positive", "negative")][string]$FixtureKind,
    [Parameter(Mandatory = $true)][pscustomobject]$NativeRuns,
    [Parameter(Mandatory = $true)][pscustomobject]$Layout,
    [Parameter(Mandatory = $true)][pscustomobject]$Artifacts,
    [Parameter(Mandatory = $true)][System.Collections.IDictionary]$Checks,
    [Parameter(Mandatory = $true)][System.Collections.Generic.List[string]]$FailedChecks,
    [Parameter(Mandatory = $true)][pscustomobject]$DispatchExpectation,
    [Parameter(Mandatory = $true)][pscustomobject]$DispatchIrCheck,
    [Parameter(Mandatory = $true)][pscustomobject]$Objc3Expectation,
    [Parameter(Mandatory = $true)][pscustomobject]$Objc3IrCheck
  )

  if ($FixtureKind -eq "positive") {
    Add-LoweringPositiveArtifactFailures `
      -NativeRuns $NativeRuns `
      -Artifacts $Artifacts `
      -Checks $Checks `
      -FailedChecks $FailedChecks `
      -DispatchExpectation $DispatchExpectation `
      -DispatchIrCheck $DispatchIrCheck `
      -Objc3Expectation $Objc3Expectation `
      -Objc3IrCheck $Objc3IrCheck
    return
  }

  Add-LoweringNegativeArtifactFailures `
    -NativeRuns $NativeRuns `
    -Layout $Layout `
    -Artifacts $Artifacts `
    -Checks $Checks `
    -FailedChecks $FailedChecks
}

Export-ModuleMember -Function "Add-LoweringExpectedArtifactFailures"
