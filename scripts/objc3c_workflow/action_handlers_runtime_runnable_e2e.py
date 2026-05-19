"""Runtime runnable end-to-end action handlers."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions.runtime_runnable_e2e import (
    RUNTIME_RUNNABLE_E2E_ACTION_GROUPS,
)
from scripts.objc3c_workflow.actions.runtime_runnable_groups import (
    runtime_runnable_action_handlers,
)

RUNTIME_RUNNABLE_E2E_ACTION_HANDLERS: dict[str, ActionHandler] = (
    runtime_runnable_action_handlers(RUNTIME_RUNNABLE_E2E_ACTION_GROUPS)
)


__all__ = ["RUNTIME_RUNNABLE_E2E_ACTION_HANDLERS"]
