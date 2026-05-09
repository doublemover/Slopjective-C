"""Source-policy, dependency-boundary, and hygiene check actions."""

from __future__ import annotations

import sys

from ..commands import run
from .hygiene_paths import (
    DEPENDENCY_BOUNDARIES_PY,
    RELEASE_EVIDENCE_PY,
    REPO_SUPERCLEAN_SURFACE_PY,
    SOURCE_HYGIENE_AUTHENTICITY_PY,
    SOURCE_HYGIENE_HARD_CUTOVER_PY,
    SPEC_LINT_PY,
    TASK_HYGIENE_PY,
)


def action_check_dependency_boundaries(_: list[str]) -> int:
    return run([sys.executable, str(DEPENDENCY_BOUNDARIES_PY), "--strict"])


def action_check_release_evidence(_: list[str]) -> int:
    return run([sys.executable, str(RELEASE_EVIDENCE_PY)])


def action_check_source_hygiene_authenticity(_: list[str]) -> int:
    return run([sys.executable, str(SOURCE_HYGIENE_AUTHENTICITY_PY)])


def action_check_source_hygiene_hard_cutover(_: list[str]) -> int:
    return run([sys.executable, str(SOURCE_HYGIENE_HARD_CUTOVER_PY)])


def action_check_task_hygiene(_: list[str]) -> int:
    return run([sys.executable, str(TASK_HYGIENE_PY)])


def action_check_repo_superclean_surface(_: list[str]) -> int:
    return run([sys.executable, str(REPO_SUPERCLEAN_SURFACE_PY)])


def action_lint_spec(_: list[str]) -> int:
    return run([sys.executable, str(SPEC_LINT_PY)])
