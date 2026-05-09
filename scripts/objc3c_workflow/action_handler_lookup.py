"""Registered workflow action-handler lookup."""

from __future__ import annotations

from scripts.objc3c_workflow.action_handlers import ACTION_HANDLERS
from scripts.objc3c_workflow.action_handler_lookup_policy import (
    maybe_action_handler,
    required_action_handler,
)
from scripts.objc3c_workflow.action_spec import ActionHandler


def registered_action_handler(action: str) -> ActionHandler | None:
    return maybe_action_handler(ACTION_HANDLERS, action)


def require_registered_action_handler(action: str) -> ActionHandler:
    return required_action_handler(ACTION_HANDLERS, action)


__all__ = ["registered_action_handler", "require_registered_action_handler"]
