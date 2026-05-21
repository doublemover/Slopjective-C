from __future__ import annotations

import json
from pathlib import Path

from scripts.objc3c_workflow.action_catalog_application_architecture import (
    APPLICATION_ARCHITECTURE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_application_developer_tooling import (
    APPLICATION_DEVELOPER_TOOLING_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_application_playground import (
    APPLICATION_PLAYGROUND_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_application_stdlib import (
    APPLICATION_STDLIB_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_application_stdlib_integrations import (
    APPLICATION_STDLIB_INTEGRATION_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_application_stdlib_runnable import (
    APPLICATION_STDLIB_RUNNABLE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_application_stdlib_workspace import (
    APPLICATION_STDLIB_WORKSPACE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_application_workspaces import (
    APPLICATION_WORKSPACE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_core_getting_started import (
    CORE_GETTING_STARTED_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_core_showcase import (
    CORE_SHOWCASE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_core_showcase_examples import (
    CORE_SHOWCASE_EXAMPLE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_core_stdlib_surface import (
    CORE_STDLIB_SURFACE_ACTION_SPECS,
)

ROOT = Path(__file__).resolve().parents[2]


def test_application_workspace_catalog_aggregates_owner_catalogs() -> None:
    assert tuple(APPLICATION_WORKSPACE_ACTION_SPECS) == (
        "materialize-playground-workspace",
        "materialize-canonical-application-workspace",
        "validate-application-architecture",
        "validate-application-framework-samples",
        "validate-runnable-application-architecture",
        "validate-runnable-developer-tooling",
    )
    assert (
        APPLICATION_WORKSPACE_ACTION_SPECS["materialize-playground-workspace"]
        is APPLICATION_PLAYGROUND_ACTION_SPECS["materialize-playground-workspace"]
    )
    for action, spec in APPLICATION_ARCHITECTURE_ACTION_SPECS.items():
        assert APPLICATION_WORKSPACE_ACTION_SPECS[action] is spec
    assert (
        APPLICATION_WORKSPACE_ACTION_SPECS["validate-runnable-developer-tooling"]
        is APPLICATION_DEVELOPER_TOOLING_ACTION_SPECS[
            "validate-runnable-developer-tooling"
        ]
    )


def test_application_stdlib_catalog_aggregates_owner_catalogs() -> None:
    assert tuple(APPLICATION_STDLIB_ACTION_SPECS) == (
        "materialize-stdlib-workspace",
        "validate-stdlib-foundation",
        "validate-stdlib-advanced",
        "validate-stdlib-program",
        "validate-runnable-stdlib-advanced",
        "validate-runnable-stdlib-foundation",
        "validate-runnable-stdlib-program",
    )

    owner_catalogs = (
        APPLICATION_STDLIB_WORKSPACE_ACTION_SPECS,
        APPLICATION_STDLIB_INTEGRATION_ACTION_SPECS,
        APPLICATION_STDLIB_RUNNABLE_ACTION_SPECS,
    )
    for catalog in owner_catalogs:
        for action, spec in catalog.items():
            assert APPLICATION_STDLIB_ACTION_SPECS[action] is spec


def test_core_showcase_catalog_preserves_public_order_and_owner_identity() -> None:
    assert tuple(CORE_SHOWCASE_ACTION_SPECS) == (
        "check-showcase-surface",
        "check-stdlib-surface",
        "validate-showcase-runtime",
        "validate-showcase",
        "validate-runnable-showcase",
        "validate-getting-started",
    )
    assert (
        CORE_SHOWCASE_ACTION_SPECS["check-showcase-surface"]
        is CORE_SHOWCASE_EXAMPLE_ACTION_SPECS["check-showcase-surface"]
    )
    assert (
        CORE_SHOWCASE_ACTION_SPECS["check-stdlib-surface"]
        is CORE_STDLIB_SURFACE_ACTION_SPECS["check-stdlib-surface"]
    )
    assert (
        CORE_SHOWCASE_ACTION_SPECS["validate-getting-started"]
        is CORE_GETTING_STARTED_ACTION_SPECS["validate-getting-started"]
    )


def test_showcase_portfolio_publishes_canonical_npm_command_truth() -> None:
    payload = json.loads(
        (ROOT / "showcase" / "portfolio.json").read_text(encoding="utf-8")
    )

    assert payload["public_command_model"] == {
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
    }
    assert "public_entrypoints" not in payload
    assert payload["public_actions"] == {
        "build_native": "build-native-binaries",
        "compile_example": "compile-objc3c",
        "check_surface": "check-showcase-surface",
        "validate_showcase": "validate-showcase",
        "validate_runnable_showcase": "validate-runnable-showcase",
        "package_runnable_toolchain": "package-runnable-toolchain",
        "execution_smoke": "test-execution-smoke",
        "execution_replay": "test-execution-replay",
    }
    assert payload["command_surfaces"] == {
        name: f"npm run objc3c -- {action}"
        for name, action in payload["public_actions"].items()
    }
    assert payload["build_run_package_surface"] == {
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
    }
    assert payload["runtime_presentation_surface"] == {
        "launch_contract_helper": "scripts/objc3c_runtime_launch_contract.ps1",
        "runtime_library_resolution_model": (
            "registration-manifest-runtime-archive-path-is-authoritative"
        ),
        "driver_linker_flag_consumption_model": (
            "registration-manifest-driver-linker-flags-feed-proof-and-smoke-link-commands"
        ),
        "integrated_validation_action": "validate-showcase",
        "packaged_validation_action": "validate-runnable-showcase",
        "shared_execution_smoke_action": "test-execution-smoke",
        "shared_execution_replay_action": "test-execution-replay",
        "presentation_readme": "showcase/README.md",
    }
    assert payload["demo_packages_manifest"] == "showcase/demo_packages.json"
    assert payload["application_framework_samples"] == {
        "manifest": "showcase/applicationFrameworkSamples/manifest.json",
        "readme": "showcase/applicationFrameworkSamples/README.md",
        "tutorial": "docs/tutorials/application-framework-samples.md",
        "dependency_evidence": "showcase/applicationFrameworkSamples/dependency-evidence.json",
        "contract_fixture": "tests/tooling/fixtures/application_framework_samples/contract.json",
        "checker": "scripts/check_objc3c_application_framework_samples.py",
        "summary": "tmp/reports/application-framework-samples/summary.json",
        "validate_action": "validate-application-framework-samples",
        "validate_command": "npm run objc3c -- validate-application-framework-samples",
        "package_action": "package-runnable-toolchain",
        "package_command": "npm run objc3c -- package-runnable-toolchain",
        "runnable_validation_action": "validate-runnable-application-architecture",
        "runnable_validation_command": (
            "npm run objc3c -- validate-runnable-application-architecture"
        ),
        "manifest_contract_id": "objc3c.application_framework_samples.v1",
        "contract_fixture_id": "objc3c.application_framework_samples.contract.v1",
        "package_manifest_fields": [
            "application_framework_samples",
            "application_architecture_public_actions",
            "command_surfaces",
            "copied_files",
        ],
        "sample_ids": [
            "routeModelKit",
            "interopAdapterKit",
            "workflowStdlibCLI",
            "asyncRuntimeConsole",
        ],
        "capability_rows": [
            "applications.framework-samples.object-runtime-library",
            "applications.framework-samples.interop-adapter-library",
            "applications.framework-samples.stdlib-text-collections-cli",
            "applications.framework-samples.async-runtime-application",
        ],
    }
    for path in (
        payload["application_framework_samples"]["manifest"],
        payload["application_framework_samples"]["readme"],
        payload["application_framework_samples"]["tutorial"],
        payload["application_framework_samples"]["dependency_evidence"],
        payload["application_framework_samples"]["contract_fixture"],
        payload["application_framework_samples"]["checker"],
    ):
        assert (ROOT / path).is_file()


def test_showcase_demo_packages_publish_reproducible_application_smokes() -> None:
    portfolio = json.loads(
        (ROOT / "showcase" / "portfolio.json").read_text(encoding="utf-8")
    )
    payload = json.loads(
        (ROOT / "showcase" / "demo_packages.json").read_text(encoding="utf-8")
    )

    assert payload["contract_id"] == "objc3c.showcase.demo.packages.v1"
    assert payload["reproducibility_contract"] == {
        "lockfile": "package-lock.json",
        "offline_mirror_validation_action": "validate-package-mirror",
        "tamper_rejection_diagnostic": "O3PKG8054",
        "mixed_version_policy": "same-package-manifest-version-required",
        "package_manifest_fields": [
            "showcase_demo_packages_manifest",
            "showcase_demo_packages",
            "copied_files",
        ],
    }
    assert payload["source_authority"]["forbidden_source_roots"] == ["tmp", "artifacts"]

    examples = {entry["id"]: entry for entry in portfolio["examples"]}
    packages = payload["packages"]
    assert [entry["package_id"] for entry in packages] == [
        "showcase:auroraBoard",
        "showcase:signalMesh",
        "showcase:patchKit",
    ]
    assert [entry["coverage_domain"] for entry in packages] == [
        "object-model",
        "concurrency",
        "interop",
    ]
    for package in packages:
        example = examples[package["example_id"]]
        assert package["source"] == example["source"]
        assert package["workspace_manifest"] == example["workspace_manifest"]
        assert package["required_story_capabilities"] == example["story_capabilities"]
        assert package["stdlib_modules"] == example["stdlib_followup_modules"]
        assert all(
            not path.startswith(("tmp/", "artifacts/"))
            for path in package["manifest_inputs"]
        )
        assert all(
            command.startswith("npm run objc3c -- ")
            for command in package["smoke_commands"]
        )
        assert (ROOT / package["source"]).is_file()
        assert (ROOT / package["workspace_manifest"]).is_file()


def test_showcase_walkthrough_steps_use_workflow_actions_and_public_commands() -> None:
    payload = json.loads(
        (ROOT / "showcase" / "tutorial_walkthrough.json").read_text(encoding="utf-8")
    )

    for step in payload["steps"]:
        assert "public_entrypoint" not in step
        action = step["workflow_action"]
        command = step["public_command"]
        assert command.startswith(f"npm run objc3c -- {action}")
        assert ":" not in action
        assert not command.startswith("python ")
        assert not command.startswith("pwsh ")


def test_stdlib_program_surface_records_canonical_command_model() -> None:
    payload = json.loads(
        (ROOT / "stdlib" / "program_surface.json").read_text(encoding="utf-8")
    )

    assert payload["public_command_model"] == {
        "entrypoint_kind": "single-npm-bridge",
        "canonical_template": "npm run objc3c -- <action>",
        "authoritative_script": "package.json#scripts.objc3c",
        "retired_public_semantics": [
            "retired command surfaces",
            "direct helper script commands",
            "stdlib-local wrappers",
            "alternate import or package support lanes",
            "retired-source support claims",
        ],
    }
    assert payload["onboarding_policy"]["command_truth_rule"] == (
        "package.json objc3c is the authoritative public npm entrypoint; "
        "workflow examples use npm run objc3c -- <action>"
    )
    for command in payload["command_surfaces"].values():
        assert command.startswith("npm run objc3c -- ")
        assert not command.startswith("python ")
        assert not command.startswith("pwsh ")


def test_site_index_contract_records_public_command_model() -> None:
    payload = json.loads(
        (ROOT / "site" / "src" / "index.contract.json").read_text(encoding="utf-8")
    )

    assert payload["public_command_model"] == {
        "entrypoint_kind": "single-npm-bridge",
        "canonical_template": "npm run objc3c -- <action>",
        "authoritative_script": "package.json#scripts.objc3c",
        "forbidden_public_command_semantics": [
            "retired package-script names",
            "direct helper script commands as public entrypoints",
            "alternate command support lanes",
            "retired-source support claims",
        ],
    }
