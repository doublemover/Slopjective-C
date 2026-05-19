"""Usage error model for workflow argument parsing."""

from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class WorkflowUsageError(Exception):
    message: str
    exit_code: int = 2


__all__ = ["WorkflowUsageError"]
