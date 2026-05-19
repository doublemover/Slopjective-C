#!/usr/bin/env python3
"""Build the project-template/workspace semantics summary."""

from __future__ import annotations

from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from scripts.objc3c_workflow.public_command_api import public_workflow_action_names


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "application_architecture_testing" / "project_template_workspace_semantics.json"
PACKAGE_JSON = ROOT / "package.json"
MATERIALIZER_PATH = ROOT / "scripts" / "materialize_objc3c_project_template.py"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "application-architecture-testing" / "project-template-workspace-semantics-summary.json"




def main() -> int:
    contract = load_json(CONTRACT_PATH)
    package = load_json(PACKAGE_JSON)
    package_scripts = package.get("scripts", {})
    if not isinstance(package_scripts, dict):
        raise RuntimeError("package.json scripts field drifted from an object")

    materializer_source = MATERIALIZER_PATH.read_text(encoding="utf-8")
    checked_in_source_roots = [str(path) for path in contract["checked_in_source_roots"]]
    template_materializer = str(contract["template_materializer_implementation_anchor"])
    missing_paths = [
        raw for raw in [template_materializer, *checked_in_source_roots] if not (ROOT / raw).exists()
    ]
    package_bridge = str(contract["package_bridge"])
    package_bridge_exists = package_bridge in package_scripts
    required_actions = [str(name) for name in contract["required_actions"]]
    registered_actions = set(public_workflow_action_names())
    missing_actions = [action for action in required_actions if action not in registered_actions]
    materializer_bound_actions = [
        str(contract["template_materializer_action"]),
        "inspect-bonus-tool-integration",
        str(contract["playground_materializer_action"]),
        "benchmark-runtime-inspector",
    ]
    missing_materializer_actions = [
        action for action in materializer_bound_actions if f"\"{action}\"" not in materializer_source
    ]

    payload = {
        "contract_id": "objc3c.application.architecture.testing.project_template_workspace_semantics.summary.v1",
        "status": "PASS" if not missing_paths and package_bridge_exists and not missing_actions and not missing_materializer_actions else "FAIL",
        "template_contract": repo_rel(CONTRACT_PATH),
        "runbook": str(contract["runbook"]),
        "checked_in_source_root_count": len(checked_in_source_roots),
        "template_materializer_implementation_anchor": template_materializer,
        "required_action_count": len(required_actions),
        "materializer_bound_actions": materializer_bound_actions,
        "package_bridge_count": 1 if package_bridge_exists else 0,
        "generated_template_required_path_count": len(contract["generated_template_layout"]["required_paths"]),
        "generated_harness_required_path_count": len(contract["generated_harness_layout"]["required_paths"]),
        "template_contract_id": contract["template_contract_id"],
        "template_harness_contract_id": contract["template_harness_contract_id"],
        "playground_workspace_contract_id": contract["playground_workspace_contract_id"],
        "workspace_semantics": contract["workspace_semantics"],
        "required_actions": required_actions,
        "package_bridge": package_bridge,
        "missing_paths": missing_paths,
        "missing_actions": missing_actions,
        "missing_materializer_actions": missing_materializer_actions,
        "missing_package_bridge": [] if package_bridge_exists else [package_bridge],
        "non_goals": contract["non_goals"],
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("application-architecture-template-workspace: PASS" if payload["status"] == "PASS" else "application-architecture-template-workspace: FAIL")
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
