$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "report_rendering.psm1") -Force -DisableNameChecking

function New-Objc3cNativeRepoSupercleanBonusSurfaces {
  return New-Objc3cNativeRepoSupercleanCatalogPayload -Entries ([ordered]@{
    bonus_experience_surfaces = New-Objc3cNativeRepoSupercleanCatalogPayload -Entries ([ordered]@{
      playground = New-Objc3cNativeRepoSupercleanCatalogPayload -Entries ([ordered]@{
        source_roots = New-Objc3cNativeRepoSupercleanPathList -Paths @(
          "showcase/auroraBoard/main.objc3",
          "showcase/signalMesh/main.objc3",
          "showcase/patchKit/main.objc3",
          "tests/tooling/fixtures/native/hello.objc3"
        )
        artifact_roots = New-Objc3cNativeRepoSupercleanPathList -Paths @(
          "tmp/artifacts/playground",
          "tmp/reports/playground",
          "tmp/artifacts/showcase"
        )
        public_actions = New-Objc3cNativeRepoSupercleanActionList -Actions @(
          "materialize-playground-workspace",
          "compile-objc3c",
          "inspect-playground-repro",
          "inspect-compile-observability",
          "trace-compile-stages"
        )
      })
      runtime_inspector = New-Objc3cNativeRepoSupercleanCatalogPayload -Entries ([ordered]@{
        source_roots = New-Objc3cNativeRepoSupercleanPathList -Paths @(
          "native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp",
          "scripts/probe_objc3c_llvm_capabilities.py",
          "native/objc3c/src/runtime/objc3_runtime.cpp",
          "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
          "tests/tooling/runtime/block_arc_runtime_abi_probe.cpp",
          "tests/tooling/runtime/task_runtime_hardening_probe.cpp"
        )
        report_roots = New-Objc3cNativeRepoSupercleanPathList -Paths @(
          "tmp/reports/objc3c-public-workflow",
          "tmp/reports/developer-tooling"
        )
        public_actions = New-Objc3cNativeRepoSupercleanActionList -Actions @(
          "inspect-runtime-inspector",
          "inspect-capability-explorer",
          "benchmark-runtime-inspector",
          "trace-compile-stages",
          "validate-developer-tooling"
        )
      })
      template_and_demo_harness = New-Objc3cNativeRepoSupercleanCatalogPayload -Entries ([ordered]@{
        source_roots = New-Objc3cNativeRepoSupercleanPathList -Paths @(
          "scripts/materialize_objc3c_project_template.py",
          "showcase/README.md",
          "showcase/portfolio.json",
          "showcase/tutorial_walkthrough.json",
          "docs/tutorials/getting_started.md",
          "docs/tutorials/build_run_verify.md",
          "docs/tutorials/guided_walkthrough.md"
        )
        report_roots = New-Objc3cNativeRepoSupercleanPathList -Paths @(
          "tmp/artifacts/project-template",
          "tmp/reports/project-template",
          "tmp/reports/showcase",
          "tmp/reports/tutorials"
        )
        public_actions = New-Objc3cNativeRepoSupercleanActionList -Actions @(
          "materialize-project-template",
          "validate-showcase",
          "validate-runnable-showcase",
          "validate-getting-started"
        )
      })
    })
    bonus_tool_integration_surface = New-Objc3cNativeRepoSupercleanCatalogPayload -Entries ([ordered]@{
      source_of_truth_artifact = "tmp/artifacts/objc3c-native/repo_superclean_source_of_truth.json"
      report_root = "tmp/reports/objc3c-public-workflow"
      package_stage_root = "tmp/pkg/objc3c-native-runnable-toolchain"
      portfolio_contract = "showcase/portfolio.json"
      guided_walkthrough_manifest = "showcase/tutorial_walkthrough.json"
      public_actions = New-Objc3cNativeRepoSupercleanActionList -Actions @(
        "inspect-bonus-tool-integration",
        "materialize-project-template",
        "materialize-playground-workspace",
        "benchmark-runtime-inspector",
        "validate-showcase",
        "validate-runnable-showcase",
        "validate-getting-started",
        "package-runnable-toolchain"
      )
    })
  })
}

Export-ModuleMember -Function "New-Objc3cNativeRepoSupercleanBonusSurfaces"
