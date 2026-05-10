from __future__ import annotations

from pathlib import Path
from typing import Any

from .family_catalog import EVIDENCE_FAMILIES
from .validation_helpers import (
    base_owner_contract_checks,
    owner_contract_entry_checks,
)


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
    checks = base_owner_contract_checks(contract, owner_contracts, expected_owner_ids)

    if not isinstance(owner_contracts, dict):
        return checks

    for owner in family.owners:
        entry = owner_contracts.get(owner.owner_id, {})
        checks.update(owner_contract_entry_checks(owner, entry, root))

    return checks
