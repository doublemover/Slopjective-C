"""Action-handler completeness checks for workflow dispatch."""

from __future__ import annotations

from scripts.objc3c_workflow.action_handler_drift import action_handler_drift


def missing_action_handlers() -> list[str]:
    return action_handler_drift().missing


def orphan_action_handlers() -> list[str]:
    return action_handler_drift().orphaned


def action_handler_registry_is_complete() -> bool:
    return action_handler_drift().complete


__all__ = [
    "action_handler_registry_is_complete",
    "missing_action_handlers",
    "orphan_action_handlers",
]
