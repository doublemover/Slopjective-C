from __future__ import annotations

import os
from typing import Mapping

from .paths import ROOT


def pythonpath_with_repo_root(current_pythonpath: str | None) -> str:
    root_path = str(ROOT)
    if not current_pythonpath:
        return root_path
    entries = current_pythonpath.split(os.pathsep)
    if root_path in entries:
        return current_pythonpath
    return os.pathsep.join((root_path, current_pythonpath))


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
    overlay["PYTHONPATH"] = pythonpath_with_repo_root(
        overlay.get("PYTHONPATH", os.environ.get("PYTHONPATH"))
    )
    if python_dont_write_bytecode:
        overlay.setdefault("PYTHONDONTWRITEBYTECODE", "1")
    return environment_with_overlay(overlay) if overlay else None


__all__ = (
    "environment_with_overlay",
    "python_child_environment",
    "pythonpath_with_repo_root",
)
