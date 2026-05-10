#!/usr/bin/env python3
"""Build the package-ecosystem artifact contract summary."""

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
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "package_ecosystem" / "artifact_contract.json"
PACKAGE_JSON = ROOT / "package.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "package-ecosystem" / "artifact-contract-summary.json"




def main() -> int:
    contract = load_json(CONTRACT_PATH)
    owner_policy = require_package_ecosystem_owner_policy(contract, surface_name="package ecosystem artifact contract")
    blocker_metadata = require_package_ecosystem_blocker_metadata(
        contract,
        surface_name="package ecosystem artifact contract",
        required_blockers=("package artifact generated without checked-in source contract",),
    )
    package = load_json(PACKAGE_JSON)
    runbook_text = (ROOT / str(contract["runbook"])).read_text(encoding="utf-8")
    boundary = load_json(ROOT / str(contract["boundary_inventory"]))
    lock_policy = load_json(ROOT / str(contract["dependency_lock_policy"]))
    mirror_semantics = load_json(ROOT / str(contract["local_workspace_mirror_semantics"]))
    registry_semantics = load_json(ROOT / str(contract["registry_publication_semantics"]))

    package_scripts = package.get("scripts", {})
    if not isinstance(package_scripts, dict):
        raise RuntimeError("package.json scripts field drifted from an object")

    schemas = contract.get("schemas", {})
    schema_paths = [str(path) for path in schemas.values()] if isinstance(schemas, dict) else []
    report_slots = [str(slot) for slot in contract["report_slots"]]
    generated_artifact_roots = [str(root) for root in contract["generated_artifact_roots"]]
    package_bridge = str(contract["package_bridge"])
    package_bridge_exists = package_bridge in package_scripts
    required_actions = [str(name) for name in contract["required_actions"]]
    registered_actions = set(public_workflow_action_names())
    source_contract_paths = [
        str(contract["boundary_inventory"]),
        str(contract["dependency_lock_policy"]),
        str(contract["local_workspace_mirror_semantics"]),
        str(contract["registry_publication_semantics"]),
        *schema_paths,
    ]
    missing_paths = [path for path in source_contract_paths if not (ROOT / path).is_file()]
    missing_actions = [name for name in required_actions if name not in registered_actions]

    package_lock_schema = load_json(ROOT / str(schemas.get("package_lock", ""))) if isinstance(schemas, dict) and schemas.get("package_lock") else {}
    mirror_schema = load_json(ROOT / str(schemas.get("offline_mirror_index", ""))) if isinstance(schemas, dict) and schemas.get("offline_mirror_index") else {}
    claim_rules = contract.get("artifact_claim_rules", [])
    checks = {
        "runbook_mentions_artifact_contract": "tests/tooling/fixtures/package_ecosystem/artifact_contract.json" in runbook_text,
        "runbook_mentions_lock_schema": "schemas/objc3c-package-lock-v1.schema.json" in runbook_text,
        "runbook_mentions_mirror_schema": "schemas/objc3c-package-offline-mirror-index-v1.schema.json" in runbook_text,
        "boundary_contract_linked": boundary.get("contract_id") == "objc3c.package_ecosystem.boundary_inventory.v1",
        "lock_policy_linked": lock_policy.get("contract_id") == "objc3c.package_ecosystem.dependency_lock_policy.v1",
        "mirror_semantics_linked": mirror_semantics.get("contract_id") == "objc3c.package_ecosystem.local_workspace_mirror_semantics.v1",
        "registry_semantics_linked": registry_semantics.get("contract_id") == "objc3c.package_ecosystem.registry_publication_semantics.v1",
        "lock_schema_contract": package_lock_schema.get("$id") == "objc3c-package-lock-v1",
        "mirror_schema_contract": mirror_schema.get("$id") == "objc3c-package-offline-mirror-index-v1",
        "artifact_roots_under_tmp": all(root.startswith("tmp/artifacts/package-ecosystem/") for root in generated_artifact_roots),
        "report_root_under_tmp": str(contract["generated_report_root"]).startswith("tmp/reports/package-ecosystem"),
        "hosted_registry_claim_blocked": "hosted registry claims remain release-blocking until a later hosted-service evidence path exists" in claim_rules,
    }
    ok = not missing_paths and package_bridge_exists and not missing_actions and all(checks.values())

    payload = {
        "contract_id": "objc3c.package_ecosystem.artifact_contract.summary.v1",
        "status": "PASS" if ok else "FAIL",
        "artifact_contract": repo_rel(CONTRACT_PATH),
        "runbook": str(contract["runbook"]),
        "generated_report_root": str(contract["generated_report_root"]),
        "schema_count": len(schema_paths),
        "report_slot_count": len(report_slots),
        "source_contract_count": 4,
        "generated_artifact_root_count": len(generated_artifact_roots),
        "required_action_count": len(required_actions),
        "package_bridge_count": 1 if package_bridge_exists else 0,
        "schemas": schemas,
        "report_slots": report_slots,
        "generated_artifact_roots": generated_artifact_roots,
        "required_actions": required_actions,
        "package_bridge": package_bridge,
        "owner_policy": owner_policy,
        "blocker_metadata": blocker_metadata,
        "artifact_claim_rules": claim_rules,
        "missing_paths": missing_paths,
        "missing_actions": missing_actions,
        "missing_package_bridge": [] if package_bridge_exists else [package_bridge],
        "checks": checks,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("package-ecosystem-artifact-contract: PASS" if ok else "package-ecosystem-artifact-contract: FAIL")
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
