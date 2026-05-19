Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "..\objc3c_runnable_toolchain_package_helpers.psm1") -Force -DisableNameChecking

function New-RunnableToolchainPackageFoundationManifestSection {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$ManifestPath
  )

  return [ordered]@{
    contract_id = "objc3c-runnable-build-install-run-package/runnable_suite-packaged-end-to-end-v1"
    schema_version = 1
    package_model = "staged-runnable-toolchain-bundle-with-repo-relative-layout"
    install_model = "local-package-root-not-system-install"
    package_root = Get-RepoRelativePathCompat -RootPath $RepoRoot -TargetPath $PackageRoot
    manifest_artifact = Get-RepoRelativePathCompat -RootPath $PackageRoot -TargetPath $ManifestPath
    native_executable = "artifacts/bin/objc3c-native.exe"
    frontend_c_api_runner = "artifacts/bin/objc3c-frontend-c-api-runner.exe"
    runtime_library = "artifacts/lib/objc3_runtime.lib"
    compile_action = "compile-objc3c"
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
    release_evidence_gate_action = "check-release-evidence"
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
  }
}

Export-ModuleMember -Function @(
  "New-RunnableToolchainPackageFoundationManifestSection"
)
