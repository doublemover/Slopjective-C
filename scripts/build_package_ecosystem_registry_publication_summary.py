#!/usr/bin/env python3
"""Build the package-ecosystem registry and publication semantics summary."""

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
SEMANTICS_PATH = ROOT / "tests" / "tooling" / "fixtures" / "package_ecosystem" / "registry_publication_semantics.json"
PACKAGE_JSON = ROOT / "package.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-ecosystem" / "registry-publication-semantics-summary.json"




def main() -> int:
    semantics = load_json(SEMANTICS_PATH)
    owner_policy = require_package_ecosystem_owner_policy(semantics, surface_name="package ecosystem registry publication semantics")
    blocker_metadata = require_package_ecosystem_blocker_metadata(
        semantics,
        surface_name="package ecosystem registry publication semantics",
        required_blockers=("live public hosted registry claimed as supported",),
    )
    package = load_json(PACKAGE_JSON)
    runbook_text = (ROOT / str(semantics["runbook"])).read_text(encoding="utf-8")
    boundary = load_json(ROOT / str(semantics["boundary_inventory"]))
    lock_policy = load_json(ROOT / str(semantics["dependency_lock_policy"]))
    mirror_semantics = load_json(ROOT / str(semantics["local_workspace_mirror_semantics"]))

    package_scripts = package.get("scripts", {})
    if not isinstance(package_scripts, dict):
        raise RuntimeError("package.json scripts field drifted from an object")

    publication_inputs = [str(path) for path in semantics["publication_inputs"]]
    missing_paths = [
        path
        for path in [
            str(semantics["boundary_inventory"]),
            str(semantics["dependency_lock_policy"]),
            str(semantics["local_workspace_mirror_semantics"]),
            *publication_inputs,
        ]
        if not (ROOT / path).is_file()
    ]
    package_bridge = str(semantics["package_bridge"])
    package_bridge_exists = package_bridge in package_scripts
    required_actions = [str(name) for name in semantics["required_actions"]]
    registered_actions = set(public_workflow_action_names())
    missing_actions = [name for name in required_actions if name not in registered_actions]

    registry_layers = semantics.get("registry_layers", [])
    publication_rules = semantics.get("publication_rules", [])
    release_blocking_conditions = semantics.get("release_blocking_conditions", [])
    layer_states = {
        str(layer.get("layer_id")): str(layer.get("support_state"))
        for layer in registry_layers
        if isinstance(layer, dict)
    }
    checks = {
        "runbook_mentions_semantics": "tests/tooling/fixtures/package_ecosystem/registry_publication_semantics.json" in runbook_text,
        "boundary_contract_linked": boundary.get("contract_id") == "objc3c.package_ecosystem.boundary_inventory.v1",
        "lock_policy_linked": lock_policy.get("contract_id") == "objc3c.package_ecosystem.dependency_lock_policy.v1",
        "mirror_semantics_linked": mirror_semantics.get("contract_id") == "objc3c.package_ecosystem.local_workspace_mirror_semantics.v1",
        "local_index_supported": layer_states.get("local-index") == "supported-generated-artifact",
        "offline_mirror_supported": layer_states.get("offline-mirror") == "supported-generated-artifact",
        "publication_metadata_supported": layer_states.get("publication-metadata") == "supported-generated-artifact",
        "offline_restore_receipt_supported": layer_states.get("offline-restore-receipt") == "supported-generated-artifact",
        "hosted_registry_fixture_supported": layer_states.get("hosted-registry") == "supported-source-fixture-offline-only",
        "hosted_registry_service_supported": layer_states.get("hosted-registry-service") == "supported-source-fixture-hermetic-only",
        "release_blockers_include_hosted_claim": "live public hosted registry claimed as supported" in release_blocking_conditions,
        "release_blockers_include_cache_tamper": "offline mirror cache payload mismatch did not fail closed" in release_blocking_conditions,
    }
    ok = not missing_paths and package_bridge_exists and not missing_actions and all(checks.values())

    payload = {
        "contract_id": "objc3c.package_ecosystem.registry_publication_semantics.summary.v1",
        "status": "PASS" if ok else "FAIL",
        "semantics": repo_rel(SEMANTICS_PATH),
        "runbook": str(semantics["runbook"]),
        "boundary_inventory_contract_id": boundary.get("contract_id"),
        "dependency_lock_policy_contract_id": lock_policy.get("contract_id"),
        "local_workspace_mirror_semantics_contract_id": mirror_semantics.get("contract_id"),
        "publication_input_count": len(publication_inputs),
        "registry_layer_count": len(registry_layers) if isinstance(registry_layers, list) else 0,
        "publication_rule_count": len(publication_rules) if isinstance(publication_rules, list) else 0,
        "required_action_count": len(required_actions),
        "package_bridge_count": 1 if package_bridge_exists else 0,
        "release_blocking_condition_count": len(release_blocking_conditions) if isinstance(release_blocking_conditions, list) else 0,
        "publication_inputs": publication_inputs,
        "registry_layers": registry_layers,
        "publication_rules": publication_rules,
        "required_actions": required_actions,
        "package_bridge": package_bridge,
        "owner_policy": owner_policy,
        "blocker_metadata": blocker_metadata,
        "release_blocking_conditions": release_blocking_conditions,
        "missing_paths": missing_paths,
        "missing_actions": missing_actions,
        "missing_package_bridge": [] if package_bridge_exists else [package_bridge],
        "checks": checks,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("package-ecosystem-registry-publication-semantics: PASS" if ok else "package-ecosystem-registry-publication-semantics: FAIL")
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
