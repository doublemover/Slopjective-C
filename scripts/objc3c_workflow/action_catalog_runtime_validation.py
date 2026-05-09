"""Runtime validation action specs."""

from __future__ import annotations

from .action_catalog_runtime_acceptance import RUNTIME_ACCEPTANCE_ACTION_SPECS
from .action_catalog_runtime_architecture import RUNTIME_ARCHITECTURE_ACTION_SPECS
from .action_catalog_runtime_runnable_conformance import (
    RUNTIME_RUNNABLE_CONFORMANCE_ACTION_SPECS,
)
from .action_catalog_runtime_runnable_e2e import RUNTIME_RUNNABLE_E2E_ACTION_SPECS
from .action_catalog_sections import merge_action_catalog_sections
from .action_spec import ActionSpec

RUNTIME_VALIDATION_ACTION_SPECS: dict[str, ActionSpec] = merge_action_catalog_sections(
    RUNTIME_ACCEPTANCE_ACTION_SPECS,
    RUNTIME_ARCHITECTURE_ACTION_SPECS,
    RUNTIME_RUNNABLE_CONFORMANCE_ACTION_SPECS,
    RUNTIME_RUNNABLE_E2E_ACTION_SPECS,
)


__all__ = ["RUNTIME_VALIDATION_ACTION_SPECS"]
