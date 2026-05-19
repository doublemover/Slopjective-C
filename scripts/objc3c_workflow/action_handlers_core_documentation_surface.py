"""Core documentation-surface handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions.docs_documentation import (
    CHECK_DOCUMENTATION_SURFACE_ACTION,
    action_check_documentation_surface,
)

CORE_DOCUMENTATION_SURFACE_ACTION_HANDLERS: dict[str, ActionHandler] = {
    CHECK_DOCUMENTATION_SURFACE_ACTION: action_check_documentation_surface,
}

__all__ = ["CORE_DOCUMENTATION_SURFACE_ACTION_HANDLERS"]
