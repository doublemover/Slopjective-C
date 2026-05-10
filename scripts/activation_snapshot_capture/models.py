"""Models and contracts for activation snapshot capture."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any, Protocol


class SnapshotError(RuntimeError):
    """Raised when snapshot inputs cannot be normalized deterministically."""


@dataclass(frozen=True)
class SnapshotOutputPaths:
    issues_output: Path
    milestones_output: Path


@dataclass(frozen=True)
class SnapshotCaptureResult:
    issues_snapshot: dict[str, Any]
    milestones_snapshot: dict[str, Any]
    output_paths: SnapshotOutputPaths


class GhClientLike(Protocol):
    def list_issues(self, *, state: str) -> list[dict[str, Any]]:
        ...

    def api_json(self, endpoint: str, *, paginate: bool = False) -> Any:
        ...


class GhClientFactory(Protocol):
    def __call__(self, *, root: Path) -> GhClientLike:
        ...


__all__ = [
    "GhClientFactory",
    "GhClientLike",
    "SnapshotCaptureResult",
    "SnapshotError",
    "SnapshotOutputPaths",
]
