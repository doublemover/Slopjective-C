"""Read-only views over the objc3c workflow action registry."""

from __future__ import annotations

from .action_spec import ActionSpec
from .registry import ACTION_SPECS


def action_spec(action: str) -> ActionSpec | None:
    return ACTION_SPECS.get(action)


def has_action(action: str) -> bool:
    return action in ACTION_SPECS


def action_names() -> list[str]:
    return list(ACTION_SPECS)


def actions_by_category(category: str) -> list[str]:
    prefix = f"{category}-"
    return [action for action in ACTION_SPECS if action == category or action.startswith(prefix)]


def action_count() -> int:
    return len(ACTION_SPECS)
