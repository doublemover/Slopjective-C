"""Filter helpers for workflow action registry views."""

from __future__ import annotations

from collections.abc import Callable, Iterable

from .action_spec import ActionSpec


ActionItem = tuple[str, ActionSpec]


def filter_actions_by_category(
    items: Iterable[ActionItem],
    category: str,
) -> list[str]:
    prefix = f"{category}-"
    return [
        action
        for action, _ in items
        if action == category or action.startswith(prefix)
    ]


def filter_actions_matching(
    items: Iterable[ActionItem],
    predicate: Callable[[str, ActionSpec], bool],
) -> list[str]:
    return [
        action
        for action, spec in items
        if predicate(action, spec)
    ]
