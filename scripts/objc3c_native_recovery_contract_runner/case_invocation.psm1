$ErrorActionPreference = "Stop"

Import-Module (Join-Path $PSScriptRoot "context.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "diagnostic_assertions.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "path_config_helpers.psm1") -Force -DisableNameChecking

function Invoke-ContractCase {
  param(
    [string]$Source,
    [string]$CaseName,
    [switch]$RequireLl,
    [switch]$RequireCompileProvenance,
    [switch]$UseCompileWrapper,
    [string[]]$ExtraArgs = @(),
    [string[]]$RequiredLlTokens = @("define i32 @objc3c_entry"),
    [string[]]$ForbiddenLlTokens = @(),
    [string[]]$RequiredManifestTokens = @(),
    [switch]$RequireObjc3ManifestSurface
  )

  $context = Get-RecoveryContractContext
  $outDir = $context.OutDir

  $resolvedSource = Resolve-RecoveryContractSourcePath -Source $Source -CaseName $CaseName
  $runPaths = New-RecoveryContractReplayRunPaths -OutDir $outDir -CaseName $CaseName
  $run1 = $runPaths.Run1
  $run2 = $runPaths.Run2

  $compileArgsRun1 = New-RecoveryContractCompileArguments -Source $resolvedSource -RunDir $run1 -ExtraArgs $ExtraArgs
  $compileArgsRun2 = New-RecoveryContractCompileArguments -Source $resolvedSource -RunDir $run2 -ExtraArgs $ExtraArgs

  $exitRun1 = Invoke-Objc3cNativeWithRecovery -Arguments $compileArgsRun1 -UseCompileWrapper:$UseCompileWrapper
  if ($exitRun1 -ne 0) { throw "contract FAIL: compile failed for $CaseName run1" }
  $exitRun2 = Invoke-Objc3cNativeWithRecovery -Arguments $compileArgsRun2 -UseCompileWrapper:$UseCompileWrapper
  if ($exitRun2 -ne 0) { throw "contract FAIL: compile failed for $CaseName run2" }

  $manifest1 = Get-Content -LiteralPath (Get-RecoveryContractArtifactPath -RunDir $run1 -FileName "module.manifest.json") -Raw
  $manifest2 = Get-Content -LiteralPath (Get-RecoveryContractArtifactPath -RunDir $run2 -FileName "module.manifest.json") -Raw
  $diag1 = Get-Content -LiteralPath (Get-RecoveryContractArtifactPath -RunDir $run1 -FileName "module.diagnostics.txt") -Raw
  $diag2 = Get-Content -LiteralPath (Get-RecoveryContractArtifactPath -RunDir $run2 -FileName "module.diagnostics.txt") -Raw

  Assert-RecoveryContractTextReplay `
    -Run1Text $manifest1 `
    -Run2Text $manifest2 `
    -FailureMessage "contract FAIL: manifest drift across replay for $CaseName"
  Assert-RecoveryContractTextReplay `
    -Run1Text $diag1 `
    -Run2Text $diag2 `
    -FailureMessage "contract FAIL: diagnostics drift across replay for $CaseName"
  if ($RequireCompileProvenance) {
    Assert-RecoveryContractCompileProvenanceReplay -CaseName $CaseName -Run1 $run1 -Run2 $run2
  }
  Assert-RecoveryContractManifestTokens `
    -CaseName $CaseName `
    -Manifest1 $manifest1 `
    -Manifest2 $manifest2 `
    -RequiredManifestTokens $RequiredManifestTokens
  if ($RequireObjc3ManifestSurface) {
    Assert-RecoveryContractManifestSurface -CaseName $CaseName -Manifest1 $manifest1 -Manifest2 $manifest2
  }

  $objSize = Assert-RecoveryContractObjectArtifact -CaseName $CaseName -RunDir $run1

  if ($RequireLl) {
    Assert-RecoveryContractLlExpectations `
      -CaseName $CaseName `
      -Run1 $run1 `
      -Run2 $run2 `
      -RequiredLlTokens $RequiredLlTokens `
      -ForbiddenLlTokens $ForbiddenLlTokens
    Write-Output "$CaseName`_deterministic_ir=true"
  }

  Write-Output "$CaseName`_deterministic_manifest=true"
  if ($RequireCompileProvenance) {
    Write-Output "$CaseName`_deterministic_compile_provenance=true"
  }
  Write-Output "$CaseName`_deterministic_diagnostics=true"
  Write-Output "$CaseName`_object_size=$objSize"
}

Export-ModuleMember -Function @(
  "Invoke-ContractCase"
)
