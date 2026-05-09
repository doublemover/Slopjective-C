"""CLI argv normalization for objc3c workflow entrypoints."""

from __future__ import annotations

from collections.abc import Sequence

from .cli_argv_source import process_argv


def workflow_argv(argv: Sequence[str] | None = None) -> list[str]:
    return process_argv() if argv is None else list(argv)


__all__ = ["process_argv", "workflow_argv"]
