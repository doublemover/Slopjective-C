"""Stress and fuzz action specs."""

from __future__ import annotations

from .action_catalog_stress_runtime import STRESS_RUNTIME_ACTION_SPECS
from .action_catalog_stress_source import STRESS_SOURCE_ACTION_SPECS
from .action_catalog_stress_validation import STRESS_VALIDATION_ACTION_SPECS
from .action_spec import ActionSpec

STRESS_ACTION_SPECS: dict[str, ActionSpec] = {
    "check-stress-surface": STRESS_SOURCE_ACTION_SPECS["check-stress-surface"],
    "test-fuzz-safety": STRESS_SOURCE_ACTION_SPECS["test-fuzz-safety"],
    "test-lowering-runtime-stress": STRESS_RUNTIME_ACTION_SPECS[
        "test-lowering-runtime-stress"
    ],
    "test-mixed-module-differential": STRESS_RUNTIME_ACTION_SPECS[
        "test-mixed-module-differential"
    ],
    "test-stress-minimization": STRESS_SOURCE_ACTION_SPECS[
        "test-stress-minimization"
    ],
    "test-stress-crash-triage": STRESS_SOURCE_ACTION_SPECS[
        "test-stress-crash-triage"
    ],
    "validate-stress": STRESS_VALIDATION_ACTION_SPECS["validate-stress"],
    "validate-stress-integration": STRESS_VALIDATION_ACTION_SPECS[
        "validate-stress-integration"
    ],
    "validate-stress-end-to-end": STRESS_VALIDATION_ACTION_SPECS[
        "validate-stress-end-to-end"
    ],
}

__all__ = ["STRESS_ACTION_SPECS"]
