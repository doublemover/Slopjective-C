param(
  [string]$PackageRoot = "",
  [string]$ManifestRelativePath = "artifacts/package/objc3c-runnable-toolchain-package.json"
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"

function Get-RepoRelativePathCompat {
  param(
    [Parameter(Mandatory = $true)][string]$RootPath,
    [Parameter(Mandatory = $true)][string]$TargetPath
  )

  $resolvedRoot = (Resolve-Path -LiteralPath $RootPath).Path
  if (Test-Path -LiteralPath $TargetPath) {
    $resolvedTarget = (Resolve-Path -LiteralPath $TargetPath).Path
  }
  else {
    $resolvedTarget = [System.IO.Path]::GetFullPath($TargetPath)
  }

  if ($resolvedRoot.EndsWith('\\') -or $resolvedRoot.EndsWith('/')) {
    $rootWithSeparator = $resolvedRoot
  }
  else {
    $rootWithSeparator = $resolvedRoot + [System.IO.Path]::DirectorySeparatorChar
  }

  $relativePath = $null
  $getRelativeMethod = [System.IO.Path].GetMethod("GetRelativePath", [Type[]]@([string], [string]))
  if ($null -ne $getRelativeMethod) {
    $relativePath = [System.IO.Path]::GetRelativePath($resolvedRoot, $resolvedTarget)
  }
  else {
    $rootUri = New-Object System.Uri($rootWithSeparator)
    $targetUri = New-Object System.Uri($resolvedTarget)
    $relativeUri = $rootUri.MakeRelativeUri($targetUri)
    $relativePath = [System.Uri]::UnescapeDataString($relativeUri.ToString())
  }

  return $relativePath.Replace('\\', '/')
}

function Resolve-PackageRoot {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [string]$RequestedRoot
  )

  if ([string]::IsNullOrWhiteSpace($RequestedRoot)) {
    $runId = "{0}_{1}" -f (Get-Date -Format "yyyyMMdd_HHmmss_fff"), $PID
    return (Join-Path $RepoRoot (Join-Path "tmp/pkg/objc3c-native-runnable-toolchain" $runId))
  }

  if ([System.IO.Path]::IsPathRooted($RequestedRoot)) {
    return [System.IO.Path]::GetFullPath($RequestedRoot)
  }

  return [System.IO.Path]::GetFullPath((Join-Path $RepoRoot $RequestedRoot))
}

function Assert-RepoFile {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RelativePath
  )

  $fullPath = Join-Path $RepoRoot ($RelativePath.Replace('/', '\\'))
  if (!(Test-Path -LiteralPath $fullPath -PathType Leaf)) {
    throw "runnable toolchain package FAIL: missing required file $RelativePath"
  }

  return $fullPath
}

function Copy-RepoRelativeFile {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$RelativePath
  )

  $sourcePath = Assert-RepoFile -RepoRoot $RepoRoot -RelativePath $RelativePath
  $destinationPath = Join-Path $PackageRoot ($RelativePath.Replace('/', '\\'))
  $destinationDir = Split-Path -Parent $destinationPath
  New-Item -ItemType Directory -Force -Path $destinationDir | Out-Null
  Copy-Item -LiteralPath $sourcePath -Destination $destinationPath -Force
  return $destinationPath
}

function Get-RepoRelativeExecutionFixtureFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $fixtureRoot = Join-Path $RepoRoot "tests/tooling/fixtures/native/execution"
  if (!(Test-Path -LiteralPath $fixtureRoot -PathType Container)) {
    throw "runnable toolchain package FAIL: missing execution fixture root $fixtureRoot"
  }

  return @(
    Get-ChildItem -LiteralPath $fixtureRoot -Recurse -File |
      Sort-Object -Property FullName |
      ForEach-Object { Get-RepoRelativePathCompat -RootPath $RepoRoot -TargetPath $_.FullName }
  )
}

function Get-RepoRelativeStdlibFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $stdlibRoot = Join-Path $RepoRoot "stdlib"
  if (!(Test-Path -LiteralPath $stdlibRoot -PathType Container)) {
    throw "runnable toolchain package FAIL: missing stdlib root $stdlibRoot"
  }

  return @(
    Get-ChildItem -LiteralPath $stdlibRoot -Recurse -File |
      Sort-Object -Property FullName |
      ForEach-Object { Get-RepoRelativePathCompat -RootPath $RepoRoot -TargetPath $_.FullName }
  )
}

function Get-RepoRelativeConformanceFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $conformanceRoot = Join-Path $RepoRoot "tests/conformance"
  if (!(Test-Path -LiteralPath $conformanceRoot -PathType Container)) {
    throw "runnable toolchain package FAIL: missing conformance root $conformanceRoot"
  }

  return @(
    Get-ChildItem -LiteralPath $conformanceRoot -Recurse -File |
      Sort-Object -Property FullName |
      ForEach-Object { Get-RepoRelativePathCompat -RootPath $RepoRoot -TargetPath $_.FullName }
  )
}

function Get-RepoRelativeNativeDocsFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $docsRoot = Join-Path $RepoRoot "docs/objc3c-native"
  if (!(Test-Path -LiteralPath $docsRoot -PathType Container)) {
    throw "runnable toolchain package FAIL: missing native docs root $docsRoot"
  }

  return @(
    Get-ChildItem -LiteralPath $docsRoot -Recurse -File |
      Sort-Object -Property FullName |
      ForEach-Object { Get-RepoRelativePathCompat -RootPath $RepoRoot -TargetPath $_.FullName }
  )
}

function Get-RepoRelativeRecoveryPositiveFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $recoveryRoot = Join-Path $RepoRoot "tests/tooling/fixtures/native/recovery/positive"
  if (!(Test-Path -LiteralPath $recoveryRoot -PathType Container)) {
    throw "runnable toolchain package FAIL: missing recovery-positive root $recoveryRoot"
  }

  return @(
    Get-ChildItem -LiteralPath $recoveryRoot -Recurse -File |
      Sort-Object -Property FullName |
      ForEach-Object { Get-RepoRelativePathCompat -RootPath $RepoRoot -TargetPath $_.FullName }
  )
}

if (!(Test-Path -LiteralPath $buildScript -PathType Leaf)) {
  throw "runnable toolchain package FAIL: missing build script $buildScript"
}

$packageRoot = Resolve-PackageRoot -RepoRoot $repoRoot -RequestedRoot $PackageRoot
$manifestPath = Join-Path $packageRoot ($ManifestRelativePath.Replace('/', '\\'))
$manifestDir = Split-Path -Parent $manifestPath
New-Item -ItemType Directory -Force -Path $manifestDir | Out-Null

& $buildScript
if ($LASTEXITCODE -ne 0) {
  exit $LASTEXITCODE
}

