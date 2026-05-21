"""Canonical application workspace and architecture workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from .application_surface_paths import (
    APPLICATION_ARCHITECTURE_INTEGRATION_PY,
    APPLICATION_FRAMEWORK_SAMPLES_PY,
    CANONICAL_APPLICATION_WORKSPACE_MATERIALIZER_PY,
    RUNNABLE_APPLICATION_ARCHITECTURE_E2E_PY,
)


def action_materialize_canonical_application_workspace(rest: list[str]) -> int:
    return run([sys.executable, str(CANONICAL_APPLICATION_WORKSPACE_MATERIALIZER_PY), *rest])


def action_validate_application_architecture(_: list[str]) -> int:
    return run([sys.executable, str(APPLICATION_ARCHITECTURE_INTEGRATION_PY)])


def action_validate_application_framework_samples(_: list[str]) -> int:
    return run([sys.executable, str(APPLICATION_FRAMEWORK_SAMPLES_PY)])


def action_validate_runnable_application_architecture(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_APPLICATION_ARCHITECTURE_E2E_PY)])
