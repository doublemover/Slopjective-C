"""Action category fields for workflow payloads."""

from __future__ import annotations


def action_category(action: str) -> str:
    return action.split("-", 1)[0]


__all__ = ["action_category"]
