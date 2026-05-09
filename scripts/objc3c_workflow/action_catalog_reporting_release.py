"""Stress, reporting, release-governance, and security action specs."""

from __future__ import annotations

from .action_catalog_conformance_stress import CONFORMANCE_STRESS_ACTION_SPECS
from .action_catalog_public_performance import PUBLIC_REPORTING_AND_PERFORMANCE_ACTION_SPECS
from .action_catalog_release_channels import RELEASE_CHANNEL_ACTION_SPECS
from .action_catalog_sections import merge_action_catalog_sections
from .action_catalog_security import SECURITY_HARDENING_ACTION_SPECS
from .action_spec import ActionSpec
from .actions.release_governance_owner_contracts import (
    RELEASE_GATE_OWNERS,
    release_action_owner_map,
)

STRESS_REPORTING_AND_RELEASE_ACTION_SPECS: dict[str, ActionSpec] = (
    merge_action_catalog_sections(
        CONFORMANCE_STRESS_ACTION_SPECS,
        PUBLIC_REPORTING_AND_PERFORMANCE_ACTION_SPECS,
        RELEASE_CHANNEL_ACTION_SPECS,
        SECURITY_HARDENING_ACTION_SPECS,
    )
)

STRESS_REPORTING_RELEASE_GATE_OWNERS: dict[str, dict[str, object]] = {
    gate_id: owner.workflow_owner_policy()
    for gate_id, owner in RELEASE_GATE_OWNERS.items()
}
STRESS_REPORTING_RELEASE_ACTION_OWNER_MAP: dict[str, dict[str, str]] = (
    release_action_owner_map()
)
