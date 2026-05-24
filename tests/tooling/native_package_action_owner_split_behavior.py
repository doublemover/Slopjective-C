from __future__ import annotations

from native_package_action_owner_split_assertions import (
    assert_contains_all,
    assert_excludes_all,
    assert_workflow_modules_import,
)
from native_package_action_owner_split_sources import (
    NATIVE_PACKAGE_ACTION_HANDLERS,
    NATIVE_PACKAGE_ACTION_SPECS,
    NATIVE_PACKAGE_PROOF_ACTION_HANDLERS,
    NATIVE_PACKAGE_PROOF_ACTION_SPECS,
    NATIVE_PACKAGE_TOOLCHAIN_ACTION_HANDLERS,
    NATIVE_PACKAGE_TOOLCHAIN_ACTION_SPECS,
    NATIVE_PACKAGE_TOOLCHAIN_OWNER_POLICY,
    catalog_owner_modules,
    category_owner_contracts,
    handler_owner_modules,
    native_package_catalog_facade_text,
    native_package_core_toolchain_manifest_text,
    native_package_file_inventory_text,
    native_package_handler_facade_text,
    native_package_helper_exports_text,
    native_package_native_execution_manifest_text,
    native_package_owner_handler_texts,
    native_package_script_text,
    native_package_staging_module_text,
)


def assert_native_package_catalog_is_owner_facade() -> None:
    facade_text = native_package_catalog_facade_text()

    assert_workflow_modules_import(catalog_owner_modules())
    for module_name in catalog_owner_modules():
        assert f"from .{module_name} import" in facade_text
    assert_excludes_all(facade_text, ["ActionSpec(", "pwsh:scripts/"])


def assert_native_package_handlers_are_owner_facade() -> None:
    facade_text = native_package_handler_facade_text()

    assert_workflow_modules_import(handler_owner_modules())
    for module_name in handler_owner_modules():
        assert f"from scripts.objc3c_workflow.{module_name} import" in facade_text
    assert_excludes_all(facade_text, ["native_build.action_"])


def assert_native_package_owner_handlers_do_not_route_through_native_build_wrapper() -> None:
    toolchain_text, proof_text = native_package_owner_handler_texts()

    assert_contains_all(
        toolchain_text,
        ["from scripts.objc3c_workflow.actions import native_build_package"],
    )
    assert_contains_all(
        proof_text,
        ["from scripts.objc3c_workflow.actions import native_build_package"],
    )
    assert_excludes_all(
        toolchain_text,
        [
            "from scripts.objc3c_workflow.actions import native_build\n",
            "native_build.action_",
        ],
    )
    assert_excludes_all(
        proof_text,
        [
            "from scripts.objc3c_workflow.actions import native_build\n",
            "native_build.action_",
        ],
    )


def assert_runnable_toolchain_package_spec_publishes_platform_owner_contract() -> None:
    assert NATIVE_PACKAGE_TOOLCHAIN_OWNER_POLICY == {
        "source_owner": "packaging-channels-source",
        "gate_owner": "packaging-channels-gate",
        "blocker_owner": "packaging-channels-blockers",
        "payload_owner_action": "package-runnable-toolchain",
        "unsupported_host_success_allowed": False,
        "toolchain_archive_claim_owner": "platform-hardening-build-package-validation",
        "toolchain_archive_claim_requires_owner": True,
        "wrapper_only_action_surface_allowed": False,
    }

    spec = NATIVE_PACKAGE_TOOLCHAIN_ACTION_SPECS["package-runnable-toolchain"]
    assert spec.pass_through_args is True
    assert_contains_all(
        spec.guarantee_owner,
        [
            "unsupported hosts cannot succeed",
            "platform-hardening-build-package-validation",
            "checked-in package script",
        ],
    )


def assert_package_inventory_facade_publishes_category_owner_contracts() -> None:
    assert category_owner_contracts() == {
        "package": {
            "action_surface_owner": "native-package-toolchain",
            "source_owner": "packaging-channels-source",
            "gate_owner": "packaging-channels-gate",
            "blocker_owner": "packaging-channels-blockers",
            "wrapper_only_action_surface_allowed": False,
        },
        "packaging": {
            "action_surface_owner": "packaging-channels",
            "source_owner": "packaging-channels-source",
            "gate_owner": "packaging-channels-gate",
            "blocker_owner": "packaging-channels-blockers",
            "wrapper_only_action_surface_allowed": False,
        },
    }


def assert_runnable_toolchain_package_uses_strictmode_safe_staging_lookup() -> None:
    script_text = native_package_script_text()
    staging_text = native_package_staging_module_text()

    assert_contains_all(script_text, ["$staging.StagedRelativePaths"])
    assert_contains_all(
        staging_text,
        [
            "-SanitizerVariant $SanitizerVariant `",
            "return [pscustomobject]@{",
        ],
    )
    assert_excludes_all(staging_text, ["return [ordered]@{"])


