Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$script:LoweringCaseModuleRoot = Split-Path -Parent $PSScriptRoot
$script:LoweringSuiteScriptRoot = Split-Path -Parent $script:LoweringCaseModuleRoot
Import-Module (Join-Path $script:LoweringSuiteScriptRoot "objc3c_lowering_regression_suite_core.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $script:LoweringCaseModuleRoot "layout.psm1") -Force -DisableNameChecking

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

Export-ModuleMember -Function @(
  "Invoke-DispatchIrArtifactExpectations",
  "Test-Objc3IrArtifactExpectations"
)
