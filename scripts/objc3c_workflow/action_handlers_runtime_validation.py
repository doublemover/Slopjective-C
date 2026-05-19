"""Runtime validation handler facade."""

from __future__ import annotations

from scripts.objc3c_workflow.action_handler_sections import merge_action_handler_sections
from scripts.objc3c_workflow.action_handlers_runtime_acceptance import (
    RUNTIME_ACCEPTANCE_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_runtime_architecture import (
    RUNTIME_ARCHITECTURE_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_runtime_runnable_conformance import (
    RUNTIME_RUNNABLE_CONFORMANCE_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_runtime_runnable_e2e import (
    RUNTIME_RUNNABLE_E2E_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_spec import ActionHandler

RUNTIME_VALIDATION_ACTION_HANDLERS: dict[str, ActionHandler] = (
    merge_action_handler_sections(
        RUNTIME_ACCEPTANCE_ACTION_HANDLERS,
        RUNTIME_ARCHITECTURE_ACTION_HANDLERS,
        RUNTIME_RUNNABLE_CONFORMANCE_ACTION_HANDLERS,
        RUNTIME_RUNNABLE_E2E_ACTION_HANDLERS,
    )
)


__all__ = ["RUNTIME_VALIDATION_ACTION_HANDLERS"]
