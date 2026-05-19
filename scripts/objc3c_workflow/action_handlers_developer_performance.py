"""Developer-tooling and performance action handlers."""

from __future__ import annotations

from scripts.objc3c_workflow.action_handler_sections import merge_action_handler_sections
from scripts.objc3c_workflow.action_handlers_developer_inspection import (
    DEVELOPER_INSPECTION_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_performance import PERFORMANCE_ACTION_HANDLERS
from scripts.objc3c_workflow.action_spec import ActionHandler

DEVELOPER_AND_PERFORMANCE_ACTION_HANDLERS: dict[str, ActionHandler] = (
    merge_action_handler_sections(
        DEVELOPER_INSPECTION_ACTION_HANDLERS,
        PERFORMANCE_ACTION_HANDLERS,
    )
)
