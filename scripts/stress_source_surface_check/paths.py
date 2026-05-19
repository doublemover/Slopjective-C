"""Path helpers for stress source-surface validation."""

from __future__ import annotations

from pathlib import Path

from .constants import ROOT


def require_path(relative_path: str, *, kind: str) -> Path:
    path = ROOT / relative_path
    if not path.exists():
        raise RuntimeError(f"missing {kind}: {relative_path}")
    return path
