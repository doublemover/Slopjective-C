param(
  [ValidateSet("full", "binaries-only", "contracts-source", "contracts-binary", "contracts-closeout", "contracts-all")]
  [string]$ExecutionMode = "full",
  [switch]$ForceReconfigure
)

$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
Import-Module (Join-Path $PSScriptRoot "objc3c_native_artifact_io.psm1") -Force
Import-Module (Join-Path $PSScriptRoot "objc3c_native_cmake.psm1") -Force
Import-Module (Join-Path $PSScriptRoot "objc3c_native_frontend_contracts.psm1") -Force
Import-Module (Join-Path $PSScriptRoot "objc3c_native_superclean_surface.psm1") -Force

$nativeToolchain = Resolve-Objc3cNativeToolchain -RepoRoot $repoRoot
$llvmRoot = $nativeToolchain.LlvmRoot
$clangxx = $nativeToolchain.Clangxx
$llvmLibTool = $nativeToolchain.LlvmLibTool
$cmakeTool = $nativeToolchain.CmakeTool
$ninjaTool = $nativeToolchain.NinjaTool
$libclang = $nativeToolchain.Libclang
$includeDir = $nativeToolchain.IncludeDir
$nativeSourceRoot = $nativeToolchain.NativeSourceRoot

# objc3c.nativebuild.toolchainparity.v1 anchor:
# - authoritative toolchain flow today is `LLVM_ROOT` -> direct wrapper
#   resolution for clang++, llvm-lib, libclang, and include/library discovery
# - later incremental backend work must preserve this authoritative wrapper
#   contract when forwarding configuration into CMake/Ninja
# - compile database parity frozen to `tmp/build-objc3c-native/compile_commands.json`
#
# objc3c.nativebuild.incrementalbackend.v1 anchor:
# - native binaries now build through a persistent CMake/Ninja tree rooted at
#   `tmp/build-objc3c-native`
# - canonical outputs remain published at `artifacts/bin` and `artifacts/lib`
# - this script still owns frontend contract-artifact generation after the native build

$nativeBuildPaths = Get-Objc3cNativeCMakeBuildPaths -RepoRoot $repoRoot
$outDir = $nativeBuildPaths.RuntimeOutputDir
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$outLibDir = $nativeBuildPaths.LibraryOutputDir
New-Item -ItemType Directory -Force -Path $outLibDir | Out-Null
$outExe = $nativeBuildPaths.NativeExecutable
$outCapiExe = $nativeBuildPaths.CapiRunnerExecutable
$outRuntimeLib = $nativeBuildPaths.RuntimeLibrary
$tmpOutDir = $nativeBuildPaths.BuildDir
New-Item -ItemType Directory -Force -Path $tmpOutDir | Out-Null
$cmakeSourceDir = $nativeBuildPaths.CmakeSourceDir
$compileCommandsPath = $nativeBuildPaths.CompileCommands
$buildFingerprintPath = $nativeBuildPaths.BuildFingerprint

# objc3c.nativebuild.commandsurface.v1 anchor:
# - current truthful state: this script remains the authoritative wrapper
#   behind the public npm build surface
# - the public npm command taxonomy now maps to:
#   - build:objc3c-native              => fast binary-build default
#   - build:objc3c-native:contracts   => source-derived + binary-derived contract-artifact path
#   - build:objc3c-native:full        => binary + full contract-artifact family path
#   - build:objc3c-native:reconfigure => reserved fingerprint refresh/self-heal
# - direct script callers still default to `full` until the helper/runner
#   migration tranche lands

# objc3c.nativebuild.incrementalbackend.runtime.v1 anchor:
# - native binary compilation now routes through a persistent CMake/Ninja build
#   tree under `tmp/build-objc3c-native`
# - final published native artifacts remain under `artifacts/bin` and
#   `artifacts/lib`
# - the wrapper still owns frontend contract-artifact generation after the native binaries
#   are built
#
# objc3c.nativebuild.contractartifactshape.v1 anchor:
# - contract-artifact generation is internally classified into source-derived,
#   binary-derived, and closeout-derived families
# - the wrapper can execute those contract-artifact families independently without
#   silently re-triggering native binary compilation
# - public command-surface exposure remains the responsibility of the shared command contract surface
$runSuffix = "{0}_{1}" -f (Get-Date -Format "yyyyMMdd_HHmmss_fff"), $PID
$stagedOutExe = Join-Path $tmpOutDir ("objc3c-native.{0}.exe" -f $runSuffix)
$stagedOutCapiExe = Join-Path $tmpOutDir ("objc3c-frontend-c-api-runner.{0}.exe" -f $runSuffix)
$stagedRuntimeObj = Join-Path $tmpOutDir ("objc3_runtime.{0}.obj" -f $runSuffix)
$stagedRuntimeLib = Join-Path $tmpOutDir ("objc3_runtime.{0}.lib" -f $runSuffix)

function Write-BuildStep {
  param([Parameter(Mandatory = $true)][string]$Message)

  Write-Host ("[build:objc3c-native] " + $Message)
}

function Test-ExecutionModeRunsNativeBuild {
  param([Parameter(Mandatory = $true)][string]$Mode)

  return $Mode -in @("full", "binaries-only")
}

