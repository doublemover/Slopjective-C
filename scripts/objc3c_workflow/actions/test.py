"""Test action inventory."""

from __future__ import annotations

from .command_facades_inventory import category_action_names

CATEGORY = "test"


def action_names() -> list[str]:
    return category_action_names(CATEGORY)