def assert_runnable_toolchain_package_includes_compile_wrapper_dependencies() -> None:
    core_toolchain_text = native_package_core_toolchain_manifest_text()
    native_execution_text = native_package_native_execution_manifest_text()
    file_inventory_text = native_package_file_inventory_text()
    helper_exports_text = native_package_helper_exports_text()
    staging_text = native_package_staging_module_text()

    assert_contains_all(
        core_toolchain_text,
        [
            "scripts/objc3c_native_compile_arguments.ps1",
            "scripts/objc3c_shared/json_io.py",
            "scripts/objc3c_shared/schema_registry.py",
            "scripts/objc3c_native_compile_io.psm1",
            "scripts/objc3c_native_compile_io/path_normalization.psm1",
            "scripts/objc3c_native_compile_io/cache_io.psm1",
            "scripts/objc3c_native_compile_io/artifact_io.psm1",
            "scripts/objc3c_native_compile_toolchain.psm1",
            "scripts/objc3c_native_compile_toolchain/artifact_resolution/definitions.psm1",
            "scripts/objc3c_native_compile_toolchain/readiness/frontend_lock.psm1",
            "scripts/objc3c_native_compile_toolchain/results/conversion.psm1",
            "scripts/objc3c_native_compile_frontend_guards.psm1",
            "scripts/objc3c_native_compile_command.psm1",
            "scripts/objc3c_runtime_launch_contract.ps1",
            "scripts/objc3c_native_compile_provenance.ps1",
            "scripts/objc3c_native_compile_provenance/provenance_capture.psm1",
            "scripts/objc3c_native_compile_wrapper.psm1",
            "scripts/objc3c_native_compile_wrapper/orchestration.psm1",
        ],
    )
    assert_contains_all(
        native_execution_text,
        [
            "scripts/objc3c_native_execution_smoke_helpers.psm1",
            "scripts/objc3c_native_execution_smoke_runner.psm1",
            "scripts/objc3c_execution_replay_proof_helpers.psm1",
        ],
    )
    assert_contains_all(
        file_inventory_text,
        [
            "function Get-RepoRelativeNativeCompileSupportFiles",
            "function Get-RepoRelativeNativeExecutionSupportFiles",
            "function Get-RepoRelativeNativeRuntimeSourceFiles",
            "function Get-RepoRelativePythonSharedFiles",
            "function Get-RepoRelativeRuntimeProbeFiles",
            "function Get-RepoRelativeWorkflowPythonFiles",
            "scripts/objc3c_native_execution_smoke_helpers",
            "scripts/objc3c_native_execution_smoke_runner",
            "scripts/objc3c_execution_replay_proof_helpers",
            "scripts/objc3c_native_compile_toolchain",
            "scripts/objc3c_native_compile_frontend_feature_guards",
            "scripts/objc3c_native_compile_frontend_conformance_guards",
        ],
    )
    assert_contains_all(
        staging_text,
        [
            "Get-RepoRelativeNativeCompileSupportFiles -RepoRoot $RepoRoot",
            "Get-RepoRelativeNativeExecutionSupportFiles -RepoRoot $RepoRoot",
            "Get-RepoRelativeNativeRuntimeSourceFiles -RepoRoot $RepoRoot",
            "Get-RepoRelativePythonSharedFiles -RepoRoot $RepoRoot",
            "Get-RepoRelativeRuntimeProbeFiles -RepoRoot $RepoRoot",
            "Get-RepoRelativeWorkflowPythonFiles -RepoRoot $RepoRoot",
        ],
    )
    assert_contains_all(
        helper_exports_text,
        [
            '"Get-RepoRelativeNativeCompileSupportFiles"',
            '"Get-RepoRelativeNativeExecutionSupportFiles"',
            '"Get-RepoRelativeNativeRuntimeSourceFiles"',
            '"Get-RepoRelativePythonSharedFiles"',
            '"Get-RepoRelativeRuntimeProbeFiles"',
            '"Get-RepoRelativeWorkflowPythonFiles"',
        ],
    )


def assert_native_package_public_order_and_owner_membership() -> None:
    assert tuple(NATIVE_PACKAGE_ACTION_SPECS) == (
        "package-runnable-toolchain",
        "package-runnable-toolchain-asan",
        "package-runnable-toolchain-ubsan",
        "proof-objc3c",
    )
    assert NATIVE_PACKAGE_ACTION_SPECS["package-runnable-toolchain"] is (
        NATIVE_PACKAGE_TOOLCHAIN_ACTION_SPECS["package-runnable-toolchain"]
    )
    assert NATIVE_PACKAGE_ACTION_SPECS["package-runnable-toolchain-asan"] is (
        NATIVE_PACKAGE_TOOLCHAIN_ACTION_SPECS["package-runnable-toolchain-asan"]
    )
    assert NATIVE_PACKAGE_ACTION_SPECS["package-runnable-toolchain-ubsan"] is (
        NATIVE_PACKAGE_TOOLCHAIN_ACTION_SPECS["package-runnable-toolchain-ubsan"]
    )
    assert NATIVE_PACKAGE_ACTION_SPECS["proof-objc3c"] is (
        NATIVE_PACKAGE_PROOF_ACTION_SPECS["proof-objc3c"]
    )
    assert NATIVE_PACKAGE_ACTION_HANDLERS["package-runnable-toolchain"] is (
        NATIVE_PACKAGE_TOOLCHAIN_ACTION_HANDLERS["package-runnable-toolchain"]
    )
    assert NATIVE_PACKAGE_ACTION_HANDLERS["package-runnable-toolchain-asan"] is (
        NATIVE_PACKAGE_TOOLCHAIN_ACTION_HANDLERS["package-runnable-toolchain-asan"]
    )
    assert NATIVE_PACKAGE_ACTION_HANDLERS["package-runnable-toolchain-ubsan"] is (
        NATIVE_PACKAGE_TOOLCHAIN_ACTION_HANDLERS["package-runnable-toolchain-ubsan"]
    )
    assert NATIVE_PACKAGE_ACTION_HANDLERS["proof-objc3c"] is (
        NATIVE_PACKAGE_PROOF_ACTION_HANDLERS["proof-objc3c"]
    )