function Invoke-FrontendPacketGeneration {
  param(
    [Parameter(Mandatory = $true)][string]$Mode,
    [Parameter(Mandatory = $true)][object[]]$PacketDefinitions,
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$NativeBinaryPath,
    [Parameter(Mandatory = $true)][string]$CapiBinaryPath,
    [Parameter(Mandatory = $true)][object[]]$Modules,
    [Parameter(Mandatory = $true)][string[]]$SharedSources
  )

  $selectedPacketDefinitions = @(Get-Objc3cNativeSelectedFrontendPacketDefinitions -Mode $Mode -PacketDefinitions $PacketDefinitions)
  if ($selectedPacketDefinitions.Count -eq 0) {
    Write-BuildStep "artifact_generation_mode=none"
    return
  }

  Assert-Objc3cNativeFrontendPacketPrerequisites `
    -PacketDefinitions $PacketDefinitions `
    -SelectedPacketDefinitions $selectedPacketDefinitions `
    -NativeBinaryPath $NativeBinaryPath `
    -CapiBinaryPath $CapiBinaryPath

  $selectedFamilies = @($selectedPacketDefinitions | ForEach-Object { $_.Family } | Select-Object -Unique)
  Write-BuildStep ("artifact_generation_mode=" + $Mode)
  Write-BuildStep ("artifact_generation_families=" + ($selectedFamilies -join ","))
  Write-BuildStep "artifact_generation_start=frontend_contract_artifacts"

  foreach ($definition in $selectedPacketDefinitions) {
    Write-BuildStep ("artifact_generation_packet=" + $definition.Name + ";family=" + $definition.Family)
    switch ($definition.Name) {
      "frontend_source_graph" {
        Write-FrontendModuleScaffoldArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $frontendScaffoldPath `
          -Modules $Modules `
          -SharedSources $SharedSources `
          -BinaryTargets @(
            (Get-Objc3cNativeRepoRelativePath -RootPath $RepoRoot -TargetPath $NativeBinaryPath),
            (Get-Objc3cNativeRepoRelativePath -RootPath $RepoRoot -TargetPath $CapiBinaryPath)
          )
      }
      "frontend_invocation_lock" {
        Write-FrontendInvocationLockArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $frontendInvocationLockPath `
          -NativeBinaryPath $NativeBinaryPath `
          -CapiBinaryPath $CapiBinaryPath `
          -FrontendScaffoldPath $frontendScaffoldPath
      }
      "frontend_core_feature_expansion" {
        Write-FrontendCoreFeatureExpansionArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $frontendCoreFeatureExpansionPath `
          -Modules $Modules `
          -SharedSources $SharedSources `
          -NativeBinaryPath $NativeBinaryPath `
          -CapiBinaryPath $CapiBinaryPath `
          -FrontendScaffoldPath $frontendScaffoldPath `
          -FrontendInvocationLockPath $frontendInvocationLockPath
      }
      "frontend_edge_compat" {
        Write-FrontendEdgeCompatibilityArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $frontendEdgeCompatPath `
          -FrontendCoreFeatureExpansionPath $frontendCoreFeatureExpansionPath
      }
      "frontend_edge_robustness" {
        Write-FrontendEdgeRobustnessArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $frontendEdgeRobustnessPath `
          -FrontendEdgeCompatibilityPath $frontendEdgeCompatPath
      }
      "frontend_diagnostics_hardening" {
        Write-FrontendDiagnosticsHardeningArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $frontendDiagnosticsHardeningPath `
          -FrontendEdgeRobustnessPath $frontendEdgeRobustnessPath
      }
      "frontend_recovery_determinism_hardening" {
        Write-FrontendRecoveryDeterminismHardeningArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $frontendRecoveryDeterminismHardeningPath `
          -FrontendDiagnosticsHardeningPath $frontendDiagnosticsHardeningPath
      }
      "frontend_conformance_matrix" {
        Write-FrontendConformanceMatrixArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $frontendConformanceMatrixPath `
          -FrontendRecoveryDeterminismHardeningPath $frontendRecoveryDeterminismHardeningPath
      }
      "frontend_conformance_corpus" {
        Write-FrontendConformanceCorpusArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $frontendConformanceCorpusPath `
          -FrontendConformanceMatrixPath $frontendConformanceMatrixPath
      }
      "frontend_integration_closeout" {
        Write-FrontendIntegrationCloseoutArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $frontendIntegrationCloseoutPath `
          -FrontendConformanceCorpusPath $frontendConformanceCorpusPath
      }
      default {
        throw ("unhandled frontend contract artifact definition: " + $definition.Name)
      }
    }
  }

  Write-BuildStep "artifact_generation_done=frontend_contract_artifacts"
}

function Publish-ArtifactWithRetry {
  param(
    [Parameter(Mandatory = $true)]
    [string]$StagedPath,
    [Parameter(Mandatory = $true)]
    [string]$FinalPath,
    [int]$MaxAttempts = 40,
    [int]$SleepMilliseconds = 250
  )

  for ($attempt = 1; $attempt -le $MaxAttempts; $attempt++) {
    try {
      Move-Item -LiteralPath $StagedPath -Destination $FinalPath -Force
      return
    } catch {
      if ($attempt -eq $MaxAttempts) {
        throw "failed to publish $FinalPath after $MaxAttempts attempt(s): $($_.Exception.Message)"
      }
      Start-Sleep -Milliseconds $SleepMilliseconds
    }
  }
}

function New-StagedObjectPath {
  param(
    [Parameter(Mandatory = $true)]
    [string]$ObjectDir,
    [Parameter(Mandatory = $true)]
    [string]$TargetName,
    [Parameter(Mandatory = $true)]
    [int]$Index
  )

  return (Join-Path $ObjectDir ("{0}.{1:D3}.obj" -f $TargetName, $Index))
}

function Compile-ObjectFiles {
  param(
    [Parameter(Mandatory = $true)]
    [string]$TargetName,
    [Parameter(Mandatory = $true)]
    [string[]]$SourcePaths,
    [Parameter(Mandatory = $true)]
    [string]$ObjectDir,
    [Parameter(Mandatory = $true)]
    [string]$Clangxx,
    [Parameter(Mandatory = $true)]
    [string]$IncludeDir,
    [Parameter(Mandatory = $true)]
    [string]$NativeSourceRoot,
    [bool]$EnableLlvmDirectObjectEmission = $true
  )

  New-Item -ItemType Directory -Force -Path $ObjectDir | Out-Null
  $objectPaths = New-Object System.Collections.Generic.List[string]
  for ($index = 0; $index -lt $SourcePaths.Count; $index++) {
    $sourcePath = $SourcePaths[$index]
    $objectPath = New-StagedObjectPath -ObjectDir $ObjectDir -TargetName $TargetName -Index $index
    $relativeSource = Get-Objc3cNativeRepoRelativePath -RootPath $repoRoot -TargetPath $sourcePath
    Write-BuildStep ("compile_unit=" + $TargetName + " [" + ($index + 1) + "/" + $SourcePaths.Count + "] -> " + $relativeSource)
    $compileArgs = @(
      "-std=c++20"
      "-Wall"
      "-Wextra"
      "-pedantic"
    )
    if ($EnableLlvmDirectObjectEmission) {
      $compileArgs += "-DOBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION=1"
    }
    $compileArgs += @(
      "-I$IncludeDir"
      "-I$NativeSourceRoot"
      "-c"
      $sourcePath
      "-o"
      $objectPath
    )
    & $Clangxx @compileArgs
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    $objectPaths.Add($objectPath) | Out-Null
  }

  return $objectPaths.ToArray()
}

function Link-ExecutableFromObjects {
  param(
    [Parameter(Mandatory = $true)]
    [string]$TargetName,
    [Parameter(Mandatory = $true)]
    [string[]]$ObjectPaths,
    [Parameter(Mandatory = $true)]
    [string]$Libclang,
    [Parameter(Mandatory = $true)]
    [string]$Clangxx,
    [Parameter(Mandatory = $true)]
    [string]$StagedOutput,
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot
  )

  Write-BuildStep ("link_start=" + $TargetName + " -> " + (Get-Objc3cNativeRepoRelativePath -RootPath $RepoRoot -TargetPath $StagedOutput))
  & $Clangxx @ObjectPaths $Libclang -o $StagedOutput
  if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
  Write-BuildStep ("link_done=" + $TargetName + " -> " + (Get-Objc3cNativeRepoRelativePath -RootPath $RepoRoot -TargetPath $StagedOutput))
}

function Write-FrontendModuleScaffoldArtifact {
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

function Write-FrontendInvocationLockArtifact {
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

function Write-FrontendCoreFeatureExpansionArtifact {
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

function Write-FrontendEdgeCompatibilityArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendCoreFeatureExpansionPath
  )

  if (!(Test-Path -LiteralPath $FrontendCoreFeatureExpansionPath -PathType Leaf)) {
    throw "frontend core feature expansion missing for edge compatibility artifact: $FrontendCoreFeatureExpansionPath"
  }

  try {
    $coreFeaturePayload = Get-Content -LiteralPath $FrontendCoreFeatureExpansionPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend core feature expansion is not valid JSON for edge compatibility artifact: $FrontendCoreFeatureExpansionPath"
  }

  $expectedCoreFeatureContractId = "objc3c-frontend-build-invocation-core-feature-expansion/parser_build-core-feature-expansion-v1"
  if ([string]$coreFeaturePayload.contract_id -ne $expectedCoreFeatureContractId) {
    throw "frontend core feature contract id mismatch for edge compatibility artifact: $FrontendCoreFeatureExpansionPath"
  }

  $allowedBackends = @()
  foreach ($backend in @($coreFeaturePayload.backend_routing.allowed_ir_object_backends)) {
    $backendText = [string]$backend
    if (![string]::IsNullOrWhiteSpace($backendText)) {
      $allowedBackends += $backendText
    }
  }
  if ($allowedBackends.Count -eq 0) {
    throw "frontend core feature expansion allowed_ir_object_backends missing for edge compatibility artifact: $FrontendCoreFeatureExpansionPath"
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedCoreFeatureContractId
      "objc3c-frontend-build-invocation-manifest-guard/parser_build-manifest-guard-v1"
    )
    backend_compat = [ordered]@{
      canonical_allowed_backends = $allowedBackends
      alias_to_canonical = [ordered]@{
        "clang" = "clang"
        "clang++" = "clang"
        "clang-cl" = "clang"
        "llvm-direct" = "llvm-direct"
        "llvm_direct" = "llvm-direct"
        "llvmdirect" = "llvm-direct"
        "llvm" = "llvm-direct"
      }
      single_value_flags = @(
        "--objc3-ir-object-backend"
        "--llvm-capabilities-summary"
      )
    }
    invocation_edge_compat = [ordered]@{
      supports_equals_form_flags = @(
        "--out-dir"
        "--objc3-ir-object-backend"
        "--llvm-capabilities-summary"
      )
      supports_boolean_equals_flags = @(
        "--use-cache"
        "--objc3-route-backend-from-capabilities"
      )
      route_flag = "--objc3-route-backend-from-capabilities"
      capability_summary_flag = "--llvm-capabilities-summary"
      fail_closed_exit_code = 2
      disallow_relative_parent_segments = $true
    }
  }

  Write-Objc3cNativeJsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
}

function Write-FrontendEdgeRobustnessArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendEdgeCompatibilityPath
  )

  if (!(Test-Path -LiteralPath $FrontendEdgeCompatibilityPath -PathType Leaf)) {
    throw "frontend edge compatibility artifact missing for edge robustness artifact: $FrontendEdgeCompatibilityPath"
  }

  try {
    $edgeCompatPayload = Get-Content -LiteralPath $FrontendEdgeCompatibilityPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend edge compatibility artifact is not valid JSON for edge robustness artifact: $FrontendEdgeCompatibilityPath"
  }

  $expectedEdgeCompatContractId = "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
  if ([string]$edgeCompatPayload.contract_id -ne $expectedEdgeCompatContractId) {
    throw "frontend edge compatibility contract id mismatch for edge robustness artifact: $FrontendEdgeCompatibilityPath"
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-edge-robustness/parser_build-edge-robustness-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedEdgeCompatContractId
      "objc3c-frontend-build-invocation-core-feature-expansion/parser_build-core-feature-expansion-v1"
    )
    wrapper_guardrails = [ordered]@{
      wrapper_single_value_flags = @(
        "--use-cache"
        "--out-dir"
      )
      compile_single_value_flags = @(
        "--objc3-ir-object-backend"
        "--llvm-capabilities-summary"
        "--objc3-route-backend-from-capabilities"
      )
      reject_empty_equals_value_flags = @(
        "--out-dir"
        "--emit-prefix"
        "--clang"
        "--objc3-ir-object-backend"
        "--llvm-capabilities-summary"
        "--objc3-route-backend-from-capabilities"
        "--use-cache"
      )
    }
  }

  Write-Objc3cNativeJsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
}

function Write-FrontendDiagnosticsHardeningArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendEdgeRobustnessPath
  )

  if (!(Test-Path -LiteralPath $FrontendEdgeRobustnessPath -PathType Leaf)) {
    throw "frontend edge robustness artifact missing for diagnostics hardening artifact: $FrontendEdgeRobustnessPath"
  }

  try {
    $edgeRobustnessPayload = Get-Content -LiteralPath $FrontendEdgeRobustnessPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend edge robustness artifact is not valid JSON for diagnostics hardening artifact: $FrontendEdgeRobustnessPath"
  }

  $expectedEdgeRobustnessContractId = "objc3c-frontend-build-invocation-edge-robustness/parser_build-edge-robustness-v1"
  if ([string]$edgeRobustnessPayload.contract_id -ne $expectedEdgeRobustnessContractId) {
    throw "frontend edge robustness contract id mismatch for diagnostics hardening artifact: $FrontendEdgeRobustnessPath"
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-diagnostics-hardening/parser_build-diagnostics-hardening-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedEdgeRobustnessContractId
      "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
    )
    wrapper_diagnostics = [ordered]@{
      fail_closed_exit_code = 2
      required_error_messages = @(
        "--use-cache can be provided at most once"
        "invalid --use-cache value"
        "--out-dir can be provided at most once"
        "missing value for --out-dir"
        "empty value for --out-dir"
        "missing value for --emit-prefix"
        "empty value for --emit-prefix"
        "missing value for --clang"
        "empty value for --clang"
      )
    }
  }

  Write-Objc3cNativeJsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
}

function Write-FrontendRecoveryDeterminismHardeningArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendDiagnosticsHardeningPath
  )

  if (!(Test-Path -LiteralPath $FrontendDiagnosticsHardeningPath -PathType Leaf)) {
    throw "frontend diagnostics hardening artifact missing for recovery determinism hardening artifact: $FrontendDiagnosticsHardeningPath"
  }

  try {
    $diagnosticsPayload = Get-Content -LiteralPath $FrontendDiagnosticsHardeningPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend diagnostics hardening artifact is not valid JSON for recovery determinism hardening artifact: $FrontendDiagnosticsHardeningPath"
  }

  $expectedDiagnosticsContractId = "objc3c-frontend-build-invocation-diagnostics-hardening/parser_build-diagnostics-hardening-v1"
  if ([string]$diagnosticsPayload.contract_id -ne $expectedDiagnosticsContractId) {
    throw "frontend diagnostics hardening contract id mismatch for recovery determinism hardening artifact: $FrontendDiagnosticsHardeningPath"
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-recovery-determinism-hardening/parser_build-recovery-determinism-hardening-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedDiagnosticsContractId
      "objc3c-frontend-build-invocation-edge-robustness/parser_build-edge-robustness-v1"
    )
    cache_determinism = [ordered]@{
      fail_closed_exit_code = 2
      entry_contract_id = "objc3c-native-cache-entry/parser_build-recovery-determinism-hardening-v1"
      cache_status_tokens = @(
        "cache_hit=true"
        "cache_hit=false"
      )
      required_entry_files = @(
        "files"
        "exit_code.txt"
        "ready.marker"
        "metadata.json"
      )
      recovery_signals = @(
        "cache_recovery=metadata_missing"
        "cache_recovery=metadata_invalid"
        "cache_recovery=metadata_contract_mismatch"
        "cache_recovery=metadata_cache_key_mismatch"
        "cache_recovery=metadata_exit_code_mismatch"
        "cache_recovery=metadata_digest_mismatch"
        "cache_recovery=restore_failed"
      )
    }
  }

  Write-Objc3cNativeJsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
}

function Write-FrontendConformanceMatrixArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendRecoveryDeterminismHardeningPath
  )

  if (!(Test-Path -LiteralPath $FrontendRecoveryDeterminismHardeningPath -PathType Leaf)) {
    throw "frontend recovery determinism hardening artifact missing for conformance matrix artifact: $FrontendRecoveryDeterminismHardeningPath"
  }

  try {
    $recoveryPayload = Get-Content -LiteralPath $FrontendRecoveryDeterminismHardeningPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend recovery determinism hardening artifact is not valid JSON for conformance matrix artifact: $FrontendRecoveryDeterminismHardeningPath"
  }

  $expectedRecoveryContractId = "objc3c-frontend-build-invocation-recovery-determinism-hardening/parser_build-recovery-determinism-hardening-v1"
  if ([string]$recoveryPayload.contract_id -ne $expectedRecoveryContractId) {
    throw "frontend recovery determinism hardening contract id mismatch for conformance matrix artifact: $FrontendRecoveryDeterminismHardeningPath"
  }

  $cacheModes = @("no-cache", "cache-aware")
  $backendModes = @("default", "clang", "llvm-direct")
  $summaryModes = @("none", "present")
  $acceptRows = New-Object System.Collections.Generic.List[object]
  $caseOrdinal = 1
  foreach ($cacheMode in $cacheModes) {
    foreach ($backendMode in $backendModes) {
      foreach ($summaryMode in $summaryModes) {
        $profileKey = "{0}|{1}|manual|{2}" -f $cacheMode, $backendMode, $summaryMode
        $acceptRows.Add([ordered]@{
          case_id = ("D009-C{0:D3}" -f $caseOrdinal)
          profile_key = $profileKey
          expected_result = "accept"
          cache_mode = $cacheMode
          backend_mode = $backendMode
          routing_mode = "manual"
          capability_summary_mode = $summaryMode
        })
        $caseOrdinal++
      }

      $profileKey = "{0}|{1}|capability-route|present" -f $cacheMode, $backendMode
      $acceptRows.Add([ordered]@{
        case_id = ("D009-C{0:D3}" -f $caseOrdinal)
        profile_key = $profileKey
        expected_result = "accept"
        cache_mode = $cacheMode
        backend_mode = $backendMode
        routing_mode = "capability-route"
        capability_summary_mode = "present"
      })
      $caseOrdinal++
    }
  }

  $rejectRows = @(
    [ordered]@{
      case_id = "D009-R001"
      profile_key = "any|any|capability-route|none"
      expected_result = "reject"
      required_diagnostic = "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary"
      fail_closed_exit_code = 2
    }
    [ordered]@{
      case_id = "D009-R002"
      profile_key = "any|unsupported-backend|any|any"
      expected_result = "reject"
      required_diagnostic = "unsupported value '<backend>' for --objc3-ir-object-backend"
      fail_closed_exit_code = 2
    }
    [ordered]@{
      case_id = "D009-R003"
      profile_key = "any|any|any|any"
      expected_result = "reject"
      required_diagnostic = "--objc3-ir-object-backend can be provided at most once"
      fail_closed_exit_code = 2
    }
    [ordered]@{
      case_id = "D009-R004"
      profile_key = "any|any|any|path-parent-segment"
      expected_result = "reject"
      required_diagnostic = "--llvm-capabilities-summary must not contain '..' relative segments"
      fail_closed_exit_code = 2
    }
    [ordered]@{
      case_id = "D009-R005"
      profile_key = "any|any|duplicate-route-flag|any"
      expected_result = "reject"
      required_diagnostic = "--objc3-route-backend-from-capabilities can be provided at most once"
      fail_closed_exit_code = 2
    }
  )

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-conformance-matrix/parser_build-conformance-matrix-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedRecoveryContractId
      "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
    )
    profile_key_fields = @(
      "cache_mode"
      "backend_mode"
      "routing_mode"
      "capability_summary_mode"
    )
    matrix_dimensions = [ordered]@{
      cache_modes = $cacheModes
      backend_modes = $backendModes
      routing_modes = @("manual", "capability-route")
      capability_summary_modes = $summaryModes
    }
    acceptance_profile_count = $acceptRows.Count
    rejection_profile_count = $rejectRows.Count
    acceptance_matrix = $acceptRows.ToArray()
    rejection_matrix = $rejectRows
  }

  $parent = Split-Path -Parent $OutputPath
  if (![string]::IsNullOrWhiteSpace($parent)) {
    New-Item -ItemType Directory -Force -Path $parent | Out-Null
  }
  Set-Content -LiteralPath $OutputPath -Value ($payload | ConvertTo-Json -Depth 10) -Encoding utf8
}

function Write-FrontendConformanceCorpusArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendConformanceMatrixPath
  )

  if (!(Test-Path -LiteralPath $FrontendConformanceMatrixPath -PathType Leaf)) {
    throw "frontend conformance matrix artifact missing for conformance corpus artifact: $FrontendConformanceMatrixPath"
  }

  try {
    $matrixPayload = Get-Content -LiteralPath $FrontendConformanceMatrixPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend conformance matrix artifact is not valid JSON for conformance corpus artifact: $FrontendConformanceMatrixPath"
  }

  $expectedMatrixContractId = "objc3c-frontend-build-invocation-conformance-matrix/parser_build-conformance-matrix-v1"
  if ([string]$matrixPayload.contract_id -ne $expectedMatrixContractId) {
    throw "frontend conformance matrix contract id mismatch for conformance corpus artifact: $FrontendConformanceMatrixPath"
  }

  $acceptRows = New-Object System.Collections.Generic.List[object]
  $acceptOrdinal = 1
  foreach ($row in @($matrixPayload.acceptance_matrix)) {
    $profileKey = [string]$row.profile_key
    if ([string]::IsNullOrWhiteSpace($profileKey)) {
      continue
    }
    $segments = $profileKey.Split("|")
    if ($segments.Length -ne 4) {
      continue
    }
    $cacheMode = [string]$segments[0]
    $backendMode = [string]$segments[1]
    $routingMode = [string]$segments[2]
    $summaryMode = [string]$segments[3]
    $compileArgs = New-Object System.Collections.Generic.List[string]
    $compileArgs.Add("tests/tooling/fixtures/parser_conformance_corpus/accept_void_pointer_param.objc3")
    $compileArgs.Add("--out-dir=tmp/reports/parser_build/backend_route_capability_smoke/out")
    if ($backendMode -ne "default") {
      $compileArgs.Add("--objc3-ir-object-backend=$backendMode")
    }
    if ($summaryMode -eq "present") {
      $compileArgs.Add("--llvm-capabilities-summary=tmp/artifacts/objc3c-native/llvm_capabilities_summary.json")
    }
    if ($routingMode -eq "capability-route") {
      $compileArgs.Add("--objc3-route-backend-from-capabilities")
    }
    $acceptRows.Add([ordered]@{
      corpus_case_id = ("D010-C{0:D3}" -f $acceptOrdinal)
      profile_key = $profileKey
      expected_result = "accept"
      use_cache = ($cacheMode -eq "cache-aware")
      expected_exit_code = 0
      compile_args = $compileArgs.ToArray()
    })
    $acceptOrdinal++
  }

  $rejectRows = @(
    [ordered]@{
      corpus_case_id = "D010-R001"
      matrix_case_id = "D009-R001"
      expected_result = "reject"
      expected_exit_code = 2
      expected_diagnostic = "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary"
      compile_args = @(
        "tests/tooling/fixtures/parser_conformance_corpus/accept_void_pointer_param.objc3"
        "--out-dir=tmp/reports/parser_build/backend_route_capability_smoke/out"
        "--objc3-route-backend-from-capabilities"
      )
    }
    [ordered]@{
      corpus_case_id = "D010-R002"
      matrix_case_id = "D009-R002"
      expected_result = "reject"
      expected_exit_code = 2
      expected_diagnostic = "unsupported value '<backend>' for --objc3-ir-object-backend"
      compile_args = @(
        "tests/tooling/fixtures/parser_conformance_corpus/accept_void_pointer_param.objc3"
        "--out-dir=tmp/reports/parser_build/backend_route_capability_smoke/out"
        "--objc3-ir-object-backend=unsupported-backend"
      )
    }
    [ordered]@{
      corpus_case_id = "D010-R003"
      matrix_case_id = "D009-R003"
      expected_result = "reject"
      expected_exit_code = 2
      expected_diagnostic = "--objc3-ir-object-backend can be provided at most once"
      compile_args = @(
        "tests/tooling/fixtures/parser_conformance_corpus/accept_void_pointer_param.objc3"
        "--out-dir=tmp/reports/parser_build/backend_route_capability_smoke/out"
        "--objc3-ir-object-backend=clang"
        "--objc3-ir-object-backend=llvm-direct"
      )
    }
    [ordered]@{
      corpus_case_id = "D010-R004"
      matrix_case_id = "D009-R004"
      expected_result = "reject"
      expected_exit_code = 2
      expected_diagnostic = "--llvm-capabilities-summary must not contain '..' relative segments"
      compile_args = @(
        "tests/tooling/fixtures/parser_conformance_corpus/accept_void_pointer_param.objc3"
        "--out-dir=tmp/reports/parser_build/backend_route_capability_smoke/out"
        "--llvm-capabilities-summary=../outside/capabilities.json"
      )
    }
    [ordered]@{
      corpus_case_id = "D010-R005"
      matrix_case_id = "D009-R005"
      expected_result = "reject"
      expected_exit_code = 2
      expected_diagnostic = "--objc3-route-backend-from-capabilities can be provided at most once"
      compile_args = @(
        "tests/tooling/fixtures/parser_conformance_corpus/accept_void_pointer_param.objc3"
        "--out-dir=tmp/reports/parser_build/backend_route_capability_smoke/out"
        "--llvm-capabilities-summary=tmp/artifacts/objc3c-native/llvm_capabilities_summary.json"
        "--objc3-route-backend-from-capabilities"
        "--objc3-route-backend-from-capabilities"
      )
    }
  )

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-conformance-corpus/parser_build-conformance-corpus-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedMatrixContractId
      "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
    )
    profile_key_fields = @(
      "cache_mode"
      "backend_mode"
      "routing_mode"
      "capability_summary_mode"
    )
    acceptance_corpus_count = $acceptRows.Count
    rejection_corpus_count = $rejectRows.Count
    corpus_case_count = $acceptRows.Count + $rejectRows.Count
    acceptance_corpus = $acceptRows.ToArray()
    rejection_corpus = $rejectRows
  }

  $parent = Split-Path -Parent $OutputPath
  if (![string]::IsNullOrWhiteSpace($parent)) {
    New-Item -ItemType Directory -Force -Path $parent | Out-Null
  }
  Set-Content -LiteralPath $OutputPath -Value ($payload | ConvertTo-Json -Depth 12) -Encoding utf8
}

function Write-FrontendIntegrationCloseoutArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendConformanceCorpusPath
  )

  if (!(Test-Path -LiteralPath $FrontendConformanceCorpusPath -PathType Leaf)) {
    throw "frontend conformance corpus artifact missing for integration closeout artifact: $FrontendConformanceCorpusPath"
  }

  try {
    $corpusPayload = Get-Content -LiteralPath $FrontendConformanceCorpusPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend conformance corpus artifact is not valid JSON for integration closeout artifact: $FrontendConformanceCorpusPath"
  }

  $expectedCorpusContractId = "objc3c-frontend-build-invocation-conformance-corpus/parser_build-conformance-corpus-v1"
  if ([string]$corpusPayload.contract_id -ne $expectedCorpusContractId) {
    throw "frontend conformance corpus contract id mismatch for integration closeout artifact: $FrontendConformanceCorpusPath"
  }

  $acceptanceCount = [int]$corpusPayload.acceptance_corpus_count
  $rejectionCount = [int]$corpusPayload.rejection_corpus_count
  if ($acceptanceCount -le 0 -or $rejectionCount -le 0) {
    throw "frontend conformance corpus must provide non-empty acceptance and rejection coverage for integration closeout"
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-integration-closeout/parser_build-integration-closeout-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedCorpusContractId
      "objc3c-frontend-build-invocation-conformance-matrix/parser_build-conformance-matrix-v1"
      "objc3c-frontend-build-invocation-recovery-determinism-hardening/parser_build-recovery-determinism-hardening-v1"
    )
    closeout_gate = [ordered]@{
      build_integration_gate_signoff = $true
      invocation_profile_gate_signoff = $true
      corpus_coverage_gate_signoff = $true
      deterministic_fail_closed_exit_code = 2
      acceptance_corpus_count = $acceptanceCount
      rejection_corpus_count = $rejectionCount
    }
  }

  $parent = Split-Path -Parent $OutputPath
  if (![string]::IsNullOrWhiteSpace($parent)) {
    New-Item -ItemType Directory -Force -Path $parent | Out-Null
  }
  Set-Content -LiteralPath $OutputPath -Value ($payload | ConvertTo-Json -Depth 12) -Encoding utf8
}

$frontendModules = @(Get-Objc3cNativeFrontendModules)
$sharedSources = @(Get-Objc3cNativeFrontendSharedSources -Modules $frontendModules)
$runtimeLibrarySourcePath = Join-Path $repoRoot "native/objc3c/src/runtime/objc3_runtime.cpp"
$runtimeLibraryHeaderPath = Join-Path $repoRoot "native/objc3c/src/runtime/public/objc3_runtime_api.h"
$frontendArtifactPaths = Get-Objc3cNativeFrontendArtifactPaths -RepoRoot $repoRoot
$frontendScaffoldPath = $frontendArtifactPaths.SourceGraph
$frontendInvocationLockPath = $frontendArtifactPaths.InvocationLock
$frontendCoreFeatureExpansionPath = $frontendArtifactPaths.CoreFeatureExpansion
$frontendEdgeCompatPath = $frontendArtifactPaths.EdgeCompatibility
$frontendEdgeRobustnessPath = $frontendArtifactPaths.EdgeRobustness
$frontendDiagnosticsHardeningPath = $frontendArtifactPaths.DiagnosticsHardening
$frontendRecoveryDeterminismHardeningPath = $frontendArtifactPaths.RecoveryDeterminismHardening
$frontendConformanceMatrixPath = $frontendArtifactPaths.ConformanceMatrix
$frontendConformanceCorpusPath = $frontendArtifactPaths.ConformanceCorpus
$frontendIntegrationCloseoutPath = $frontendArtifactPaths.IntegrationCloseout
$repoSupercleanSurfacePath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/repo_superclean_source_of_truth.json"

$nativeSources = @(
  "native/objc3c/src/main.cpp"
) + $sharedSources
$nativeSourcePaths = @($nativeSources | ForEach-Object { Join-Path $repoRoot $_ })

$capiRunnerSources = @(
  "native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp"
) + $sharedSources
$capiRunnerSourcePaths = @($capiRunnerSources | ForEach-Object { Join-Path $repoRoot $_ })

foreach ($sourcePath in @($nativeSourcePaths + $capiRunnerSourcePaths)) {
  if (!(Test-Path -LiteralPath $sourcePath -PathType Leaf)) {
    throw "native source file missing: $sourcePath"
  }
}
foreach ($runtimePath in @($runtimeLibrarySourcePath, $runtimeLibraryHeaderPath)) {
  if (!(Test-Path -LiteralPath $runtimePath -PathType Leaf)) {
    throw "runtime library file missing: $runtimePath"
  }
}

Write-BuildStep ("repo_root=" + $repoRoot)
Write-BuildStep ("llvm_root=" + $llvmRoot)
Write-BuildStep ("clangxx=" + $clangxx)
Write-BuildStep ("cmake=" + $cmakeTool)
Write-BuildStep ("ninja=" + $ninjaTool)
Write-BuildStep ("llvm_lib=" + $llvmLibTool)
Write-BuildStep ("native_sources=" + $nativeSourcePaths.Count + "; capi_sources=" + $capiRunnerSourcePaths.Count)
Write-BuildStep ("execution_mode=" + $ExecutionMode)

$frontendPacketDefinitions = @(Get-Objc3cNativeFrontendPacketDefinitions -ArtifactPaths $frontendArtifactPaths)

$buildFingerprint = Get-Objc3cNativeBuildFingerprint `
  -Clangxx $clangxx `
  -CmakeTool $cmakeTool `
  -NinjaTool $ninjaTool `
  -LlvmRoot $llvmRoot `
  -IncludeDir $includeDir `
  -Libclang $libclang `
  -BuildDir $tmpOutDir `
  -RuntimeOutputDir $outDir `
  -LibraryOutputDir $outLibDir `
  -SourceDir $cmakeSourceDir

if (Test-ExecutionModeRunsNativeBuild -Mode $ExecutionMode) {
  Invoke-Objc3cNativeCMakeConfigure `
    -CmakeTool $cmakeTool `
    -NinjaTool $ninjaTool `
    -SourceDir $cmakeSourceDir `
    -BuildDir $tmpOutDir `
    -Clangxx $clangxx `
    -LlvmRoot $llvmRoot `
    -IncludeDir $includeDir `
    -Libclang $libclang `
    -RuntimeOutputDir $outDir `
    -LibraryOutputDir $outLibDir `
    -FingerprintPath $buildFingerprintPath `
    -Fingerprint $buildFingerprint `
    -ForceReconfigure $ForceReconfigure

  Invoke-Objc3cNativeCMakeBuild `
    -CmakeTool $cmakeTool `
    -BuildDir $tmpOutDir

  if (!(Test-Path -LiteralPath $outExe -PathType Leaf)) { throw "native binary missing after CMake/Ninja build: $outExe" }
  if (!(Test-Path -LiteralPath $outCapiExe -PathType Leaf)) { throw "c-api runner missing after CMake/Ninja build: $outCapiExe" }
  if (!(Test-Path -LiteralPath $outRuntimeLib -PathType Leaf)) { throw "runtime library missing after CMake/Ninja build: $outRuntimeLib" }
  if (!(Test-Path -LiteralPath $compileCommandsPath -PathType Leaf)) { throw "compile_commands.json missing after CMake/Ninja configure: $compileCommandsPath" }

  Write-BuildStep ("artifact_ready=objc3c-native -> " + (Get-Objc3cNativeRepoRelativePath -RootPath $repoRoot -TargetPath $outExe))
  Write-BuildStep ("artifact_ready=objc3c-frontend-c-api-runner -> " + (Get-Objc3cNativeRepoRelativePath -RootPath $repoRoot -TargetPath $outCapiExe))
  Write-BuildStep ("artifact_ready=objc3_runtime -> " + (Get-Objc3cNativeRepoRelativePath -RootPath $repoRoot -TargetPath $outRuntimeLib))
  Write-BuildStep ("compile_commands=" + (Get-Objc3cNativeRepoRelativePath -RootPath $repoRoot -TargetPath $compileCommandsPath))
} else {
  Write-BuildStep "cmake_build_skip=native-binaries"
}

Invoke-FrontendPacketGeneration `
  -Mode $ExecutionMode `
  -PacketDefinitions $frontendPacketDefinitions `
  -RepoRoot $repoRoot `
  -NativeBinaryPath $outExe `
  -CapiBinaryPath $outCapiExe `
  -Modules $frontendModules `
  -SharedSources $sharedSources

Write-Objc3cNativeRepoSupercleanSourceOfTruthArtifact `
  -RepoRoot $repoRoot `
  -OutputPath $repoSupercleanSurfacePath `
  -ExecutionMode $ExecutionMode `
  -CompileCommandsPath $compileCommandsPath `
  -NativeExecutablePath $outExe `
  -FrontendCapiRunnerPath $outCapiExe `
  -RuntimeLibraryPath $outRuntimeLib `
  -FrontendDefinitions $frontendPacketDefinitions

if (Test-Path -LiteralPath $outExe -PathType Leaf) {
  Write-Output ("built=" + (Get-Objc3cNativeRepoRelativePath -RootPath $repoRoot -TargetPath $outExe))
}
if (Test-Path -LiteralPath $outCapiExe -PathType Leaf) {
  Write-Output ("built=" + (Get-Objc3cNativeRepoRelativePath -RootPath $repoRoot -TargetPath $outCapiExe))
}
if (Test-Path -LiteralPath $outRuntimeLib -PathType Leaf) {
  Write-Output ("built=" + (Get-Objc3cNativeRepoRelativePath -RootPath $repoRoot -TargetPath $outRuntimeLib))
}
foreach ($packetDefinition in $frontendPacketDefinitions) {
  if (Test-Path -LiteralPath $packetDefinition.OutputPath -PathType Leaf) {
    Write-Output ($packetDefinition.Name + "=" + (Get-Objc3cNativeRepoRelativePath -RootPath $repoRoot -TargetPath $packetDefinition.OutputPath))
  }
}
Write-Output ("repo_superclean_surface=" + (Get-Objc3cNativeRepoRelativePath -RootPath $repoRoot -TargetPath $repoSupercleanSurfacePath))
