from __future__ import annotations

import sys
from types import SimpleNamespace

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
from scripts.objc3c_tooling.json_io import write_json_file
from scripts.objc3c_workflow.registry_views import action_spec
import scripts.check_objc3c_packaging_channels_integration as packaging_integration
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
            "--use-existing-validate-report",
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


def _packaging_workflow_surface() -> dict[str, object]:
    return {
        "contract_id": "objc3c.packaging.channels.workflow.surface.v1",
        "validate_action": "validate-packaging-channels",
        "integrated_required_steps": [
            "validate-release-foundation",
            "check-packaging-channels-surface",
            "check-packaging-channels-schema-surface",
            "build-package-channels",
        ],
        "owner_policy": {
            "source_owner": "packaging-channels-source",
            "blocker_owner": "packaging-channels-blockers",
            "evidence_log_allowed": False,
        },
        "blocker_metadata": {
            "blocker_owner": "packaging-channels-blockers",
            "blocking_conditions": [],
        },
    }


def _packaging_workflow_report(*, status: str = "PASS", failed_step: str | None = None) -> dict[str, object]:
    steps: list[dict[str, object]] = []
    for action in _packaging_workflow_surface()["integrated_required_steps"]:  # type: ignore[index]
        exit_code = 1 if action == failed_step else 0
        steps.append({"action": action, "exit_code": exit_code})
    return {
        "action": "validate-packaging-channels",
        "status": status,
        "generated_at_utc": "2026-05-24T00:00:00+00:00",
        "steps": steps,
    }


def test_packaging_channels_integration_regenerates_public_report_by_default(
    monkeypatch,
    tmp_path,
) -> None:
    workflow_surface = tmp_path / "workflow_surface.json"
    report_root = tmp_path / "public-workflow"
    summary_path = tmp_path / "package-channels" / "integration-summary.json"
    write_json_file(workflow_surface, _packaging_workflow_surface())
    write_json_file(report_root / "validate-packaging-channels.json", _packaging_workflow_report())
    observed_commands: list[list[str]] = []

    def fake_run_capture(command: list[str]) -> SimpleNamespace:
        observed_commands.append(list(command))
        return SimpleNamespace(returncode=0)

    monkeypatch.setattr(packaging_integration, "WORKFLOW_SURFACE", workflow_surface)
    monkeypatch.setattr(packaging_integration, "PUBLIC_REPORT_ROOT", report_root)
    monkeypatch.setattr(packaging_integration, "SUMMARY_PATH", summary_path)
    monkeypatch.setattr(packaging_integration, "run_capture", fake_run_capture)
    monkeypatch.setattr(packaging_integration, "repo_rel", lambda path: str(path))
    monkeypatch.setattr(packaging_integration, "parse_args", lambda: SimpleNamespace(use_existing_validate_report=False))

    assert packaging_integration.main() == 0
    assert observed_commands == [["npm", "run", "objc3c", "--", "validate-packaging-channels"]]
    summary = packaging_integration.load_json(summary_path)
    assert summary["status"] == "PASS"
    assert summary["used_existing_validate_report"] is False
    assert summary["stale_failure_reports_allowed"] is False


def test_packaging_channels_integration_rejects_existing_failed_public_report(
    monkeypatch,
    tmp_path,
) -> None:
    workflow_surface = tmp_path / "workflow_surface.json"
    report_root = tmp_path / "public-workflow"
    write_json_file(workflow_surface, _packaging_workflow_surface())
    write_json_file(
        report_root / "validate-packaging-channels.json",
        _packaging_workflow_report(
            status="FAIL",
            failed_step="check-packaging-channels-schema-surface",
        ),
    )

    monkeypatch.setattr(packaging_integration, "WORKFLOW_SURFACE", workflow_surface)
    monkeypatch.setattr(packaging_integration, "PUBLIC_REPORT_ROOT", report_root)
    monkeypatch.setattr(packaging_integration, "repo_rel", lambda path: str(path))
    monkeypatch.setattr(
        packaging_integration,
        "parse_args",
        lambda: SimpleNamespace(use_existing_validate_report=True),
    )

    try:
        packaging_integration.main()
    except RuntimeError as exc:
        message = str(exc)
    else:
        raise AssertionError("expected failed existing report to be rejected")

    assert "public packaging-channels report is not PASS" in message
    assert "check-packaging-channels-schema-surface" in message
