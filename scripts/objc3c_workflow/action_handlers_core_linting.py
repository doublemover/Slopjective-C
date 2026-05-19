"""Core lint and dependency-boundary handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import hygiene

CORE_LINTING_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "lint": hygiene.action_lint,
    "check-dependency-boundaries": hygiene.action_check_dependency_boundaries,
}

__all__ = ["CORE_LINTING_ACTION_HANDLERS"]
