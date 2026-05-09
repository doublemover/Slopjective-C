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
from .test_orchestration_profile_model import TestOrchestrationProfile

TEST_NIGHTLY_PROFILE = TestOrchestrationProfile(
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
)
