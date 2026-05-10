Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$script:LoweringSuiteScriptRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $script:LoweringSuiteScriptRoot "objc3c_lowering_regression_suite_core.psm1") -Force -DisableNameChecking

function New-LoweringFailedCheckList {
  Write-Output -NoEnumerate (New-Object 'System.Collections.Generic.List[string]')
}

function New-LoweringDiagnosticAssertions {
  param(
    [Parameter(Mandatory = $true)][pscustomobject]$NativeRuns,
    [Parameter(Mandatory = $true)][pscustomobject]$Artifacts
  )

  return [pscustomobject]@{
    ExitCodeDeterministic = $NativeRuns.Run1Exit -eq $NativeRuns.Run2Exit
    DiagnosticsDeterministic =
      ($null -ne $Artifacts.Diagnostics1Bytes) -and
      ($null -ne $Artifacts.Diagnostics2Bytes) -and
      (Test-ByteArrayEqual -Left $Artifacts.Diagnostics1Bytes -Right $Artifacts.Diagnostics2Bytes)
  }
}

function New-LoweringCheckCatalog {
  param(
    [Parameter(Mandatory = $true)][pscustomobject]$Diagnostics,
    [Parameter(Mandatory = $true)][pscustomobject]$Expectations,
    [Parameter(Mandatory = $true)][pscustomobject]$DispatchIrCheck,
    [Parameter(Mandatory = $true)][pscustomobject]$Objc3IrCheck
  )

  $checks = [ordered]@{
    exit_code_deterministic = $Diagnostics.ExitCodeDeterministic
    diagnostics_deterministic = $Diagnostics.DiagnosticsDeterministic
    dispatch_ir_expectations_enabled = $Expectations.Dispatch.Enabled
    dispatch_ir_expectation_valid = [string]::IsNullOrWhiteSpace($Expectations.Dispatch.ParseError)
  }

  if ($Expectations.Dispatch.Enabled) {
    $checks["dispatch_ir_expectation_token_count"] = $Expectations.Dispatch.Tokens.Count
    $checks["dispatch_ir_compile_success"] = $DispatchIrCheck.CompileSuccess
    $checks["dispatch_ir_present"] = $DispatchIrCheck.Present
    $checks["dispatch_ir_deterministic"] = $DispatchIrCheck.Deterministic
    $checks["dispatch_ir_expectations_match"] = $DispatchIrCheck.ExpectationsMatch
  }
  $checks["objc3_ir_expectations_enabled"] = $Expectations.Objc3.Enabled
  $checks["objc3_ir_expectation_valid"] = [string]::IsNullOrWhiteSpace($Expectations.Objc3.ParseError)
  if ($Expectations.Objc3.Enabled) {
    $checks["objc3_ir_expectation_token_count"] = $Expectations.Objc3.Tokens.Count
    $checks["objc3_ir_present"] = $Objc3IrCheck.Present
    $checks["objc3_ir_deterministic"] = $Objc3IrCheck.Deterministic
    $checks["objc3_ir_expectations_match"] = $Objc3IrCheck.ExpectationsMatch
  }

  Write-Output -NoEnumerate $checks
}

function Add-LoweringExpectationValidityFailures {
  param(
    [Parameter(Mandatory = $true)][System.Collections.Generic.List[string]]$FailedChecks,
    [Parameter(Mandatory = $true)][pscustomobject]$DispatchExpectation,
    [Parameter(Mandatory = $true)][pscustomobject]$Objc3Expectation
  )

  if (-not [string]::IsNullOrWhiteSpace($DispatchExpectation.ParseError)) {
    $FailedChecks.Add("dispatch_ir_expectation_valid")
  }
  if (-not [string]::IsNullOrWhiteSpace($Objc3Expectation.ParseError)) {
    $FailedChecks.Add("objc3_ir_expectation_valid")
  }
}

function Add-LoweringDiagnosticFailures {
  param(
    [Parameter(Mandatory = $true)][System.Collections.Generic.List[string]]$FailedChecks,
    [Parameter(Mandatory = $true)][pscustomobject]$Diagnostics
  )

  if (-not $Diagnostics.ExitCodeDeterministic) {
    $FailedChecks.Add("exit_code_deterministic")
  }
  if (-not $Diagnostics.DiagnosticsDeterministic) {
    $FailedChecks.Add("diagnostics_deterministic")
  }
}

function New-LoweringCaseDetail {
  param(
    [Parameter(Mandatory = $true)][bool]$Passed,
    [Parameter(Mandatory = $true)][System.Collections.Generic.List[string]]$FailedChecks,
    [Parameter(Mandatory = $true)][pscustomobject]$DispatchExpectation,
    [Parameter(Mandatory = $true)][pscustomobject]$DispatchIrCheck,
    [Parameter(Mandatory = $true)][pscustomobject]$Objc3Expectation,
    [Parameter(Mandatory = $true)][pscustomobject]$Objc3IrCheck
  )

  if ($Passed) {
    return "all checks passed"
  }

  $detailSegments = New-Object 'System.Collections.Generic.List[string]'
  $null = $detailSegments.Add(($FailedChecks -join ","))
  if ($DispatchExpectation.Enabled -and -not $DispatchIrCheck.ExpectationsMatch) {
    if ($DispatchIrCheck.Run1Missing.Count -gt 0) {
      $null = $detailSegments.Add(("dispatch_run1_missing={0}" -f ($DispatchIrCheck.Run1Missing -join "|")))
    }
    if ($DispatchIrCheck.Run2Missing.Count -gt 0) {
      $null = $detailSegments.Add(("dispatch_run2_missing={0}" -f ($DispatchIrCheck.Run2Missing -join "|")))
    }
  }
  if ($Objc3Expectation.Enabled -and -not $Objc3IrCheck.ExpectationsMatch) {
    if ($Objc3IrCheck.Run1Missing.Count -gt 0) {
      $null = $detailSegments.Add(("objc3_run1_missing={0}" -f ($Objc3IrCheck.Run1Missing -join "|")))
    }
    if ($Objc3IrCheck.Run2Missing.Count -gt 0) {
      $null = $detailSegments.Add(("objc3_run2_missing={0}" -f ($Objc3IrCheck.Run2Missing -join "|")))
    }
  }
  if (-not [string]::IsNullOrWhiteSpace($DispatchExpectation.ParseError)) {
    $null = $detailSegments.Add(("dispatch_expectation_error={0}" -f $DispatchExpectation.ParseError))
  }
  if (-not [string]::IsNullOrWhiteSpace($Objc3Expectation.ParseError)) {
    $null = $detailSegments.Add(("objc3_expectation_error={0}" -f $Objc3Expectation.ParseError))
  }

  return $detailSegments -join "; "
}

Export-ModuleMember -Function @(
  "Add-LoweringDiagnosticFailures",
  "Add-LoweringExpectationValidityFailures",
  "New-LoweringCaseDetail",
  "New-LoweringCheckCatalog",
  "New-LoweringDiagnosticAssertions",
  "New-LoweringFailedCheckList"
)
