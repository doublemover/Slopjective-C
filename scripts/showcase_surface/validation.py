from __future__ import annotations

from typing import Any


WORKSPACE_CONTRACT_ID = "objc3c.showcase.example.workspace.v1"
GUIDED_WALKTHROUGH_CONTRACT_ID = "objc3c.showcase.tutorial.walkthrough.v1"
SHOWCASE_SUMMARY_CONTRACT_ID = "objc3c.showcase.surface.summary.v1"
EXPECTED_EXAMPLE_IDS = ["auroraBoard", "signalMesh", "patchKit"]


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

    entrypoints = payload.get("public_entrypoints")
    if entrypoints != {
        "build_native": "build:objc3c-native",
        "compile_example": "compile:objc3c",
        "check_surface": "check:showcase:surface",
        "validate_showcase": "test:showcase",
        "validate_runnable_showcase": "test:showcase:e2e",
        "package_runnable_toolchain": "package:objc3c-native:runnable-toolchain",
        "execution_smoke": "test:objc3c:execution-smoke",
        "execution_replay": "test:objc3c:execution-replay-proof",
    }:
        return "public_entrypoints drifted"

    build_run_package_surface = payload.get("build_run_package_surface")
    if build_run_package_surface != {
        "emit_prefix": "module",
        "workspace_layout": "checked-in showcase directories rooted at showcase/<example-id>",
        "artifact_root": "tmp/artifacts/showcase",
        "report_root": "tmp/reports/showcase",
        "package_stage_root": "tmp/pkg/objc3c-native-runnable-toolchain",
        "build_native_entrypoint": "build:objc3c-native",
        "compile_entrypoint": "compile:objc3c",
        "surface_check_entrypoint": "check:showcase:surface",
        "integrated_validation_entrypoint": "test:showcase",
        "packaged_validation_entrypoint": "test:showcase:e2e",
        "package_entrypoint": "package:objc3c-native:runnable-toolchain",
        "execution_smoke_entrypoint": "test:objc3c:execution-smoke",
        "execution_replay_entrypoint": "test:objc3c:execution-replay-proof",
    }:
        return "build_run_package_surface drifted"
    tutorial_build_run_verify_surface = payload.get("tutorial_build_run_verify_surface")
    if tutorial_build_run_verify_surface != {
        "getting_started_readme": "docs/tutorials/getting_started.md",
        "build_run_verify_readme": "docs/tutorials/build_run_verify.md",
        "migration_readme": "docs/tutorials/objc2_to_objc3_migration.md",
        "build_native_entrypoint": "build:objc3c-native",
        "compile_entrypoint": "compile:objc3c",
        "surface_check_entrypoint": "check:showcase:surface",
        "integrated_validation_entrypoint": "test:showcase",
        "packaged_validation_entrypoint": "test:showcase:e2e",
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
        "integrated_validation_entrypoint": "test:showcase",
        "packaged_validation_entrypoint": "test:showcase:e2e",
        "shared_execution_smoke_entrypoint": "test:objc3c:execution-smoke",
        "shared_execution_replay_entrypoint": "test:objc3c:execution-replay-proof",
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


def validate_guided_walkthrough_contract(walkthrough_payload: dict[str, Any]) -> str | None:
    if walkthrough_payload.get("contract_id") != GUIDED_WALKTHROUGH_CONTRACT_ID:
        return "guided walkthrough contract_id drifted"
    if walkthrough_payload.get("schema_version") != 1:
        return "guided walkthrough schema_version drifted"
    if walkthrough_payload.get("tutorial_readme") != "docs/tutorials/guided_walkthrough.md":
        return "guided walkthrough tutorial_readme drifted"
    if walkthrough_payload.get("build_run_verify_readme") != "docs/tutorials/build_run_verify.md":
        return "guided walkthrough build_run_verify_readme drifted"
    if walkthrough_payload.get("portfolio_contract") != "showcase/portfolio.json":
        return "guided walkthrough portfolio_contract drifted"
    walkthrough_steps = walkthrough_payload.get("steps")
    if walkthrough_steps != [
        {
            "id": "build-native",
            "public_entrypoint": "build:objc3c-native",
        },
        {
            "id": "compile-auroraBoard",
            "public_entrypoint": "compile:objc3c",
            "example_id": "auroraBoard",
            "source": "showcase/auroraBoard/main.objc3",
            "artifact_root": "tmp/artifacts/showcase/auroraBoard",
        },
        {
            "id": "compile-signalMesh",
            "public_entrypoint": "compile:objc3c",
            "example_id": "signalMesh",
            "source": "showcase/signalMesh/main.objc3",
            "artifact_root": "tmp/artifacts/showcase/signalMesh",
        },
        {
            "id": "compile-patchKit",
            "public_entrypoint": "compile:objc3c",
            "example_id": "patchKit",
            "source": "showcase/patchKit/main.objc3",
            "artifact_root": "tmp/artifacts/showcase/patchKit",
        },
        {
            "id": "check-showcase-surface",
            "public_entrypoint": "check:showcase:surface",
            "report_root": "tmp/reports/showcase",
        },
        {
            "id": "validate-showcase",
            "public_entrypoint": "test:showcase",
            "report_root": "tmp/reports/showcase",
        },
    ]:
        return "guided walkthrough steps drifted"
    return None


def showcase_example_ids(examples: list[Any]) -> list[Any]:
    return [entry.get("id") for entry in examples if isinstance(entry, dict)]


def known_story_capabilities(examples: list[Any]) -> set[str]:
    return {
        capability
        for entry in examples
        if isinstance(entry, dict)
        for capability in entry.get("story_capabilities", [])
        if isinstance(capability, str)
    }


def validate_requested_ids(requested_ids: set[str], ids: list[Any]) -> str | None:
    if requested_ids:
        unknown_ids = sorted(requested_ids.difference(ids))
        if unknown_ids:
            return f"unknown showcase example ids: {', '.join(unknown_ids)}"
    return None


def validate_requested_capabilities(
    requested_capabilities: set[str],
    known_capabilities: set[str],
) -> str | None:
    if requested_capabilities:
        unknown_capabilities = sorted(requested_capabilities.difference(known_capabilities))
        if unknown_capabilities:
            return f"unknown showcase capabilities: {', '.join(unknown_capabilities)}"
    return None


def validate_showcase_entry(entry: Any) -> str | None:
    if not isinstance(entry, dict):
        return "example entry must be an object"
    example_id = entry.get("id")
    if not isinstance(example_id, str):
        return "example entry missing id/source"
    workspace_manifest = entry.get("workspace_manifest")
    if not isinstance(workspace_manifest, str):
        return f"example entry missing workspace_manifest for {example_id}"
    return None


def validate_workspace_contract(
    entry: dict[str, Any],
    workspace_payload: dict[str, Any],
) -> str | None:
    example_id = entry.get("id")
    if not isinstance(example_id, str):
        return "example entry missing id/source"
    if workspace_payload.get("contract_id") != WORKSPACE_CONTRACT_ID:
        return f"workspace manifest contract drifted for {example_id}"
    if workspace_payload.get("schema_version") != 1:
        return f"workspace manifest schema drifted for {example_id}"
    if workspace_payload.get("example_id") != example_id:
        return f"workspace manifest example_id drifted for {example_id}"
    if workspace_payload.get("workspace_root") != f"showcase/{example_id}":
        return f"workspace manifest workspace_root drifted for {example_id}"
    if workspace_payload.get("entry_source") != entry.get("source"):
        return f"workspace manifest entry_source drifted for {example_id}"
    if workspace_payload.get("emit_prefix") != "module":
        return f"workspace manifest emit_prefix drifted for {example_id}"
    if workspace_payload.get("artifact_root") != f"tmp/artifacts/showcase/{example_id}":
        return f"workspace manifest artifact_root drifted for {example_id}"
    if workspace_payload.get("package_stage_root") != f"showcase/{example_id}":
        return f"workspace manifest package_stage_root drifted for {example_id}"
    stdlib_followup_modules = workspace_payload.get("stdlib_followup_modules")
    if not isinstance(stdlib_followup_modules, list) or not all(
        isinstance(value, str) and value for value in stdlib_followup_modules
    ):
        return f"workspace manifest stdlib_followup_modules malformed for {example_id}"
    if workspace_payload.get("story_capabilities") != entry.get("story_capabilities"):
        return f"workspace manifest story_capabilities drifted for {example_id}"
    if stdlib_followup_modules != entry.get("stdlib_followup_modules"):
        return f"workspace manifest stdlib_followup_modules drifted for {example_id}"
    runtime_surface = workspace_payload.get("runtime_surface")
    if runtime_surface != {
        "launch_contract_helper": "scripts/objc3c_runtime_launch_contract.ps1",
        "runtime_library_resolution_model": "registration-manifest-runtime-archive-path-is-authoritative",
        "driver_linker_flag_consumption_model": "registration-manifest-driver-linker-flags-feed-proof-and-smoke-link-commands",
        "runtime_output_root": f"tmp/artifacts/showcase/{example_id}/runtime",
        "expected_exit_code": {"auroraBoard": 33, "signalMesh": 13, "patchKit": 7}[example_id],
    }:
        return f"workspace manifest runtime_surface drifted for {example_id}"
    presentation = workspace_payload.get("presentation")
    expected_headlines = {
        "auroraBoard": "categories, reflection, synthesized behaviors",
        "signalMesh": "status bridging, actor-shaped messaging, runtime messaging",
        "patchKit": "derives, macros, property behaviors, interop",
    }
    if presentation != {
        "title": entry.get("title"),
        "headline": expected_headlines[example_id],
    }:
        return f"workspace manifest presentation drifted for {example_id}"
    return None
