"""Contract errors for deterministic replay inputs."""

from __future__ import annotations


class DeterminismContractError(ValueError):
    """Raised when CLI input cannot be validated for deterministic replay."""


__all__ = ["DeterminismContractError"]
