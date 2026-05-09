"""CLI argv normalization for objc3c workflow entrypoints."""

from __future__ import annotations

import sys
from collections.abc import Sequence


def workflow_argv(argv: Sequence[str] | None = None) -> list[str]:
    return list(sys.argv[1:] if argv is None else argv)


__all__ = ["workflow_argv"]
