"""Packaging channel action specs."""

from __future__ import annotations

from .action_spec import ActionSpec
from .actions.release_governance_owner_contracts import (
    PACKAGING_CHANNEL_ACTION_CONTRACTS,
    release_action_specs,
)

PACKAGING_CHANNEL_ACTION_SPECS: dict[str, ActionSpec] = release_action_specs(
    PACKAGING_CHANNEL_ACTION_CONTRACTS
)


__all__ = ["PACKAGING_CHANNEL_ACTION_SPECS"]
