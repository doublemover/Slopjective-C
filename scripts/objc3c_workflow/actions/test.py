"""Test action inventory."""

from __future__ import annotations

from ..registry import actions_by_category

CATEGORY = "test"


def action_names() -> list[str]:
    return actions_by_category(CATEGORY)
