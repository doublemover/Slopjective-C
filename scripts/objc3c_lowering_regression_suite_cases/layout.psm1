Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$script:LoweringSuiteScriptRoot = Split-Path -Parent $PSScriptRoot
Import-Module (Join-Path $script:LoweringSuiteScriptRoot "objc3c_lowering_regression_suite_core.psm1") -Force -DisableNameChecking

function New-LoweringCaseLayout {
  param(
    [Parameter(Mandatory = $true)][System.IO.FileInfo]$Fixture,
    [Parameter(Mandatory = $true)][ValidateSet("positive", "negative")][string]$FixtureKind,
    [Parameter(Mandatory = $true)][pscustomobject]$SuiteContext
  )

  $repoRoot = [string]$SuiteContext.RepoRoot
  $runDir = [string]$SuiteContext.RunDir
  $fixtureRel = Get-RepoRelativePath -Path $Fixture.FullName -Root $repoRoot
  $fixtureHash = Get-ShortHash -Value $fixtureRel
  $caseDir = Join-Path $runDir ("{0}_{1}_{2}" -f $FixtureKind, $fixtureHash, $Fixture.BaseName)
  $run1Dir = Join-Path $caseDir "run1"
  $run2Dir = Join-Path $caseDir "run2"

  return [pscustomobject]@{
    RepoRoot = $repoRoot
    RunDir = $runDir
    NativeCompilerExe = [string]$SuiteContext.NativeCompilerExe
    ClangCommand = [string]$SuiteContext.ClangCommand
    FixtureRel = $fixtureRel
    FixtureHash = $fixtureHash
    CaseDir = $caseDir
    Run1Dir = $run1Dir
    Run2Dir = $run2Dir
    Run1Log = Join-Path $caseDir "run1.log"
    Run2Log = Join-Path $caseDir "run2.log"
    Run1ObjPath = Join-Path $run1Dir "module.obj"
    Run2ObjPath = Join-Path $run2Dir "module.obj"
    Run1ManifestPath = Join-Path $run1Dir "module.manifest.json"
    Run2ManifestPath = Join-Path $run2Dir "module.manifest.json"
    Run1DiagnosticsPath = Join-Path $run1Dir "module.diagnostics.txt"
    Run2DiagnosticsPath = Join-Path $run2Dir "module.diagnostics.txt"
    Run1LlPath = Join-Path $run1Dir "module.ll"
    Run2LlPath = Join-Path $run2Dir "module.ll"
    DispatchRun1Path = Join-Path $run1Dir "module.dispatch.ll"
    DispatchRun2Path = Join-Path $run2Dir "module.dispatch.ll"
    DispatchRun1Log = Join-Path $caseDir "run1.dispatch-ir.log"
    DispatchRun2Log = Join-Path $caseDir "run2.dispatch-ir.log"
  }
}

function Initialize-LoweringCaseLayout {
  param([Parameter(Mandatory = $true)][pscustomobject]$Layout)

  New-Item -ItemType Directory -Force -Path $Layout.Run1Dir | Out-Null
  New-Item -ItemType Directory -Force -Path $Layout.Run2Dir | Out-Null
}

function Get-LoweringCaseRelativePathIfPresent {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$Root
  )

  if (Test-Path -LiteralPath $Path -PathType Leaf) {
    return Get-RepoRelativePath -Path $Path -Root $Root
  }
  return ""
}

Export-ModuleMember -Function @(
  "Get-LoweringCaseRelativePathIfPresent",
  "Initialize-LoweringCaseLayout",
  "New-LoweringCaseLayout"
)
