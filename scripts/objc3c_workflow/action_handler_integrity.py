"""Action-handler completeness checks for workflow dispatch."""

from __future__ import annotations

from scripts.objc3c_workflow.action_handlers import ACTION_HANDLERS
from scripts.objc3c_workflow.registry_views import action_names


def missing_action_handlers() -> list[str]:
    return sorted(set(action_names()) - set(ACTION_HANDLERS))


def orphan_action_handlers() -> list[str]:
    return sorted(set(ACTION_HANDLERS) - set(action_names()))


def action_handler_registry_is_complete() -> bool:
    return not missing_action_handlers() and not orphan_action_handlers()
