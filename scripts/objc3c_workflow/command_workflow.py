"""Nested workflow command construction helpers."""

from __future__ import annotations

import sys

from .environment import WORKFLOW_MODULE


def workflow_command(action: str, *args: str) -> list[str]:
    return [sys.executable, "-m", WORKFLOW_MODULE, action, *args]


__all__ = ["workflow_command"]
