"""Stdlib surface workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from .application_surface_paths import STDLIB_SURFACE_PY


def action_check_stdlib_surface(_: list[str]) -> int:
    return run([sys.executable, str(STDLIB_SURFACE_PY)])


__all__ = ["action_check_stdlib_surface"]
