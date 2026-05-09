"""Action-handler drift reporting against the public catalog."""

from __future__ import annotations

from dataclasses import dataclass

from scripts.objc3c_workflow.action_handlers import ACTION_HANDLERS
from scripts.objc3c_workflow.registry_views import action_names


@dataclass(frozen=True)
class ActionHandlerDrift:
    missing: list[str]
    orphaned: list[str]

    @property
    def complete(self) -> bool:
        return not self.missing and not self.orphaned


def action_handler_drift() -> ActionHandlerDrift:
    catalog_actions = set(action_names())
    handler_actions = set(ACTION_HANDLERS)
    return ActionHandlerDrift(
        missing=sorted(catalog_actions - handler_actions),
        orphaned=sorted(handler_actions - catalog_actions),
    )


__all__ = ["ActionHandlerDrift", "action_handler_drift"]
