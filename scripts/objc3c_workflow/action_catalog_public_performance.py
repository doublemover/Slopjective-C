"""Public conformance reporting and performance-governance action specs."""

from __future__ import annotations

from .action_catalog_performance_governance import PERFORMANCE_GOVERNANCE_ACTION_SPECS
from .action_catalog_public_conformance_reporting import (
    PUBLIC_CONFORMANCE_REPORTING_ACTION_SPECS,
)
from .action_spec import ActionSpec

PUBLIC_REPORTING_AND_PERFORMANCE_ACTION_SPECS: dict[str, ActionSpec] = {
    **PUBLIC_CONFORMANCE_REPORTING_ACTION_SPECS,
    **PERFORMANCE_GOVERNANCE_ACTION_SPECS,
}

__all__ = ["PUBLIC_REPORTING_AND_PERFORMANCE_ACTION_SPECS"]
