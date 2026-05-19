"""Smoke test orchestration profile."""

from __future__ import annotations

from .test_orchestration_commands import (
    python_script,
    pwsh_script,
    runtime_acceptance_step,
)
from .test_orchestration_paths import BEHAVIOR_MATRIX_PY, REPLAY_PS1
from .test_orchestration_profile_model import TestOrchestrationProfile, workflow_step

TEST_SMOKE_PROFILE = TestOrchestrationProfile(
    action="test-smoke",
    profile_owner="test_orchestration_smoke_profile",
    source_owner="test_orchestration_paths",
    command_owner="test_orchestration_commands",
    report_owner="validation_timing_child_reports",
    hard_blocking_decision_owner="test_orchestration_composites",
    steps=(
        workflow_step(
            "test-behavior-matrix",
            python_script(BEHAVIOR_MATRIX_PY),
            source_owner="test_orchestration_paths",
        ),
        workflow_step(
            "test-runtime-acceptance-fast",
            runtime_acceptance_step("test-runtime-acceptance-fast"),
            source_owner="test_orchestration_commands",
        ),
        workflow_step(
            "test-execution-replay-focused",
            pwsh_script(REPLAY_PS1, "-Limit", "1"),
            source_owner="test_orchestration_paths",
        ),
    ),
)
