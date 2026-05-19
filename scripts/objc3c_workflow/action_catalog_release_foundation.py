"""Release foundation action specs."""

from __future__ import annotations

from .actions.release_governance_owner_contracts import (
    RELEASE_FOUNDATION_ACTION_CONTRACTS,
    release_action_specs,
    release_gate_child_actions,
    release_gate_public_actions,
)
from .action_spec import ActionSpec

RELEASE_FOUNDATION_PUBLIC_ACTIONS: tuple[str, ...] = release_gate_public_actions(
    "release-foundation"
)

RELEASE_FOUNDATION_VALIDATE_CHILD_ACTIONS: tuple[str, ...] = release_gate_child_actions(
    "release-foundation"
)

RELEASE_FOUNDATION_ACTION_SPECS: dict[str, ActionSpec] = release_action_specs(
    RELEASE_FOUNDATION_ACTION_CONTRACTS
)


__all__ = [
    "RELEASE_FOUNDATION_ACTION_CONTRACTS",
    "RELEASE_FOUNDATION_ACTION_SPECS",
    "RELEASE_FOUNDATION_PUBLIC_ACTIONS",
    "RELEASE_FOUNDATION_VALIDATE_CHILD_ACTIONS",
]
