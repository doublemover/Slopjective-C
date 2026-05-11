"""Failure handling for distribution-credibility source-surface validation."""

from __future__ import annotations

import sys


def fail(message: str) -> int:
    print(f"distribution-credibility-source-surface: {message}", file=sys.stderr)
    return 1


class DistributionCredibilitySurfaceError(RuntimeError):
    """Raised when a distribution-credibility source contract drifts."""
