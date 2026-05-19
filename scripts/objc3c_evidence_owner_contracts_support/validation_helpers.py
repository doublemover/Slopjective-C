from __future__ import annotations

from pathlib import Path
from typing import Any

from .models import EvidenceOwner
from .source_owner_policy import HARD_CUTOVER_SOURCE_OWNER_CONTRACT


def relative_file_exists(root: Path, relative_path: str) -> bool:
    return (root / relative_path).is_file()


def relative_files_exist(root: Path, relative_paths: tuple[str, ...]) -> bool:
    return all(relative_file_exists(root, path) for path in relative_paths)


def base_owner_contract_checks(
    contract: dict[str, Any],
    owner_contracts: Any,
    expected_owner_ids: tuple[str, ...],
) -> dict[str, bool]:
    return {
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


def owner_contract_entry_checks(
    owner: EvidenceOwner,
    entry: dict[str, Any],
    root: Path,
) -> dict[str, bool]:
    return {
        f"{owner.owner_id}_source_contract_matches": (
            entry.get("source_contract") == owner.source_contract
        ),
        f"{owner.owner_id}_source_contract_exists": relative_file_exists(
            root,
            owner.source_contract,
        ),
        f"{owner.owner_id}_summary_anchor_matches": (
            entry.get("summary_implementation_anchor")
            == owner.summary_implementation_anchor
        ),
        f"{owner.owner_id}_summary_anchor_exists": relative_file_exists(
            root,
            owner.summary_implementation_anchor,
        ),
        f"{owner.owner_id}_check_anchors_match": tuple(
            entry.get("check_implementation_anchors", ())
        )
        == owner.check_implementation_anchors,
        f"{owner.owner_id}_check_anchors_exist": relative_files_exist(
            root,
            owner.check_implementation_anchors,
        ),
        f"{owner.owner_id}_supporting_contracts_match": tuple(
            entry.get("supporting_contracts", ())
        )
        == owner.supporting_contracts,
        f"{owner.owner_id}_supporting_contracts_exist": relative_files_exist(
            root,
            owner.supporting_contracts,
        ),
        f"{owner.owner_id}_evidence_log_disallowed": (
            entry.get("evidence_log_allowed") is False
        ),
        f"{owner.owner_id}_retired_route_claims_disallowed": (
            entry.get("retired_route_claims_allowed") is False
        ),
        f"{owner.owner_id}_generated_report_claims_disallowed": (
            entry.get("generated_report_claims_allowed") is False
        ),
    }


__all__ = [
    "base_owner_contract_checks",
    "owner_contract_entry_checks",
    "relative_file_exists",
    "relative_files_exist",
]
