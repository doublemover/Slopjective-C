param(
  [string]$PackageRoot = "",
  [string]$ManifestRelativePath = "artifacts/package/objc3c-runnable-toolchain-package.json"
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "objc3c_runnable_toolchain_package_helpers.psm1") -Force -DisableNameChecking

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"

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
  "scripts/objc3c_workflow/__init__.py",
  "scripts/objc3c_workflow/__main__.py",
  "scripts/materialize_objc3c_project_template.py",
  "scripts/materialize_objc3c_canonical_application_workspace.py",
  "scripts/check_application_architecture_template_harness.py",
  "scripts/check_objc3c_application_architecture_integration.py",
  "scripts/check_objc3c_runnable_application_architecture_end_to_end.py",
  "scripts/build_application_architecture_testing_boundary_inventory_summary.py",
  "scripts/build_application_architecture_testing_semantic_summary.py",
  "scripts/build_application_architecture_template_workspace_summary.py",
  "scripts/build_application_architecture_layering_summary.py",
  "scripts/build_application_architecture_artifact_contract_summary.py",
  "scripts/build_package_ecosystem_boundary_inventory_summary.py",
  "scripts/build_package_ecosystem_dependency_lock_policy_summary.py",
  "scripts/build_package_ecosystem_local_workspace_mirror_summary.py",
  "scripts/build_package_ecosystem_registry_publication_summary.py",
  "scripts/build_package_ecosystem_artifact_contract_summary.py",
  "scripts/package_ecosystem_contracts.py",
  "scripts/build_objc3c_package_lock.py",
  "scripts/check_objc3c_package_authoring_workflow.py",
  "scripts/build_objc3c_package_mirror.py",
  "scripts/check_objc3c_package_registry_mirror_reproducibility.py",
  "scripts/check_objc3c_package_ecosystem_integration.py",
  "scripts/check_objc3c_runnable_package_ecosystem_end_to_end.py",
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
  "scripts/build_adoption_legibility_boundary_inventory_summary.py",
  "scripts/build_adoption_legibility_public_claim_policy_summary.py",
  "scripts/build_adoption_legibility_capability_comparison_summary.py",
  "scripts/build_adoption_legibility_adoption_replay_summary.py",
  "scripts/build_adoption_legibility_artifact_contract_summary.py",
  "scripts/build_objc3c_adoption_legibility_evidence.py",
  "scripts/check_objc3c_adoption_legibility_integration.py",
  "scripts/publish_objc3c_adoption_legibility_metadata.py",
  "scripts/build_governance_budget_inventory_summary.py",
  "scripts/build_governance_policy_summary.py",
  "scripts/build_governance_maintainer_review_summary.py",
  "scripts/build_governance_extension_review_policy_summary.py",
  "scripts/build_governance_extension_review_workflow_summary.py",
  "scripts/build_governance_stewardship_semantics_summary.py",
  "scripts/check_governance_sustainability_schema_surface.py",
  "scripts/check_governance_sustainability_budget_enforcement.py",
  "scripts/build_governance_anti_regression_summary.py",
  "scripts/build_governance_artifact_contract_summary.py",
  "scripts/check_objc3c_governance_sustainability_integration.py",
  "scripts/build_objc3c_governance_sustainability_evidence.py",
  "scripts/publish_objc3c_governance_sustainability_metadata.py",
  "showcase/README.md",
  "showcase/portfolio.json",
  "showcase/tutorial_walkthrough.json",
  "showcase/auroraBoard/main.objc3",
  "showcase/auroraBoard/workspace.json",
  "showcase/signalMesh/main.objc3",
  "showcase/signalMesh/workspace.json",
  "showcase/patchKit/main.objc3",
  "showcase/patchKit/workspace.json",
  "docs/runbooks/objc3c_application_architecture_testing.md",
  "docs/runbooks/objc3c_adoption_legibility.md",
  "docs/runbooks/objc3c_governance_sustainability.md",
  "docs/runbooks/objc3c_package_ecosystem.md",
  "docs/runbooks/objc3c_conformance_corpus.md",
  "docs/runbooks/objc3c_compiler_throughput.md",
  "docs/runbooks/objc3c_developer_tooling.md",
  "docs/runbooks/objc3c_platform_hardening.md",
  "docs/runbooks/objc3c_public_command_surface.md",
  "docs/runbooks/objc3c_packaging_channels.md",
  "docs/runbooks/objc3c_release_foundation.md",
  "docs/runbooks/objc3c_release_operations.md",
  "docs/runbooks/objc3c_runtime_performance.md",
  "docs/runbooks/objc3c_stdlib_program.md",
  "docs/tutorials/README.md",
  "docs/tutorials/getting_started.md",
  "docs/tutorials/objc2_to_objc3_migration.md",
  "docs/tutorials/objc2_swift_cpp_comparison.md",
  "docs/tutorials/build_run_verify.md",
  "docs/tutorials/guided_walkthrough.md",
  "site/src/index.body.md",
  "scripts/probe_objc3c_llvm_capabilities.py",
  "scripts/build_objc3c_platform_support_matrix.py",
  "scripts/platform_hardening_contracts.py",
  "scripts/check_objc3c_platform_hardening_integration.py",
  "scripts/check_objc3c_runnable_platform_hardening_end_to_end.py",
  "scripts/build_platform_hardening_boundary_inventory_summary.py",
  "scripts/build_platform_hardening_support_tier_policy_summary.py",
  "scripts/build_platform_hardening_unsupported_host_policy_summary.py",
  "scripts/build_platform_hardening_toolchain_archive_policy_summary.py",
  "scripts/build_platform_hardening_artifact_contract_summary.py",
  "scripts/check_platform_hardening_build_package_validation.py",
  "scripts/check_platform_hardening_toolchain_range_replay.py",
  "scripts/check_platform_hardening_install_matrix_integration.py",
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
  "native/objc3c/src/runtime/public/objc3_runtime_api.h",
  "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
  "schemas/objc3c-application-architecture-evidence-summary-v1.schema.json",
  "schemas/objc3c-developer-tooling-editor-surface-v1.schema.json",
  "schemas/objc3-conformance-dashboard-status-v1.schema.json",
  "schemas/objc3c-platform-support-matrix-v1.schema.json",
  "schemas/objc3c-package-channels-manifest-v1.schema.json",
  "schemas/objc3c-package-install-receipt-v1.schema.json",
  "schemas/objc3c-package-lock-v1.schema.json",
  "schemas/objc3c-package-offline-mirror-index-v1.schema.json",
  "schemas/objc3c-adoption-legibility-evidence-v1.schema.json",
  "schemas/objc3c-governance-sustainability-evidence-v1.schema.json",
  "schemas/objc3c-update-manifest-v1.schema.json",
  "schemas/objc3c-upgrade-support-report-v1.schema.json",
  "schemas/objc3c-release-manifest-v1.schema.json",
  "schemas/objc3c-release-sbom-v1.schema.json",
  "schemas/objc3c-release-attestation-v1.schema.json",
  "scripts/check_release_evidence.py",
  "scripts/check_release_foundation_source_surface.py",
  "scripts/check_release_foundation_schema_surface.py",
  "scripts/build_objc3c_release_manifest.py",
  "scripts/publish_objc3c_release_provenance.py",
  "scripts/check_objc3c_release_foundation_integration.py",
  "scripts/check_packaging_channels_source_surface.py",
  "scripts/check_packaging_channels_schema_surface.py",
  "scripts/build_objc3c_package_channels.py",
  "scripts/check_objc3c_packaging_channels_integration.py",
  "scripts/check_objc3c_packaging_channels_end_to_end.py",
  "scripts/check_release_operations_source_surface.py",
  "scripts/check_release_operations_schema_surface.py",
  "scripts/build_objc3c_update_manifest.py",
  "scripts/publish_objc3c_release_operations_metadata.py",
  "scripts/check_objc3c_release_operations_integration.py",
  "scripts/check_objc3c_release_operations_end_to_end.py",
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
  "tests/tooling/runtime/support/dispatch_expectations.h",
  "tests/tooling/runtime/support/json_probe_writer.h",
  "tests/tooling/runtime/support/output_expectations.h",
  "tests/tooling/runtime/support/runtime_snapshot_json.h",
  "tests/tooling/runtime/support/runtime_snapshot_stabilizers.h",
  "tests/tooling/runtime/support/runtime_snapshot_text.h",
  "tests/tooling/fixtures/developer_tooling/boundary_inventory.json",
  "tests/tooling/fixtures/developer_tooling/language_server_navigation_implementation_contract.json",
  "tests/tooling/fixtures/developer_tooling/formatter_debug_implementation_contract.json",
  "tests/tooling/fixtures/developer_tooling/workspace_editor_debug_integration_contract.json",
  "tests/tooling/fixtures/developer_tooling/packaged_cli_to_editor_contract.json",
  "tests/tooling/fixtures/developer_tooling/messy_hello.objc3",
  "tests/tooling/fixtures/developer_tooling/formatted_hello.objc3",
  "tests/tooling/fixtures/application_architecture_testing/boundary_inventory.json",
  "tests/tooling/fixtures/application_architecture_testing/first_party_testing_semantics.json",
  "tests/tooling/fixtures/application_architecture_testing/project_template_workspace_semantics.json",
  "tests/tooling/fixtures/application_architecture_testing/canonical_application_architecture_semantics.json",
  "tests/tooling/fixtures/application_architecture_testing/artifact_contract.json",
  "tests/tooling/fixtures/package_ecosystem/boundary_inventory.json",
  "tests/tooling/fixtures/package_ecosystem/dependency_lock_policy.json",
  "tests/tooling/fixtures/package_ecosystem/local_workspace_mirror_semantics.json",
  "tests/tooling/fixtures/package_ecosystem/registry_publication_semantics.json",
  "tests/tooling/fixtures/package_ecosystem/artifact_contract.json",
  "tests/tooling/fixtures/package_ecosystem/package_authoring_workflow_contract.json",
  "tests/tooling/fixtures/package_ecosystem/registry_mirror_reproducibility_contract.json",
  "tests/tooling/fixtures/adoption_legibility/boundary_inventory.json",
  "tests/tooling/fixtures/adoption_legibility/public_claim_policy.json",
  "tests/tooling/fixtures/adoption_legibility/capability_comparison_semantics.json",
  "tests/tooling/fixtures/adoption_legibility/adoption_replay_semantics.json",
  "tests/tooling/fixtures/adoption_legibility/artifact_contract.json",
  "tests/tooling/fixtures/governance_sustainability/anti_regression_reporting_contract.json",
  "tests/tooling/fixtures/governance_sustainability/artifact_contract.json",
  "tests/tooling/fixtures/governance_sustainability/budget_inventory.json",
  "tests/tooling/fixtures/governance_sustainability/extension_review_policy.json",
  "tests/tooling/fixtures/governance_sustainability/maintainer_review_regression_contract.json",
  "tests/tooling/fixtures/governance_sustainability/new_work_proposal_sample.json",
  "tests/tooling/fixtures/governance_sustainability/new_work_proposal_template.json",
  "tests/tooling/fixtures/governance_sustainability/schema_surface.json",
  "tests/tooling/fixtures/governance_sustainability/stewardship_semantics.json",
  "tests/tooling/fixtures/governance_sustainability/sustainable_progress_policy.json",
  "tests/tooling/fixtures/governance_sustainability/waiver_registry.json",
  "tests/tooling/fixtures/platform_hardening/boundary_inventory.json",
  "tests/tooling/fixtures/platform_hardening/platform_support_tier_policy.json",
  "tests/tooling/fixtures/platform_hardening/unsupported_host_fail_closed_policy.json",
  "tests/tooling/fixtures/platform_hardening/toolchain_archive_claim_policy.json",
  "tests/tooling/fixtures/platform_hardening/platform_matrix_artifact_contract.json",
  "tests/tooling/fixtures/platform_hardening/build_package_validation_contract.json",
  "tests/tooling/fixtures/platform_hardening/toolchain_range_replay_contract.json",
  "tests/tooling/fixtures/platform_hardening/install_matrix_integration_contract.json",
  "tests/tooling/fixtures/platform_hardening/packaged_smoke_integration_contract.json",
  "tests/tooling/fixtures/packaging_channels/source_surface.json",
  "tests/tooling/fixtures/packaging_channels/supported_platforms.json",
  "tests/tooling/fixtures/packaging_channels/installer_policy.json",
  "tests/tooling/fixtures/packaging_channels/metadata_surface.json",
  "tests/tooling/fixtures/packaging_channels/schema_surface.json",
  "tests/tooling/fixtures/release_operations/source_surface.json",
  "tests/tooling/fixtures/release_operations/versioning_model.json",
  "tests/tooling/fixtures/release_operations/upgrade_path_surface.json",
  "tests/tooling/fixtures/release_operations/update_channel_policy.json",
  "tests/tooling/fixtures/release_operations/upgrade_support_claim_policy.json",
  "tests/tooling/fixtures/release_operations/fail_closed_diagnostics_policy.json",
  "tests/tooling/fixtures/release_operations/metadata_surface.json",
  "tests/tooling/fixtures/release_operations/schema_surface.json",
  "tests/tooling/fixtures/release_operations/workflow_surface.json",
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
$pythonToolingFiles = @(Get-RepoRelativePythonToolingFiles -RepoRoot $repoRoot)
$runtimeAcceptanceFiles = @(Get-RepoRelativeRuntimeAcceptanceFiles -RepoRoot $repoRoot)
$recoveryPositiveFiles = @(Get-RepoRelativeRecoveryPositiveFiles -RepoRoot $repoRoot)
$stdlibFiles = @(Get-RepoRelativeStdlibFiles -RepoRoot $repoRoot)
$conformanceFiles = @(Get-RepoRelativeConformanceFiles -RepoRoot $repoRoot)
$copiedRelativePaths = New-Object System.Collections.Generic.List[string]
foreach ($relativePath in @($requiredRelativeFiles + $executionFixtureFiles + $nativeDocsFiles + $pythonToolingFiles + $runtimeAcceptanceFiles + $recoveryPositiveFiles + $stdlibFiles + $conformanceFiles)) {
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
Assert-RequiredPackageSurfaceKeys `
  -Payload $repoSupercleanSurfacePayload `
  -RelativePath $repoSupercleanSurfaceRelativePath `
  -RequiredKeys @(
    "bonus_experience_surfaces",
    "performance_benchmark_surface",
    "runtime_performance_surface",
    "compiler_throughput_surface",
    "conformance_corpus_surface",
    "stdlib_foundation_surface",
    "stdlib_program_surface"
  )

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
  runtime_public_header = "native/objc3c/src/runtime/public/objc3_runtime_api.h"
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
  package_bridge = "objc3c"
  application_architecture_runbook = "docs/runbooks/objc3c_application_architecture_testing.md"
  application_architecture_boundary_inventory = "tests/tooling/fixtures/application_architecture_testing/boundary_inventory.json"
  application_architecture_testing_semantics = "tests/tooling/fixtures/application_architecture_testing/first_party_testing_semantics.json"
  application_architecture_template_semantics = "tests/tooling/fixtures/application_architecture_testing/project_template_workspace_semantics.json"
  application_architecture_layering_semantics = "tests/tooling/fixtures/application_architecture_testing/canonical_application_architecture_semantics.json"
  application_architecture_artifact_contract = "tests/tooling/fixtures/application_architecture_testing/artifact_contract.json"
  application_architecture_evidence_schema = "schemas/objc3c-application-architecture-evidence-summary-v1.schema.json"
  application_architecture_surface = [ordered]@{
    canonical_workspace_materializer = "scripts/materialize_objc3c_canonical_application_workspace.py"
    template_harness_checker = "scripts/check_application_architecture_template_harness.py"
    integration_validation = "scripts/check_objc3c_application_architecture_integration.py"
    runnable_end_to_end_validation = "scripts/check_objc3c_runnable_application_architecture_end_to_end.py"
  }
  application_architecture_scripts = [ordered]@{
    boundary_inventory_summary = "scripts/build_application_architecture_testing_boundary_inventory_summary.py"
    semantics_summary = "scripts/build_application_architecture_testing_semantic_summary.py"
    template_workspace_summary = "scripts/build_application_architecture_template_workspace_summary.py"
    layering_summary = "scripts/build_application_architecture_layering_summary.py"
    artifact_contract_summary = "scripts/build_application_architecture_artifact_contract_summary.py"
    template_harness_validation = "scripts/check_application_architecture_template_harness.py"
    canonical_workspace_materializer = "scripts/materialize_objc3c_canonical_application_workspace.py"
    integration_validation = "scripts/check_objc3c_application_architecture_integration.py"
    runnable_end_to_end_validation = "scripts/check_objc3c_runnable_application_architecture_end_to_end.py"
  }
  application_architecture_public_actions = @(
    "materialize-project-template",
    "materialize-canonical-application-workspace",
    "validate-application-architecture",
    "validate-runnable-application-architecture"
  )
  package_ecosystem_runbook = "docs/runbooks/objc3c_package_ecosystem.md"
  package_ecosystem_boundary_inventory = "tests/tooling/fixtures/package_ecosystem/boundary_inventory.json"
  package_ecosystem_dependency_lock_policy = "tests/tooling/fixtures/package_ecosystem/dependency_lock_policy.json"
  package_ecosystem_local_workspace_mirror_semantics = "tests/tooling/fixtures/package_ecosystem/local_workspace_mirror_semantics.json"
  package_ecosystem_registry_publication_semantics = "tests/tooling/fixtures/package_ecosystem/registry_publication_semantics.json"
  package_ecosystem_artifact_contract = "tests/tooling/fixtures/package_ecosystem/artifact_contract.json"
  package_ecosystem_package_authoring_contract = "tests/tooling/fixtures/package_ecosystem/package_authoring_workflow_contract.json"
  package_ecosystem_registry_mirror_contract = "tests/tooling/fixtures/package_ecosystem/registry_mirror_reproducibility_contract.json"
  package_ecosystem_lock_schema = "schemas/objc3c-package-lock-v1.schema.json"
  package_ecosystem_mirror_schema = "schemas/objc3c-package-offline-mirror-index-v1.schema.json"
  package_ecosystem_surface = [ordered]@{
    lock_generator = "scripts/build_objc3c_package_lock.py"
    authoring_validation = "scripts/check_objc3c_package_authoring_workflow.py"
    mirror_generator = "scripts/build_objc3c_package_mirror.py"
    mirror_validation = "scripts/check_objc3c_package_registry_mirror_reproducibility.py"
    integration_validation = "scripts/check_objc3c_package_ecosystem_integration.py"
    runnable_end_to_end_validation = "scripts/check_objc3c_runnable_package_ecosystem_end_to_end.py"
  }
  package_ecosystem_scripts = [ordered]@{
    boundary_inventory_summary = "scripts/build_package_ecosystem_boundary_inventory_summary.py"
    dependency_lock_policy_summary = "scripts/build_package_ecosystem_dependency_lock_policy_summary.py"
    local_workspace_mirror_summary = "scripts/build_package_ecosystem_local_workspace_mirror_summary.py"
    registry_publication_summary = "scripts/build_package_ecosystem_registry_publication_summary.py"
    artifact_contract_summary = "scripts/build_package_ecosystem_artifact_contract_summary.py"
    lock_generator = "scripts/build_objc3c_package_lock.py"
    authoring_validation = "scripts/check_objc3c_package_authoring_workflow.py"
    mirror_generator = "scripts/build_objc3c_package_mirror.py"
    mirror_validation = "scripts/check_objc3c_package_registry_mirror_reproducibility.py"
    integration_validation = "scripts/check_objc3c_package_ecosystem_integration.py"
    runnable_end_to_end_validation = "scripts/check_objc3c_runnable_package_ecosystem_end_to_end.py"
  }
  package_ecosystem_public_actions = @(
    "build-package-lock",
    "validate-package-authoring",
    "validate-package-mirror",
    "validate-package-ecosystem",
    "validate-runnable-package-ecosystem"
  )
  long_horizon_operations_runbook = "docs/runbooks/objc3c_long_horizon_operations.md"
  long_horizon_operations_artifact_contract = "tests/tooling/fixtures/long_horizon_operations/artifact_contract.json"
  long_horizon_operations_schema = "schemas/objc3c-long-horizon-operations-evidence-v1.schema.json"
  long_horizon_operations_surface = [ordered]@{
    evidence_generator = "scripts/build_objc3c_long_horizon_operations_evidence.py"
    integration_validation = "scripts/check_objc3c_long_horizon_operations_integration.py"
    support_window_publication = "scripts/publish_objc3c_long_horizon_operations_metadata.py"
  }
  long_horizon_operations_public_actions = @(
    "validate-long-horizon-operations",
    "publish-long-horizon-operations"
  )
  adoption_legibility_runbook = "docs/runbooks/objc3c_adoption_legibility.md"
  adoption_legibility_artifact_contract = "tests/tooling/fixtures/adoption_legibility/artifact_contract.json"
  adoption_legibility_schema = "schemas/objc3c-adoption-legibility-evidence-v1.schema.json"
  adoption_legibility_surface = [ordered]@{
    boundary_inventory = "tests/tooling/fixtures/adoption_legibility/boundary_inventory.json"
    public_claim_policy = "tests/tooling/fixtures/adoption_legibility/public_claim_policy.json"
    capability_comparison_semantics = "tests/tooling/fixtures/adoption_legibility/capability_comparison_semantics.json"
    adoption_replay_semantics = "tests/tooling/fixtures/adoption_legibility/adoption_replay_semantics.json"
    evidence_generator = "scripts/build_objc3c_adoption_legibility_evidence.py"
    integration_validation = "scripts/check_objc3c_adoption_legibility_integration.py"
    evaluator_publication = "scripts/publish_objc3c_adoption_legibility_metadata.py"
  }
  adoption_legibility_public_actions = @(
    "validate-adoption-legibility",
    "publish-adoption-legibility"
  )
  governance_sustainability_runbook = "docs/runbooks/objc3c_governance_sustainability.md"
  governance_sustainability_artifact_contract = "tests/tooling/fixtures/governance_sustainability/artifact_contract.json"
  governance_sustainability_schema = "schemas/objc3c-governance-sustainability-evidence-v1.schema.json"
  governance_sustainability_surface = [ordered]@{
    budget_inventory = "tests/tooling/fixtures/governance_sustainability/budget_inventory.json"
    extension_review_policy = "tests/tooling/fixtures/governance_sustainability/extension_review_policy.json"
    stewardship_semantics = "tests/tooling/fixtures/governance_sustainability/stewardship_semantics.json"
    evidence_generator = "scripts/build_objc3c_governance_sustainability_evidence.py"
    integration_validation = "scripts/check_objc3c_governance_sustainability_integration.py"
    governance_publication = "scripts/publish_objc3c_governance_sustainability_metadata.py"
  }
  governance_sustainability_public_actions = @(
    "validate-governance-sustainability",
    "publish-governance-sustainability"
  )
  platform_hardening_runbook = "docs/runbooks/objc3c_platform_hardening.md"
  platform_hardening_boundary_inventory = "tests/tooling/fixtures/platform_hardening/boundary_inventory.json"
  platform_support_tier_policy = "tests/tooling/fixtures/platform_hardening/platform_support_tier_policy.json"
  platform_unsupported_host_policy = "tests/tooling/fixtures/platform_hardening/unsupported_host_fail_closed_policy.json"
  platform_toolchain_archive_policy = "tests/tooling/fixtures/platform_hardening/toolchain_archive_claim_policy.json"
  platform_support_matrix_schema = "schemas/objc3c-platform-support-matrix-v1.schema.json"
  platform_support_matrix_contract = "tests/tooling/fixtures/platform_hardening/platform_matrix_artifact_contract.json"
  platform_build_package_validation_contract = "tests/tooling/fixtures/platform_hardening/build_package_validation_contract.json"
  platform_toolchain_range_replay_contract = "tests/tooling/fixtures/platform_hardening/toolchain_range_replay_contract.json"
  platform_install_matrix_integration_contract = "tests/tooling/fixtures/platform_hardening/install_matrix_integration_contract.json"
  platform_packaged_smoke_contract = "tests/tooling/fixtures/platform_hardening/packaged_smoke_integration_contract.json"
  platform_hardening_scripts = [ordered]@{
    support_matrix = "scripts/build_objc3c_platform_support_matrix.py"
    boundary_inventory_summary = "scripts/build_platform_hardening_boundary_inventory_summary.py"
    support_tier_policy_summary = "scripts/build_platform_hardening_support_tier_policy_summary.py"
    unsupported_host_policy_summary = "scripts/build_platform_hardening_unsupported_host_policy_summary.py"
    toolchain_archive_policy_summary = "scripts/build_platform_hardening_toolchain_archive_policy_summary.py"
    artifact_contract_summary = "scripts/build_platform_hardening_artifact_contract_summary.py"
    live_integration_validation = "scripts/check_objc3c_platform_hardening_integration.py"
    build_package_validation = "scripts/check_platform_hardening_build_package_validation.py"
    toolchain_range_replay = "scripts/check_platform_hardening_toolchain_range_replay.py"
    install_matrix_integration = "scripts/check_platform_hardening_install_matrix_integration.py"
    runnable_end_to_end_validation = "scripts/check_objc3c_runnable_platform_hardening_end_to_end.py"
  }
  platform_hardening_public_actions = @(
    "build-platform-support-matrix",
    "validate-platform-hardening",
    "validate-platform-hardening-end-to-end"
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
    build = "npm run objc3c -- build-native-binaries"
    package = "npm run objc3c -- package-runnable-toolchain"
    package_channels = "npm run objc3c -- build-package-channels"
    compile = "npm run objc3c -- compile-objc3c <input.objc3> --out-dir <out_dir> --emit-prefix module"
    build_playground = "npm run objc3c -- materialize-playground-workspace"
    build_application_workspace = "npm run objc3c -- materialize-canonical-application-workspace"
    build_package_lock = "npm run objc3c -- build-package-lock"
    build_stdlib = "npm run objc3c -- materialize-stdlib-workspace"
    build_template = "npm run objc3c -- materialize-project-template"
    application_architecture = "npm run objc3c -- validate-application-architecture"
    application_architecture_e2e = "npm run objc3c -- validate-runnable-application-architecture"
    package_authoring = "npm run objc3c -- validate-package-authoring"
    package_mirror = "npm run objc3c -- validate-package-mirror"
    package_ecosystem = "npm run objc3c -- validate-package-ecosystem"
    package_ecosystem_e2e = "npm run objc3c -- validate-runnable-package-ecosystem"
    adoption_legibility = "npm run objc3c -- validate-adoption-legibility"
    publish_adoption_legibility = "npm run objc3c -- publish-adoption-legibility"
    bonus_experiences = "npm run objc3c -- validate-bonus-experiences"
    bonus_experiences_e2e = "npm run objc3c -- validate-runnable-bonus-experiences"
    check_stdlib_surface = "npm run objc3c -- check-stdlib-surface"
    stdlib_advanced = "npm run objc3c -- validate-stdlib-advanced"
    stdlib_advanced_e2e = "npm run objc3c -- validate-runnable-stdlib-advanced"
    stdlib_program = "npm run objc3c -- validate-stdlib-program"
    stdlib_program_e2e = "npm run objc3c -- validate-runnable-stdlib-program"
    inspect_bonus_tools = "npm run objc3c -- inspect-bonus-tool-integration"
    inspect_playground = "npm run objc3c -- inspect-playground-repro"
    inspect_editor_tooling = "npm run objc3c -- inspect-editor-tooling tests/tooling/fixtures/native/hello.objc3"
    inspect_platform_matrix = "npm run objc3c -- build-platform-support-matrix"
    inspect_benchmark = "npm run objc3c -- benchmark-runtime-inspector"
    inspect_performance = "npm run objc3c -- benchmark-performance"
    inspect_release_manifest = "npm run objc3c -- build-release-manifest"
    inspect_runtime_performance = "npm run objc3c -- benchmark-runtime-performance"
    inspect_compiler_throughput = "npm run objc3c -- benchmark-compiler-throughput"
    inspect_comparative_baselines = "npm run objc3c -- benchmark-comparative-baselines"
    inspect_capabilities = "npm run objc3c -- inspect-capability-explorer"
    inspect_runtime = "npm run objc3c -- inspect-runtime-inspector"
    format_objc3c = "npm run objc3c -- format-objc3c tests/tooling/fixtures/developer_tooling/messy_hello.objc3"
    trace_stages = "npm run objc3c -- trace-compile-stages"
    developer_tooling = "npm run objc3c -- validate-developer-tooling"
    runnable_developer_tooling = "npm run objc3c -- validate-runnable-developer-tooling"
    conformance_corpus = "npm run objc3c -- validate-conformance-corpus"
    conformance_corpus_e2e = "npm run objc3c -- validate-runnable-conformance-corpus"
    stdlib = "npm run objc3c -- validate-stdlib-foundation"
    stdlib_e2e = "npm run objc3c -- validate-runnable-stdlib-foundation"
    runtime_performance = "npm run objc3c -- validate-runtime-performance"
    runtime_performance_e2e = "npm run objc3c -- validate-runnable-runtime-performance"
    compiler_throughput = "npm run objc3c -- validate-compiler-throughput"
    compiler_throughput_e2e = "npm run objc3c -- validate-runnable-compiler-throughput"
    platform_hardening = "npm run objc3c -- validate-platform-hardening"
    platform_hardening_e2e = "npm run objc3c -- validate-platform-hardening-end-to-end"
    release_operations = "npm run objc3c -- validate-release-operations"
    release_operations_e2e = "npm run objc3c -- validate-release-operations-end-to-end"
    check_release_foundation_surface = "npm run objc3c -- check-release-foundation-surface"
    check_release_foundation_schema_surface = "npm run objc3c -- check-release-foundation-schema-surface"
    release_foundation = "npm run objc3c -- validate-release-foundation"
    publish_release_provenance = "npm run objc3c -- publish-release-provenance"
    runnable_performance = "npm run objc3c -- validate-runnable-performance"
    showcase = "npm run objc3c -- validate-showcase"
    showcase_e2e = "npm run objc3c -- validate-runnable-showcase"
    getting_started = "npm run objc3c -- validate-getting-started"
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
