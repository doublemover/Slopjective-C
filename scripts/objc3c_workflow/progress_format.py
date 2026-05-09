"""Progress command formatting helpers."""

from __future__ import annotations

from collections.abc import Sequence


def format_command(command: Sequence[str]) -> str:
    return " ".join(str(part) for part in command)


__all__ = ["format_command"]
