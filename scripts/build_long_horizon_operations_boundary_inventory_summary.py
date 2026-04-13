#!/usr/bin/env python3
"""Build the long-horizon operations boundary inventory summary."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "long_horizon_operations" / "boundary_inventory.json"
PACKAGE_JSON = ROOT / "package.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "long-horizon-operations" / "boundary-inventory-summary.json"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


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
    required_public_scripts = [str(name) for name in contract["required_existing_public_scripts"]]
    missing_paths = missing_files(checked_paths)
    missing_public_scripts = [name for name in required_public_scripts if name not in scripts]

    payload = {
        "contract_id": "objc3c.long_horizon_operations.boundary_inventory.summary.v1",
        "status": "PASS" if not missing_paths and not missing_public_scripts else "FAIL",
        "boundary_contract": repo_rel(CONTRACT_PATH),
        "runbook": str(contract["runbook"]),
        "working_scope_count": len(contract["working_scope"]),
        "non_goal_count": len(contract["non_goals"]),
        "substrate_runbook_count": len(contract["substrate_runbooks"]),
        "substrate_fixture_surface_count": len(contract["substrate_fixture_surfaces"]),
        "substrate_script_count": len(contract["substrate_scripts"]),
        "required_existing_public_script_count": len(required_public_scripts),
        "successor_surface_count": len(contract["successor_surfaces"]),
        "checked_paths": sorted(set(checked_paths)),
        "missing_paths": missing_paths,
        "required_existing_public_scripts": required_public_scripts,
        "missing_public_scripts": missing_public_scripts,
        "machine_owned_output_roots": contract["machine_owned_output_roots"],
        "working_scope": contract["working_scope"],
        "non_goals": contract["non_goals"],
        "successor_surfaces": contract["successor_surfaces"],
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("long-horizon-boundary-inventory: PASS" if payload["status"] == "PASS" else "long-horizon-boundary-inventory: FAIL")
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
