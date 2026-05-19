"""Public conformance reporting action specs."""

from __future__ import annotations

from .actions.release_governance_public_conformance_action_fragments import (
    PUBLIC_CONFORMANCE_ACTION_FRAGMENTS,
    public_conformance_action_specs,
)
from .action_spec import ActionSpec

PUBLIC_CONFORMANCE_REPORTING_ACTION_SPECS: dict[str, ActionSpec] = (
    public_conformance_action_specs()
)

__all__ = [
    "PUBLIC_CONFORMANCE_ACTION_FRAGMENTS",
    "PUBLIC_CONFORMANCE_REPORTING_ACTION_SPECS",
    "public_conformance_action_specs",
]
