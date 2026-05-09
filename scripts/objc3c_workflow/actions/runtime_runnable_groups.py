"""Typed action groups for runtime-runnable workflow owners."""

from __future__ import annotations

from collections.abc import Sequence
from dataclasses import dataclass

from scripts.objc3c_workflow.action_spec import ActionHandler, ActionSpec

RUNTIME_RUNNABLE_VALIDATION_TIER = "full"


@dataclass(frozen=True)
class RuntimeRunnableActionGroup:
    action: str
    summary: str
    backend: str
    handler: ActionHandler
    guarantee_owner: str
    validation_tier: str = RUNTIME_RUNNABLE_VALIDATION_TIER

    def spec(self) -> ActionSpec:
        return ActionSpec(
            self.action,
            self.summary,
            self.backend,
            validation_tier=self.validation_tier,
            guarantee_owner=self.guarantee_owner,
        )


def runtime_runnable_action_specs(
    groups: Sequence[RuntimeRunnableActionGroup],
) -> dict[str, ActionSpec]:
    return {group.action: group.spec() for group in _strict_groups(groups)}


def runtime_runnable_action_handlers(
    groups: Sequence[RuntimeRunnableActionGroup],
) -> dict[str, ActionHandler]:
    return {group.action: group.handler for group in _strict_groups(groups)}


def _strict_groups(
    groups: Sequence[RuntimeRunnableActionGroup],
) -> tuple[RuntimeRunnableActionGroup, ...]:
    strict_groups = tuple(groups)
    action_names = [group.action for group in strict_groups]
    if len(action_names) != len(set(action_names)):
        raise ValueError(f"duplicate runtime-runnable workflow actions: {action_names}")
    for group in strict_groups:
        if group.spec().action != group.action:
            raise ValueError(f"runtime-runnable action key mismatch: {group.action}")
    return strict_groups


__all__ = [
    "RUNTIME_RUNNABLE_VALIDATION_TIER",
    "RuntimeRunnableActionGroup",
    "runtime_runnable_action_handlers",
    "runtime_runnable_action_specs",
]
