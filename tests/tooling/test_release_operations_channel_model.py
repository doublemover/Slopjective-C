from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
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
