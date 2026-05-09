"""Repository hygiene, dependency, and source-cleanliness workflow actions."""

from __future__ import annotations

import sys
from collections.abc import Sequence

from ..commands import run
from ..composite_validation import run_composite_validation
from ..environment import PWSH, ROOT
from .native_build import BUILD_PS1

REPO_SUPERCLEAN_SURFACE_PY = ROOT / "scripts" / "check_repo_superclean_surface.py"
DEPENDENCY_BOUNDARIES_PY = ROOT / "scripts" / "check_objc3c_dependency_boundaries.py"
RELEASE_EVIDENCE_PY = ROOT / "scripts" / "check_release_evidence.py"
SOURCE_HYGIENE_AUTHENTICITY_PY = ROOT / "scripts" / "check_source_hygiene_authenticity.py"
SOURCE_HYGIENE_HARD_CUTOVER_PY = ROOT / "scripts" / "check_source_hygiene_hard_cutover.py"
SPEC_LINT_PY = ROOT / "scripts" / "spec_lint.py"
TASK_HYGIENE_PY = ROOT / "scripts" / "ci" / "run_task_hygiene_gate.py"


def _run_steps(actions: Sequence[str]) -> int:
    from scripts.objc3c_workflow.action_dispatch import execute_registered_action

    for action in actions:
        rc = execute_registered_action(action, [])
        if rc != 0:
            return rc
    return 0


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


def action_validate_repo_superclean(_: list[str]) -> int:
    return run_composite_validation(
        "validate-repo-superclean",
        [
            (
                "build-native-contracts",
                [
                    PWSH,
                    "-NoProfile",
                    "-ExecutionPolicy",
                    "Bypass",
                    "-File",
                    str(BUILD_PS1),
                    "-ExecutionMode",
                    "contracts-binary",
                ],
            ),
            ("task-hygiene", [sys.executable, str(TASK_HYGIENE_PY)]),
            ("source-hygiene", [sys.executable, str(SOURCE_HYGIENE_AUTHENTICITY_PY)]),
        ],
    )


def action_lint_spec(_: list[str]) -> int:
    return run([sys.executable, str(SPEC_LINT_PY)])


def action_lint(_: list[str]) -> int:
    return _run_steps(
        [
            "check-source-hygiene-hard-cutover",
            "check-task-hygiene",
            "build-site",
            "check-markdown",
        ]
    )
