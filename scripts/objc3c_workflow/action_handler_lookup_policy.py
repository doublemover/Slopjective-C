"""Owned lookup policy for registered workflow action handlers."""

from __future__ import annotations

from collections.abc import Mapping

from scripts.objc3c_workflow.action_spec import ActionHandler

ACTION_HANDLER_LOOKUP_OWNER = "objc3c-workflow-action-handler-lookup"
ACTION_HANDLER_REQUIRED_LOOKUP_OWNER = "objc3c-workflow-action-handler-required-lookup"


def maybe_action_handler(
    handlers: Mapping[str, ActionHandler],
    action: str,
) -> ActionHandler | None:
    return handlers.get(action)


def required_action_handler(
    handlers: Mapping[str, ActionHandler],
    action: str,
) -> ActionHandler:
    return handlers[action]


__all__ = [
    "ACTION_HANDLER_LOOKUP_OWNER",
    "ACTION_HANDLER_REQUIRED_LOOKUP_OWNER",
    "maybe_action_handler",
    "required_action_handler",
]
