"""CI test orchestration profile."""

from __future__ import annotations

from .application_surfaces import (
    STDLIB_ADVANCED_INTEGRATION_PY,
    STDLIB_FOUNDATION_INTEGRATION_PY,
    STDLIB_PROGRAM_INTEGRATION_PY,
)
from .developer_tooling_paths import (
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
from .test_orchestration_profile_model import TestOrchestrationProfile, workflow_step

TEST_CI_PROFILE = TestOrchestrationProfile(
    action="test-ci",
    profile_owner="test_orchestration_ci_profile",
    source_owner="test_orchestration_paths",
    command_owner="test_orchestration_commands",
    report_owner="validation_timing_child_reports",
    hard_blocking_decision_owner="test_orchestration_composites",
    steps=(
        workflow_step(
            "task-hygiene",
            python_script(TASK_HYGIENE_PY),
            source_owner="hygiene",
        ),
        workflow_step(
            "validate-developer-tooling",
            python_script(DEVELOPER_TOOLING_INTEGRATION_PY),
            source_owner="developer_tooling",
        ),
        workflow_step(
            "validate-bonus-experiences",
            python_script(BONUS_EXPERIENCE_INTEGRATION_PY),
            source_owner="developer_tooling",
        ),
        workflow_step(
            "validate-stdlib-foundation",
            python_script(STDLIB_FOUNDATION_INTEGRATION_PY),
            source_owner="application_surfaces",
        ),
        workflow_step(
            "validate-stdlib-advanced",
            python_script(STDLIB_ADVANCED_INTEGRATION_PY),
            source_owner="application_surfaces",
        ),
        workflow_step(
            "validate-stdlib-program",
            python_script(STDLIB_PROGRAM_INTEGRATION_PY),
            source_owner="application_surfaces",
        ),
        workflow_step(
            "validate-performance-governance",
            workflow_action("validate-performance-governance"),
            source_owner="test_orchestration_commands",
        ),
        workflow_step(
            "test-execution-smoke",
            pwsh_script(SMOKE_PS1),
            source_owner="test_orchestration_paths",
        ),
        workflow_step(
            "test-runtime-acceptance",
            runtime_acceptance_step("test-runtime-acceptance"),
            source_owner="test_orchestration_commands",
        ),
        workflow_step(
            "test-execution-replay",
            pwsh_script(REPLAY_PS1),
            source_owner="test_orchestration_paths",
        ),
    ),
)
