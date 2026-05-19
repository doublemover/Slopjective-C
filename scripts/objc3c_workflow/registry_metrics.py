"""Action-catalog metric helpers for workflow registry views."""

from __future__ import annotations

from .action_catalog import ACTION_SPECS


def catalog_action_count() -> int:
    return len(ACTION_SPECS)


__all__ = ["catalog_action_count"]
