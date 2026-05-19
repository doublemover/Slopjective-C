"""Conformance, stress, and external-validation action specs."""

from __future__ import annotations

from .action_catalog_conformance import CONFORMANCE_ACTION_SPECS
from .action_catalog_external_validation import EXTERNAL_VALIDATION_ACTION_SPECS
from .action_catalog_stress import STRESS_ACTION_SPECS
from .action_spec import ActionSpec

CONFORMANCE_STRESS_ACTION_SPECS: dict[str, ActionSpec] = {
    **CONFORMANCE_ACTION_SPECS,
    **STRESS_ACTION_SPECS,
    **EXTERNAL_VALIDATION_ACTION_SPECS,
}

__all__ = ["CONFORMANCE_STRESS_ACTION_SPECS"]
