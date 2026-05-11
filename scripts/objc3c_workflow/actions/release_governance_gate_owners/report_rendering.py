"""Release-governance gate owner report rendering."""

from __future__ import annotations

from .models import ReleaseGateOwner, ReleaseGateOwnerCatalog


def render_release_action_owner_map(
    catalog: ReleaseGateOwnerCatalog,
) -> dict[str, dict[str, str]]:
    owner_map: dict[str, dict[str, str]] = {}
    for owner in catalog.values():
        owner_map.update(owner.action_owner_map())
    return owner_map


def render_release_gate_child_actions(owner: ReleaseGateOwner) -> tuple[str, ...]:
    return owner.workflow_child_actions


def render_release_gate_guardrails(owner: ReleaseGateOwner) -> dict[str, object]:
    return dict(owner.hard_cutover_guardrails)


def render_release_gate_public_actions(owner: ReleaseGateOwner) -> tuple[str, ...]:
    return owner.owned_actions


__all__ = [
    "render_release_action_owner_map",
    "render_release_gate_child_actions",
    "render_release_gate_guardrails",
    "render_release_gate_public_actions",
]
