#!/usr/bin/env python3
"""Build the adoption and evaluator-legibility boundary inventory summary."""

from __future__ import annotations

from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from scripts.objc3c_workflow.public_command_api import public_workflow_action_names


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "adoption_legibility" / "boundary_inventory.json"
PACKAGE_JSON = ROOT / "package.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "adoption-legibility" / "boundary-inventory-summary.json"




def missing_files(paths: list[str]) -> list[str]:
    return [path for path in paths if not (ROOT / path).is_file()]


def glob_files(root: str, pattern: str) -> list[str]:
    base = ROOT / root
    if not base.exists():
        return []
    return sorted(repo_rel(path) for path in base.rglob(pattern) if path.is_file())


def main() -> int:
    contract = load_json(CONTRACT_PATH)
    package = load_json(PACKAGE_JSON)
    scripts = package.get("scripts", {})
    if not isinstance(scripts, dict):
        raise RuntimeError("package.json scripts field drifted from an object")

    checked_paths = (
        [str(contract["runbook"])]
        + [str(path) for path in contract["primary_evaluator_surfaces"]]
        + [str(path) for path in contract["substrate_runbooks"]]
        + [str(path) for path in contract["substrate_fixture_surfaces"]]
        + [str(path) for path in contract["substrate_scripts"]]
    )
    required_actions = [str(name) for name in contract["required_actions"]]
    package_bridge = str(contract["package_bridge"])
    registered_actions = set(public_workflow_action_names())
    missing_paths = missing_files(checked_paths)
    missing_actions = [name for name in required_actions if name not in registered_actions]
    package_bridge_exists = package_bridge in scripts

    tutorial_docs = glob_files("docs/tutorials", "*.md")
    showcase_sources = glob_files("showcase", "*.objc3")
    showcase_workspaces = [path for path in glob_files("showcase", "workspace.json") if "/workspace.json" in path]
    site_docs = glob_files("site", "*.md")
    evaluator_actions = sorted(name for name in registered_actions if any(token in name for token in ("documentation", "site", "showcase", "application", "package", "conformance", "performance", "long-horizon")))

    failures = []
    if missing_paths:
        failures.append("missing checked boundary paths")
    if not package_bridge_exists:
        failures.append("missing package bridge")
    if missing_actions:
        failures.append("missing required workflow actions")
    if len(tutorial_docs) < 4:
        failures.append("tutorial surface is too narrow")
    if len(showcase_sources) < 3:
        failures.append("showcase source surface is too narrow")
    if len(showcase_workspaces) < 3:
        failures.append("showcase workspace surface is too narrow")
    if len(site_docs) < 1:
        failures.append("site documentation surface is missing")

    payload = {
        "contract_id": "objc3c.adoption_legibility.boundary_inventory.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "boundary_contract": repo_rel(CONTRACT_PATH),
        "runbook": str(contract["runbook"]),
        "working_scope_count": len(contract["working_scope"]),
        "non_goal_count": len(contract["non_goals"]),
        "primary_evaluator_surface_count": len(contract["primary_evaluator_surfaces"]),
        "substrate_runbook_count": len(contract["substrate_runbooks"]),
        "substrate_fixture_surface_count": len(contract["substrate_fixture_surfaces"]),
        "substrate_script_count": len(contract["substrate_scripts"]),
        "required_action_count": len(required_actions),
        "package_bridge_count": 1 if package_bridge_exists else 0,
        "tutorial_doc_count": len(tutorial_docs),
        "showcase_source_count": len(showcase_sources),
        "showcase_workspace_count": len(showcase_workspaces),
        "site_doc_count": len(site_docs),
        "evaluator_workflow_action_count": len(evaluator_actions),
        "successor_surface_count": len(contract["successor_surfaces"]),
        "checked_paths": sorted(set(checked_paths)),
        "missing_paths": missing_paths,
        "required_actions": required_actions,
        "missing_actions": missing_actions,
        "package_bridge": package_bridge,
        "missing_package_bridge": [] if package_bridge_exists else [package_bridge],
        "tutorial_docs": tutorial_docs,
        "showcase_sources": showcase_sources,
        "showcase_workspaces": showcase_workspaces,
        "site_docs": site_docs,
        "evaluator_workflow_actions": evaluator_actions,
        "measured_inventory_queries": contract["measured_inventory_queries"],
        "machine_owned_output_roots": contract["machine_owned_output_roots"],
        "working_scope": contract["working_scope"],
        "non_goals": contract["non_goals"],
        "successor_surfaces": contract["successor_surfaces"],
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("adoption-legibility-boundary-inventory: PASS" if not failures else "adoption-legibility-boundary-inventory: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
