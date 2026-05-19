"""Runtime runnable end-to-end action specs."""

from __future__ import annotations

from .actions.runtime_runnable_e2e import (
    RUNTIME_RUNNABLE_E2E_ACTION_GROUPS,
)
from .actions.runtime_runnable_groups import (
    runtime_runnable_action_specs,
)

from .action_spec import ActionSpec

RUNTIME_RUNNABLE_E2E_ACTION_SPECS: dict[str, ActionSpec] = (
    runtime_runnable_action_specs(RUNTIME_RUNNABLE_E2E_ACTION_GROUPS)
)


__all__ = ["RUNTIME_RUNNABLE_E2E_ACTION_SPECS"]
