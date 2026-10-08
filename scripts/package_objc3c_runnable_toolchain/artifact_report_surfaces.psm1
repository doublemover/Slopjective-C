Set-StrictMode -Version Latest

function New-RunnableToolchainPackageSurfaceManifestSection {
  param(
    [Parameter(Mandatory = $true)]$SurfacePayloads,
    [Parameter(Mandatory = $true)][string[]]$StagedRelativePaths
  )

  $repoSupercleanSurfaceRelativePath = $SurfacePayloads.RepoSupercleanSurfaceRelativePath
  $repoSupercleanSurfacePayload = $SurfacePayloads.RepoSupercleanSurfacePayload
  $stdlibLoweringImportSurfaceRelativePath = $SurfacePayloads.StdlibLoweringImportSurfaceRelativePath
  $stdlibLoweringImportSurfacePayload = $SurfacePayloads.StdlibLoweringImportSurfacePayload
  $stdlibCompatibilityGatesRelativePath = $SurfacePayloads.StdlibCompatibilityGatesRelativePath
  $stdlibCompatibilityGatesPayload = $SurfacePayloads.StdlibCompatibilityGatesPayload
  $stdlibAdvancedHelperPackageSurfaceRelativePath = $SurfacePayloads.StdlibAdvancedHelperPackageSurfaceRelativePath
  $stdlibAdvancedHelperPackageSurfacePayload = $SurfacePayloads.StdlibAdvancedHelperPackageSurfacePayload
  $stdlibProgramSurfaceRelativePath = $SurfacePayloads.StdlibProgramSurfaceRelativePath
  $stdlibProgramSurfacePayload = $SurfacePayloads.StdlibProgramSurfacePayload
  $showcaseDemoPackagesRelativePath = $SurfacePayloads.ShowcaseDemoPackagesRelativePath
  $showcaseDemoPackagesPayload = $SurfacePayloads.ShowcaseDemoPackagesPayload

  return [ordered]@{
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
    showcase_demo_packages_manifest = $showcaseDemoPackagesRelativePath
    showcase_demo_packages_contract_id = $showcaseDemoPackagesPayload["contract_id"]
    showcase_demo_packages = $showcaseDemoPackagesPayload["packages"]
    showcase_demo_package_reproducibility = $showcaseDemoPackagesPayload["reproducibility_contract"]
    stdlib_workspace_manifest = "stdlib/workspace.json"
    stdlib_module_inventory = "stdlib/module_inventory.json"
    stdlib_stability_policy = "stdlib/stability_policy.json"
    stdlib_package_surface = "stdlib/package_surface.json"
    stdlib_advanced_architecture = "stdlib/advanced_architecture.json"
    stdlib_compatibility_gates = $stdlibCompatibilityGatesRelativePath
    stdlib_compatibility_gate_summary = [ordered]@{
      contract_id = $stdlibCompatibilityGatesPayload["contract_id"]
      stdlib_major_version = $stdlibCompatibilityGatesPayload["stdlib_major_version"]
      abi_gate_mode = $stdlibCompatibilityGatesPayload["abi_gate"]["mode"]
      semantic_gate_mode = $stdlibCompatibilityGatesPayload["semantic_gate"]["mode"]
      package_gate_manifest_fields = $stdlibCompatibilityGatesPayload["package_gate"]["required_manifest_fields"]
      conformance_positive_fixture = $stdlibCompatibilityGatesPayload["conformance_gate"]["positive_fixture"]
      conformance_negative_fixture = $stdlibCompatibilityGatesPayload["conformance_gate"]["negative_fixture"]
    }
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
      package_asan = "npm run objc3c -- package-runnable-toolchain-asan"
      package_ubsan = "npm run objc3c -- package-runnable-toolchain-ubsan"
      package_channels = "npm run objc3c -- build-package-channels"
      package_channels_asan = "npm run objc3c -- build-package-channels-asan"
      package_channels_ubsan = "npm run objc3c -- build-package-channels-ubsan"
      compile = "npm run objc3c -- compile-objc3c <input.objc3> --out-dir <out_dir> --emit-prefix module"
      build_playground = "npm run objc3c -- materialize-playground-workspace"
      build_application_workspace = "npm run objc3c -- materialize-canonical-application-workspace"
      application_framework_samples = "npm run objc3c -- validate-application-framework-samples"
      build_package_lock = "npm run objc3c -- build-package-lock"
      package_manager_model = "npm run objc3c -- validate-package-manager-model"
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
      runtime_debug_trace = "npm run objc3c -- trace-runtime-debug tests/tooling/fixtures/native/hello.objc3"
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
      smoke = "npm run objc3c -- test-execution-smoke"
      replay = "npm run objc3c -- test-execution-replay"
    }
    truthful_boundary = @(
      "staged local package root only",
      "no system install claim",
      "no cross-platform packaging claim",
      "no toolchain auto-provisioning claim"
    )
    copied_files = @($StagedRelativePaths)
    copied_file_count = $StagedRelativePaths.Count
  }
}

Export-ModuleMember -Function @(
  "New-RunnableToolchainPackageSurfaceManifestSection"
)
