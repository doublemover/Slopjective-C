from __future__ import annotations

import json
from pathlib import Path
from typing import Any

REQUIRED_OWNER_ROLES = (
    "semantic_model_owner",
    "lowering_abi_owner",
    "runtime_abi_owner",
    "executable_proof_owner",
    "artifact_owner",
    "boundary_inventory_owner",
    "scheduler_task_lifecycle_owner",
    "bridge_policy_owner",
)

REQUIRED_OWNER_POLICY = {
    "source_authority": "checked-in-runtime-closure-owner-contracts",
    "missing_artifact_behavior": "fail-closed",
    "report_only_allowed": False,
    "fallback_allowed": False,
    "compatibility_shims_allowed": False,
    "generated_reports_are_source": False,
    "public_claims_require_executable_proof": True,
}


def read_json_object(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"{path} did not contain a JSON object")
    return payload


def owner_contract_path(root: Path, boundary_contract: dict[str, Any]) -> Path:
    rel_path = boundary_contract.get("owner_split_contract")
    if not isinstance(rel_path, str) or not rel_path:
        raise KeyError("boundary contract did not publish owner_split_contract")
    return root / rel_path


def load_runtime_closure_owner_contract(
    root: Path, boundary_contract: dict[str, Any]
) -> dict[str, Any]:
    return read_json_object(owner_contract_path(root, boundary_contract))


def runtime_closure_owner_checks(
    root: Path,
    boundary_contract: dict[str, Any],
    owner_contract: dict[str, Any],
) -> dict[str, bool]:
    roles = owner_contract.get("role_contracts")
    policy = owner_contract.get("owner_policy")
    hard_cutover = owner_contract.get("hard_cutover_requirements")
    blocked_claims = owner_contract.get("blocked_claim_shapes")

    source_paths: list[str] = []
    summary_scripts: list[str] = []
    if isinstance(roles, dict):
        for role in REQUIRED_OWNER_ROLES:
            entry = roles.get(role)
            if not isinstance(entry, dict):
                continue
            source = entry.get("source")
            summary_script = entry.get("summary_script")
            if isinstance(source, str):
                source_paths.append(source)
            if isinstance(summary_script, str):
                summary_scripts.append(summary_script)

    return {
        "owner_split_contract_path_exists": owner_contract_path(root, boundary_contract).is_file(),
        "owner_contract_links_boundary_inventory": owner_contract.get("boundary_inventory")
        == boundary_contract.get("summary_implementation_anchor"),
        "all_required_owner_roles_published": isinstance(roles, dict)
        and set(REQUIRED_OWNER_ROLES).issubset(roles),
        "all_owner_roles_have_source_and_owner": isinstance(roles, dict)
        and all(
            isinstance(roles.get(role), dict)
            and isinstance(roles[role].get("owner"), str)
            and roles[role].get("owner")
            and isinstance(roles[role].get("source"), str)
            and roles[role].get("source")
            for role in REQUIRED_OWNER_ROLES
        ),
        "all_owner_sources_are_checked_in": all((root / path).is_file() for path in source_paths),
        "all_summary_scripts_are_checked_in": all((root / path).is_file() for path in summary_scripts),
        "owner_policy_is_fail_closed": isinstance(policy, dict)
        and all(policy.get(key) == value for key, value in REQUIRED_OWNER_POLICY.items()),
        "hard_cutover_requirements_forbid_fallbacks": isinstance(hard_cutover, list)
        and "no-fallback-runtime-closure-claims" in hard_cutover
        and "no-report-only-runtime-closure-publication" in hard_cutover
        and "no-generated-report-as-source-authority" in hard_cutover,
        "blocked_claim_shapes_cover_report_only_and_fallbacks": isinstance(blocked_claims, list)
        and "fallback-runtime-behavior" in blocked_claims
        and "report-only-runtime-closure" in blocked_claims
        and "compatibility-shim-runtime-closure" in blocked_claims,
    }


def runtime_closure_owner_summary(owner_contract: dict[str, Any]) -> dict[str, Any]:
    roles = owner_contract.get("role_contracts")
    policy = owner_contract.get("owner_policy")
    return {
        "owner_contract_id": owner_contract.get("contract_id"),
        "owner_family": owner_contract.get("family"),
        "owner_role_count": len(roles) if isinstance(roles, dict) else 0,
        "owner_roles": sorted(roles) if isinstance(roles, dict) else [],
        "report_only_allowed": (
            policy.get("report_only_allowed") if isinstance(policy, dict) else None
        ),
        "fallback_allowed": (
            policy.get("fallback_allowed") if isinstance(policy, dict) else None
        ),
        "missing_artifact_behavior": (
            policy.get("missing_artifact_behavior") if isinstance(policy, dict) else None
        ),
    }


__all__ = [
    "REQUIRED_OWNER_POLICY",
    "REQUIRED_OWNER_ROLES",
    "load_runtime_closure_owner_contract",
    "runtime_closure_owner_checks",
    "runtime_closure_owner_summary",
]
