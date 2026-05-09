"""Core documentation-surface handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import docs

CORE_DOCUMENTATION_SURFACE_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "check-documentation-surface": docs.action_check_documentation_surface,
}

__all__ = ["CORE_DOCUMENTATION_SURFACE_ACTION_HANDLERS"]
