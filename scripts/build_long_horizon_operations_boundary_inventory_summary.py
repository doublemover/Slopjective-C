#!/usr/bin/env python3
"""Build the long-horizon operations boundary inventory summary."""

from __future__ import annotations

from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from scripts.objc3c_workflow.public_command_api import public_workflow_action_names


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "long_horizon_operations" / "boundary_inventory.json"
PACKAGE_JSON = ROOT / "package.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "long-horizon-operations" / "boundary-inventory-summary.json"




def missing_files(paths: list[str]) -> list[str]:
    return [path for path in paths if not (ROOT / path).is_file()]


def main() -> int:
    contract = load_json(CONTRACT_PATH)
    package = load_json(PACKAGE_JSON)
    scripts = package.get("scripts", {})
    if not isinstance(scripts, dict):
        raise RuntimeError("package.json scripts field drifted from an object")

    checked_paths = (
        [str(contract["runbook"])]
        + [str(path) for path in contract["substrate_runbooks"]]
        + [str(path) for path in contract["substrate_fixture_surfaces"]]
        + [str(path) for path in contract["substrate_scripts"]]
    )
    package_bridge = str(contract["package_bridge"])
    package_bridge_exists = package_bridge in scripts
    required_actions = [str(name) for name in contract["required_actions"]]
    registered_actions = set(public_workflow_action_names())
    missing_paths = missing_files(checked_paths)
    missing_actions = [name for name in required_actions if name not in registered_actions]

    payload = {
        "contract_id": "objc3c.long_horizon_operations.boundary_inventory.summary.v1",
        "status": "PASS" if not missing_paths and package_bridge_exists and not missing_actions else "FAIL",
        "boundary_contract": repo_rel(CONTRACT_PATH),
        "runbook": str(contract["runbook"]),
        "working_scope_count": len(contract["working_scope"]),
        "non_goal_count": len(contract["non_goals"]),
        "substrate_runbook_count": len(contract["substrate_runbooks"]),
        "substrate_fixture_surface_count": len(contract["substrate_fixture_surfaces"]),
        "substrate_script_count": len(contract["substrate_scripts"]),
        "required_action_count": len(required_actions),
        "package_bridge_count": 1 if package_bridge_exists else 0,
        "successor_surface_count": len(contract["successor_surfaces"]),
        "checked_paths": sorted(set(checked_paths)),
        "missing_paths": missing_paths,
        "required_actions": required_actions,
        "package_bridge": package_bridge,
        "missing_actions": missing_actions,
        "missing_package_bridge": [] if package_bridge_exists else [package_bridge],
        "machine_owned_output_roots": contract["machine_owned_output_roots"],
        "working_scope": contract["working_scope"],
        "non_goals": contract["non_goals"],
        "successor_surfaces": contract["successor_surfaces"],
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("long-horizon-boundary-inventory: PASS" if payload["status"] == "PASS" else "long-horizon-boundary-inventory: FAIL")
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
