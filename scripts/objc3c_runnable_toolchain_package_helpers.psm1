Set-StrictMode -Version Latest

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

function Get-RepoRelativeFilesUnderRoot {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RelativeRoot,
    [Parameter(Mandatory = $true)][string]$MissingRootMessage,
    [string]$Filter = "*"
  )

  $root = Join-Path $RepoRoot $RelativeRoot
  if (!(Test-Path -LiteralPath $root -PathType Container)) {
    throw $MissingRootMessage
  }

  return @(
    Get-ChildItem -LiteralPath $root -Recurse -File -Filter $Filter |
      Sort-Object -Property FullName |
      ForEach-Object { Get-RepoRelativePathCompat -RootPath $RepoRoot -TargetPath $_.FullName }
  )
}

function Get-RepoRelativeExecutionFixtureFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $fixtureRoot = Join-Path $RepoRoot "tests/tooling/fixtures/native/execution"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "tests/tooling/fixtures/native/execution" `
    -MissingRootMessage "runnable toolchain package FAIL: missing execution fixture root $fixtureRoot")
}

function Get-RepoRelativeStdlibFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $stdlibRoot = Join-Path $RepoRoot "stdlib"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "stdlib" `
    -MissingRootMessage "runnable toolchain package FAIL: missing stdlib root $stdlibRoot")
}

function Get-RepoRelativeConformanceFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $conformanceRoot = Join-Path $RepoRoot "tests/conformance"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "tests/conformance" `
    -MissingRootMessage "runnable toolchain package FAIL: missing conformance root $conformanceRoot")
}

function Get-RepoRelativeNativeDocsFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $docsRoot = Join-Path $RepoRoot "docs/objc3c-native"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "docs/objc3c-native" `
    -MissingRootMessage "runnable toolchain package FAIL: missing native docs root $docsRoot")
}

function Get-RepoRelativePythonToolingFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $toolingRoot = Join-Path $RepoRoot "scripts/objc3c_tooling"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "scripts/objc3c_tooling" `
    -MissingRootMessage "runnable toolchain package FAIL: missing Python tooling root $toolingRoot" `
    -Filter "*.py")
}

function Get-RepoRelativeRuntimeAcceptanceFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $acceptanceRoot = Join-Path $RepoRoot "scripts/objc3c_runtime_acceptance"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "scripts/objc3c_runtime_acceptance" `
    -MissingRootMessage "runnable toolchain package FAIL: missing runtime acceptance package root $acceptanceRoot" `
    -Filter "*.py")
}

function Get-RepoRelativeRecoveryPositiveFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $recoveryRoot = Join-Path $RepoRoot "tests/tooling/fixtures/native/recovery/positive"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "tests/tooling/fixtures/native/recovery/positive" `
    -MissingRootMessage "runnable toolchain package FAIL: missing recovery-positive root $recoveryRoot")
}

function Assert-RequiredPackageSurfaceKeys {
  param(
    [Parameter(Mandatory = $true)][hashtable]$Payload,
    [Parameter(Mandatory = $true)][string]$RelativePath,
    [Parameter(Mandatory = $true)][string[]]$RequiredKeys
  )

  foreach ($requiredKey in $RequiredKeys) {
    if (-not $Payload.ContainsKey($requiredKey)) {
      throw "runnable toolchain package FAIL: missing $requiredKey in $RelativePath"
    }
  }
}

function Get-RequiredRunnableToolchainPackageFiles {
  return @(
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

}

Export-ModuleMember -Function @(
  "Assert-RequiredPackageSurfaceKeys",
  "Copy-RepoRelativeFile",
  "Get-RepoRelativeConformanceFiles",
  "Get-RepoRelativeExecutionFixtureFiles",
  "Get-RepoRelativeNativeDocsFiles",
  "Get-RepoRelativePathCompat",
  "Get-RepoRelativePythonToolingFiles",
  "Get-RepoRelativeRecoveryPositiveFiles",
  "Get-RepoRelativeRuntimeAcceptanceFiles",
  "Get-RequiredRunnableToolchainPackageFiles",
  "Get-RepoRelativeStdlibFiles",
  "Resolve-PackageRoot"
)
