"""Nested workflow command construction helpers."""

from __future__ import annotations

from .public_bridge_constants import WORKFLOW_BRIDGE_SCRIPT


def workflow_command(action: str, *args: str) -> list[str]:
    return ["npm", "run", WORKFLOW_BRIDGE_SCRIPT, "--", action, *args]


__all__ = ["workflow_command"]
