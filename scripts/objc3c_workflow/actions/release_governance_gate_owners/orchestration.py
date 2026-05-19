"""Release-governance gate owner orchestration."""

from __future__ import annotations

from .catalog import RELEASE_GATE_OWNERS
from .inputs import parse_release_gate_lookup
from .models import ReleaseGateOwner
from .report_rendering import (
    render_release_action_owner_map,
    render_release_gate_child_actions,
    render_release_gate_guardrails,
    render_release_gate_public_actions,
)


def release_gate_owner(gate_id: str) -> ReleaseGateOwner:
    lookup = parse_release_gate_lookup(gate_id)
    return RELEASE_GATE_OWNERS[lookup.gate_id]


def release_gate_public_actions(gate_id: str) -> tuple[str, ...]:
    return render_release_gate_public_actions(release_gate_owner(gate_id))


def release_gate_child_actions(gate_id: str) -> tuple[str, ...]:
    return render_release_gate_child_actions(release_gate_owner(gate_id))


def release_gate_hard_cutover_guardrails(gate_id: str) -> dict[str, object]:
    return render_release_gate_guardrails(release_gate_owner(gate_id))


def release_action_owner_map() -> dict[str, dict[str, str]]:
    return render_release_action_owner_map(RELEASE_GATE_OWNERS)


__all__ = [
    "release_action_owner_map",
    "release_gate_child_actions",
    "release_gate_hard_cutover_guardrails",
    "release_gate_owner",
    "release_gate_public_actions",
]
