"""Release foundation, channel, operation, and distribution action specs."""

from __future__ import annotations

from .action_catalog_distribution_credibility import DISTRIBUTION_CREDIBILITY_ACTION_SPECS
from .action_catalog_packaging_channels import PACKAGING_CHANNEL_ACTION_SPECS
from .action_catalog_release_foundation import RELEASE_FOUNDATION_ACTION_SPECS
from .action_catalog_release_operations import RELEASE_OPERATIONS_ACTION_SPECS
from .action_catalog_sections import merge_action_catalog_sections
from .action_spec import ActionSpec

RELEASE_CHANNEL_ACTION_SPECS: dict[str, ActionSpec] = merge_action_catalog_sections(
    RELEASE_FOUNDATION_ACTION_SPECS,
    PACKAGING_CHANNEL_ACTION_SPECS,
    RELEASE_OPERATIONS_ACTION_SPECS,
    DISTRIBUTION_CREDIBILITY_ACTION_SPECS,
)


__all__ = ["RELEASE_CHANNEL_ACTION_SPECS"]
