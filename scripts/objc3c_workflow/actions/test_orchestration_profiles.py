"""Named public test orchestration profile definitions."""

from __future__ import annotations

from .test_orchestration_ci_profile import TEST_CI_PROFILE
from .test_orchestration_full_profile import TEST_FULL_PROFILE
from .test_orchestration_nightly_profile import TEST_NIGHTLY_PROFILE
from .test_orchestration_owner_contracts import (
    require_test_orchestration_profile_payload,
    require_test_orchestration_profile_payloads,
)
from .test_orchestration_profile_model import (
    TestOrchestrationProfile,
    TestOrchestrationStep,
    WorkflowStep,
    workflow_step,
)
from .test_orchestration_smoke_profile import TEST_SMOKE_PROFILE

TEST_ORCHESTRATION_PROFILES: dict[str, TestOrchestrationProfile] = {
    "test-smoke": TEST_SMOKE_PROFILE,
    "test-ci": TEST_CI_PROFILE,
    "test-full": TEST_FULL_PROFILE,
    "test-nightly": TEST_NIGHTLY_PROFILE,
}


def test_orchestration_steps(action: str) -> list[WorkflowStep]:
    return TEST_ORCHESTRATION_PROFILES[action].materialize()


def test_orchestration_profile_payload(action: str) -> dict[str, object]:
    payload = TEST_ORCHESTRATION_PROFILES[action].owner_payload()
    require_test_orchestration_profile_payload(payload)
    return payload


def test_orchestration_profile_payloads() -> dict[str, dict[str, object]]:
    payloads = {
        action: profile.owner_payload()
        for action, profile in TEST_ORCHESTRATION_PROFILES.items()
    }
    require_test_orchestration_profile_payloads(payloads)
    return payloads


__all__ = [
    "TEST_ORCHESTRATION_PROFILES",
    "TestOrchestrationProfile",
    "TestOrchestrationStep",
    "WorkflowStep",
    "require_test_orchestration_profile_payload",
    "require_test_orchestration_profile_payloads",
    "test_orchestration_profile_payload",
    "test_orchestration_profile_payloads",
    "test_orchestration_steps",
    "workflow_step",
]
