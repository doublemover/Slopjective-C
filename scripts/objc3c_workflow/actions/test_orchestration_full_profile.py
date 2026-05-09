"""Full test orchestration profile."""

from __future__ import annotations

from .test_orchestration_commands import (
    python_script,
    pwsh_script,
    runtime_acceptance_step,
)
from .test_orchestration_paths import (
    BEHAVIOR_MATRIX_PY,
    COMPILE_WRAPPER_SELF_AUDIT_PY,
    REPLAY_PS1,
    SMOKE_PS1,
)
from .test_orchestration_profile_model import TestOrchestrationProfile

TEST_FULL_PROFILE = TestOrchestrationProfile(
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
)
