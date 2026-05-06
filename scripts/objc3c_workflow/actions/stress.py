"""Stress action inventory."""

from __future__ import annotations

from ..registry import ACTION_SPECS


def action_names() -> list[str]:
    return [action for action in ACTION_SPECS if "stress" in action or "fuzz" in action]
