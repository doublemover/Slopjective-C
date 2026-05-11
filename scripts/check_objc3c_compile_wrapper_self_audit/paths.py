"""Path formatting helpers for the objc3c compile-wrapper self-audit checker."""

from __future__ import annotations

from pathlib import Path

from .config import ROOT


def repo_display_path(path: Path) -> str:
    try:
        return path.resolve().relative_to(ROOT.resolve()).as_posix()
    except ValueError:
        return path.as_posix()


__all__ = ["repo_display_path"]
