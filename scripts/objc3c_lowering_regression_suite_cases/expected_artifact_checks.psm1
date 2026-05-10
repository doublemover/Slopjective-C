Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$script:LoweringSuiteScriptRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $script:LoweringSuiteScriptRoot "objc3c_lowering_regression_suite_core.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $script:LoweringSuiteScriptRoot "objc3c_lowering_regression_suite_expectations.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "layout.psm1") -Force -DisableNameChecking

function Get-LoweringCaseArtifacts {
  param([Parameter(Mandatory = $true)][pscustomobject]$Layout)

  $run1ObjExists = Test-Path -LiteralPath $Layout.Run1ObjPath -PathType Leaf
  $run2ObjExists = Test-Path -LiteralPath $Layout.Run2ObjPath -PathType Leaf
  $run1ObjBytes = if ($run1ObjExists) { (Get-Item -LiteralPath $Layout.Run1ObjPath).Length } else { 0 }
  $run2ObjBytes = if ($run2ObjExists) { (Get-Item -LiteralPath $Layout.Run2ObjPath).Length } else { 0 }

  $ll1Bytes = Get-FileBytesOrNull -Path $Layout.Run1LlPath
  $ll2Bytes = Get-FileBytesOrNull -Path $Layout.Run2LlPath
  $llPresentRun1 = $null -ne $ll1Bytes
  $llPresentRun2 = $null -ne $ll2Bytes
  $llPresent = $llPresentRun1 -or $llPresentRun2
  if ($llPresent) {
    $llDeterministic = $llPresentRun1 -and $llPresentRun2 -and (Test-ByteArrayEqual -Left $ll1Bytes -Right $ll2Bytes)
  }
  else {
    $llDeterministic = $true
  }

  return [pscustomobject]@{
    Run1ObjExists = $run1ObjExists
    Run2ObjExists = $run2ObjExists
    Run1ObjBytes = $run1ObjBytes
    Run2ObjBytes = $run2ObjBytes
    Manifest1Bytes = Get-FileBytesOrNull -Path $Layout.Run1ManifestPath
    Manifest2Bytes = Get-FileBytesOrNull -Path $Layout.Run2ManifestPath
    Diagnostics1Bytes = Get-FileBytesOrNull -Path $Layout.Run1DiagnosticsPath
    Diagnostics2Bytes = Get-FileBytesOrNull -Path $Layout.Run2DiagnosticsPath
    Ll1Bytes = $ll1Bytes
    Ll2Bytes = $ll2Bytes
    LlPresentRun1 = $llPresentRun1
    LlPresentRun2 = $llPresentRun2
    LlPresent = $llPresent
    LlDeterministic = $llDeterministic
  }
}

