"""Progress output helpers for workflow actions."""

from __future__ import annotations

from collections.abc import Sequence

from .progress_format import format_command


def print_action_step(action: str, command: Sequence[str]) -> None:
    print(f"[objc3c:{action}] {format_command(command)}")


__all__ = ["format_command", "print_action_step"]
