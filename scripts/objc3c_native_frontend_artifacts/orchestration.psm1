$ErrorActionPreference = "Stop"

function Write-Objc3cNativeFrontendArtifactStep {
  param([Parameter(Mandatory = $true)][string]$Message)

  Write-Host ("[build:objc3c-native] " + $Message)
}

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

function Write-Objc3cNativeFrontendModuleScaffoldArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [object[]]$Modules,
    [Parameter(Mandatory = $true)]
    [string[]]$SharedSources,
    [Parameter(Mandatory = $true)]
    [string[]]$BinaryTargets
  )

  $payload = New-Objc3cNativeFrontendModuleScaffoldPayload `
    -Modules $Modules `
    -SharedSources $SharedSources `
    -BinaryTargets $BinaryTargets

  Write-Objc3cNativeJsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
}

function Write-Objc3cNativeFrontendInvocationLockArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$NativeBinaryPath,
    [Parameter(Mandatory = $true)]
    [string]$CapiBinaryPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendScaffoldPath
  )

  $contracts = Get-Objc3cNativeFrontendArtifactContractIds
  Assert-Objc3cNativeFrontendArtifactPath `
    -Path $NativeBinaryPath `
    -MissingMessage "native binary missing for invocation lock artifact: $NativeBinaryPath"
  Assert-Objc3cNativeFrontendArtifactPath `
    -Path $CapiBinaryPath `
    -MissingMessage "c-api runner missing for invocation lock artifact: $CapiBinaryPath"

  $scaffoldPayload = Read-Objc3cNativeFrontendJsonArtifact `
    -Path $FrontendScaffoldPath `
    -MissingMessage "frontend source graph missing for invocation lock artifact: $FrontendScaffoldPath" `
    -InvalidJsonMessage "frontend source graph is not valid JSON for invocation lock artifact: $FrontendScaffoldPath"

  Assert-Objc3cNativeFrontendArtifactContractId `
    -Payload $scaffoldPayload `
    -ExpectedContractId $contracts.ModuleScaffold `
    -MismatchMessage "frontend source graph contract id mismatch for invocation lock artifact: $FrontendScaffoldPath"

  $payload = New-Objc3cNativeFrontendInvocationLockPayload `
    -RepoRoot $RepoRoot `
    -ScaffoldPayload $scaffoldPayload `
    -FrontendScaffoldPath $FrontendScaffoldPath `
    -NativeBinaryPath $NativeBinaryPath `
    -CapiBinaryPath $CapiBinaryPath

  Write-Objc3cNativeJsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
}

function Write-Objc3cNativeFrontendCoreFeatureExpansionArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [object[]]$Modules,
    [Parameter(Mandatory = $true)]
    [string[]]$SharedSources,
    [Parameter(Mandatory = $true)]
    [string]$NativeBinaryPath,
    [Parameter(Mandatory = $true)]
    [string]$CapiBinaryPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendScaffoldPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendInvocationLockPath
  )

  $contracts = Get-Objc3cNativeFrontendArtifactContractIds
  Assert-Objc3cNativeFrontendArtifactPath `
    -Path $FrontendScaffoldPath `
    -MissingMessage "frontend source graph missing for core feature expansion artifact: $FrontendScaffoldPath"
  Assert-Objc3cNativeFrontendArtifactPath `
    -Path $FrontendInvocationLockPath `
    -MissingMessage "frontend invocation lock missing for core feature expansion artifact: $FrontendInvocationLockPath"
  Assert-Objc3cNativeFrontendArtifactPath `
    -Path $NativeBinaryPath `
    -MissingMessage "native binary missing for core feature expansion artifact: $NativeBinaryPath"
  Assert-Objc3cNativeFrontendArtifactPath `
    -Path $CapiBinaryPath `
    -MissingMessage "c-api runner missing for core feature expansion artifact: $CapiBinaryPath"

  $scaffoldPayload = Read-Objc3cNativeFrontendJsonArtifact `
    -Path $FrontendScaffoldPath `
    -MissingMessage "frontend source graph missing for core feature expansion artifact: $FrontendScaffoldPath" `
    -InvalidJsonMessage "frontend source graph is not valid JSON for core feature expansion artifact: $FrontendScaffoldPath"

  $invocationLockPayload = Read-Objc3cNativeFrontendJsonArtifact `
    -Path $FrontendInvocationLockPath `
    -MissingMessage "frontend invocation lock missing for core feature expansion artifact: $FrontendInvocationLockPath" `
    -InvalidJsonMessage "frontend invocation lock is not valid JSON for core feature expansion artifact: $FrontendInvocationLockPath"

  Assert-Objc3cNativeFrontendArtifactContractId `
    -Payload $scaffoldPayload `
    -ExpectedContractId $contracts.ModuleScaffold `
    -MismatchMessage "frontend source graph contract id mismatch for core feature expansion artifact: $FrontendScaffoldPath"
  Assert-Objc3cNativeFrontendArtifactContractId `
    -Payload $invocationLockPayload `
    -ExpectedContractId $contracts.InvocationLock `
    -MismatchMessage "frontend invocation lock contract id mismatch for core feature expansion artifact: $FrontendInvocationLockPath"

  $payload = New-Objc3cNativeFrontendCoreFeatureExpansionPayload `
    -RepoRoot $RepoRoot `
    -Modules $Modules `
    -SharedSources $SharedSources `
    -NativeBinaryPath $NativeBinaryPath `
    -CapiBinaryPath $CapiBinaryPath

  Write-Objc3cNativeJsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
}
