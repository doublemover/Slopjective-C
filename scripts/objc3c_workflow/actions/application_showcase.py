"""Showcase and getting-started workflow actions."""

from __future__ import annotations

import sys

from ..commands import pwsh_file, run
from .application_surface_paths import (
    GETTING_STARTED_INTEGRATION_PY,
    RUNNABLE_SHOWCASE_E2E_PY,
    SHOWCASE_INTEGRATION_PY,
    SHOWCASE_RUNTIME_PS1,
    SHOWCASE_SURFACE_PY,
)


def action_check_showcase_surface(rest: list[str]) -> int:
    return run([sys.executable, str(SHOWCASE_SURFACE_PY), *rest])


def action_validate_showcase_runtime(rest: list[str]) -> int:
    return pwsh_file(SHOWCASE_RUNTIME_PS1, *rest)


def action_validate_showcase(_: list[str]) -> int:
    return run([sys.executable, str(SHOWCASE_INTEGRATION_PY)])


def action_validate_runnable_showcase(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_SHOWCASE_E2E_PY)])


def action_validate_getting_started(_: list[str]) -> int:
    return run([sys.executable, str(GETTING_STARTED_INTEGRATION_PY)])