$requiredRelativeFiles = @(
  "package.json",
  "artifacts/bin/objc3c-native.exe",
  "artifacts/bin/objc3c-frontend-c-api-runner.exe",
  "artifacts/lib/objc3_runtime.lib",
  "scripts/build_objc3c_native.ps1",
  "scripts/objc3c_native_compile.ps1",
  "scripts/objc3c_public_workflow_runner.py",
  "scripts/build_objc3c_editor_tooling_surface.py",
  "scripts/format_objc3c_source.py",
  "scripts/check_developer_tooling_language_server_navigation.py",
  "scripts/check_developer_tooling_formatter_debug_surface.py",
  "scripts/check_developer_tooling_workspace_integration.py",
  "scripts/check_objc3c_developer_tooling_integration.py",
  "scripts/check_objc3c_runnable_developer_tooling_end_to_end.py",
  "scripts/benchmark_objc3c_runtime_inspector.py",
  "scripts/objc3c_runtime_launch_contract.ps1",
  "scripts/run_objc3c_native_compile_proof.ps1",
  "scripts/check_objc3c_native_execution_smoke.ps1",
  "scripts/check_objc3c_execution_replay_proof.ps1",
  "scripts/build_objc3c_native_docs.py",
  "scripts/render_objc3c_public_command_surface.py",
  "showcase/README.md",
  "showcase/portfolio.json",
  "showcase/tutorial_walkthrough.json",
  "showcase/auroraBoard/main.objc3",
  "showcase/auroraBoard/workspace.json",
  "showcase/signalMesh/main.objc3",
  "showcase/signalMesh/workspace.json",
  "showcase/patchKit/main.objc3",
  "showcase/patchKit/workspace.json",
  "docs/runbooks/objc3c_conformance_corpus.md",
  "docs/runbooks/objc3c_compiler_throughput.md",
  "docs/runbooks/objc3c_developer_tooling.md",
  "docs/runbooks/objc3c_public_command_surface.md",
  "docs/runbooks/objc3c_release_foundation.md",
  "docs/runbooks/objc3c_runtime_performance.md",
  "docs/runbooks/objc3c_stdlib_program.md",
  "docs/tutorials/README.md",
  "docs/tutorials/getting_started.md",
  "docs/tutorials/objc2_swift_cpp_comparison.md",
  "docs/tutorials/build_run_verify.md",
  "docs/tutorials/guided_walkthrough.md",
  "site/src/index.body.md",
  "scripts/probe_objc3c_llvm_capabilities.py",
  "scripts/check_objc3c_native_perf_budget.ps1",
  "scripts/benchmark_objc3c_runtime_performance.py",
  "scripts/check_objc3c_runtime_acceptance.py",
  "tmp/artifacts/objc3c-native/frontend_source_graph.json",
  "tmp/artifacts/objc3c-native/frontend_invocation_lock.json",
  "tmp/artifacts/objc3c-native/frontend_core_feature_expansion.json",
  "tmp/artifacts/objc3c-native/frontend_edge_compat.json",
  "tmp/artifacts/objc3c-native/frontend_edge_robustness.json",
  "tmp/artifacts/objc3c-native/frontend_diagnostics_hardening.json",
  "tmp/artifacts/objc3c-native/frontend_recovery_determinism_hardening.json",
  "tmp/artifacts/objc3c-native/frontend_conformance_matrix.json",
  "tmp/artifacts/objc3c-native/frontend_conformance_corpus.json",
  "tmp/artifacts/objc3c-native/frontend_integration_closeout.json",
  "tmp/artifacts/objc3c-native/repo_superclean_source_of_truth.json",
  "native/objc3c/src/runtime/objc3_runtime.h",
  "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
  "schemas/objc3c-developer-tooling-editor-surface-v1.schema.json",
  "schemas/objc3-conformance-dashboard-status-v1.schema.json",
  "schemas/objc3c-release-manifest-v1.schema.json",
  "schemas/objc3c-release-sbom-v1.schema.json",
  "schemas/objc3c-release-attestation-v1.schema.json",
  "scripts/check_release_evidence.py",
  "scripts/check_release_foundation_source_surface.py",
  "scripts/check_release_foundation_schema_surface.py",
  "scripts/build_objc3c_release_manifest.py",
  "scripts/publish_objc3c_release_provenance.py",
  "scripts/check_objc3c_release_foundation_integration.py",
  "scripts/check_conformance_suite.ps1",
  "scripts/check_conformance_corpus_surface.py",
  "scripts/generate_conformance_corpus_index.py",
  "spec/conformance/release_evidence_gate_maintenance.md",
  "tests/tooling/runtime/object_model_lookup_reflection_runtime_probe.cpp",
  "tests/tooling/runtime/runtime_installation_loader_lifecycle_probe.cpp",
  "tests/tooling/runtime/live_dispatch_fast_path_probe.cpp",
  "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
  "tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp",
  "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
  "tests/tooling/runtime/block_arc_runtime_abi_probe.cpp",
  "tests/tooling/runtime/live_error_runtime_integration_probe.cpp",
  "tests/tooling/runtime/live_continuation_runtime_integration_probe.cpp",
  "tests/tooling/runtime/live_task_runtime_and_executor_implementation_probe.cpp",
  "tests/tooling/runtime/live_actor_mailbox_runtime_probe.cpp",
  "tests/tooling/runtime/macro_host_process_cache_integration_probe.cpp",
  "tests/tooling/runtime/bridge_packaging_toolchain_probe.cpp",
  "tests/tooling/runtime/header_module_bridge_generation_probe.cpp",
  "tests/tooling/runtime/release_candidate_claim_runtime_probe.cpp",
  "tests/tooling/runtime/release_candidate_evidence_runtime_probe.cpp",
  "tests/tooling/fixtures/developer_tooling/boundary_inventory.json",
  "tests/tooling/fixtures/developer_tooling/language_server_navigation_implementation_contract.json",
  "tests/tooling/fixtures/developer_tooling/formatter_debug_implementation_contract.json",
  "tests/tooling/fixtures/developer_tooling/workspace_editor_debug_integration_contract.json",
  "tests/tooling/fixtures/developer_tooling/packaged_cli_to_editor_contract.json",
  "tests/tooling/fixtures/developer_tooling/messy_hello.objc3",
  "tests/tooling/fixtures/developer_tooling/formatted_hello.objc3",
  "tests/tooling/fixtures/native/hello.objc3",
  "tests/tooling/fixtures/native/negative_undefined_symbol.objc3",
  "tests/tooling/fixtures/release_foundation/artifact_taxonomy.json",
  "tests/tooling/fixtures/release_foundation/distribution_trust_model.json",
  "tests/tooling/fixtures/release_foundation/distribution_audit.json",
  "tests/tooling/fixtures/release_foundation/reproducibility_policy.json",
  "tests/tooling/fixtures/release_foundation/release_payload_policy.json",
  "tests/tooling/fixtures/release_foundation/provenance_policy.json",
  "tests/tooling/fixtures/release_foundation/source_surface.json",
  "tests/tooling/fixtures/release_foundation/schema_surface.json",
  "tests/tooling/fixtures/release_foundation/workflow_surface.json",
  "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
  "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
  "tests/tooling/fixtures/native/live_dispatch_fast_path_positive.objc3",
  "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
  "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
  "tests/tooling/fixtures/native/runtime_metadata_source_records_class_protocol_property_ivar.objc3",
  "tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3",
  "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
  "tests/tooling/fixtures/native/live_error_runtime_integration_positive.objc3",
  "tests/tooling/fixtures/native/live_continuation_runtime_integration_positive.objc3",
  "tests/tooling/fixtures/native/live_task_runtime_and_executor_implementation_positive.objc3",
  "tests/tooling/fixtures/native/actor_lowering_runtime_positive.objc3",
  "tests/tooling/fixtures/native/macro_host_process_provider.objc3",
  "tests/tooling/fixtures/native/macro_host_process_consumer.objc3",
  "tests/tooling/fixtures/native/bridge_packaging_toolchain_provider.objc3",
  "tests/tooling/fixtures/native/bridge_packaging_toolchain_consumer.objc3",
  "tests/tooling/fixtures/native/header_module_bridge_provider.objc3",
  "tests/tooling/fixtures/native/header_module_bridge_consumer.objc3",
  "tests/tooling/fixtures/performance/benchmark_portfolio.json",
  "tests/tooling/fixtures/performance/measurement_policy.json",
  "tests/tooling/fixtures/performance/benchmark_parameters.json",
  "tests/tooling/fixtures/performance/comparative_baseline_manifest.json",
  "tests/tooling/fixtures/performance/baselines/objc2_reference_workload.m",
  "tests/tooling/fixtures/performance/baselines/swift_reference_workload.swift",
  "tests/tooling/fixtures/performance/baselines/cpp_reference_workload.cpp",
  "schemas/objc3c-performance-telemetry-v1.schema.json",
  "tests/tooling/fixtures/compiler_throughput/source_surface.json",
  "tests/tooling/fixtures/compiler_throughput/workload_manifest.json",
  "tests/tooling/fixtures/compiler_throughput/validation_tier_map.json",
  "tests/tooling/fixtures/compiler_throughput/optimization_policy.json",
  "tests/tooling/fixtures/compiler_throughput/artifact_surface.json",
  "schemas/objc3c-compiler-throughput-summary-v1.schema.json",
  "tests/tooling/fixtures/runtime_performance/source_surface.json",
  "tests/tooling/fixtures/runtime_performance/workload_manifest.json",
  "tests/tooling/fixtures/runtime_performance/artifact_surface.json",
  "tests/tooling/fixtures/runtime_performance/optimization_policy.json",
  "tests/tooling/fixtures/runtime_performance/README.md",
  "schemas/objc3c-runtime-performance-telemetry-v1.schema.json"
)

