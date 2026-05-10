$ErrorActionPreference = "Stop"

Import-Module (Join-Path $PSScriptRoot "objc3c_native_artifact_io.psm1") -Force
Import-Module (Join-Path $PSScriptRoot "objc3c_native_frontend_contracts.psm1") -Force
Import-Module (Join-Path $PSScriptRoot "objc3c_native_frontend_closeout_artifacts.psm1") -Force

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

  $modulePayload = New-Object System.Collections.Generic.List[object]
  foreach ($module in $Modules) {
    $name = [string]$module.name
    $sources = @($module.sources)
    if ([string]::IsNullOrWhiteSpace($name) -or $sources.Count -eq 0) {
    throw "frontend source graph module metadata is invalid"
    }
    $modulePayload.Add([ordered]@{
      name = $name
      source_count = $sources.Count
      sources = $sources
    })
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-modular-scaffold/parser_build-modular-scaffold-v1"
    schema_version = 1
    module_count = $modulePayload.Count
    shared_source_count = $SharedSources.Count
    modules = $modulePayload.ToArray()
    shared_sources = $SharedSources
    binary_targets = $BinaryTargets
  }

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

  if (!(Test-Path -LiteralPath $NativeBinaryPath -PathType Leaf)) {
    throw "native binary missing for invocation lock artifact: $NativeBinaryPath"
  }
  if (!(Test-Path -LiteralPath $CapiBinaryPath -PathType Leaf)) {
    throw "c-api runner missing for invocation lock artifact: $CapiBinaryPath"
  }
  if (!(Test-Path -LiteralPath $FrontendScaffoldPath -PathType Leaf)) {
    throw "frontend source graph missing for invocation lock artifact: $FrontendScaffoldPath"
  }

  try {
    $scaffoldPayload = Get-Content -LiteralPath $FrontendScaffoldPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend source graph is not valid JSON for invocation lock artifact: $FrontendScaffoldPath"
  }

  $expectedScaffoldContractId = "objc3c-frontend-build-invocation-modular-scaffold/parser_build-modular-scaffold-v1"
  if ([string]$scaffoldPayload.contract_id -ne $expectedScaffoldContractId) {
    throw "frontend source graph contract id mismatch for invocation lock artifact: $FrontendScaffoldPath"
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-manifest-guard/parser_build-manifest-guard-v1"
    schema_version = 1
    scaffold_contract_id = [string]$scaffoldPayload.contract_id
    scaffold = [ordered]@{
      path = Get-Objc3cNativeRepoRelativePath -RootPath $RepoRoot -TargetPath $FrontendScaffoldPath
      sha256 = Get-Objc3cNativeFileSha256Hex -Path $FrontendScaffoldPath
    }
    binaries = @(
      [ordered]@{
        name = "objc3c-native"
        path = Get-Objc3cNativeRepoRelativePath -RootPath $RepoRoot -TargetPath $NativeBinaryPath
        sha256 = Get-Objc3cNativeFileSha256Hex -Path $NativeBinaryPath
      },
      [ordered]@{
        name = "objc3c-frontend-c-api-runner"
        path = Get-Objc3cNativeRepoRelativePath -RootPath $RepoRoot -TargetPath $CapiBinaryPath
        sha256 = Get-Objc3cNativeFileSha256Hex -Path $CapiBinaryPath
      }
    )
  }

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

  if (!(Test-Path -LiteralPath $FrontendScaffoldPath -PathType Leaf)) {
    throw "frontend source graph missing for core feature expansion artifact: $FrontendScaffoldPath"
  }
  if (!(Test-Path -LiteralPath $FrontendInvocationLockPath -PathType Leaf)) {
    throw "frontend invocation lock missing for core feature expansion artifact: $FrontendInvocationLockPath"
  }
  if (!(Test-Path -LiteralPath $NativeBinaryPath -PathType Leaf)) {
    throw "native binary missing for core feature expansion artifact: $NativeBinaryPath"
  }
  if (!(Test-Path -LiteralPath $CapiBinaryPath -PathType Leaf)) {
    throw "c-api runner missing for core feature expansion artifact: $CapiBinaryPath"
  }

  try {
    $scaffoldPayload = Get-Content -LiteralPath $FrontendScaffoldPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend source graph is not valid JSON for core feature expansion artifact: $FrontendScaffoldPath"
  }

  try {
    $invocationLockPayload = Get-Content -LiteralPath $FrontendInvocationLockPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend invocation lock is not valid JSON for core feature expansion artifact: $FrontendInvocationLockPath"
  }

  $expectedScaffoldContractId = "objc3c-frontend-build-invocation-modular-scaffold/parser_build-modular-scaffold-v1"
  if ([string]$scaffoldPayload.contract_id -ne $expectedScaffoldContractId) {
    throw "frontend source graph contract id mismatch for core feature expansion artifact: $FrontendScaffoldPath"
  }
  $expectedInvocationLockContractId = "objc3c-frontend-build-invocation-manifest-guard/parser_build-manifest-guard-v1"
  if ([string]$invocationLockPayload.contract_id -ne $expectedInvocationLockContractId) {
    throw "frontend invocation lock contract id mismatch for core feature expansion artifact: $FrontendInvocationLockPath"
  }

  $moduleNames = @($Modules | ForEach-Object { [string]$_.name })
  foreach ($moduleName in $moduleNames) {
    if ([string]::IsNullOrWhiteSpace($moduleName)) {
      throw "frontend module metadata contains empty module name for core feature expansion artifact"
    }
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-core-feature-expansion/parser_build-core-feature-expansion-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedScaffoldContractId
      $expectedInvocationLockContractId
    )
    module_names = $moduleNames
    shared_source_count = $SharedSources.Count
    binaries = @(
      [ordered]@{
        name = "objc3c-native"
        path = Get-Objc3cNativeRepoRelativePath -RootPath $RepoRoot -TargetPath $NativeBinaryPath
      },
      [ordered]@{
        name = "objc3c-frontend-c-api-runner"
        path = Get-Objc3cNativeRepoRelativePath -RootPath $RepoRoot -TargetPath $CapiBinaryPath
      }
    )
    invocation = [ordered]@{
      default_out_dir = "tmp/artifacts/compilation/objc3c-native"
      cache_root = "tmp/artifacts/objc3c-native/cache"
      supports_cache = $true
    }
    backend_routing = [ordered]@{
      allowed_ir_object_backends = @("clang", "llvm-direct")
      supports_capability_routing = $true
      capability_summary_flag = "--llvm-capabilities-summary"
      route_flag = "--objc3-route-backend-from-capabilities"
    }
  }

  Write-Objc3cNativeJsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
}

Export-ModuleMember -Function "Invoke-Objc3cNativeFrontendPacketGeneration"
