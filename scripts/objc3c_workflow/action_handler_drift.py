"""Action-handler drift reporting against the public catalog."""

from __future__ import annotations

from dataclasses import dataclass

from scripts.objc3c_workflow.action_handler_drift_policy import (
    handler_registry_complete,
    sorted_missing_handlers,
    sorted_orphan_handlers,
)
from scripts.objc3c_workflow.action_handlers import ACTION_HANDLERS
from scripts.objc3c_workflow.registry_views import action_names


@dataclass(frozen=True)
class ActionHandlerDrift:
    missing: list[str]
    orphaned: list[str]

    @property
    def complete(self) -> bool:
        return handler_registry_complete(self.missing, self.orphaned)


def action_handler_drift() -> ActionHandlerDrift:
    catalog_actions = action_names()
    handler_actions = ACTION_HANDLERS
    return ActionHandlerDrift(
        missing=sorted_missing_handlers(catalog_actions, handler_actions),
        orphaned=sorted_orphan_handlers(catalog_actions, handler_actions),
    )


__all__ = ["ActionHandlerDrift", "action_handler_drift"]
