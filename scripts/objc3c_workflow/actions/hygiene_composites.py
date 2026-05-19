"""Composite hygiene and superclean workflow actions."""

from __future__ import annotations

import sys
from collections.abc import Sequence

from ..composite_validation import run_composite_validation
from ..environment import PWSH
from .hygiene_paths import (
    REPO_SUPERCLEAN_SURFACE_PY,
    SOURCE_HYGIENE_AUTHENTICITY_PY,
    TASK_HYGIENE_PY,
)
from .native_build_paths import BUILD_PS1


def _run_steps(actions: Sequence[str]) -> int:
    from scripts.objc3c_workflow.action_execution_dispatch import (
        execute_registered_action,
    )

    for action in actions:
        rc = execute_registered_action(action, [])
        if rc != 0:
            return rc
    return 0


def action_validate_repo_superclean(_: list[str]) -> int:
    return run_composite_validation(
        "validate-repo-superclean",
        [
            (
                "build-native-contracts",
                [
                    PWSH,
                    "-NoProfile",
                    "-NonInteractive",
                    "-ExecutionPolicy",
                    "Bypass",
                    "-File",
                    str(BUILD_PS1),
                    "-ExecutionMode",
                    "full",
                ],
            ),
            (
                "repo-superclean-surface",
                [sys.executable, str(REPO_SUPERCLEAN_SURFACE_PY)],
            ),
            ("task-hygiene", [sys.executable, str(TASK_HYGIENE_PY)]),
            ("source-hygiene", [sys.executable, str(SOURCE_HYGIENE_AUTHENTICITY_PY)]),
        ],
    )


def action_lint(_: list[str]) -> int:
    return _run_steps(
        [
            "check-source-hygiene-hard-cutover",
            "check-task-hygiene",
            "build-site",
            "check-markdown",
        ]
    )
