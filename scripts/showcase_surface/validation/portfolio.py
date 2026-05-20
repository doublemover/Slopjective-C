from __future__ import annotations

from typing import Any

from .constants import EXPECTED_EXAMPLE_IDS
from .examples import showcase_example_ids


def validate_portfolio_contract(payload: dict[str, Any]) -> str | None:
    if payload.get("contract_id") != "objc3c.showcase.portfolio.surface.v1":
        return "unexpected contract_id"
    if payload.get("schema_version") != 1:
        return "unexpected schema_version"
    if payload.get("showcase_root") != "showcase":
        return "showcase_root drifted"
    if payload.get("machine_output_root") != "tmp/artifacts/showcase":
        return "machine_output_root drifted"
    if payload.get("machine_report_root") != "tmp/reports/showcase":
        return "machine_report_root drifted"
    if payload.get("package_stage_root") != "tmp/pkg/objc3c-native-runnable-toolchain":
        return "package_stage_root drifted"
    if payload.get("demo_packages_manifest") != "showcase/demo_packages.json":
        return "demo_packages_manifest drifted"

    if "public_entrypoints" in payload:
        return "retired public_entrypoints present"
    if payload.get("public_command_model") != {
        "entrypoint_kind": "single-npm-bridge",
        "canonical_template": "npm run objc3c -- <action>",
        "authoritative_script": "package.json#scripts.objc3c",
        "retired_public_semantics": [
            "retired command surfaces",
            "direct helper script commands",
            "showcase-local wrappers",
            "alternate compile or runtime support lanes",
            "retired-source support claims",
        ],
    }:
        return "public_command_model drifted"

    public_actions = {
        "build_native": "build-native-binaries",
        "compile_example": "compile-objc3c",
        "check_surface": "check-showcase-surface",
        "validate_showcase": "validate-showcase",
        "validate_runnable_showcase": "validate-runnable-showcase",
        "package_runnable_toolchain": "package-runnable-toolchain",
        "execution_smoke": "test-execution-smoke",
        "execution_replay": "test-execution-replay",
    }
    if payload.get("public_actions") != public_actions:
        return "public_actions drifted"
    if payload.get("command_surfaces") != {
        name: f"npm run objc3c -- {action}"
        for name, action in public_actions.items()
    }:
        return "command_surfaces drifted"

    build_run_package_surface = payload.get("build_run_package_surface")
    if build_run_package_surface != {
        "emit_prefix": "module",
        "workspace_layout": "checked-in showcase directories rooted at showcase/<example-id>",
        "artifact_root": "tmp/artifacts/showcase",
        "report_root": "tmp/reports/showcase",
        "package_stage_root": "tmp/pkg/objc3c-native-runnable-toolchain",
        "build_native_action": "build-native-binaries",
        "compile_action": "compile-objc3c",
        "surface_check_action": "check-showcase-surface",
        "integrated_validation_action": "validate-showcase",
        "packaged_validation_action": "validate-runnable-showcase",
        "package_action": "package-runnable-toolchain",
        "execution_smoke_action": "test-execution-smoke",
        "execution_replay_action": "test-execution-replay",
    }:
        return "build_run_package_surface drifted"
    tutorial_build_run_verify_surface = payload.get("tutorial_build_run_verify_surface")
    if tutorial_build_run_verify_surface != {
        "getting_started_readme": "docs/tutorials/getting_started.md",
        "build_run_verify_readme": "docs/tutorials/build_run_verify.md",
        "objc2_conversion_readme": "docs/tutorials/objc2_to_objc3_migration.md",
        "build_native_action": "build-native-binaries",
        "compile_action": "compile-objc3c",
        "surface_check_action": "check-showcase-surface",
        "integrated_validation_action": "validate-showcase",
        "packaged_validation_action": "validate-runnable-showcase",
        "artifact_root": "tmp/artifacts/showcase",
        "report_root": "tmp/reports/showcase",
        "package_stage_root": "tmp/pkg/objc3c-native-runnable-toolchain",
    }:
        return "tutorial_build_run_verify_surface drifted"
    if payload.get("guided_walkthrough_manifest") != "showcase/tutorial_walkthrough.json":
        return "guided_walkthrough_manifest drifted"
    runtime_presentation_surface = payload.get("runtime_presentation_surface")
    if runtime_presentation_surface != {
        "launch_contract_helper": "scripts/objc3c_runtime_launch_contract.ps1",
        "runtime_library_resolution_model": "registration-manifest-runtime-archive-path-is-authoritative",
        "driver_linker_flag_consumption_model": "registration-manifest-driver-linker-flags-feed-proof-and-smoke-link-commands",
        "integrated_validation_action": "validate-showcase",
        "packaged_validation_action": "validate-runnable-showcase",
        "shared_execution_smoke_action": "test-execution-smoke",
        "shared_execution_replay_action": "test-execution-replay",
        "presentation_readme": "showcase/README.md",
    }:
        return "runtime_presentation_surface drifted"

    examples = payload.get("examples")
    if not isinstance(examples, list) or len(examples) != 3:
        return "examples inventory drifted"

    ids = showcase_example_ids(examples)
    if ids != EXPECTED_EXAMPLE_IDS:
        return "example ids drifted"
    return None
