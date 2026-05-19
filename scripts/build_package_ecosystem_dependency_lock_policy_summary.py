#!/usr/bin/env python3
"""Build the package-ecosystem dependency lock policy summary."""

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
POLICY_PATH = ROOT / "tests" / "tooling" / "fixtures" / "package_ecosystem" / "dependency_lock_policy.json"
PACKAGE_JSON = ROOT / "package.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-ecosystem" / "dependency-lock-policy-summary.json"




def main() -> int:
    policy = load_json(POLICY_PATH)
    owner_policy = require_package_ecosystem_owner_policy(policy, surface_name="package ecosystem dependency lock policy")
    blocker_metadata = require_package_ecosystem_blocker_metadata(
        policy,
        surface_name="package ecosystem dependency lock policy",
        required_blockers=("missing dependency provenance",),
    )
    package = load_json(PACKAGE_JSON)
    runbook_text = (ROOT / str(policy["runbook"])).read_text(encoding="utf-8")
    boundary = load_json(ROOT / str(policy["boundary_inventory"]))

    package_scripts = package.get("scripts", {})
    if not isinstance(package_scripts, dict):
        raise RuntimeError("package.json scripts field drifted from an object")

    source_surfaces = [str(path) for path in policy["source_package_surfaces"]]
    missing_paths = [
        path
        for path in [str(policy["boundary_inventory"]), *source_surfaces]
        if not (ROOT / path).is_file()
    ]
    package_bridge = str(policy["package_bridge"])
    package_bridge_exists = package_bridge in package_scripts
    required_actions = [str(name) for name in policy["required_actions"]]
    registered_actions = set(public_workflow_action_names())
    missing_actions = [name for name in required_actions if name not in registered_actions]
    resolver_stages = policy.get("resolver_stages", [])
    lock_required_fields = policy.get("lock_required_fields", [])
    allowed_sources = policy.get("allowed_dependency_sources", [])
    forbidden_sources = policy.get("forbidden_dependency_sources", [])
    claim_rules = policy.get("claim_rules", [])
    release_blocking_conditions = policy.get("release_blocking_conditions", [])

    checks = {
        "runbook_mentions_policy": "tests/tooling/fixtures/package_ecosystem/dependency_lock_policy.json" in runbook_text,
        "local_first_policy": "checked-in-local-workspace" in allowed_sources,
        "offline_mirror_allowed": "offline-mirror-artifact" in allowed_sources,
        "network_fetch_forbidden": "implicit-network-fetch" in forbidden_sources,
        "tmp_source_forbidden": "tmp-only-source-of-truth" in forbidden_sources,
        "lock_has_replay_field": "replay" in lock_required_fields,
        "lock_has_provenance_field": "provenance" in lock_required_fields,
        "release_blockers_include_registry_overclaim": "registry claim without local package evidence" in release_blocking_conditions,
        "boundary_contract_linked": boundary.get("contract_id") == "objc3c.package_ecosystem.boundary_inventory.v1",
    }
    ok = not missing_paths and package_bridge_exists and not missing_actions and all(checks.values())

    payload = {
        "contract_id": "objc3c.package_ecosystem.dependency_lock_policy.summary.v1",
        "status": "PASS" if ok else "FAIL",
        "policy": repo_rel(POLICY_PATH),
        "runbook": str(policy["runbook"]),
        "boundary_inventory_contract_id": boundary.get("contract_id"),
        "source_package_surface_count": len(source_surfaces),
        "resolver_stage_count": len(resolver_stages) if isinstance(resolver_stages, list) else 0,
        "lock_required_field_count": len(lock_required_fields) if isinstance(lock_required_fields, list) else 0,
        "allowed_dependency_source_count": len(allowed_sources) if isinstance(allowed_sources, list) else 0,
        "forbidden_dependency_source_count": len(forbidden_sources) if isinstance(forbidden_sources, list) else 0,
        "claim_rule_count": len(claim_rules) if isinstance(claim_rules, list) else 0,
        "release_blocking_condition_count": len(release_blocking_conditions) if isinstance(release_blocking_conditions, list) else 0,
        "required_action_count": len(required_actions),
        "package_bridge_count": 1 if package_bridge_exists else 0,
        "source_package_surfaces": source_surfaces,
        "resolver_stages": resolver_stages,
        "lock_required_fields": lock_required_fields,
        "allowed_dependency_sources": allowed_sources,
        "forbidden_dependency_sources": forbidden_sources,
        "required_actions": required_actions,
        "package_bridge": package_bridge,
        "owner_policy": owner_policy,
        "blocker_metadata": blocker_metadata,
        "claim_rules": claim_rules,
        "release_blocking_conditions": release_blocking_conditions,
        "missing_paths": missing_paths,
        "missing_actions": missing_actions,
        "missing_package_bridge": [] if package_bridge_exists else [package_bridge],
        "checks": checks,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("package-ecosystem-dependency-lock-policy: PASS" if ok else "package-ecosystem-dependency-lock-policy: FAIL")
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
