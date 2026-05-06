"""Package action inventory."""

from __future__ import annotations

from ..registry import actions_by_category

CATEGORIES = ("package", "packaging")


def action_names() -> list[str]:
    names: list[str] = []
    for category in CATEGORIES:
        names.extend(actions_by_category(category))
    return names
