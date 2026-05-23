from __future__ import annotations

import sys

from scripts.objc3c_workflow.actions import (
    release_governance_distribution_credibility_validation as distribution_validation,
)
from scripts.objc3c_workflow.actions import (
    release_governance_foundation_validation as foundation_validation,
)
from scripts.objc3c_workflow.actions import (
    release_governance_operations_validation as operations_validation,
)
from scripts.objc3c_workflow.actions import (
    release_governance_packaging_validation as packaging_validation,
)
from scripts.objc3c_workflow.actions.test_orchestration_nightly_profile import (
    TEST_NIGHTLY_PROFILE,
)
from scripts.objc3c_workflow.registry_views import action_spec
import scripts.check_platform_hardening_toolchain_range_replay as toolchain_range_replay


def test_validate_release_foundation_can_reuse_performance_governance_report(
    monkeypatch,
) -> None:
    observed_steps: list[tuple[str, list[str]]] = []

    def fake_composite(action: str, steps: list[tuple[str, list[str]]]) -> int:
        assert action == "validate-release-foundation"
        observed_steps.extend((name, list(command)) for name, command in steps)
        return 0

    monkeypatch.setattr(foundation_validation, "run_composite_validation", fake_composite)

    assert foundation_validation.action_validate_release_foundation(["--reuse-upstream-report"]) == 0
    assert observed_steps[0] == (
        "validate-performance-governance",
        [
            sys.executable,
            str(foundation_validation.PERFORMANCE_GOVERNANCE_INTEGRATION_PY),
        ],
    )


def test_validate_packaging_channels_can_reuse_release_foundation_report(
    monkeypatch,
) -> None:
    observed_steps: list[tuple[str, list[str]]] = []

    def fake_composite(action: str, steps: list[tuple[str, list[str]]]) -> int:
        assert action == "validate-packaging-channels"
        observed_steps.extend((name, list(command)) for name, command in steps)
        return 0

    monkeypatch.setattr(packaging_validation, "run_composite_validation", fake_composite)

    assert packaging_validation.action_validate_packaging_channels(["--reuse-upstream-report"]) == 0
    assert observed_steps[0] == (
        "validate-release-foundation",
        [
            sys.executable,
            str(packaging_validation.RELEASE_FOUNDATION_INTEGRATION_PY),
        ],
    )
    assert observed_steps[-1] == (
        "build-package-channels",
        [
            sys.executable,
            str(packaging_validation.PACKAGE_CHANNELS_BUILD_PY),
            "--reuse-release-foundation-artifacts",
        ],
    )


def test_validate_release_operations_can_reuse_packaging_channels_report(
    monkeypatch,
) -> None:
    observed_steps: list[tuple[str, list[str]]] = []
    observed_final_commands: list[list[str]] = []

    def fake_composite(action: str, steps: list[tuple[str, list[str]]]) -> int:
        assert action == "validate-release-operations"
        observed_steps.extend((name, list(command)) for name, command in steps)
        return 0

    def fake_run(command: list[str]) -> int:
        observed_final_commands.append(list(command))
        return 0

    monkeypatch.setattr(operations_validation, "run_composite_validation", fake_composite)
    monkeypatch.setattr(operations_validation, "run", fake_run)

    assert operations_validation.action_validate_release_operations(["--reuse-upstream-report"]) == 0
    assert observed_steps[0] == (
        "validate-packaging-channels",
        [
            sys.executable,
            str(operations_validation.PACKAGING_CHANNELS_INTEGRATION_PY),
        ],
    )
    assert observed_final_commands == [
        [
            sys.executable,
            str(operations_validation.RELEASE_OPERATIONS_END_TO_END_PY),
            "--skip-upstream",
        ]
    ]


def test_platform_toolchain_range_replay_reuses_build_package_validation(
    monkeypatch,
) -> None:
    observed_steps: list[str] = []

    def fake_require_packaging_validation_input() -> dict[str, object]:
        observed_steps.append("reuse-platform-build-package-validation")
        return {
            "step": "reuse-platform-build-package-validation",
            "command": ["report", "build-package-validation", "package-channels"],
            "status": "PASS",
        }

    def fake_run_refresh_step(step: str, command: list[str]) -> dict[str, object]:
        observed_steps.append(step)
        assert "validate-packaging-channels" not in command
        return {"step": step, "command": command, "status": "PASS"}

    monkeypatch.setattr(
        toolchain_range_replay,
        "require_packaging_validation_input",
        fake_require_packaging_validation_input,
    )
    monkeypatch.setattr(toolchain_range_replay, "run_refresh_step", fake_run_refresh_step)

    steps = toolchain_range_replay.refresh_release_operations_metadata()

    assert [step["step"] for step in steps] == [
        "reuse-platform-build-package-validation",
        "check-release-operations-surface",
        "check-release-operations-schema-surface",
        "build-update-manifest",
        "publish-release-operations",
    ]
    assert observed_steps[0] == "reuse-platform-build-package-validation"
    assert "validate-packaging-channels" not in observed_steps


def test_validate_distribution_credibility_can_reuse_release_operations_report(
    monkeypatch,
) -> None:
    observed_steps: list[tuple[str, list[str]]] = []

    def fake_composite(action: str, steps: list[tuple[str, list[str]]]) -> int:
        assert action == "validate-distribution-credibility"
        observed_steps.extend((name, list(command)) for name, command in steps)
        return 0

    monkeypatch.setattr(
        distribution_validation,
        "require_distribution_credibility_owner_contract",
        lambda action: None,
    )
    monkeypatch.setattr(distribution_validation, "run_composite_validation", fake_composite)

    assert (
        distribution_validation.action_validate_distribution_credibility(
            ["--reuse-upstream-report"]
        )
        == 0
    )
    assert observed_steps[0] == (
        "validate-release-operations",
        [
            sys.executable,
            str(distribution_validation.RELEASE_OPERATIONS_INTEGRATION_PY),
        ],
    )
    assert observed_steps[1] == (
        "validate-packaging-channels-end-to-end",
        [
            "npm",
            "run",
            "objc3c",
            "--",
            "validate-packaging-channels-end-to-end",
            "--use-existing-build-report",
        ],
    )


def test_nightly_release_lane_uses_existing_upstream_reports() -> None:
    commands_by_action = {
        step.action: step.command
        for step in TEST_NIGHTLY_PROFILE.steps
        if step.action.startswith("validate-release")
        or step.action.startswith("validate-packaging")
        or step.action.startswith("validate-distribution")
    }

    assert commands_by_action["validate-release-foundation"][-1] == "--reuse-upstream-report"
    assert commands_by_action["validate-packaging-channels"][-1] == "--reuse-upstream-report"
    assert commands_by_action["validate-release-operations"][-1] == "--reuse-upstream-report"
    assert commands_by_action["validate-distribution-credibility"][-1] == "--reuse-upstream-report"


def test_release_reuse_actions_accept_runner_arguments() -> None:
    for action in (
        "validate-release-foundation",
        "validate-packaging-channels",
        "validate-release-operations",
        "validate-distribution-credibility",
        "validate-packaging-channels-end-to-end",
    ):
        assert action_spec(action).pass_through_args is True
