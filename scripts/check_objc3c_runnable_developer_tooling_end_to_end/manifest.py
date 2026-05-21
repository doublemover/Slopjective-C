"""Package manifest validation for runnable developer-tooling end-to-end checks."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import require_json_object as load_json

from .assertions import expect
from .constants import PACKAGE_CONTRACT_ID
from .paths import package_path

PACKAGED_FILE_FIELDS = (
    "developer_tooling_runbook",
    "developer_tooling_boundary_inventory",
    "developer_tooling_editor_surface_schema",
    "developer_tooling_navigation_contract",
    "developer_tooling_workspace_semantic_navigation_contract",
    "developer_tooling_formatter_debug_contract",
    "developer_tooling_formatter_rewrite_contract",
    "developer_tooling_diagnostic_quality_contract",
    "developer_tooling_workspace_contract",
    "developer_tooling_packaged_contract",
    "developer_tooling_runtime_debug_trace_script",
    "developer_tooling_runtime_debug_trace_schema",
    "developer_tooling_example_source",
    "developer_tooling_negative_source",
    "developer_tooling_formatter_source",
    "developer_tooling_expected_formatted_source",
)


def validate_manifest_contract(
    *,
    manifest: dict[str, Any],
    contract: dict[str, Any],
    package_root: Path,
) -> tuple[Any, str, Any]:
    expect(
        manifest.get("contract_id") == PACKAGE_CONTRACT_ID,
        "runnable toolchain package manifest published the wrong contract id",
    )

    for field in contract["manifest_fields"]:
        value = manifest.get(field)
        expect(value not in (None, "", []), f"package manifest did not publish {field}")

    for field in PACKAGED_FILE_FIELDS:
        candidate = package_path(package_root, str(manifest[field]))
        expect(
            candidate.is_file(),
            f"packaged runnable toolchain missing {field} at {manifest[field]}",
        )

    scripts = manifest.get("developer_tooling_scripts", {})
    expect(
        isinstance(scripts, dict),
        "package manifest did not publish developer_tooling_scripts",
    )
    for script_name, relative_path in scripts.items():
        expect(
            isinstance(relative_path, str) and relative_path,
            f"package manifest script entry {script_name} is empty",
        )
        expect(
            package_path(package_root, relative_path).is_file(),
            (
                "packaged runnable toolchain missing developer tooling script "
                f"{script_name} at {relative_path}"
            ),
        )

    command_surfaces = manifest.get("command_surfaces", {})
    expect(
        isinstance(command_surfaces, dict),
        "package manifest did not publish command_surfaces",
    )
    for command_name in contract["required_command_surfaces"]:
        expect(
            command_name in command_surfaces,
            f"package manifest missing developer tooling command surface: {command_name}",
        )
    expect(
        command_surfaces.get("runtime_debug_trace")
        == contract["expected_runtime_debug_trace_command_surface"],
        "package manifest runtime debug trace command surface drifted",
    )

    expected_runtime_fields = {
        "developer_tooling_runtime_debug_trace_script": "scripts/build_objc3c_runtime_debug_trace.py",
        "developer_tooling_runtime_debug_trace_schema": contract[
            "expected_runtime_debug_trace_schema"
        ],
        "developer_tooling_runtime_debug_trace_path": contract[
            "expected_runtime_debug_trace_path"
        ],
        "developer_tooling_runtime_debug_trace_model": contract[
            "expected_runtime_debug_trace_model"
        ],
    }
    for field, expected in expected_runtime_fields.items():
        expect(
            manifest.get(field) == expected,
            f"package manifest {field} drifted",
        )
    expect(
        scripts.get("runtime_debug_trace")
        == expected_runtime_fields["developer_tooling_runtime_debug_trace_script"],
        "package manifest runtime debug trace script surface drifted",
    )

    public_actions = manifest.get("developer_tooling_public_actions", [])
    package_bridge = str(contract["package_bridge"])
    manifest_package_bridge = manifest.get("package_bridge")
    for action in contract["public_actions"]:
        expect(
            action in public_actions,
            f"package manifest missing developer tooling public action: {action}",
        )
    expect(
        manifest_package_bridge == package_bridge,
        f"package manifest missing package bridge {package_bridge}",
    )
    return public_actions, package_bridge, manifest_package_bridge


def load_and_validate_manifest(
    *,
    contract: dict[str, Any],
    package_root: Path,
    manifest_path: Path,
) -> tuple[dict[str, Any], Any, str, Any]:
    manifest = load_json(manifest_path)
    public_actions, package_bridge, manifest_package_bridge = validate_manifest_contract(
        manifest=manifest,
        contract=contract,
        package_root=package_root,
    )
    return manifest, public_actions, package_bridge, manifest_package_bridge


__all__ = [
    "PACKAGED_FILE_FIELDS",
    "load_and_validate_manifest",
    "validate_manifest_contract",
]
