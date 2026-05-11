"""Failure reporting for stress source-surface validation."""

from __future__ import annotations

import sys


class StressSourceSurfaceError(RuntimeError):
    """Raised when the stress source surface violates its checked-in contract."""


def fail(message: str) -> int:
    print(f"stress-source-surface: FAIL\n- {message}", file=sys.stderr)
    return 1


def require(condition: bool, message: str) -> None:
    if not condition:
        raise StressSourceSurfaceError(message)
