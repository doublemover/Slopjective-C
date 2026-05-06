"""Progress output helpers for workflow actions."""

from __future__ import annotations

from collections.abc import Sequence


def format_command(command: Sequence[str]) -> str:
    return " ".join(str(part) for part in command)


def print_action_step(action: str, command: Sequence[str]) -> None:
    print(f"[objc3c:{action}] {format_command(command)}")
