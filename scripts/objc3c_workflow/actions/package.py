"""Package action inventory."""

from __future__ import annotations

from .command_facades_inventory import category_group_action_names

CATEGORIES = ("package", "packaging")


def action_names() -> list[str]:
    return category_group_action_names(CATEGORIES)
