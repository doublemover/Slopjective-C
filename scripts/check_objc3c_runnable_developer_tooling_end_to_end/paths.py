"""Path helpers for runnable developer-tooling end-to-end validation."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.paths import normalize_rel_path


def package_path(package_root: Path, relative_path: str) -> Path:
    return package_root / normalize_rel_path(relative_path)


__all__ = ["package_path"]
