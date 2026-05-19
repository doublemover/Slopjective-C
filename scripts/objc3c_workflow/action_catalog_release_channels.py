"""Release foundation, channel, operation, and distribution action specs."""

from __future__ import annotations

from .action_catalog_distribution_credibility import DISTRIBUTION_CREDIBILITY_ACTION_SPECS
from .action_catalog_packaging_channels import PACKAGING_CHANNEL_ACTION_SPECS
from .action_catalog_release_foundation import RELEASE_FOUNDATION_ACTION_SPECS
from .action_catalog_release_operations import RELEASE_OPERATIONS_ACTION_SPECS
from .action_catalog_sections import merge_action_catalog_sections
from .action_spec import ActionSpec
from .actions.release_governance_owner_contracts import (
    RELEASE_GATE_OWNERS,
)

_RELEASE_CHANNEL_GATE_IDS = (
    "release-foundation",
    "packaging-channels",
    "release-operations",
    "distribution-credibility",
)

RELEASE_CHANNEL_ACTION_SPECS: dict[str, ActionSpec] = merge_action_catalog_sections(
    RELEASE_FOUNDATION_ACTION_SPECS,
    PACKAGING_CHANNEL_ACTION_SPECS,
    RELEASE_OPERATIONS_ACTION_SPECS,
    DISTRIBUTION_CREDIBILITY_ACTION_SPECS,
)

RELEASE_CHANNEL_GATE_OWNERS: dict[str, dict[str, object]] = {
    gate_id: owner.workflow_owner_policy()
    for gate_id, owner in RELEASE_GATE_OWNERS.items()
    if gate_id in _RELEASE_CHANNEL_GATE_IDS
}

RELEASE_CHANNEL_ACTION_OWNER_MAP: dict[str, dict[str, str]] = {
    action_name: action_owner
    for gate_id in _RELEASE_CHANNEL_GATE_IDS
    for action_name, action_owner in RELEASE_GATE_OWNERS[gate_id].action_owner_map().items()
}


__all__ = [
    "RELEASE_CHANNEL_ACTION_OWNER_MAP",
    "RELEASE_CHANNEL_ACTION_SPECS",
    "RELEASE_CHANNEL_GATE_OWNERS",
]
