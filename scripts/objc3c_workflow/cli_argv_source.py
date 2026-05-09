"""Process argv source for workflow CLI entrypoints."""

from __future__ import annotations

import sys


def process_argv() -> list[str]:
    return list(sys.argv[1:])


__all__ = ["process_argv"]
