"""Build action inventory."""

from __future__ import annotations

from .command_facades_inventory import category_action_names

CATEGORY = "build"


def action_names() -> list[str]:
    return category_action_names(CATEGORY)