function New-LoweringIrExpectation {
  param(
    [Parameter(Mandatory = $true)][pscustomobject]$Spec,
    [Parameter(Mandatory = $true)][string]$RequiredExtension,
    [Parameter(Mandatory = $true)][string]$ExtensionError,
    [Parameter(Mandatory = $true)][System.IO.FileInfo]$Fixture,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  $enabled = $Spec.enabled
  $path = $Spec.path
  $tokens = @($Spec.tokens)
  $parseError = $Spec.parse_error
  if ($enabled -and ($Fixture.Extension -ine $RequiredExtension)) {
    $parseError = $ExtensionError
  }

  $pathRel = ""
  if ($enabled) {
    $pathRel = Get-RepoRelativePath -Path $path -Root $RepoRoot
  }

  return [pscustomobject]@{
    Enabled = $enabled
    Path = $path
    PathRel = $pathRel
    Tokens = $tokens
    ParseError = $parseError
  }
}

function Get-LoweringIrExpectations {
  param(
    [Parameter(Mandatory = $true)][System.IO.FileInfo]$Fixture,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  $dispatchSpec = Get-DispatchIrExpectation -FixturePath $Fixture.FullName
  $objc3Spec = Get-Objc3IrExpectation -FixturePath $Fixture.FullName

  return [pscustomobject]@{
    Dispatch = New-LoweringIrExpectation `
      -Spec $dispatchSpec `
      -RequiredExtension ".m" `
      -ExtensionError "dispatch IR expectations require Objective-C .m fixtures" `
      -Fixture $Fixture `
      -RepoRoot $RepoRoot
    Objc3 = New-LoweringIrExpectation `
      -Spec $objc3Spec `
      -RequiredExtension ".objc3" `
      -ExtensionError "objc3 IR expectations require .objc3 fixtures" `
      -Fixture $Fixture `
      -RepoRoot $RepoRoot
  }
}

function Test-Objc3IrArtifactExpectations {
  param(
    [Parameter(Mandatory = $true)][pscustomobject]$Layout,
    [Parameter(Mandatory = $true)][pscustomobject]$Artifacts,
    [Parameter(Mandatory = $true)][pscustomobject]$Expectation
  )

  $present = $true
  $deterministic = $true
  $expectationsMatch = $true
  $run1Missing = @()
  $run2Missing = @()

  if ($Expectation.Enabled -and [string]::IsNullOrWhiteSpace($Expectation.ParseError)) {
    $present = $Artifacts.LlPresentRun1 -and $Artifacts.LlPresentRun2
    if ($present) {
      $deterministic = Test-ByteArrayEqual -Left $Artifacts.Ll1Bytes -Right $Artifacts.Ll2Bytes
      $run1Text = Get-FileTextOrEmpty -Path $Layout.Run1LlPath
      $run2Text = Get-FileTextOrEmpty -Path $Layout.Run2LlPath
      $run1Missing = @(Get-MissingTokens -Text $run1Text -Tokens $Expectation.Tokens)
      $run2Missing = @(Get-MissingTokens -Text $run2Text -Tokens $Expectation.Tokens)
      $expectationsMatch = ($run1Missing.Count -eq 0) -and ($run2Missing.Count -eq 0)
    }
    else {
      $deterministic = $false
      $expectationsMatch = $false
    }
  }

  return [pscustomobject]@{
    Present = $present
    Deterministic = $deterministic
    ExpectationsMatch = $expectationsMatch
    Run1Missing = $run1Missing
    Run2Missing = $run2Missing
  }
}

function Invoke-DispatchIrArtifactExpectations {
  param(
    [Parameter(Mandatory = $true)][System.IO.FileInfo]$Fixture,
    [Parameter(Mandatory = $true)][pscustomobject]$Layout,
    [Parameter(Mandatory = $true)][pscustomobject]$Expectation
  )

  $run1Exit = 0
  $run2Exit = 0
  $compileSuccess = $true
  $present = $true
  $deterministic = $true
  $expectationsMatch = $true
  $run1Missing = @()
  $run2Missing = @()

  if ($Expectation.Enabled -and [string]::IsNullOrWhiteSpace($Expectation.ParseError)) {
    $run1Exit = Invoke-LoggedCommand `
      -Command $Layout.ClangCommand `
      -Arguments @("-x", "objective-c", "-std=gnu11", "-S", "-emit-llvm", $Fixture.FullName, "-o", $Layout.DispatchRun1Path) `
      -LogPath $Layout.DispatchRun1Log
    $run2Exit = Invoke-LoggedCommand `
      -Command $Layout.ClangCommand `
      -Arguments @("-x", "objective-c", "-std=gnu11", "-S", "-emit-llvm", $Fixture.FullName, "-o", $Layout.DispatchRun2Path) `
      -LogPath $Layout.DispatchRun2Log

    $compileSuccess = ($run1Exit -eq 0) -and ($run2Exit -eq 0)

    if ($compileSuccess) {
      $run1Bytes = Get-FileBytesOrNull -Path $Layout.DispatchRun1Path
      $run2Bytes = Get-FileBytesOrNull -Path $Layout.DispatchRun2Path
      $run1Present = $null -ne $run1Bytes
      $run2Present = $null -ne $run2Bytes
      $present = $run1Present -and $run2Present
      if ($present) {
        $deterministic = Test-ByteArrayEqual -Left $run1Bytes -Right $run2Bytes
        $run1Text = Get-FileTextOrEmpty -Path $Layout.DispatchRun1Path
        $run2Text = Get-FileTextOrEmpty -Path $Layout.DispatchRun2Path
        $run1Missing = @(Get-MissingTokens -Text $run1Text -Tokens $Expectation.Tokens)
        $run2Missing = @(Get-MissingTokens -Text $run2Text -Tokens $Expectation.Tokens)
        $expectationsMatch = ($run1Missing.Count -eq 0) -and ($run2Missing.Count -eq 0)
      }
      else {
        $deterministic = $false
        $expectationsMatch = $false
      }
    }
    else {
      $present = $false
      $deterministic = $false
      $expectationsMatch = $false
    }
  }

  return [pscustomobject]@{
    Run1Exit = $run1Exit
    Run2Exit = $run2Exit
    CompileSuccess = $compileSuccess
    Present = $present
    Deterministic = $deterministic
    ExpectationsMatch = $expectationsMatch
    Run1Missing = $run1Missing
    Run2Missing = $run2Missing
    Run1PathRel = Get-LoweringCaseRelativePathIfPresent -Path $Layout.DispatchRun1Path -Root $Layout.RepoRoot
    Run2PathRel = Get-LoweringCaseRelativePathIfPresent -Path $Layout.DispatchRun2Path -Root $Layout.RepoRoot
  }
}

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

Export-ModuleMember -Function @(
  "Add-LoweringExpectedArtifactFailures",
  "Get-LoweringCaseArtifacts",
  "Get-LoweringIrExpectations",
  "Invoke-DispatchIrArtifactExpectations",
  "Test-Objc3IrArtifactExpectations"
)
