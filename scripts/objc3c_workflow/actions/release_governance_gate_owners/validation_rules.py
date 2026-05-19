"""Release-governance gate owner validation rules."""

from __future__ import annotations

from .constants import RELEASE_GATE_IDS
from .models import ReleaseGateOwner, ReleaseGateOwnerCatalog


def validate_release_gate_catalog(
    catalog: ReleaseGateOwnerCatalog,
) -> ReleaseGateOwnerCatalog:
    _require_expected_gate_ids(catalog)

    action_owners: dict[str, str] = {}
    for gate_id, owner in catalog.items():
        _require_gate_owner_key(gate_id, owner)
        _require_owner_fields(owner)
        _require_unique_actions(gate_id, owner, action_owners)
        _require_unique_guardrails(gate_id, owner)

    return catalog


def _require_expected_gate_ids(catalog: ReleaseGateOwnerCatalog) -> None:
    actual_gate_ids = tuple(catalog)
    if actual_gate_ids != RELEASE_GATE_IDS:
        raise ValueError(
            "release gate catalog order mismatch: "
            f"expected {RELEASE_GATE_IDS!r}, got {actual_gate_ids!r}"
        )


def _require_gate_owner_key(gate_id: str, owner: ReleaseGateOwner) -> None:
    if owner.gate_id != gate_id:
        raise ValueError(
            f"release gate catalog key {gate_id!r} does not match owner "
            f"gate_id {owner.gate_id!r}"
        )


def _require_owner_fields(owner: ReleaseGateOwner) -> None:
    fields = (
        owner.gate_id,
        owner.source_owner,
        owner.gate_owner,
        owner.blocker_owner,
        owner.source_surface,
        owner.workflow_surface,
    )
    if any(not isinstance(value, str) or not value for value in fields):
        raise ValueError(f"release gate {owner.gate_id!r} has an empty owner field")
    if not owner.owned_actions:
        raise ValueError(f"release gate {owner.gate_id!r} has no owned actions")


def _require_unique_actions(
    gate_id: str,
    owner: ReleaseGateOwner,
    action_owners: dict[str, str],
) -> None:
    for action in owner.owned_actions:
        previous_gate_id = action_owners.get(action)
        if previous_gate_id is not None:
            raise ValueError(
                f"release action {action!r} is owned by both "
                f"{previous_gate_id!r} and {gate_id!r}"
            )
        action_owners[action] = gate_id


def _require_unique_guardrails(gate_id: str, owner: ReleaseGateOwner) -> None:
    guardrail_names: set[str] = set()
    for guardrail_name, _guardrail_value in owner.hard_cutover_guardrails:
        if guardrail_name in guardrail_names:
            raise ValueError(
                f"release gate {gate_id!r} duplicates guardrail "
                f"{guardrail_name!r}"
            )
        guardrail_names.add(guardrail_name)


__all__ = ["validate_release_gate_catalog"]
