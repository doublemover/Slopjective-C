"""Bonus-experience validation and project-template actions."""

from __future__ import annotations

import sys

from ..commands import run
from .developer_tooling_paths import (
    BONUS_EXPERIENCE_INTEGRATION_PY,
    PROJECT_TEMPLATE_MATERIALIZER_PY,
    RUNNABLE_BONUS_EXPERIENCE_E2E_PY,
)


def action_validate_bonus_experiences(_: list[str]) -> int:
    return run([sys.executable, str(BONUS_EXPERIENCE_INTEGRATION_PY)])


def action_materialize_project_template(rest: list[str]) -> int:
    return run([sys.executable, str(PROJECT_TEMPLATE_MATERIALIZER_PY), *rest])


def action_validate_runnable_bonus_experiences(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_BONUS_EXPERIENCE_E2E_PY)])
