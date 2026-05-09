from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow.actions.test_orchestration_ci_profile import (
    TEST_CI_PROFILE,
)
from scripts.objc3c_workflow.actions.test_orchestration_full_profile import (
    TEST_FULL_PROFILE,
)
from scripts.objc3c_workflow.actions.test_orchestration_nightly_profile import (
    TEST_NIGHTLY_PROFILE,
)
from scripts.objc3c_workflow.actions.test_orchestration_profile_model import (
    TestOrchestrationProfile,
    TestOrchestrationStep,
)
from scripts.objc3c_workflow.actions.test_orchestration_profiles import (
    TEST_ORCHESTRATION_PROFILES,
    test_orchestration_profile_payload,
    test_orchestration_profile_payloads,
    test_orchestration_steps,
)
from scripts.objc3c_workflow.actions.test_orchestration_smoke_profile import (
    TEST_SMOKE_PROFILE,
)

ROOT = Path(__file__).resolve().parents[2]
ACTION_ROOT = ROOT / "scripts" / "objc3c_workflow" / "actions"

PROFILE_OWNER_MODULES = (
    "test_orchestration_profile_model",
    "test_orchestration_smoke_profile",
    "test_orchestration_ci_profile",
    "test_orchestration_full_profile",
    "test_orchestration_nightly_profile",
)


def test_test_orchestration_profiles_is_catalog_facade() -> None:
    facade_text = (ACTION_ROOT / "test_orchestration_profiles.py").read_text(
        encoding="utf-8"
    )

    for module_name in PROFILE_OWNER_MODULES:
        assert importlib.import_module(
            f"scripts.objc3c_workflow.actions.{module_name}"
        )
        assert f"from .{module_name} import" in facade_text
    assert "python_script(" not in facade_text
    assert "pwsh_script(" not in facade_text
    assert "runtime_acceptance_step(" not in facade_text


def test_test_orchestration_profile_catalog_preserves_public_actions() -> None:
    assert TEST_ORCHESTRATION_PROFILES == {
        "test-smoke": TEST_SMOKE_PROFILE,
        "test-ci": TEST_CI_PROFILE,
        "test-full": TEST_FULL_PROFILE,
        "test-nightly": TEST_NIGHTLY_PROFILE,
    }
    for profile in TEST_ORCHESTRATION_PROFILES.values():
        assert isinstance(profile, TestOrchestrationProfile)
        assert profile.profile_owner
        assert profile.command_owner == "test_orchestration_commands"
        assert profile.report_owner == "validation_timing_child_reports"
        assert profile.hard_blocking_decision_owner == "test_orchestration_composites"
        assert all(isinstance(step, TestOrchestrationStep) for step in profile.steps)


def test_test_orchestration_steps_materialize_mutable_command_lists() -> None:
    steps = test_orchestration_steps("test-smoke")

    assert steps[0][0] == "test-behavior-matrix"
    assert isinstance(steps[0][1], list)
    assert steps[1][0] == "test-runtime-acceptance-fast"
    assert steps[2][0] == "test-execution-replay-focused"


def test_test_orchestration_profile_payloads_are_owner_explicit() -> None:
    payloads = test_orchestration_profile_payloads()

    assert set(payloads) == set(TEST_ORCHESTRATION_PROFILES)
    smoke = test_orchestration_profile_payload("test-smoke")
    assert smoke == payloads["test-smoke"]
    assert smoke["profile_owner"] == "test_orchestration_smoke_profile"
    assert smoke["source_owner"] == "test_orchestration_paths"
    assert smoke["command_owner"] == "test_orchestration_commands"
    assert smoke["report_owner"] == "validation_timing_child_reports"
    assert smoke["hard_blocking_decision_owner"] == "test_orchestration_composites"
    assert smoke["steps"] == [
        {
            "action": "test-behavior-matrix",
            "source_owner": "test_orchestration_paths",
            "command_owner": "test_orchestration_commands",
            "hard_blocking_decision_owner": "test_orchestration_composites",
        },
        {
            "action": "test-runtime-acceptance-fast",
            "source_owner": "test_orchestration_commands",
            "command_owner": "test_orchestration_commands",
            "hard_blocking_decision_owner": "test_orchestration_composites",
        },
        {
            "action": "test-execution-replay-focused",
            "source_owner": "test_orchestration_paths",
            "command_owner": "test_orchestration_commands",
            "hard_blocking_decision_owner": "test_orchestration_composites",
        },
    ]
