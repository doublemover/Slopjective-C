#!/usr/bin/env python3
"""Build the first-party testing semantics summary."""

from __future__ import annotations

from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from scripts.objc3c_workflow.public_command_api import public_workflow_action_names


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "application_architecture_testing" / "first_party_testing_semantics.json"
PACKAGE_JSON = ROOT / "package.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "application-architecture-testing" / "first-party-testing-semantics-summary.json"




def main() -> int:
    contract = load_json(CONTRACT_PATH)
    package = load_json(PACKAGE_JSON)
    package_scripts = package.get("scripts", {})
    if not isinstance(package_scripts, dict):
        raise RuntimeError("package.json scripts field drifted from an object")

    required_paths = [str(contract["workflow_runner"]), *[str(path) for path in contract["checked_in_fixture_roots"]]]
    missing_paths = []
    for raw in required_paths:
      path = ROOT / raw
      if not path.exists():
        missing_paths.append(raw)

    package_bridge = str(contract["package_bridge"])
    package_bridge_exists = package_bridge in package_scripts
    required_actions = [str(name) for name in contract["required_actions"]]
    registered_actions = set(public_workflow_action_names())
    missing_actions = [name for name in required_actions if name not in registered_actions]

    tooling_test_count = len(list((ROOT / "tests" / "tooling").glob("test_*.py")))
    showcase_workspace_count = len(list((ROOT / "showcase").glob("*/workspace.json")))
    tutorial_doc_count = len(list((ROOT / "docs" / "tutorials").glob("*.md")))

    payload = {
        "contract_id": "objc3c.application.architecture.testing.first_party_testing_semantics.summary.v1",
        "status": "PASS" if not missing_paths and package_bridge_exists and not missing_actions else "FAIL",
        "testing_contract": repo_rel(CONTRACT_PATH),
        "runbook": str(contract["runbook"]),
        "testing_layer_count": len(contract["testing_layers"]),
        "checked_in_fixture_root_count": len(contract["checked_in_fixture_roots"]),
        "generated_output_root_count": len(contract["generated_output_roots"]),
        "required_action_count": len(required_actions),
        "package_bridge_count": 1 if package_bridge_exists else 0,
        "tooling_test_count": tooling_test_count,
        "showcase_workspace_count": showcase_workspace_count,
        "tutorial_doc_count": tutorial_doc_count,
        "required_actions": required_actions,
        "package_bridge": package_bridge,
        "testing_layers": contract["testing_layers"],
        "fixture_rules": contract["fixture_rules"],
        "missing_paths": missing_paths,
        "missing_actions": missing_actions,
        "missing_package_bridge": [] if package_bridge_exists else [package_bridge],
        "non_goals": contract["non_goals"],
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("application-architecture-testing-semantics: PASS" if payload["status"] == "PASS" else "application-architecture-testing-semantics: FAIL")
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
