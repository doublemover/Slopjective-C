$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$script:Objc3cDiagnosticsAssertionModuleRoot = Join-Path $PSScriptRoot "objc3c_diagnostics_regression_suite_assertions"

Import-Module (Join-Path $script:Objc3cDiagnosticsAssertionModuleRoot "byte_arrays.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $script:Objc3cDiagnosticsAssertionModuleRoot "diagnostics_files.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $script:Objc3cDiagnosticsAssertionModuleRoot "code_sets.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $script:Objc3cDiagnosticsAssertionModuleRoot "fail_closed_artifacts.psm1") -Force -DisableNameChecking

function Get-Objc3cDiagnosticsByteArraySha256Hex {
  param([byte[]]$Bytes)

  return Get-Objc3cDiagnosticsByteArraySha256HexCore -Bytes $Bytes
}

function Get-Objc3cDiagnosticsData {
  param([string]$OutDir)

  return Get-Objc3cDiagnosticsDataCore -OutDir $OutDir
}

function Get-Objc3cDiagnosticsJsonData {
  param([string]$OutDir)

  return Get-Objc3cDiagnosticsJsonDataCore -OutDir $OutDir
}

function Test-Objc3cDiagnosticsByteArrayEqual {
  param(
    [byte[]]$Left,
    [byte[]]$Right
  )

  return Test-Objc3cDiagnosticsByteArrayEqualCore -Left $Left -Right $Right
}

function Test-Objc3cDiagnosticsCodeSetExactMatch {
  param(
    [string[]]$Expected,
    [string[]]$Actual
  )

  return Test-Objc3cDiagnosticsCodeSetExactMatchCore -Expected $Expected -Actual $Actual
}

function Get-Objc3cDiagnosticsUnexpectedFailClosedArtifacts {
  param(
    [string]$OutDir,
    [string[]]$ArtifactNames = @("module.manifest.json", "module.ll", "module.obj")
  )

  return Get-Objc3cDiagnosticsUnexpectedFailClosedArtifactsCore -OutDir $OutDir -ArtifactNames $ArtifactNames
}

function Test-Objc3cDiagnosticsExpectedCodesForRun {
  param(
    [pscustomobject]$ExpectedSpec,
    [pscustomobject]$Diagnostics,
    [string]$RunLabel
  )

  return Test-Objc3cDiagnosticsExpectedCodesForRunCore `
    -ExpectedSpec $ExpectedSpec `
    -Diagnostics $Diagnostics `
    -RunLabel $RunLabel
}

Export-ModuleMember -Function @(
  "Get-Objc3cDiagnosticsByteArraySha256Hex",
  "Get-Objc3cDiagnosticsData",
  "Get-Objc3cDiagnosticsJsonData",
  "Test-Objc3cDiagnosticsByteArrayEqual",
  "Test-Objc3cDiagnosticsCodeSetExactMatch",
  "Get-Objc3cDiagnosticsUnexpectedFailClosedArtifacts",
  "Test-Objc3cDiagnosticsExpectedCodesForRun"
)
