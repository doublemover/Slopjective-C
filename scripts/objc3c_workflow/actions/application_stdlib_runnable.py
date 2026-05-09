"""Runnable stdlib package validation workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from .application_surface_paths import (
    RUNNABLE_STDLIB_ADVANCED_E2E_PY,
    RUNNABLE_STDLIB_FOUNDATION_E2E_PY,
    RUNNABLE_STDLIB_PROGRAM_E2E_PY,
)


def action_validate_runnable_stdlib_foundation(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STDLIB_FOUNDATION_E2E_PY)])


def action_validate_runnable_stdlib_advanced(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STDLIB_ADVANCED_E2E_PY)])


def action_validate_runnable_stdlib_program(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STDLIB_PROGRAM_E2E_PY)])


__all__ = [
    "action_validate_runnable_stdlib_advanced",
    "action_validate_runnable_stdlib_foundation",
    "action_validate_runnable_stdlib_program",
]
