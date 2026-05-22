from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from scripts.objc3c_workflow.actions import (  # noqa: E402
    release_governance_operations_validation as release_validation,
)

import check_objc3c_cross_lane_e2e as cross_lane  # noqa: E402


def test_validate_release_operations_skip_upstream_omits_packaging_channels(
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

    monkeypatch.setattr(release_validation, "run_composite_validation", fake_composite)
    monkeypatch.setattr(release_validation, "run", fake_run)

    assert release_validation.action_validate_release_operations(["--skip-upstream"]) == 0

    assert observed_steps
    assert "validate-packaging-channels" not in {name for name, _ in observed_steps}
    assert observed_final_commands == [
        [
            sys.executable,
            str(release_validation.RELEASE_OPERATIONS_END_TO_END_PY),
            "--skip-upstream",
        ]
    ]


def test_validate_release_operations_default_includes_packaging_channels(
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

    monkeypatch.setattr(release_validation, "run_composite_validation", fake_composite)
    monkeypatch.setattr(
        release_validation,
        "workflow_command",
        lambda action: ["npm", "run", "objc3c", "--", action],
    )
    monkeypatch.setattr(release_validation, "run", fake_run)

    assert release_validation.action_validate_release_operations([]) == 0

    assert observed_steps[0] == (
        "validate-packaging-channels",
        ["npm", "run", "objc3c", "--", "validate-packaging-channels"],
    )
    assert observed_final_commands == [
        [
            sys.executable,
            str(release_validation.RELEASE_OPERATIONS_END_TO_END_PY),
            "--skip-upstream",
        ]
    ]


def test_cross_lane_release_preflight_generates_then_validates_skip_upstream(
    monkeypatch,
) -> None:
    calls: list[tuple[str, list[str], str]] = []
    release_model = {
        "model": "tests/tooling/fixtures/release_operations/channel_operations_model.json",
        "clean_install_prerequisite_channels": [
            "candidate",
            "nightly",
            "preview",
            "stable",
        ],
        "rollback_channels": {
            "stable": "local-installer",
            "nightly": "offline-bundle",
        },
        "release_gate_actions": {
            "stable": ["validate-release-operations-end-to-end"],
            "nightly": ["test-nightly"],
        },
        "fail_closed_rule_count": 1,
    }

    def fake_run_public_workflow_action(
        action: str,
        *,
        args: list[str] | None = None,
        log_path: Path,
        domain: str,
    ) -> None:
        calls.append((action, list(args or []), domain))

    monkeypatch.setattr(
        cross_lane,
        "validate_distribution_release_operations_model",
        lambda family_id: release_model,
    )
    monkeypatch.setattr(
        cross_lane,
        "clean_release_operations_preflight_outputs",
        lambda: {
            "requested": True,
            "owned_roots": [],
            "removed_owned_outputs": [],
            "generated_from_clean_owned_outputs": True,
        },
    )
    monkeypatch.setattr(cross_lane, "run_public_workflow_action", fake_run_public_workflow_action)
    monkeypatch.setattr(cross_lane, "require_artifact", lambda path, label: None)
    monkeypatch.setattr(
        cross_lane,
        "release_operations_artifact_digests",
        lambda: {
            "tmp/reports/release-operations/end-to-end-summary.json": "summary-sha",
            "tmp/artifacts/release-operations/update-manifest/objc3c-update-manifest.json": "update-sha",
            "tmp/artifacts/release-operations/channel-manifest/objc3c-release-channel-manifest.json": "channel-sha",
        },
    )

    payload = cross_lane.prepare_release_operations_preflight(
        [
            {
                "family": {
                    "family_id": "distribution_package_lifecycle",
                    "expectation": "tests/tooling/fixtures/cross_lane_e2e/distribution_package_lifecycle.expectation.json",
                }
            }
        ]
    )

    assert payload["status"] == "PASS"
    assert payload["consumer_family_id"] == "distribution_package_lifecycle"
    assert payload["action"] == "validate-release-operations"
    assert payload["args"] == ["--skip-upstream"]
    assert payload["generated_from_clean_owned_outputs"] is True
    assert payload["release_model"] == release_model
    assert calls == [
        (
            "build-package-channels",
            [],
            "distribution_package_lifecycle.deterministic release artifact generation",
        ),
        (
            "validate-release-operations",
            ["--skip-upstream"],
            "distribution_package_lifecycle.release operations preflight",
        ),
    ]
