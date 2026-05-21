#!/usr/bin/env python3
"""Build the package-ecosystem boundary inventory summary."""

from __future__ import annotations

from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from scripts.objc3c_workflow.public_command_api import public_workflow_action_names
from package_ecosystem_contracts import (
    require_package_ecosystem_blocker_metadata,
    require_package_ecosystem_owner_policy,
)


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "package_ecosystem" / "boundary_inventory.json"
PACKAGE_JSON = ROOT / "package.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-ecosystem" / "boundary-inventory-summary.json"




def existing_files(paths: list[str]) -> list[str]:
    return [path for path in paths if (ROOT / path).is_file()]


def missing_files(paths: list[str]) -> list[str]:
    return [path for path in paths if not (ROOT / path).is_file()]


def main() -> int:
    contract = load_json(CONTRACT_PATH)
    owner_policy = require_package_ecosystem_owner_policy(contract, surface_name="package ecosystem boundary inventory")
    blocker_metadata = require_package_ecosystem_blocker_metadata(
        contract,
        surface_name="package ecosystem boundary inventory",
        required_blockers=("missing checked-in package source contract",),
    )
    package = load_json(PACKAGE_JSON)
    stdlib_workspace = load_json(ROOT / str(contract["stdlib_workspace_contract"]))
    stdlib_package_surface = load_json(ROOT / str(contract["stdlib_package_surface"]))
    advanced_helper_surface = load_json(ROOT / str(contract["advanced_helper_package_surface"]))
    app_arch_boundary = load_json(ROOT / str(contract["application_architecture_boundary"]))

    package_scripts = package.get("scripts", {})
    if not isinstance(package_scripts, dict):
        raise RuntimeError("package.json scripts field drifted from an object")

    runbooks = [str(path) for path in contract["release_and_package_channel_runbooks"]]
    schemas = [str(path) for path in contract["release_and_package_channel_schemas"]]
    generators = [
        str(path)
        for path in contract.get(
            "package_workflow_generators",
            contract.get("package_workflow_implementation_anchors", []),
        )
    ]
    validation_surfaces = [
        str(path)
        for path in contract.get(
            "existing_package_validation_surfaces",
            contract.get("package_validation_implementation_anchors", []),
        )
    ]
    package_bridge = str(contract["package_bridge"])
    package_bridge_exists = package_bridge in package_scripts
    required_actions = [str(name) for name in contract["required_actions"]]
    registered_actions = set(public_workflow_action_names())

    missing_paths = missing_files(runbooks + schemas + generators + validation_surfaces)
    missing_actions = [name for name in required_actions if name not in registered_actions]
    module_imports = stdlib_package_surface.get("module_imports", [])
    advanced_helper_modules = advanced_helper_surface.get("advanced_helper_modules", [])

    payload = {
        "contract_id": "objc3c.package_ecosystem.boundary_inventory.summary.v1",
        "status": "PASS" if not missing_paths and package_bridge_exists and not missing_actions else "FAIL",
        "boundary_contract": repo_rel(CONTRACT_PATH),
        "runbook": str(contract["runbook"]),
        "stdlib_workspace_contract_id": stdlib_workspace.get("contract_id"),
        "stdlib_package_surface_contract_id": stdlib_package_surface.get("contract_id"),
        "advanced_helper_package_surface_contract_id": advanced_helper_surface.get("contract_id"),
        "application_architecture_boundary_contract_id": app_arch_boundary.get("contract_id"),
        "module_import_count": len(module_imports) if isinstance(module_imports, list) else 0,
        "advanced_helper_module_count": len(advanced_helper_modules) if isinstance(advanced_helper_modules, list) else 0,
        "release_and_package_channel_runbook_count": len(runbooks),
        "release_and_package_channel_schema_count": len(schemas),
        "package_workflow_generator_count": len(generators),
        "existing_package_validation_surface_count": len(validation_surfaces),
        "required_action_count": len(required_actions),
        "package_bridge_count": 1 if package_bridge_exists else 0,
        "direct_successor_milestone_count": len(contract["direct_successor_milestones"]),
        "release_and_package_channel_runbooks": existing_files(runbooks),
        "release_and_package_channel_schemas": existing_files(schemas),
        "package_workflow_generators": existing_files(generators),
        "existing_package_validation_surfaces": existing_files(validation_surfaces),
        "required_actions": required_actions,
        "package_bridge": package_bridge,
        "owner_policy": owner_policy,
        "blocker_metadata": blocker_metadata,
        "missing_paths": missing_paths,
        "missing_actions": missing_actions,
        "missing_package_bridge": [] if package_bridge_exists else [package_bridge],
        "working_scope": contract["working_scope"],
        "non_goals": contract["non_goals"],
        "direct_successor_milestones": contract["direct_successor_milestones"],
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("package-ecosystem-boundary-inventory: PASS" if payload["status"] == "PASS" else "package-ecosystem-boundary-inventory: FAIL")
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
