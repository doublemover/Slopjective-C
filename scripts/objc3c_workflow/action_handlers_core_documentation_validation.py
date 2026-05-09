"""Core documentation validation handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import docs

CORE_DOCUMENTATION_VALIDATION_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "validate-documentation-surface": docs.action_validate_documentation_surface,
}

__all__ = ["CORE_DOCUMENTATION_VALIDATION_ACTION_HANDLERS"]
