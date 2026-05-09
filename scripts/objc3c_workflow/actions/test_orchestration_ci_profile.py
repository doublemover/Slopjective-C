"""CI test orchestration profile."""

from __future__ import annotations

from .application_surfaces import (
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
from .test_orchestration_paths import REPLAY_PS1, SMOKE_PS1
from .test_orchestration_profile_model import TestOrchestrationProfile

TEST_CI_PROFILE = TestOrchestrationProfile(
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
)
