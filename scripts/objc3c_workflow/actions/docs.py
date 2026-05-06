"""Documentation action inventory."""

from __future__ import annotations

from ..registry import ACTION_SPECS

DOC_ACTION_MARKERS = ("docs", "documentation", "markdown", "site", "command-surface", "command-contract")


def action_names() -> list[str]:
    return [
        action
        for action, spec in ACTION_SPECS.items()
        if any(marker in action or marker in spec.summary for marker in DOC_ACTION_MARKERS)
    ]
