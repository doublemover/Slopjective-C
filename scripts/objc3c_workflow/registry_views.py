"""Read-only views over the objc3c workflow action registry."""

from __future__ import annotations

from collections.abc import Callable

from .action_spec import ActionSpec
from .action_catalog import ACTION_SPECS


def action_spec(action: str) -> ActionSpec | None:
    return ACTION_SPECS.get(action)


def require_action_spec(action: str) -> ActionSpec:
    return ACTION_SPECS[action]


def has_action(action: str) -> bool:
    return action in ACTION_SPECS


def action_names() -> list[str]:
    return list(ACTION_SPECS)


def action_specs() -> list[ActionSpec]:
    return list(ACTION_SPECS.values())


def action_items() -> list[tuple[str, ActionSpec]]:
    return list(ACTION_SPECS.items())


def actions_by_category(category: str) -> list[str]:
    prefix = f"{category}-"
    return [action for action in ACTION_SPECS if action == category or action.startswith(prefix)]


def actions_matching(predicate: Callable[[str, ActionSpec], bool]) -> list[str]:
    return [
        action
        for action, spec in ACTION_SPECS.items()
        if predicate(action, spec)
    ]


def action_count() -> int:
    return len(ACTION_SPECS)
