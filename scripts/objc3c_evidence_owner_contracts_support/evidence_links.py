from __future__ import annotations

from collections.abc import Iterable

from .models import EvidenceOwner


FIXTURE_ROOT = "tests/tooling/fixtures"
SCRIPT_ROOT = "scripts"


def family_fixture_path(family_name: str, filename: str) -> str:
    return f"{FIXTURE_ROOT}/{family_name}/{filename}"


def boundary_inventory_path(family_name: str) -> str:
    return family_fixture_path(family_name, "boundary_inventory.json")


def script_anchor(filename: str) -> str:
    return f"{SCRIPT_ROOT}/{filename}"


def family_contract_paths(family_name: str, filenames: Iterable[str]) -> tuple[str, ...]:
    return tuple(family_fixture_path(family_name, filename) for filename in filenames)


def script_anchor_paths(filenames: Iterable[str]) -> tuple[str, ...]:
    return tuple(script_anchor(filename) for filename in filenames)


def owner_contract(
    family_name: str,
    owner_id: str,
    source_contract: str,
    *,
    summary_script: str,
    check_scripts: Iterable[str] = (),
    supporting_contracts: Iterable[str] = (),
) -> EvidenceOwner:
    return EvidenceOwner(
        owner_id=owner_id,
        source_contract=family_fixture_path(family_name, source_contract),
        summary_implementation_anchor=script_anchor(summary_script),
        check_implementation_anchors=script_anchor_paths(check_scripts),
        supporting_contracts=family_contract_paths(family_name, supporting_contracts),
    )


__all__ = [
    "boundary_inventory_path",
    "family_contract_paths",
    "family_fixture_path",
    "owner_contract",
    "script_anchor",
    "script_anchor_paths",
]
