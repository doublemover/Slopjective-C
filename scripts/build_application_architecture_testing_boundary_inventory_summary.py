#!/usr/bin/env python3
"""Build the application-architecture/testing boundary inventory summary."""

from __future__ import annotations

from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from scripts.objc3c_workflow.public_command_api import public_workflow_action_names


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "application_architecture_testing" / "boundary_inventory.json"
PACKAGE_JSON = ROOT / "package.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "application-architecture-testing" / "boundary-inventory-summary.json"




def main() -> int:
    contract = load_json(CONTRACT_PATH)
    package = load_json(PACKAGE_JSON)
    portfolio = load_json(ROOT / str(contract["portfolio_manifest"]))
    stdlib_workspace = load_json(ROOT / str(contract["stdlib_workspace_contract"]))

    tutorial_docs: list[str] = []
    for raw_root in contract["tutorial_roots"]:
        tutorial_root = ROOT / str(raw_root)
        tutorial_docs.extend(sorted(repo_rel(path) for path in tutorial_root.glob("*.md")))

    showcase_examples = portfolio.get("examples", [])
    showcase_workspace_manifests = [
        str(entry["workspace_manifest"])
        for entry in showcase_examples
        if isinstance(entry, dict) and isinstance(entry.get("workspace_manifest"), str)
    ]
    showcase_workspace_filesystem = sorted(repo_rel(path) for path in (ROOT / "showcase").glob("*/workspace.json"))

    template_materializers = [
        str(path) for path in contract["template_materializer_implementation_anchors"]
    ]
    existing_testing_surfaces = [
        str(path) for path in contract["testing_surface_implementation_anchors"]
    ]
    required_actions = [str(name) for name in contract["required_actions"]]
    package_bridge = str(contract["package_bridge"])
    package_scripts = package.get("scripts", {})
    if not isinstance(package_scripts, dict):
        raise RuntimeError("package.json scripts field drifted from an object")
    registered_actions = set(public_workflow_action_names())

    missing_paths = [
        raw_path
        for raw_path in template_materializers + existing_testing_surfaces
        if not (ROOT / raw_path).is_file()
    ]
    missing_actions = [name for name in required_actions if name not in registered_actions]
    package_bridge_exists = package_bridge in package_scripts

    payload = {
        "contract_id": "objc3c.application.architecture.testing.boundary.inventory.summary.v1",
        "status": "PASS" if not missing_paths and package_bridge_exists and not missing_actions else "FAIL",
        "boundary_contract": repo_rel(CONTRACT_PATH),
        "runbook": str(contract["runbook"]),
        "showcase_example_count": len(showcase_examples) if isinstance(showcase_examples, list) else 0,
        "showcase_workspace_manifest_count": len(showcase_workspace_manifests),
        "showcase_workspace_filesystem_count": len(showcase_workspace_filesystem),
        "tutorial_doc_count": len(tutorial_docs),
        "template_materializer_count": len(template_materializers),
        "existing_testing_surface_count": len(existing_testing_surfaces),
        "required_action_count": len(required_actions),
        "package_bridge_count": 1 if package_bridge_exists else 0,
        "direct_successor_milestone_count": len(contract["direct_successor_milestones"]),
        "stdlib_workspace_contract_id": stdlib_workspace.get("contract_id"),
        "showcase_workspace_manifests": showcase_workspace_manifests,
        "showcase_workspace_filesystem": showcase_workspace_filesystem,
        "tutorial_docs": tutorial_docs,
        "template_materializers": template_materializers,
        "existing_testing_surfaces": existing_testing_surfaces,
        "required_actions": required_actions,
        "package_bridge": package_bridge,
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
    print("application-architecture-testing-boundary-inventory: PASS" if payload["status"] == "PASS" else "application-architecture-testing-boundary-inventory: FAIL")
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
