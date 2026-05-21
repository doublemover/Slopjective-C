from __future__ import annotations

import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from scripts.build_objc3c_update_manifest import (
    release_evidence_payload,
    validate_package_channel_freshness,
    validate_channel_operations_model,
)


CHANNEL_MODEL = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "release_operations"
    / "channel_operations_model.json"
)


def load_channel_model() -> dict:
    return json.loads(CHANNEL_MODEL.read_text(encoding="utf-8"))


def update_channel_policy_for(model: dict) -> dict:
    channel_ids = [entry["channel_id"] for entry in model["channels"]]
    return {
        "default_channel": "stable",
        "channel_order": channel_ids,
        "channels": [
            {
                "channel_id": channel_id,
                "support_status": "supported" if channel_id == "stable" else "release-drill",
                "warning_classes": [],
                "permitted_upgrade_targets": [],
                "revert_channel": "local-installer",
            }
            for channel_id in channel_ids
        ],
    }


def test_release_operations_channel_model_separates_stable_and_nightly_gates() -> None:
    model = load_channel_model()
    channels = {entry["channel_id"]: entry for entry in model["channels"]}

    assert model["required_channels"] == ["stable", "nightly"]
    assert channels["stable"]["operation_class"] == "stable"
    assert channels["nightly"]["operation_class"] == "nightly"

    stable_gates = set(channels["stable"]["release_gate_actions"])
    nightly_gates = set(channels["nightly"]["release_gate_actions"])

    assert "validate-release-candidate-conformance" in stable_gates
    assert "test-nightly" in nightly_gates
    assert stable_gates != nightly_gates


def test_release_operations_channel_model_requires_blocking_rollback_safety() -> None:
    model = load_channel_model()

    for channel in model["channels"]:
        rollback_safety = channel["rollback_safety"]
        assert rollback_safety["operator_command"] == (
            "npm run objc3c -- validate-packaging-channels-end-to-end"
        )
        assert rollback_safety["blocks_publication_on_failure"] is True
        assert rollback_safety["rollback_channel"] in {
            "local-installer",
            "offline-bundle",
        }


def test_release_operations_channels_require_from_nothing_clean_install() -> None:
    model = load_channel_model()

    for channel in model["channels"]:
        assert channel["clean_install_prerequisite"] == {
            "required_action": "validate-package-install-distribution",
            "required_flag": "--from-nothing",
            "required_summary": "tmp/reports/package-ecosystem/install-distribution-credibility-summary.json",
            "blocks_publication_on_failure": True,
        }


def test_release_operations_channels_require_package_channel_freshness() -> None:
    model = load_channel_model()

    for channel in model["channels"]:
        assert channel["package_channel_freshness"] == {
            "timestamp_sources": [
                "package_channels_summary.generated_at_utc",
                "package_channels_manifest.generated_at_utc",
                "platform_support_matrix.generated_at_utc",
            ],
            "max_artifact_skew_hours": 6,
            "stale_behavior": "fail-closed",
            "refresh_command": "npm run objc3c -- build-package-channels",
            "blocks_publication_on_stale": True,
        }


def test_release_operations_package_channel_freshness_accepts_coherent_artifacts() -> None:
    model = load_channel_model()
    channel_by_id = validate_channel_operations_model(
        channel_operations_model=model,
        update_channel_policy=update_channel_policy_for(model),
    )

    payload = validate_package_channel_freshness(
        package_channels_summary={"generated_at_utc": "2026-05-21T10:00:00Z"},
        package_channels_manifest={"generated_at_utc": "2026-05-21T10:20:00Z"},
        platform_support_matrix={"generated_at_utc": "2026-05-21T11:00:00Z"},
        channel_operations_by_id=channel_by_id,
    )

    assert payload["stale_behavior"] == "fail-closed"
    assert payload["artifact_skew_hours"] == 1.0
    assert payload["channel_max_artifact_skew_hours"] == {
        "stable": 6,
        "candidate": 6,
        "nightly": 6,
        "preview": 6,
    }


def test_release_operations_package_channel_freshness_fails_closed_on_stale_artifacts() -> None:
    model = load_channel_model()
    channel_by_id = validate_channel_operations_model(
        channel_operations_model=model,
        update_channel_policy=update_channel_policy_for(model),
    )

    with pytest.raises(RuntimeError, match="package channel evidence is stale"):
        validate_package_channel_freshness(
            package_channels_summary={"generated_at_utc": "2026-05-21T00:00:00Z"},
            package_channels_manifest={"generated_at_utc": "2026-05-21T01:00:00Z"},
            platform_support_matrix={"generated_at_utc": "2026-05-21T12:00:00Z"},
            channel_operations_by_id=channel_by_id,
        )


def test_update_manifest_channel_validation_accepts_source_model() -> None:
    model = load_channel_model()

    channel_by_id = validate_channel_operations_model(
        channel_operations_model=model,
        update_channel_policy=update_channel_policy_for(model),
    )

    assert set(channel_by_id) == {entry["channel_id"] for entry in model["channels"]}
    assert channel_by_id["stable"]["release_notes_policy"]["source_mode"] == "source-derived"
    assert channel_by_id["nightly"]["rollback_safety"]["rollback_channel"] == "offline-bundle"


def test_update_manifest_channel_validation_fails_closed_on_gate_collapse() -> None:
    model = load_channel_model()
    channels = [dict(entry) for entry in model["channels"]]
    collapsed_gates = sorted(
        {
            action
            for entry in channels
            if entry["channel_id"] in {"stable", "nightly"}
            for action in entry["release_gate_actions"]
        }
    )
    for entry in channels:
        if entry["channel_id"] in {"stable", "nightly"}:
            entry["release_gate_actions"] = collapsed_gates
    drifted = {**model, "channels": channels}

    with pytest.raises(RuntimeError, match="stable and nightly release gates must differ"):
        validate_channel_operations_model(
            channel_operations_model=drifted,
            update_channel_policy=update_channel_policy_for(model),
        )


def test_update_manifest_channel_validation_rejects_unknown_channel() -> None:
    model = load_channel_model()
    drifted = {
        **model,
        "channels": [
            *model["channels"],
            {
                **model["channels"][0],
                "channel_id": "ghost",
            },
        ],
    }

    with pytest.raises(RuntimeError, match="channels outside update policy: ghost"):
        validate_channel_operations_model(
            channel_operations_model=drifted,
            update_channel_policy=update_channel_policy_for(model),
        )


def test_release_evidence_payload_is_derived_from_channel_actions() -> None:
    model = load_channel_model()
    channel_entries = [
        {
            "channel_id": "stable",
            "release_gate_actions": [
                "validate-release-candidate-conformance",
                "validate-release-operations",
            ],
        },
        {
            "channel_id": "nightly",
            "release_gate_actions": [
                "test-nightly",
                "validate-release-operations",
            ],
        },
    ]

    payload = release_evidence_payload(
        channel_operations_model=model,
        evidence_artifacts=["tmp/artifacts/release-operations/update-manifest/objc3c-update-manifest.json"],
        channel_entries=channel_entries,
    )

    assert payload["release_note_sources"] == model["release_note_sources"]
    assert payload["public_changelog_sources"] == model["public_changelog_sources"]
    assert payload["replayable_public_commands"] == [
        "npm run objc3c -- test-nightly",
        "npm run objc3c -- validate-release-candidate-conformance",
        "npm run objc3c -- validate-release-operations",
    ]
