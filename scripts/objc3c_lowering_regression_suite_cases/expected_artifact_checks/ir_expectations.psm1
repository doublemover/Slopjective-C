Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$script:LoweringCaseModuleRoot = Split-Path -Parent $PSScriptRoot
$script:LoweringSuiteScriptRoot = Split-Path -Parent $script:LoweringCaseModuleRoot
Import-Module (Join-Path $script:LoweringSuiteScriptRoot "objc3c_lowering_regression_suite_core.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $script:LoweringSuiteScriptRoot "objc3c_lowering_regression_suite_expectations.psm1") -Force -DisableNameChecking

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

Export-ModuleMember -Function "Get-LoweringIrExpectations"
