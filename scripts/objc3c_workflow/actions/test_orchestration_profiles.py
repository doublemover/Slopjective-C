"""Named public test orchestration profile definitions."""

from __future__ import annotations

from .test_orchestration_ci_profile import TEST_CI_PROFILE
from .test_orchestration_full_profile import TEST_FULL_PROFILE
from .test_orchestration_nightly_profile import TEST_NIGHTLY_PROFILE
from .test_orchestration_profile_model import TestOrchestrationProfile, WorkflowStep
from .test_orchestration_smoke_profile import TEST_SMOKE_PROFILE

TEST_ORCHESTRATION_PROFILES: dict[str, TestOrchestrationProfile] = {
    "test-smoke": TEST_SMOKE_PROFILE,
    "test-ci": TEST_CI_PROFILE,
    "test-full": TEST_FULL_PROFILE,
    "test-nightly": TEST_NIGHTLY_PROFILE,
}


def test_orchestration_steps(action: str) -> list[WorkflowStep]:
    return TEST_ORCHESTRATION_PROFILES[action].materialize()


__all__ = [
    "TEST_ORCHESTRATION_PROFILES",
    "TestOrchestrationProfile",
    "WorkflowStep",
    "test_orchestration_steps",
]
