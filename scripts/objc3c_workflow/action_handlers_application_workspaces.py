"""Application workspace handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import developer_tooling

APPLICATION_WORKSPACE_HANDLER_OWNER_SURFACE = (
    "scripts/objc3c_workflow/action_handlers_application_workspaces.py"
)

APPLICATION_WORKSPACE_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "materialize-playground-workspace": developer_tooling.action_materialize_playground_workspace,
}


__all__ = [
    "APPLICATION_WORKSPACE_ACTION_HANDLERS",
    "APPLICATION_WORKSPACE_HANDLER_OWNER_SURFACE",
]
