"""Host tool lookup helpers for workflow environment constants."""

from __future__ import annotations

import shutil


def first_available_tool(*candidates: str, fallback: str) -> str:
    for candidate in candidates:
        resolved = shutil.which(candidate)
        if resolved:
            return resolved
    return fallback


__all__ = ["first_available_tool"]
