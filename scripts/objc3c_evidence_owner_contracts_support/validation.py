from __future__ import annotations

from pathlib import Path
from typing import Any

from .families import EVIDENCE_FAMILIES
from .source_owner_policy import HARD_CUTOVER_SOURCE_OWNER_CONTRACT


def owner_contract_ids(family_name: str) -> tuple[str, ...]:
    return tuple(owner.owner_id for owner in EVIDENCE_FAMILIES[family_name].owners)


def owner_contract_count(family_name: str) -> int:
    return len(EVIDENCE_FAMILIES[family_name].owners)


def validate_boundary_owner_contracts(
    family_name: str,
    contract: dict[str, Any],
    root: Path,
) -> dict[str, bool]:
    family = EVIDENCE_FAMILIES[family_name]
    owner_contracts = contract.get("source_owner_contracts", {})
    expected_owner_ids = owner_contract_ids(family_name)

    checks = {
        "hard_cutover_source_owner_contract_matches": (
            contract.get("hard_cutover_source_owner_contract")
            == HARD_CUTOVER_SOURCE_OWNER_CONTRACT
        ),
        "source_owner_contract_ids_match": (
            sorted(owner_contracts) == sorted(expected_owner_ids)
            if isinstance(owner_contracts, dict)
            else False
        ),
    }

    if not isinstance(owner_contracts, dict):
        return checks

    for owner in family.owners:
        entry = owner_contracts.get(owner.owner_id, {})
        checks[f"{owner.owner_id}_source_contract_matches"] = (
            entry.get("source_contract") == owner.source_contract
        )
        checks[f"{owner.owner_id}_source_contract_exists"] = (
            root / owner.source_contract
        ).is_file()
        checks[f"{owner.owner_id}_summary_anchor_matches"] = (
            entry.get("summary_implementation_anchor")
            == owner.summary_implementation_anchor
        )
        checks[f"{owner.owner_id}_summary_anchor_exists"] = (
            root / owner.summary_implementation_anchor
        ).is_file()
        checks[f"{owner.owner_id}_check_anchors_match"] = tuple(
            entry.get("check_implementation_anchors", ())
        ) == owner.check_implementation_anchors
        checks[f"{owner.owner_id}_check_anchors_exist"] = all(
            (root / path).is_file() for path in owner.check_implementation_anchors
        )
        checks[f"{owner.owner_id}_supporting_contracts_match"] = tuple(
            entry.get("supporting_contracts", ())
        ) == owner.supporting_contracts
        checks[f"{owner.owner_id}_supporting_contracts_exist"] = all(
            (root / path).is_file() for path in owner.supporting_contracts
        )
        checks[f"{owner.owner_id}_evidence_log_disallowed"] = (
            entry.get("evidence_log_allowed") is False
        )
        checks[f"{owner.owner_id}_retired_route_claims_disallowed"] = (
            entry.get("retired_route_claims_allowed") is False
        )
        checks[f"{owner.owner_id}_generated_report_claims_disallowed"] = (
            entry.get("generated_report_claims_allowed") is False
        )

    return checks
