"""Named public test orchestration profile definitions."""

from __future__ import annotations

from collections.abc import Sequence
from dataclasses import dataclass

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
from .test_orchestration_commands import (
    python_script,
    pwsh_script,
    runtime_acceptance_step,
    workflow_action,
)
from .test_orchestration_paths import (
    BEHAVIOR_MATRIX_PY,
    COMPILE_WRAPPER_SELF_AUDIT_PY,
    MATRIX_PS1,
    NEGATIVE_EXPECTATIONS_PS1,
    RECOVERY_PS1,
    REPLAY_PS1,
    SMOKE_PS1,
)

WorkflowStep = tuple[str, Sequence[str]]


@dataclass(frozen=True)
class TestOrchestrationProfile:
    action: str
    steps: tuple[WorkflowStep, ...]

    def materialize(self) -> list[WorkflowStep]:
        return [(action, list(command)) for action, command in self.steps]


TEST_ORCHESTRATION_PROFILES: dict[str, TestOrchestrationProfile] = {
    "test-smoke": TestOrchestrationProfile(
        action="test-smoke",
        steps=(
            ("test-behavior-matrix", python_script(BEHAVIOR_MATRIX_PY)),
            (
                "test-runtime-acceptance-fast",
                runtime_acceptance_step("test-runtime-acceptance-fast"),
            ),
            ("test-execution-replay-focused", pwsh_script(REPLAY_PS1, "-Limit", "1")),
        ),
    ),
    "test-ci": TestOrchestrationProfile(
        action="test-ci",
        steps=(
            ("task-hygiene", python_script(TASK_HYGIENE_PY)),
            (
                "validate-developer-tooling",
                python_script(DEVELOPER_TOOLING_INTEGRATION_PY),
            ),
            (
                "validate-bonus-experiences",
                python_script(BONUS_EXPERIENCE_INTEGRATION_PY),
            ),
            (
                "validate-stdlib-foundation",
                python_script(STDLIB_FOUNDATION_INTEGRATION_PY),
            ),
            (
                "validate-stdlib-advanced",
                python_script(STDLIB_ADVANCED_INTEGRATION_PY),
            ),
            (
                "validate-stdlib-program",
                python_script(STDLIB_PROGRAM_INTEGRATION_PY),
            ),
            (
                "validate-performance-governance",
                workflow_action("validate-performance-governance"),
            ),
            ("test-execution-smoke", pwsh_script(SMOKE_PS1)),
            (
                "test-runtime-acceptance",
                runtime_acceptance_step("test-runtime-acceptance"),
            ),
            ("test-execution-replay", pwsh_script(REPLAY_PS1)),
        ),
    ),
    "test-full": TestOrchestrationProfile(
        action="test-full",
        steps=(
            ("test-behavior-matrix", python_script(BEHAVIOR_MATRIX_PY)),
            (
                "test-compile-wrapper-self-audit",
                python_script(COMPILE_WRAPPER_SELF_AUDIT_PY),
            ),
            ("test-execution-smoke", pwsh_script(SMOKE_PS1, "-Limit", "24")),
            (
                "test-runtime-acceptance-fast",
                runtime_acceptance_step("test-runtime-acceptance-fast"),
            ),
            ("test-execution-replay-focused", pwsh_script(REPLAY_PS1, "-Limit", "1")),
        ),
    ),
    "test-nightly": TestOrchestrationProfile(
        action="test-nightly",
        steps=(
            ("test-execution-smoke", pwsh_script(SMOKE_PS1)),
            (
                "test-runtime-acceptance",
                runtime_acceptance_step("test-runtime-acceptance"),
            ),
            ("test-execution-replay", pwsh_script(REPLAY_PS1)),
            (
                "validate-conformance-corpus",
                python_script(CONFORMANCE_CORPUS_INTEGRATION_PY),
            ),
            ("validate-stress", workflow_action("validate-stress")),
            (
                "validate-external-validation",
                workflow_action("validate-external-validation"),
            ),
            (
                "validate-public-conformance-reporting",
                workflow_action("validate-public-conformance-reporting"),
            ),
            (
                "validate-performance-governance",
                workflow_action("validate-performance-governance"),
            ),
            (
                "validate-release-foundation",
                workflow_action("validate-release-foundation"),
            ),
            (
                "validate-packaging-channels",
                workflow_action("validate-packaging-channels"),
            ),
            (
                "validate-release-operations",
                workflow_action("validate-release-operations"),
            ),
            (
                "validate-distribution-credibility",
                workflow_action("validate-distribution-credibility"),
            ),
            ("test-recovery", pwsh_script(RECOVERY_PS1)),
            ("test-fixture-matrix", pwsh_script(MATRIX_PS1)),
            (
                "test-negative-expectations",
                pwsh_script(NEGATIVE_EXPECTATIONS_PS1),
            ),
        ),
    ),
}


def test_orchestration_steps(action: str) -> list[WorkflowStep]:
    return TEST_ORCHESTRATION_PROFILES[action].materialize()

