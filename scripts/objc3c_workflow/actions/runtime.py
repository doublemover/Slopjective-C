"""Runtime action inventory."""

from __future__ import annotations

from ..registry_views import actions_matching


def action_names() -> list[str]:
    return actions_matching(lambda action, _: "runtime" in action)
