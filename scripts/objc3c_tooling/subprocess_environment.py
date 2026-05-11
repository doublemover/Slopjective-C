from __future__ import annotations

import os
from typing import Mapping


def environment_with_overlay(env_overlay: Mapping[str, str] | None = None) -> dict[str, str] | None:
    if env_overlay is None:
        return None
    env = os.environ.copy()
    env.update(env_overlay)
    return env


def python_child_environment(
    env_overlay: Mapping[str, str] | None = None,
    *,
    python_dont_write_bytecode: bool = True,
) -> dict[str, str] | None:
    overlay = dict(env_overlay or {})
    if python_dont_write_bytecode:
        overlay.setdefault("PYTHONDONTWRITEBYTECODE", "1")
    return environment_with_overlay(overlay) if overlay else None


__all__ = (
    "environment_with_overlay",
    "python_child_environment",
)
