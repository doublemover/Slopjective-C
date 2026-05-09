"""Nightly test orchestration profile."""

from __future__ import annotations

from .application_surfaces import CONFORMANCE_CORPUS_INTEGRATION_PY
from .test_orchestration_commands import (
    python_script,
    pwsh_script,
    runtime_acceptance_step,
    workflow_action,
)
from .test_orchestration_paths import (
    MATRIX_PS1,
    NEGATIVE_EXPECTATIONS_PS1,
    RECOVERY_PS1,
    REPLAY_PS1,
    SMOKE_PS1,
)
from .test_orchestration_profile_model import TestOrchestrationProfile, workflow_step

TEST_NIGHTLY_PROFILE = TestOrchestrationProfile(
    action="test-nightly",
    profile_owner="test_orchestration_nightly_profile",
    source_owner="test_orchestration_paths",
    command_owner="test_orchestration_commands",
    report_owner="validation_timing_child_reports",
    hard_blocking_decision_owner="test_orchestration_composites",
    steps=(
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
        workflow_step(
            "validate-conformance-corpus",
            python_script(CONFORMANCE_CORPUS_INTEGRATION_PY),
            source_owner="application_surfaces",
        ),
        workflow_step(
            "validate-stress",
            workflow_action("validate-stress"),
            source_owner="test_orchestration_commands",
        ),
        workflow_step(
            "validate-external-validation",
            workflow_action("validate-external-validation"),
            source_owner="test_orchestration_commands",
        ),
        workflow_step(
            "validate-public-conformance-reporting",
            workflow_action("validate-public-conformance-reporting"),
            source_owner="test_orchestration_commands",
        ),
        workflow_step(
            "validate-performance-governance",
            workflow_action("validate-performance-governance"),
            source_owner="test_orchestration_commands",
        ),
        workflow_step(
            "validate-release-foundation",
            workflow_action("validate-release-foundation"),
            source_owner="test_orchestration_commands",
        ),
        workflow_step(
            "validate-packaging-channels",
            workflow_action("validate-packaging-channels"),
            source_owner="test_orchestration_commands",
        ),
        workflow_step(
            "validate-release-operations",
            workflow_action("validate-release-operations"),
            source_owner="test_orchestration_commands",
        ),
        workflow_step(
            "validate-distribution-credibility",
            workflow_action("validate-distribution-credibility"),
            source_owner="test_orchestration_commands",
        ),
        workflow_step(
            "test-recovery",
            pwsh_script(RECOVERY_PS1),
            source_owner="test_orchestration_paths",
        ),
        workflow_step(
            "test-fixture-matrix",
            pwsh_script(MATRIX_PS1),
            source_owner="test_orchestration_paths",
        ),
        workflow_step(
            "test-negative-expectations",
            pwsh_script(NEGATIVE_EXPECTATIONS_PS1),
            source_owner="test_orchestration_paths",
        ),
    ),
)
