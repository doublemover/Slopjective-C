"""Output helpers for the remaining-task extraction CLI."""

from __future__ import annotations

import sys

__all__ = [
    "write_stdout",
]


def write_stdout(value: str) -> None:
    buffer = getattr(sys.stdout, "buffer", None)
    if buffer is not None:
        buffer.write(value.encode("utf-8"))
        return
    sys.stdout.write(value)
