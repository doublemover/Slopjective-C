"""Release operations action specs."""

from __future__ import annotations

from .actions.release_governance_owner_contracts import (
    RELEASE_OPERATIONS_ACTION_CONTRACTS,
    release_action_specs,
)
from .action_spec import ActionSpec

RELEASE_OPERATIONS_ACTION_SPECS: dict[str, ActionSpec] = release_action_specs(
    RELEASE_OPERATIONS_ACTION_CONTRACTS
)


__all__ = ["RELEASE_OPERATIONS_ACTION_CONTRACTS", "RELEASE_OPERATIONS_ACTION_SPECS"]
