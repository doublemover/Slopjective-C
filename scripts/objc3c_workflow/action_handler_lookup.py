"""Registered workflow action-handler lookup."""

from __future__ import annotations

from scripts.objc3c_workflow.action_handlers import ACTION_HANDLERS
from scripts.objc3c_workflow.action_spec import ActionHandler


def registered_action_handler(action: str) -> ActionHandler | None:
    return ACTION_HANDLERS.get(action)


def require_registered_action_handler(action: str) -> ActionHandler:
    return ACTION_HANDLERS[action]


__all__ = ["registered_action_handler", "require_registered_action_handler"]