$executionFixtureFiles = @(Get-RepoRelativeExecutionFixtureFiles -RepoRoot $repoRoot)
$nativeDocsFiles = @(Get-RepoRelativeNativeDocsFiles -RepoRoot $repoRoot)
$recoveryPositiveFiles = @(Get-RepoRelativeRecoveryPositiveFiles -RepoRoot $repoRoot)
$stdlibFiles = @(Get-RepoRelativeStdlibFiles -RepoRoot $repoRoot)
$conformanceFiles = @(Get-RepoRelativeConformanceFiles -RepoRoot $repoRoot)
$copiedRelativePaths = New-Object System.Collections.Generic.List[string]
foreach ($relativePath in @($requiredRelativeFiles + $executionFixtureFiles + $nativeDocsFiles + $recoveryPositiveFiles + $stdlibFiles + $conformanceFiles)) {
  Copy-RepoRelativeFile -RepoRoot $repoRoot -PackageRoot $packageRoot -RelativePath $relativePath | Out-Null
  $copiedRelativePaths.Add($relativePath.Replace('\\', '/')) | Out-Null
}

$stagedRelativePaths = @(
  Get-ChildItem -LiteralPath $packageRoot -Recurse -File |
    Where-Object { $_.FullName -ne $manifestPath } |
    Sort-Object -Property FullName |
    ForEach-Object { (Get-RepoRelativePathCompat -RootPath $packageRoot -TargetPath $_.FullName).Replace('\', '/') }
)

$packagedNativeExecutablePath = Join-Path $packageRoot "artifacts\bin\objc3c-native.exe"
$packagedFrontendRunnerPath = Join-Path $packageRoot "artifacts\bin\objc3c-frontend-c-api-runner.exe"
$packagedRuntimeLibraryPath = Join-Path $packageRoot "artifacts\lib\objc3_runtime.lib"
$normalizedOutputTimestamp = [datetime]::UtcNow
foreach ($outputPath in @($packagedNativeExecutablePath, $packagedFrontendRunnerPath, $packagedRuntimeLibraryPath)) {
  if (Test-Path -LiteralPath $outputPath -PathType Leaf) {
    $item = Get-Item -LiteralPath $outputPath
    $item.LastWriteTimeUtc = $normalizedOutputTimestamp
  }
}

$repoSupercleanSurfaceRelativePath = "tmp/artifacts/objc3c-native/repo_superclean_source_of_truth.json"
$repoSupercleanSurfacePath = Join-Path $packageRoot ($repoSupercleanSurfaceRelativePath.Replace('/', '\'))
$repoSupercleanSurfacePayload = Get-Content -LiteralPath $repoSupercleanSurfacePath -Raw | ConvertFrom-Json -AsHashtable
if (-not $repoSupercleanSurfacePayload.ContainsKey("bonus_experience_surfaces")) {
  throw "runnable toolchain package FAIL: missing bonus_experience_surfaces in $repoSupercleanSurfaceRelativePath"
}
if (-not $repoSupercleanSurfacePayload.ContainsKey("performance_benchmark_surface")) {
  throw "runnable toolchain package FAIL: missing performance_benchmark_surface in $repoSupercleanSurfaceRelativePath"
}
if (-not $repoSupercleanSurfacePayload.ContainsKey("runtime_performance_surface")) {
  throw "runnable toolchain package FAIL: missing runtime_performance_surface in $repoSupercleanSurfaceRelativePath"
}
if (-not $repoSupercleanSurfacePayload.ContainsKey("compiler_throughput_surface")) {
  throw "runnable toolchain package FAIL: missing compiler_throughput_surface in $repoSupercleanSurfaceRelativePath"
}
if (-not $repoSupercleanSurfacePayload.ContainsKey("conformance_corpus_surface")) {
  throw "runnable toolchain package FAIL: missing conformance_corpus_surface in $repoSupercleanSurfaceRelativePath"
}
if (-not $repoSupercleanSurfacePayload.ContainsKey("stdlib_foundation_surface")) {
  throw "runnable toolchain package FAIL: missing stdlib_foundation_surface in $repoSupercleanSurfaceRelativePath"
}
if (-not $repoSupercleanSurfacePayload.ContainsKey("stdlib_program_surface")) {
  throw "runnable toolchain package FAIL: missing stdlib_program_surface in $repoSupercleanSurfaceRelativePath"
}

$stdlibLoweringImportSurfaceRelativePath = "stdlib/lowering_import_surface.json"
$stdlibLoweringImportSurfacePath = Join-Path $packageRoot ($stdlibLoweringImportSurfaceRelativePath.Replace('/', '\'))
$stdlibLoweringImportSurfacePayload = Get-Content -LiteralPath $stdlibLoweringImportSurfacePath -Raw | ConvertFrom-Json -AsHashtable
$stdlibAdvancedHelperPackageSurfaceRelativePath = "stdlib/advanced_helper_package_surface.json"
$stdlibAdvancedHelperPackageSurfacePath = Join-Path $packageRoot ($stdlibAdvancedHelperPackageSurfaceRelativePath.Replace('/', '\'))
$stdlibAdvancedHelperPackageSurfacePayload = Get-Content -LiteralPath $stdlibAdvancedHelperPackageSurfacePath -Raw | ConvertFrom-Json -AsHashtable
$stdlibProgramSurfaceRelativePath = "stdlib/program_surface.json"
$stdlibProgramSurfacePath = Join-Path $packageRoot ($stdlibProgramSurfaceRelativePath.Replace('/', '\'))
$stdlibProgramSurfacePayload = Get-Content -LiteralPath $stdlibProgramSurfacePath -Raw | ConvertFrom-Json -AsHashtable

$manifestPayload = [ordered]@{
  contract_id = "objc3c-runnable-build-install-run-package/runnable_suite-packaged-end-to-end-v1"
  schema_version = 1
  package_model = "staged-runnable-toolchain-bundle-with-repo-relative-layout"
  install_model = "local-package-root-not-system-install"
  package_root = Get-RepoRelativePathCompat -RootPath $repoRoot -TargetPath $packageRoot
  manifest_artifact = Get-RepoRelativePathCompat -RootPath $packageRoot -TargetPath $manifestPath
  native_executable = "artifacts/bin/objc3c-native.exe"
  frontend_c_api_runner = "artifacts/bin/objc3c-frontend-c-api-runner.exe"
  runtime_library = "artifacts/lib/objc3_runtime.lib"
  compile_wrapper = "scripts/objc3c_native_compile.ps1"
  runtime_launch_contract_script = "scripts/objc3c_runtime_launch_contract.ps1"
  compile_proof_script = "scripts/run_objc3c_native_compile_proof.ps1"
  execution_smoke_script = "scripts/check_objc3c_native_execution_smoke.ps1"
  execution_replay_script = "scripts/check_objc3c_execution_replay_proof.ps1"
  showcase_root = "showcase"
  showcase_readme = "showcase/README.md"
  showcase_portfolio = "showcase/portfolio.json"
  showcase_examples = @(
    [ordered]@{
      example_id = "auroraBoard"
      source = "showcase/auroraBoard/main.objc3"
      workspace_manifest = "showcase/auroraBoard/workspace.json"
      expected_exit_code = 33
    },
    [ordered]@{
      example_id = "signalMesh"
      source = "showcase/signalMesh/main.objc3"
      workspace_manifest = "showcase/signalMesh/workspace.json"
      expected_exit_code = 13
    },
    [ordered]@{
      example_id = "patchKit"
      source = "showcase/patchKit/main.objc3"
      workspace_manifest = "showcase/patchKit/workspace.json"
      expected_exit_code = 7
    }
  )
  canonical_runnable_fixture = "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3"
  runtime_public_header = "native/objc3c/src/runtime/objc3_runtime.h"
  runtime_internal_header = "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h"
  release_evidence_dashboard_schema = "schemas/objc3-conformance-dashboard-status-v1.schema.json"
  release_evidence_gate_script = "scripts/check_release_evidence.py"
  release_evidence_runbook = "spec/conformance/release_evidence_gate_maintenance.md"
  release_candidate_fixture = "tests/tooling/fixtures/native/hello.objc3"
  release_candidate_claim_probe = "tests/tooling/runtime/release_candidate_claim_runtime_probe.cpp"
  release_candidate_evidence_probe = "tests/tooling/runtime/release_candidate_evidence_runtime_probe.cpp"
  developer_tooling_runbook = "docs/runbooks/objc3c_developer_tooling.md"
  developer_tooling_boundary_inventory = "tests/tooling/fixtures/developer_tooling/boundary_inventory.json"
  developer_tooling_editor_surface_schema = "schemas/objc3c-developer-tooling-editor-surface-v1.schema.json"
  developer_tooling_navigation_contract = "tests/tooling/fixtures/developer_tooling/language_server_navigation_implementation_contract.json"
  developer_tooling_formatter_debug_contract = "tests/tooling/fixtures/developer_tooling/formatter_debug_implementation_contract.json"
  developer_tooling_workspace_contract = "tests/tooling/fixtures/developer_tooling/workspace_editor_debug_integration_contract.json"
  developer_tooling_packaged_contract = "tests/tooling/fixtures/developer_tooling/packaged_cli_to_editor_contract.json"
  developer_tooling_example_source = "tests/tooling/fixtures/native/hello.objc3"
  developer_tooling_negative_source = "tests/tooling/fixtures/native/negative_undefined_symbol.objc3"
  developer_tooling_formatter_source = "tests/tooling/fixtures/developer_tooling/messy_hello.objc3"
  developer_tooling_expected_formatted_source = "tests/tooling/fixtures/developer_tooling/formatted_hello.objc3"
  developer_tooling_scripts = [ordered]@{
    editor_surface = "scripts/build_objc3c_editor_tooling_surface.py"
    formatter = "scripts/format_objc3c_source.py"
    language_server_navigation_validation = "scripts/check_developer_tooling_language_server_navigation.py"
    formatter_debug_validation = "scripts/check_developer_tooling_formatter_debug_surface.py"
    workspace_validation = "scripts/check_developer_tooling_workspace_integration.py"
    integration_validation = "scripts/check_objc3c_developer_tooling_integration.py"
    runnable_end_to_end_validation = "scripts/check_objc3c_runnable_developer_tooling_end_to_end.py"
  }
  developer_tooling_public_actions = @(
    "inspect-editor-tooling",
    "format-objc3c",
    "materialize-playground-workspace",
    "validate-developer-tooling",
    "validate-runnable-developer-tooling"
  )
  developer_tooling_public_scripts = @(
    "inspect:objc3c:editor",
    "format:objc3c",
    "build:objc3c:playground",
    "test:objc3c:developer-tooling",
    "test:objc3c:runnable-developer-tooling"
  )
  object_model_probe = "tests/tooling/runtime/object_model_lookup_reflection_runtime_probe.cpp"
  block_arc_fixture = "tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3"
  block_arc_runtime_abi_probe = "tests/tooling/runtime/block_arc_runtime_abi_probe.cpp"
  block_arc_byref_forwarding_probe = "tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp"
  storage_reflection_fixture = "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3"
  storage_reflection_probe = "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp"
  error_runtime_fixture = "tests/tooling/fixtures/native/live_error_runtime_integration_positive.objc3"
  error_runtime_probe = "tests/tooling/runtime/live_error_runtime_integration_probe.cpp"
  continuation_runtime_fixture = "tests/tooling/fixtures/native/live_continuation_runtime_integration_positive.objc3"
  continuation_runtime_probe = "tests/tooling/runtime/live_continuation_runtime_integration_probe.cpp"
  task_runtime_fixture = "tests/tooling/fixtures/native/live_task_runtime_and_executor_implementation_positive.objc3"
  task_runtime_probe = "tests/tooling/runtime/live_task_runtime_and_executor_implementation_probe.cpp"
  actor_runtime_fixture = "tests/tooling/fixtures/native/actor_lowering_runtime_positive.objc3"
  actor_runtime_probe = "tests/tooling/runtime/live_actor_mailbox_runtime_probe.cpp"
  metaprogramming_runtime_fixture = "tests/tooling/fixtures/native/macro_host_process_provider.objc3"
  metaprogramming_runtime_consumer_fixture = "tests/tooling/fixtures/native/macro_host_process_consumer.objc3"
  metaprogramming_runtime_probe = "tests/tooling/runtime/macro_host_process_cache_integration_probe.cpp"
  interop_runtime_fixture = "tests/tooling/fixtures/native/bridge_packaging_toolchain_provider.objc3"
  interop_runtime_consumer_fixture = "tests/tooling/fixtures/native/bridge_packaging_toolchain_consumer.objc3"
  interop_header_bridge_fixture = "tests/tooling/fixtures/native/header_module_bridge_provider.objc3"
  interop_header_bridge_consumer_fixture = "tests/tooling/fixtures/native/header_module_bridge_consumer.objc3"
  interop_packaging_probe = "tests/tooling/runtime/bridge_packaging_toolchain_probe.cpp"
  interop_bridge_generation_probe = "tests/tooling/runtime/header_module_bridge_generation_probe.cpp"
  execution_fixture_root = "tests/tooling/fixtures/native/execution"
  frontend_contract_artifacts = @(
    "tmp/artifacts/objc3c-native/frontend_source_graph.json",
    "tmp/artifacts/objc3c-native/frontend_invocation_lock.json",
    "tmp/artifacts/objc3c-native/frontend_core_feature_expansion.json",
    "tmp/artifacts/objc3c-native/frontend_edge_compat.json",
    "tmp/artifacts/objc3c-native/frontend_edge_robustness.json",
    "tmp/artifacts/objc3c-native/frontend_diagnostics_hardening.json",
    "tmp/artifacts/objc3c-native/frontend_recovery_determinism_hardening.json",
    "tmp/artifacts/objc3c-native/frontend_conformance_matrix.json",
    "tmp/artifacts/objc3c-native/frontend_conformance_corpus.json",
    "tmp/artifacts/objc3c-native/frontend_integration_closeout.json"
  )
  repo_superclean_surface = $repoSupercleanSurfaceRelativePath
  bonus_experience_surfaces = $repoSupercleanSurfacePayload["bonus_experience_surfaces"]
  bonus_tool_integration_surface = $repoSupercleanSurfacePayload["bonus_tool_integration_surface"]
  performance_benchmark_surface = $repoSupercleanSurfacePayload["performance_benchmark_surface"]
  release_foundation_surface = $repoSupercleanSurfacePayload["release_foundation_surface"]
  runtime_performance_surface = $repoSupercleanSurfacePayload["runtime_performance_surface"]
  compiler_throughput_surface = $repoSupercleanSurfacePayload["compiler_throughput_surface"]
  conformance_corpus_surface = $repoSupercleanSurfacePayload["conformance_corpus_surface"]
  conformance_suite_readme = "tests/conformance/README.md"
  conformance_coverage_map = "tests/conformance/COVERAGE_MAP.md"
  conformance_runbook = "docs/runbooks/objc3c_conformance_corpus.md"
  conformance_surface_check_script = "scripts/check_conformance_corpus_surface.py"
  conformance_coverage_index_script = "scripts/generate_conformance_corpus_index.py"
  conformance_legacy_suite_gate_script = "scripts/check_conformance_suite.ps1"
  conformance_longitudinal_manifest = "tests/conformance/longitudinal_suites.json"
  stdlib_foundation_surface = $repoSupercleanSurfacePayload["stdlib_foundation_surface"]
  stdlib_program_surface = $repoSupercleanSurfacePayload["stdlib_program_surface"]
  guided_walkthrough_manifest = "showcase/tutorial_walkthrough.json"
  stdlib_root = "stdlib"
  stdlib_program_contract = $stdlibProgramSurfaceRelativePath
  stdlib_program_runbook = "docs/runbooks/objc3c_stdlib_program.md"
  stdlib_program_site_entry = "site/src/index.body.md"
  stdlib_program_command_surfaces = $stdlibProgramSurfacePayload["command_surfaces"]
  stdlib_program_publish_inputs = $stdlibProgramSurfacePayload["publish_inputs"]
  stdlib_program_examples = $stdlibProgramSurfacePayload["capability_demo_examples"]
  stdlib_workspace_manifest = "stdlib/workspace.json"
  stdlib_module_inventory = "stdlib/module_inventory.json"
  stdlib_stability_policy = "stdlib/stability_policy.json"
  stdlib_package_surface = "stdlib/package_surface.json"
  stdlib_advanced_architecture = "stdlib/advanced_architecture.json"
  stdlib_lowering_import_surface = $stdlibLoweringImportSurfaceRelativePath
  stdlib_advanced_helper_package_surface = $stdlibAdvancedHelperPackageSurfaceRelativePath
  stdlib_lowering_artifact_filenames = $stdlibLoweringImportSurfacePayload["artifact_filenames"]
  stdlib_import_surface = $stdlibLoweringImportSurfacePayload["import_surface"]
  advanced_helper_modules = $stdlibAdvancedHelperPackageSurfacePayload["advanced_helper_modules"]
  advanced_helper_command_surfaces = $stdlibAdvancedHelperPackageSurfacePayload["advanced_helper_command_surfaces"]
  advanced_helper_profile_gates = $stdlibAdvancedHelperPackageSurfacePayload["advanced_helper_profile_gates"]
  stdlib_modules = @(
    [ordered]@{
      canonical_module = "objc3.core"
      implementation_module = "objc3_core"
      manifest = "stdlib/modules/objc3.core/module.json"
      source = "stdlib/modules/objc3.core/module.objc3"
      smoke_source = "stdlib/modules/objc3.core/smoke.objc3"
    },
    [ordered]@{
      canonical_module = "objc3.errors"
      implementation_module = "objc3_errors"
      manifest = "stdlib/modules/objc3.errors/module.json"
      source = "stdlib/modules/objc3.errors/module.objc3"
      smoke_source = "stdlib/modules/objc3.errors/smoke.objc3"
    },
    [ordered]@{
      canonical_module = "objc3.concurrency"
      implementation_module = "objc3_concurrency"
      manifest = "stdlib/modules/objc3.concurrency/module.json"
      source = "stdlib/modules/objc3.concurrency/module.objc3"
      smoke_source = "stdlib/modules/objc3.concurrency/smoke.objc3"
    },
    [ordered]@{
      canonical_module = "objc3.keypath"
      implementation_module = "objc3_keypath"
      manifest = "stdlib/modules/objc3.keypath/module.json"
      source = "stdlib/modules/objc3.keypath/module.objc3"
      smoke_source = "stdlib/modules/objc3.keypath/smoke.objc3"
    },
    [ordered]@{
      canonical_module = "objc3.system"
      implementation_module = "objc3_system"
      manifest = "stdlib/modules/objc3.system/module.json"
      source = "stdlib/modules/objc3.system/module.objc3"
      smoke_source = "stdlib/modules/objc3.system/smoke.objc3"
    }
  )
  tutorial_guides = @(
    "docs/tutorials/getting_started.md",
    "docs/tutorials/build_run_verify.md",
    "docs/tutorials/guided_walkthrough.md"
  )
  capability_probe_script = "scripts/probe_objc3c_llvm_capabilities.py"
  command_surfaces = [ordered]@{
    build = "npm run build:objc3c-native"
    package = "npm run package:objc3c-native:runnable-toolchain"
    compile = "pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 <input.objc3> --out-dir <out_dir> --emit-prefix module"
    build_playground = "npm run build:objc3c:playground"
    build_stdlib = "npm run build:objc3c:stdlib"
    build_template = "npm run build:objc3c:template"
    bonus_experiences = "npm run test:bonus-experiences"
    bonus_experiences_e2e = "npm run test:bonus-experiences:e2e"
    check_stdlib_surface = "npm run check:stdlib:surface"
    stdlib_advanced = "npm run test:stdlib:advanced"
    stdlib_advanced_e2e = "npm run test:stdlib:advanced:e2e"
    stdlib_program = "npm run test:stdlib:program"
    stdlib_program_e2e = "npm run test:stdlib:program:e2e"
    inspect_bonus_tools = "npm run inspect:objc3c:bonus-tools"
    inspect_playground = "npm run inspect:objc3c:playground"
    inspect_editor_tooling = "npm run inspect:objc3c:editor -- tests/tooling/fixtures/native/hello.objc3"
    inspect_benchmark = "npm run inspect:objc3c:benchmark"
    inspect_performance = "npm run inspect:objc3c:performance"
    inspect_release_manifest = "npm run inspect:objc3c:release-manifest"
    inspect_runtime_performance = "npm run inspect:objc3c:runtime-performance"
    inspect_compiler_throughput = "npm run inspect:objc3c:compiler-throughput"
    inspect_comparative_baselines = "npm run inspect:objc3c:comparative-baselines"
    inspect_capabilities = "npm run inspect:objc3c:capabilities"
    inspect_runtime = "npm run inspect:objc3c:runtime"
    format_objc3c = "npm run format:objc3c -- tests/tooling/fixtures/developer_tooling/messy_hello.objc3"
    trace_stages = "npm run trace:objc3c:stages"
    developer_tooling = "npm run test:objc3c:developer-tooling"
    runnable_developer_tooling = "npm run test:objc3c:runnable-developer-tooling"
    conformance_corpus = "npm run test:objc3c:conformance-corpus"
    conformance_corpus_e2e = "npm run test:objc3c:runnable-conformance-corpus"
    stdlib = "npm run test:stdlib"
    stdlib_e2e = "npm run test:stdlib:e2e"
    runtime_performance = "npm run test:objc3c:runtime-performance"
    runtime_performance_e2e = "npm run test:objc3c:runnable-runtime-performance"
    compiler_throughput = "npm run test:objc3c:compiler-throughput"
    compiler_throughput_e2e = "npm run test:objc3c:runnable-compiler-throughput"
    check_release_foundation_surface = "npm run check:objc3c:release-foundation:surface"
    check_release_foundation_schema_surface = "npm run check:objc3c:release-foundation:schemas"
    release_foundation = "npm run test:objc3c:release-foundation"
    publish_release_provenance = "npm run publish:objc3c:release-provenance"
    runnable_performance = "npm run test:objc3c:runnable-performance"
    showcase = "npm run test:showcase"
    showcase_e2e = "npm run test:showcase:e2e"
    getting_started = "npm run test:getting-started"
    smoke = "pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_native_execution_smoke.ps1"
    replay = "pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_execution_replay_proof.ps1"
  }
  truthful_boundary = @(
    "staged local package root only",
    "no system install claim",
    "no cross-platform packaging claim",
    "no toolchain auto-provisioning claim"
  )
  copied_files = @($stagedRelativePaths)
  copied_file_count = $stagedRelativePaths.Count
}

$manifestPayload | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $manifestPath -Encoding utf8

Write-Output "status: PASS"
Write-Output ("package_root: " + $manifestPayload.package_root)
Write-Output ("manifest: " + $manifestPayload.manifest_artifact)
