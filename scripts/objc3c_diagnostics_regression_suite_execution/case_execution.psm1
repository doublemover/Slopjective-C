$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$script:Objc3cDiagnosticsScriptsRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $script:Objc3cDiagnosticsScriptsRoot "objc3c_diagnostics_regression_suite_catalog.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $script:Objc3cDiagnosticsScriptsRoot "objc3c_diagnostics_regression_suite_assertions.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "command_invocation.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "result_classification.psm1") -Force -DisableNameChecking

function Invoke-Objc3cDiagnosticsRegressionCase {
  param(
    [System.IO.FileInfo]$Fixture,
    [string]$RepoRoot,
    [string]$RunDir,
    [string]$ExePath
  )

  $fixtureRel = Get-Objc3cDiagnosticsRepoRelativePath -Path $Fixture.FullName -Root $RepoRoot
  $fixtureSlug = "$(Get-Objc3cDiagnosticsShortHash -Value $fixtureRel)_$($Fixture.BaseName)"
  $fixtureRunDir = Join-Path $RunDir $fixtureSlug
  $run1OutDir = Join-Path $fixtureRunDir "run1"
  $run2OutDir = Join-Path $fixtureRunDir "run2"
  $run1LogPath = Join-Path $fixtureRunDir "run1.compile.log"
  $run2LogPath = Join-Path $fixtureRunDir "run2.compile.log"
  New-Item -ItemType Directory -Force -Path $fixtureRunDir | Out-Null

  $expectedSpec = Get-Objc3cDiagnosticsExpectedCodesFromFixture -FixturePath $Fixture.FullName
  $exit1 = Invoke-Objc3cDiagnosticsLoggedNativeCommand `
    -Command $ExePath `
    -Arguments @($Fixture.FullName, "--out-dir", $run1OutDir, "--emit-prefix", "module") `
    -LogPath $run1LogPath
  $exit2 = Invoke-Objc3cDiagnosticsLoggedNativeCommand `
    -Command $ExePath `
    -Arguments @($Fixture.FullName, "--out-dir", $run2OutDir, "--emit-prefix", "module") `
    -LogPath $run2LogPath

  $diag1 = Get-Objc3cDiagnosticsData -OutDir $run1OutDir
  $diag2 = Get-Objc3cDiagnosticsData -OutDir $run2OutDir
  $diagJson1 = Get-Objc3cDiagnosticsJsonData -OutDir $run1OutDir
  $diagJson2 = Get-Objc3cDiagnosticsJsonData -OutDir $run2OutDir

  $classification = New-Objc3cDiagnosticsRegressionCaseClassification `
    -ExpectedSpec $expectedSpec `
    -Run1ExitCode $exit1 `
    -Run2ExitCode $exit2 `
    -Run1Diagnostics $diag1 `
    -Run2Diagnostics $diag2 `
    -Run1DiagnosticsJson $diagJson1 `
    -Run2DiagnosticsJson $diagJson2 `
    -Run1OutDir $run1OutDir `
    -Run2OutDir $run2OutDir

  return [pscustomobject]@{
    fixture = $fixtureRel
    expected_codes = @($expectedSpec.codes)
    passed = $classification.passed
    run1_exit_code = $exit1
    run2_exit_code = $exit2
    run1_diagnostic_codes = @($diag1.codes)
    run2_diagnostic_codes = @($diag2.codes)
    run1_diagnostics_sha256 = $diag1.sha256
    run2_diagnostics_sha256 = $diag2.sha256
    run1_diagnostics_json_codes = @($diagJson1.codes)
    run2_diagnostics_json_codes = @($diagJson2.codes)
    run1_diagnostics_json_sha256 = $diagJson1.sha256
    run2_diagnostics_json_sha256 = $diagJson2.sha256
    run1_diagnostics = (Get-Objc3cDiagnosticsRepoRelativePath -Path $diag1.path -Root $RepoRoot)
    run2_diagnostics = (Get-Objc3cDiagnosticsRepoRelativePath -Path $diag2.path -Root $RepoRoot)
    run1_diagnostics_json = (Get-Objc3cDiagnosticsRepoRelativePath -Path $diagJson1.path -Root $RepoRoot)
    run2_diagnostics_json = (Get-Objc3cDiagnosticsRepoRelativePath -Path $diagJson2.path -Root $RepoRoot)
    run1_log = (Get-Objc3cDiagnosticsRepoRelativePath -Path $run1LogPath -Root $RepoRoot)
    run2_log = (Get-Objc3cDiagnosticsRepoRelativePath -Path $run2LogPath -Root $RepoRoot)
    out_dir = (Get-Objc3cDiagnosticsRepoRelativePath -Path $fixtureRunDir -Root $RepoRoot)
    checks = $classification.checks
    errors = @($classification.errors)
  }
}

Export-ModuleMember -Function @(
  "Invoke-Objc3cDiagnosticsRegressionCase"
)
