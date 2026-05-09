"""Composite public test orchestration actions."""

from __future__ import annotations

import sys

from ..commands import workflow_command
from ..composite_validation import run_composite_validation
from ..environment import PWSH
from .application_surfaces import (
    CONFORMANCE_CORPUS_INTEGRATION_PY,
    STDLIB_ADVANCED_INTEGRATION_PY,
    STDLIB_FOUNDATION_INTEGRATION_PY,
    STDLIB_PROGRAM_INTEGRATION_PY,
)
from .developer_tooling import (
    BONUS_EXPERIENCE_INTEGRATION_PY,
    DEVELOPER_TOOLING_INTEGRATION_PY,
)
from .hygiene import TASK_HYGIENE_PY
from .runtime_tests import RUNTIME_ACCEPTANCE_PY
from .test_orchestration_paths import (
    BEHAVIOR_MATRIX_PY,
    COMPILE_WRAPPER_SELF_AUDIT_PY,
    MATRIX_PS1,
    NEGATIVE_EXPECTATIONS_PS1,
    RECOVERY_PS1,
    REPLAY_PS1,
    SMOKE_PS1,
)


def _pwsh_script(script: object, *args: str) -> list[str]:
    return [
        PWSH,
        "-NoProfile",
        "-ExecutionPolicy",
        "Bypass",
        "-File",
        str(script),
        *args,
    ]


def _runtime_acceptance(*args: str) -> list[str]:
    return [sys.executable, str(RUNTIME_ACCEPTANCE_PY), *args]


def action_test_smoke(_: list[str]) -> int:
    return run_composite_validation(
        "test-smoke",
        [
            ("test-behavior-matrix", [sys.executable, str(BEHAVIOR_MATRIX_PY)]),
            ("test-runtime-acceptance-fast", _runtime_acceptance("--suite", "fast")),
            ("test-execution-replay-focused", _pwsh_script(REPLAY_PS1, "-Limit", "1")),
        ],
    )


def action_test_ci(_: list[str]) -> int:
    return run_composite_validation(
        "test-ci",
        [
            ("task-hygiene", [sys.executable, str(TASK_HYGIENE_PY)]),
            (
                "validate-developer-tooling",
                [sys.executable, str(DEVELOPER_TOOLING_INTEGRATION_PY)],
            ),
            (
                "validate-bonus-experiences",
                [sys.executable, str(BONUS_EXPERIENCE_INTEGRATION_PY)],
            ),
            (
                "validate-stdlib-foundation",
                [sys.executable, str(STDLIB_FOUNDATION_INTEGRATION_PY)],
            ),
            (
                "validate-stdlib-advanced",
                [sys.executable, str(STDLIB_ADVANCED_INTEGRATION_PY)],
            ),
            (
                "validate-stdlib-program",
                [sys.executable, str(STDLIB_PROGRAM_INTEGRATION_PY)],
            ),
            (
                "validate-performance-governance",
                workflow_command("validate-performance-governance"),
            ),
            ("test-execution-smoke", _pwsh_script(SMOKE_PS1)),
            ("test-runtime-acceptance", _runtime_acceptance()),
            ("test-execution-replay", _pwsh_script(REPLAY_PS1)),
        ],
    )


def action_test_full(_: list[str]) -> int:
    return run_composite_validation(
        "test-full",
        [
            ("test-behavior-matrix", [sys.executable, str(BEHAVIOR_MATRIX_PY)]),
            (
                "test-compile-wrapper-self-audit",
                [sys.executable, str(COMPILE_WRAPPER_SELF_AUDIT_PY)],
            ),
            ("test-execution-smoke", _pwsh_script(SMOKE_PS1, "-Limit", "24")),
            ("test-runtime-acceptance-fast", _runtime_acceptance("--suite", "fast")),
            ("test-execution-replay-focused", _pwsh_script(REPLAY_PS1, "-Limit", "1")),
        ],
    )


def action_test_nightly(_: list[str]) -> int:
    return run_composite_validation(
        "test-nightly",
        [
            ("test-execution-smoke", _pwsh_script(SMOKE_PS1)),
            ("test-runtime-acceptance", _runtime_acceptance()),
            ("test-execution-replay", _pwsh_script(REPLAY_PS1)),
            (
                "validate-conformance-corpus",
                [sys.executable, str(CONFORMANCE_CORPUS_INTEGRATION_PY)],
            ),
            ("validate-stress", workflow_command("validate-stress")),
            ("validate-external-validation", workflow_command("validate-external-validation")),
            (
                "validate-public-conformance-reporting",
                workflow_command("validate-public-conformance-reporting"),
            ),
            (
                "validate-performance-governance",
                workflow_command("validate-performance-governance"),
            ),
            ("validate-release-foundation", workflow_command("validate-release-foundation")),
            ("validate-packaging-channels", workflow_command("validate-packaging-channels")),
            ("validate-release-operations", workflow_command("validate-release-operations")),
            (
                "validate-distribution-credibility",
                workflow_command("validate-distribution-credibility"),
            ),
            ("test-recovery", _pwsh_script(RECOVERY_PS1)),
            ("test-fixture-matrix", _pwsh_script(MATRIX_PS1)),
            ("test-negative-expectations", _pwsh_script(NEGATIVE_EXPECTATIONS_PS1)),
        ],
    )
