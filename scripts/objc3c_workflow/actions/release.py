"""Release action inventory."""

from __future__ import annotations

from ..action_catalog import ACTION_SPECS


def action_names() -> list[str]:
    return [action for action in ACTION_SPECS if "release" in action]
