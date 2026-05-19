"""Quality-gate decision contract error types."""

from __future__ import annotations


class ContractDriftError(RuntimeError):
    """Raised when baseline contract rows are structurally valid but semantically drifted."""


class ContractHardFailError(RuntimeError):
    """Raised when baseline contract rows are malformed and not safely consumable."""


__all__ = ["ContractDriftError", "ContractHardFailError"]
