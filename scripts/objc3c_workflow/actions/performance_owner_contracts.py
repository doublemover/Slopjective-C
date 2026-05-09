"""Performance owner-contract helpers."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from ..environment import ROOT

OWNER_CONTRACTS_JSON = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "performance_governance"
    / "owner_contracts.json"
)
OWNER_CONTRACT_ID = "objc3c.performance.governance.owner.contracts.v1"


def load_performance_owner_contracts(path: Path = OWNER_CONTRACTS_JSON) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def owner_contract_index(
    contract: dict[str, Any],
    collection_name: str,
    key_name: str,
) -> dict[str, dict[str, Any]]:
    entries = contract.get(collection_name, [])
    if not isinstance(entries, list):
        return {}
    indexed: dict[str, dict[str, Any]] = {}
    for entry in entries:
        if isinstance(entry, dict) and isinstance(entry.get(key_name), str):
            indexed[str(entry[key_name])] = entry
    return indexed


def forbidden_performance_claim_classes(contract: dict[str, Any]) -> tuple[str, ...]:
    restrictions = contract.get("claim_restrictions", {})
    if not isinstance(restrictions, dict):
        return ()
    values = restrictions.get("forbidden_evidence_classes", [])
    if not isinstance(values, list):
        return ()
    return tuple(value for value in values if isinstance(value, str))


def required_public_claim_inputs(contract: dict[str, Any]) -> tuple[str, ...]:
    restrictions = contract.get("claim_restrictions", {})
    if not isinstance(restrictions, dict):
        return ()
    values = restrictions.get("required_public_claim_inputs", [])
    if not isinstance(values, list):
        return ()
    return tuple(value for value in values if isinstance(value, str))


def validate_performance_owner_contract_shape(contract: dict[str, Any]) -> list[str]:
    failures: list[str] = []
    if contract.get("contract_id") != OWNER_CONTRACT_ID:
        failures.append("contract_id drifted")

    benchmark_owners = owner_contract_index(
        contract,
        "benchmark_source_owners",
        "owner_id",
    )
    lab_owners = owner_contract_index(contract, "lab_owners", "lab_profile_id")
    budget_owners = owner_contract_index(contract, "budget_owners", "budget_id")
    breach_owners = owner_contract_index(contract, "breach_owners", "breach_id")

    if not benchmark_owners:
        failures.append("benchmark source owners are missing")
    if not lab_owners:
        failures.append("lab owners are missing")
    if not budget_owners:
        failures.append("budget owners are missing")
    if not breach_owners:
        failures.append("breach owners are missing")

    known_breach_owner_ids = {owner.get("owner_id") for owner in breach_owners.values()}
    for budget_id, budget_owner in budget_owners.items():
        source_owner_id = budget_owner.get("benchmark_source_owner_id")
        if source_owner_id not in benchmark_owners:
            failures.append(f"{budget_id} references an unknown benchmark source owner")
        for breach_owner_id in budget_owner.get("breach_owner_ids", []):
            if breach_owner_id not in known_breach_owner_ids:
                failures.append(f"{budget_id} references an unknown breach owner")

    for owner_id, source_owner in benchmark_owners.items():
        for lab_profile_id in source_owner.get("lab_profile_ids", []):
            if lab_profile_id not in lab_owners:
                failures.append(f"{owner_id} references an unknown lab profile")
        if not source_owner.get("report_paths"):
            failures.append(f"{owner_id} does not name report paths")

    if not forbidden_performance_claim_classes(contract):
        failures.append("forbidden performance claim classes are missing")
    if not required_public_claim_inputs(contract):
        failures.append("public claim inputs are missing")

    return failures


__all__ = [
    "OWNER_CONTRACTS_JSON",
    "OWNER_CONTRACT_ID",
    "forbidden_performance_claim_classes",
    "load_performance_owner_contracts",
    "owner_contract_index",
    "required_public_claim_inputs",
    "validate_performance_owner_contract_shape",
]
