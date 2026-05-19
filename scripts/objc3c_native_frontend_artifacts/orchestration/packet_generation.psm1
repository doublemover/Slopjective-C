$ErrorActionPreference = "Stop"

Import-Module (Join-Path $PSScriptRoot "..\..\objc3c_native_artifact_io.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "..\..\objc3c_native_frontend_contracts\exports.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "..\..\objc3c_native_frontend_closeout_edge_artifacts\orchestration.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "..\..\objc3c_native_frontend_closeout_conformance_artifacts\orchestration.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "status.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "core_artifacts.psm1") -Force -DisableNameChecking

function Invoke-Objc3cNativeFrontendPacketGeneration {
  param(
    [Parameter(Mandatory = $true)][string]$Mode,
    [Parameter(Mandatory = $true)][object[]]$PacketDefinitions,
    [Parameter(Mandatory = $true)]$ArtifactPaths,
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$NativeBinaryPath,
    [Parameter(Mandatory = $true)][string]$CapiBinaryPath,
    [Parameter(Mandatory = $true)][object[]]$Modules,
    [Parameter(Mandatory = $true)][string[]]$SharedSources
  )

  $selectedPacketDefinitions = @(Get-Objc3cNativeSelectedFrontendPacketDefinitions -Mode $Mode -PacketDefinitions $PacketDefinitions)
  if ($selectedPacketDefinitions.Count -eq 0) {
    Write-Objc3cNativeFrontendArtifactStep "artifact_generation_mode=none"
    return
  }

  Assert-Objc3cNativeFrontendPacketPrerequisites `
    -PacketDefinitions $PacketDefinitions `
    -SelectedPacketDefinitions $selectedPacketDefinitions `
    -NativeBinaryPath $NativeBinaryPath `
    -CapiBinaryPath $CapiBinaryPath

  $selectedFamilies = @($selectedPacketDefinitions | ForEach-Object { $_.Family } | Select-Object -Unique)
  Write-Objc3cNativeFrontendArtifactStep ("artifact_generation_mode=" + $Mode)
  Write-Objc3cNativeFrontendArtifactStep ("artifact_generation_families=" + ($selectedFamilies -join ","))
  Write-Objc3cNativeFrontendArtifactStep "artifact_generation_start=frontend_contract_artifacts"

  foreach ($definition in $selectedPacketDefinitions) {
    Write-Objc3cNativeFrontendArtifactStep ("artifact_generation_packet=" + $definition.Name + ";family=" + $definition.Family)
    switch ($definition.Name) {
      "frontend_source_graph" {
        Write-Objc3cNativeFrontendModuleScaffoldArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $ArtifactPaths.SourceGraph `
          -Modules $Modules `
          -SharedSources $SharedSources `
          -BinaryTargets @(
            (Get-Objc3cNativeRepoRelativePath -RootPath $RepoRoot -TargetPath $NativeBinaryPath),
            (Get-Objc3cNativeRepoRelativePath -RootPath $RepoRoot -TargetPath $CapiBinaryPath)
          )
      }
      "frontend_invocation_lock" {
        Write-Objc3cNativeFrontendInvocationLockArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $ArtifactPaths.InvocationLock `
          -NativeBinaryPath $NativeBinaryPath `
          -CapiBinaryPath $CapiBinaryPath `
          -FrontendScaffoldPath $ArtifactPaths.SourceGraph
      }
      "frontend_core_feature_expansion" {
        Write-Objc3cNativeFrontendCoreFeatureExpansionArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $ArtifactPaths.CoreFeatureExpansion `
          -Modules $Modules `
          -SharedSources $SharedSources `
          -NativeBinaryPath $NativeBinaryPath `
          -CapiBinaryPath $CapiBinaryPath `
          -FrontendScaffoldPath $ArtifactPaths.SourceGraph `
          -FrontendInvocationLockPath $ArtifactPaths.InvocationLock
      }
      "frontend_edge_compat" {
        Write-Objc3cNativeFrontendEdgeCompatibilityArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $ArtifactPaths.EdgeCompatibility `
          -FrontendCoreFeatureExpansionPath $ArtifactPaths.CoreFeatureExpansion
      }
      "frontend_edge_robustness" {
        Write-Objc3cNativeFrontendEdgeRobustnessArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $ArtifactPaths.EdgeRobustness `
          -FrontendEdgeCompatibilityPath $ArtifactPaths.EdgeCompatibility
      }
      "frontend_diagnostics_hardening" {
        Write-Objc3cNativeFrontendDiagnosticsHardeningArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $ArtifactPaths.DiagnosticsHardening `
          -FrontendEdgeRobustnessPath $ArtifactPaths.EdgeRobustness
      }
      "frontend_recovery_determinism_hardening" {
        Write-Objc3cNativeFrontendRecoveryDeterminismHardeningArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $ArtifactPaths.RecoveryDeterminismHardening `
          -FrontendDiagnosticsHardeningPath $ArtifactPaths.DiagnosticsHardening
      }
      "frontend_conformance_matrix" {
        Write-Objc3cNativeFrontendConformanceMatrixArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $ArtifactPaths.ConformanceMatrix `
          -FrontendRecoveryDeterminismHardeningPath $ArtifactPaths.RecoveryDeterminismHardening
      }
      "frontend_conformance_corpus" {
        Write-Objc3cNativeFrontendConformanceCorpusArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $ArtifactPaths.ConformanceCorpus `
          -FrontendConformanceMatrixPath $ArtifactPaths.ConformanceMatrix
      }
      "frontend_integration_closeout" {
        Write-Objc3cNativeFrontendIntegrationCloseoutArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $ArtifactPaths.IntegrationCloseout `
          -FrontendConformanceCorpusPath $ArtifactPaths.ConformanceCorpus
      }
      default {
        throw ("unhandled frontend contract artifact definition: " + $definition.Name)
      }
    }
  }

  Write-Objc3cNativeFrontendArtifactStep "artifact_generation_done=frontend_contract_artifacts"
}
