"""Shared types for runbook documentation source catalog rows."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class RunbookSourceSpec:
    path: Path
    required_tokens: tuple[str, ...] = ()
    forbidden_tokens: tuple[str, ...] = ()


__all__ = ("RunbookSourceSpec",)
