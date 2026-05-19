"""Application and stdlib workspace action handlers."""

from __future__ import annotations

from scripts.objc3c_workflow.action_handler_sections import merge_action_handler_sections
from scripts.objc3c_workflow.action_handlers_application_stdlib import (
    APPLICATION_STDLIB_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_application_workspaces import (
    APPLICATION_WORKSPACE_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_spec import ActionHandler

APPLICATION_HANDLER_OWNER_SURFACE = "scripts/objc3c_workflow/action_handlers_application.py"

APPLICATION_ACTION_HANDLERS: dict[str, ActionHandler] = merge_action_handler_sections(
    APPLICATION_WORKSPACE_ACTION_HANDLERS,
    APPLICATION_STDLIB_ACTION_HANDLERS,
)


__all__ = ["APPLICATION_ACTION_HANDLERS", "APPLICATION_HANDLER_OWNER_SURFACE"]
