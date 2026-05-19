"""Runtime acceptance report surface builders."""

from __future__ import annotations

from .claim_boundary import build_claim_boundary
from .suite_surface import build_acceptance_suite_surface

__all__ = [
    "build_acceptance_suite_surface",
    "build_claim_boundary",
]
