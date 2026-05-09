"""Site action inventory."""

from __future__ import annotations

from .command_facades_inventory import matching_action_names


def action_names() -> list[str]:
    return matching_action_names(lambda action, _: "site" in action)
