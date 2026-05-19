from __future__ import annotations

import json
from pathlib import Path
from typing import Any

REQUIRED_BOUNDARY_FAILURE_KEYS = (
    "missing_owner_split_contract",
    "missing_authoritative_code_path",
    "missing_authoritative_runtime_symbol",
    "missing_authoritative_probe_path",
    "missing_authoritative_fixture_path",
    "missing_public_workflow_action",
    "missing_executable_proof",
)

REQUIRED_CLOSURE_PUBLICATION_CONTRACT = {
    "source_owner_role": "semantic_model_owner",
    "runtime_owner_role": "runtime_abi_owner",
    "executable_proof_owner_role": "executable_proof_owner",
    "claim_publication_mode": "checked-in-owner-contract-plus-executable-proof",
    "missing_artifact_behavior": "fail-closed",
    "evidence_log_executable_proof_claims_allowed": False,
    "retired_route_runtime_semantics_allowed": False,
    "compatibility_runtime_semantics_allowed": False,
    "wrapper_only_runnable_actions_allowed": False,
}

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
    "evidence_log_allowed": False,
    "retired_route_allowed": False,
    "compatibility_gates_allowed": False,
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
    publication_contract = owner_contract.get("closure_publication_contract")
    boundary_failures = boundary_contract.get("fail_closed_boundary_inventory")
    public_workflow_actions = boundary_contract.get("public_workflow_actions")

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
        "closure_publication_contract_is_hard_cutover": isinstance(
            publication_contract, dict
        )
        and all(
            publication_contract.get(key) == value
            for key, value in REQUIRED_CLOSURE_PUBLICATION_CONTRACT.items()
        ),
        "closure_publication_roles_are_checked_in": isinstance(
            publication_contract, dict
        )
        and isinstance(roles, dict)
        and all(
            publication_contract.get(role_key) in roles
            for role_key in (
                "source_owner_role",
                "runtime_owner_role",
                "executable_proof_owner_role",
            )
        ),
        "boundary_inventory_failure_modes_are_fail_closed": isinstance(
            boundary_failures, dict
        )
        and all(
            boundary_failures.get(key) == "fail-closed"
            for key in REQUIRED_BOUNDARY_FAILURE_KEYS
        ),
        "boundary_inventory_publishes_workflow_actions": isinstance(
            public_workflow_actions, list
        )
        and all(isinstance(action, str) and action for action in public_workflow_actions),
        "hard_cutover_requirements_forbid_retired_routes": isinstance(hard_cutover, list)
        and "no-retired-route-runtime-closure-claims" in hard_cutover
        and "no-evidence-log-runtime-closure-publication" in hard_cutover
        and "no-generated-report-as-source-authority" in hard_cutover,
        "blocked_claim_shapes_cover_evidence_log_and_retired_routes": isinstance(blocked_claims, list)
        and "retired-route-runtime-behavior" in blocked_claims
        and "evidence-log-runtime-closure" in blocked_claims
        and "compatibility-gate-runtime-closure" in blocked_claims,
    }


def runtime_closure_owner_summary(owner_contract: dict[str, Any]) -> dict[str, Any]:
    roles = owner_contract.get("role_contracts")
    policy = owner_contract.get("owner_policy")
    publication_contract = owner_contract.get("closure_publication_contract")
    return {
        "owner_contract_id": owner_contract.get("contract_id"),
        "owner_family": owner_contract.get("family"),
        "owner_role_count": len(roles) if isinstance(roles, dict) else 0,
        "owner_roles": sorted(roles) if isinstance(roles, dict) else [],
        "evidence_log_allowed": (
            policy.get("evidence_log_allowed") if isinstance(policy, dict) else None
        ),
        "retired_route_allowed": (
            policy.get("retired_route_allowed") if isinstance(policy, dict) else None
        ),
        "missing_artifact_behavior": (
            policy.get("missing_artifact_behavior") if isinstance(policy, dict) else None
        ),
        "claim_publication_mode": (
            publication_contract.get("claim_publication_mode")
            if isinstance(publication_contract, dict)
            else None
        ),
        "wrapper_only_runnable_actions_allowed": (
            publication_contract.get("wrapper_only_runnable_actions_allowed")
            if isinstance(publication_contract, dict)
            else None
        ),
    }


__all__ = [
    "REQUIRED_BOUNDARY_FAILURE_KEYS",
    "REQUIRED_CLOSURE_PUBLICATION_CONTRACT",
    "REQUIRED_OWNER_POLICY",
    "REQUIRED_OWNER_ROLES",
    "load_runtime_closure_owner_contract",
    "runtime_closure_owner_checks",
    "runtime_closure_owner_summary",
]
