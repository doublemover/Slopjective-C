#!/usr/bin/env python3
"""Build the package-ecosystem local workspace and offline mirror semantics summary."""

from __future__ import annotations

from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file


ROOT = Path(__file__).resolve().parents[1]
SEMANTICS_PATH = ROOT / "tests" / "tooling" / "fixtures" / "package_ecosystem" / "local_workspace_mirror_semantics.json"
PACKAGE_JSON = ROOT / "package.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-ecosystem" / "local-workspace-mirror-semantics-summary.json"




def main() -> int:
    semantics = load_json(SEMANTICS_PATH)
    package = load_json(PACKAGE_JSON)
    runbook_text = (ROOT / str(semantics["runbook"])).read_text(encoding="utf-8")
    boundary = load_json(ROOT / str(semantics["boundary_inventory"]))
    lock_policy = load_json(ROOT / str(semantics["dependency_lock_policy"]))

    package_scripts = package.get("scripts", {})
    if not isinstance(package_scripts, dict):
        raise RuntimeError("package.json scripts field drifted from an object")

    workspace_inputs = semantics.get("workspace_inputs", [])
    workspace_paths: list[str] = []
    if isinstance(workspace_inputs, list):
        for workspace in workspace_inputs:
            if isinstance(workspace, dict):
                workspace_paths.extend(
                    str(workspace[field])
                    for field in ("source", "materializer", "validation_script")
                    if isinstance(workspace.get(field), str)
                )
    missing_paths = [
        path
        for path in [
            str(semantics["boundary_inventory"]),
            str(semantics["dependency_lock_policy"]),
            *workspace_paths,
        ]
        if not (ROOT / path).is_file()
    ]
    required_public_scripts = [str(name) for name in semantics["required_public_scripts"]]
    missing_public_scripts = [name for name in required_public_scripts if name not in package_scripts]

    generated_roots = semantics.get("generated_roots", {})
    lockfile_semantics = semantics.get("lockfile_semantics", {})
    offline_mirror_semantics = semantics.get("offline_mirror_semantics", {})
    workspace_rules = semantics.get("workspace_rules", [])
    non_goals = semantics.get("non_goals", [])
    checks = {
        "runbook_mentions_semantics": "tests/tooling/fixtures/package_ecosystem/local_workspace_mirror_semantics.json" in runbook_text,
        "boundary_contract_linked": boundary.get("contract_id") == "objc3c.package_ecosystem.boundary_inventory.v1",
        "lock_policy_linked": lock_policy.get("contract_id") == "objc3c.package_ecosystem.dependency_lock_policy.v1",
        "lock_root_under_tmp": str(generated_roots.get("lock_root", "")).startswith("tmp/artifacts/package-ecosystem/"),
        "mirror_root_under_tmp": str(generated_roots.get("mirror_root", "")).startswith("tmp/artifacts/package-ecosystem/"),
        "report_root_under_tmp": str(generated_roots.get("report_root", "")).startswith("tmp/reports/package-ecosystem"),
        "lock_ordering_stable": lockfile_semantics.get("ordering") == "stable-by-package-id-then-source-path",
        "mirror_no_network": offline_mirror_semantics.get("network_policy") == "no-network-during-validation",
        "mirror_lock_derived": offline_mirror_semantics.get("index_model") == "lock-derived-package-index",
    }
    ok = not missing_paths and not missing_public_scripts and all(checks.values())

    payload = {
        "contract_id": "objc3c.package_ecosystem.local_workspace_mirror_semantics.summary.v1",
        "status": "PASS" if ok else "FAIL",
        "semantics": repo_rel(SEMANTICS_PATH),
        "runbook": str(semantics["runbook"]),
        "boundary_inventory_contract_id": boundary.get("contract_id"),
        "dependency_lock_policy_contract_id": lock_policy.get("contract_id"),
        "workspace_input_count": len(workspace_inputs) if isinstance(workspace_inputs, list) else 0,
        "workspace_rule_count": len(workspace_rules) if isinstance(workspace_rules, list) else 0,
        "required_public_script_count": len(required_public_scripts),
        "non_goal_count": len(non_goals) if isinstance(non_goals, list) else 0,
        "workspace_inputs": workspace_inputs,
        "generated_roots": generated_roots,
        "lockfile_semantics": lockfile_semantics,
        "offline_mirror_semantics": offline_mirror_semantics,
        "workspace_rules": workspace_rules,
        "required_public_scripts": required_public_scripts,
        "non_goals": non_goals,
        "missing_paths": missing_paths,
        "missing_public_scripts": missing_public_scripts,
        "checks": checks,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("package-ecosystem-local-workspace-mirror-semantics: PASS" if ok else "package-ecosystem-local-workspace-mirror-semantics: FAIL")
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
