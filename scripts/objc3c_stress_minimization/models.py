"""Data models for stress minimization cases."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class MinCase:
    case_id: str
    subsystem: str
    source_path: Path


__all__ = ["MinCase"]
