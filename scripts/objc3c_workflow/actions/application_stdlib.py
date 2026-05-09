"""Stdlib workspace and validation workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from .application_surface_paths import (
    MATERIALIZE_STDLIB_PY,
    RUNNABLE_STDLIB_ADVANCED_E2E_PY,
    RUNNABLE_STDLIB_FOUNDATION_E2E_PY,
    RUNNABLE_STDLIB_PROGRAM_E2E_PY,
    STDLIB_ADVANCED_INTEGRATION_PY,
    STDLIB_FOUNDATION_INTEGRATION_PY,
    STDLIB_PROGRAM_INTEGRATION_PY,
    STDLIB_SURFACE_PY,
)


def action_check_stdlib_surface(_: list[str]) -> int:
    return run([sys.executable, str(STDLIB_SURFACE_PY)])


def action_materialize_stdlib_workspace(rest: list[str]) -> int:
    return run([sys.executable, str(MATERIALIZE_STDLIB_PY), *rest])


def action_validate_stdlib_foundation(_: list[str]) -> int:
    return run([sys.executable, str(STDLIB_FOUNDATION_INTEGRATION_PY)])


def action_validate_stdlib_advanced(_: list[str]) -> int:
    return run([sys.executable, str(STDLIB_ADVANCED_INTEGRATION_PY)])


def action_validate_stdlib_program(_: list[str]) -> int:
    return run([sys.executable, str(STDLIB_PROGRAM_INTEGRATION_PY)])


def action_validate_runnable_stdlib_foundation(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STDLIB_FOUNDATION_E2E_PY)])


def action_validate_runnable_stdlib_advanced(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STDLIB_ADVANCED_E2E_PY)])


def action_validate_runnable_stdlib_program(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STDLIB_PROGRAM_E2E_PY)])
