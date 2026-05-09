"""Stdlib workspace materialization workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from .application_surface_paths import MATERIALIZE_STDLIB_PY


def action_materialize_stdlib_workspace(rest: list[str]) -> int:
    return run([sys.executable, str(MATERIALIZE_STDLIB_PY), *rest])


__all__ = ["action_materialize_stdlib_workspace"]
