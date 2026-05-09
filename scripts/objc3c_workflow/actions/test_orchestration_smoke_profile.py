"""Smoke test orchestration profile."""

from __future__ import annotations

from .test_orchestration_commands import (
    python_script,
    pwsh_script,
    runtime_acceptance_step,
)
from .test_orchestration_paths import BEHAVIOR_MATRIX_PY, REPLAY_PS1
from .test_orchestration_profile_model import TestOrchestrationProfile

TEST_SMOKE_PROFILE = TestOrchestrationProfile(
    action="test-smoke",
    steps=(
        ("test-behavior-matrix", python_script(BEHAVIOR_MATRIX_PY)),
        (
            "test-runtime-acceptance-fast",
            runtime_acceptance_step("test-runtime-acceptance-fast"),
        ),
        ("test-execution-replay-focused", pwsh_script(REPLAY_PS1, "-Limit", "1")),
    ),
)
