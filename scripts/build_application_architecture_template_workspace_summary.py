#!/usr/bin/env python3
"""Build the project-template/workspace semantics summary."""

from __future__ import annotations

from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file


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
    missing_paths = [raw for raw in [str(contract["template_materializer"]), *checked_in_source_roots] if not (ROOT / raw).exists()]
    public_scripts = [str(name) for name in contract["public_scripts"]]
    missing_public_scripts = [name for name in public_scripts if name not in package_scripts]
    missing_actions = [action for action in contract["public_actions"] if f"\"{action}\"" not in materializer_source and action != contract["playground_materializer_action"]]

    payload = {
        "contract_id": "objc3c.application.architecture.testing.project_template_workspace_semantics.summary.v1",
        "status": "PASS" if not missing_paths and not missing_public_scripts and not missing_actions else "FAIL",
        "template_contract": repo_rel(CONTRACT_PATH),
        "runbook": str(contract["runbook"]),
        "checked_in_source_root_count": len(checked_in_source_roots),
        "public_action_count": len(contract["public_actions"]),
        "public_script_count": len(public_scripts),
        "generated_template_required_path_count": len(contract["generated_template_layout"]["required_paths"]),
        "generated_harness_required_path_count": len(contract["generated_harness_layout"]["required_paths"]),
        "template_contract_id": contract["template_contract_id"],
        "template_harness_contract_id": contract["template_harness_contract_id"],
        "playground_workspace_contract_id": contract["playground_workspace_contract_id"],
        "workspace_semantics": contract["workspace_semantics"],
        "public_actions": contract["public_actions"],
        "public_scripts": public_scripts,
        "missing_paths": missing_paths,
        "missing_public_scripts": missing_public_scripts,
        "missing_actions": missing_actions,
        "non_goals": contract["non_goals"],
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("application-architecture-template-workspace: PASS" if payload["status"] == "PASS" else "application-architecture-template-workspace: FAIL")
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
