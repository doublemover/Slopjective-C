from __future__ import annotations

import sys
from copy import deepcopy
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

import check_objc3c_release_channel_operations_policy as checker


def load_policy() -> dict:
    return checker.load_json(checker.POLICY_PATH)


def load_schema() -> dict:
    return checker.load_json(checker.SCHEMA_PATH)


def validate(policy: dict) -> dict:
    return checker.validate_policy(policy, load_schema())


def test_release_channel_operations_policy_validates_source_truth() -> None:
    summary = validate(load_policy())

    assert summary["contract_id"] == "objc3c.release.channel.operations.policy.summary.v1"
    assert summary["status"] == "PASS"
    assert summary["required_channels"] == ["stable", "nightly"]
    assert summary["releasable_channels"] == []
    assert summary["blocked_until_verified_evidence_channels"] == ["stable", "nightly"]
    assert set(summary["required_releasable_evidence"]) == {
        "release-foundation",
        "update-manifest",
        "rollback-proof",
        "signed-artifacts",
    }
    assert "validate-release-candidate-conformance" in summary["stable_gate_actions"]
    assert "test-nightly" in summary["nightly_gate_actions"]
    assert summary["source_truth_path_count"] >= 8
    assert summary["generated_evidence_artifact_count"] >= 8


def test_release_channel_operations_policy_cli_writes_summary() -> None:
    summary_path = (
        checker.ROOT
        / "tmp"
        / "reports"
        / "release-channel-operations-policy-test-summary.json"
    )
    summary_path.unlink(missing_ok=True)

    try:
        assert checker.main(["--summary", str(summary_path)]) == 0
        summary = checker.load_json(summary_path)

        assert summary["status"] == "PASS"
        assert (
            summary["schema"]
            == "schemas/objc3c-release-channel-operations-policy-v1.schema.json"
        )
    finally:
        summary_path.unlink(missing_ok=True)


def test_release_channel_operations_policy_rejects_stable_releasable_without_verified_evidence() -> None:
    policy = load_policy()
    stable = next(channel for channel in policy["channels"] if channel["channel_id"] == "stable")
    stable["releasability"]["state"] = "releasable"
    stable["releasability"]["may_call_releasable"] = True

    with pytest.raises(
        checker.ReleaseChannelPolicyError,
        match="stable channel cannot be releasable without verified evidence",
    ):
        validate(policy)


def test_release_channel_operations_policy_rejects_nightly_stable_claim() -> None:
    policy = load_policy()
    nightly = next(channel for channel in policy["channels"] if channel["channel_id"] == "nightly")
    nightly["claim_boundaries"]["stable_support_claim_allowed"] = True

    with pytest.raises(
        checker.ReleaseChannelPolicyError,
        match="nightly channel must not be allowed to carry stable support",
    ):
        validate(policy)


def test_release_channel_operations_policy_rejects_missing_rollback_primitive() -> None:
    policy = load_policy()
    policy["rollback_policy"]["required_primitives"] = ["local-installer"]

    with pytest.raises(
        checker.ReleaseChannelPolicyError,
        match="rollback policy missing primitives: offline-bundle",
    ):
        validate(policy)


def test_release_channel_operations_policy_rejects_update_distribution_claim_drift() -> None:
    policy = load_policy()
    policy["update_policy"]["blocked_update_mechanics"].remove("hosted background updater")

    with pytest.raises(
        checker.ReleaseChannelPolicyError,
        match="update policy must block hosted background updater",
    ):
        validate(policy)


def test_release_channel_operations_policy_rejects_tmp_source_truth_root() -> None:
    policy = load_policy()
    policy["release_evidence_policy"]["source_truth_roots"].append("tmp/reports/")

    with pytest.raises(
        checker.ReleaseChannelPolicyError,
        match="release evidence source truth roots must not include generated output roots",
    ):
        validate(policy)


def test_release_channel_operations_policy_rejects_generated_release_note_source() -> None:
    policy = load_policy()
    stable = next(channel for channel in policy["channels"] if channel["channel_id"] == "stable")
    stable["publication_mechanics"]["release_notes_sources"][0] = (
        "tmp/artifacts/release-foundation/manifest/objc3c-release-manifest.json"
    )

    with pytest.raises(
        checker.ReleaseChannelPolicyError,
        match="stable\\.publication_mechanics\\.release_notes_sources\\[0\\] must not use generated output as source truth",
    ):
        validate(policy)


def test_release_channel_operations_policy_rejects_generated_source_truth() -> None:
    policy = load_policy()
    nightly = next(channel for channel in policy["channels"] if channel["channel_id"] == "nightly")
    nightly["evidence"]["source_truth"][0] = "tmp/reports/release-operations/nightly.json"

    with pytest.raises(
        checker.ReleaseChannelPolicyError,
        match="nightly\\.evidence\\.source_truth\\[0\\] must not use generated output as source truth",
    ):
        validate(policy)


def test_release_channel_operations_policy_rejects_checked_file_as_generated_evidence() -> None:
    policy = load_policy()
    stable = next(channel for channel in policy["channels"] if channel["channel_id"] == "stable")
    stable["evidence"]["generated_evidence_artifacts"][0] = (
        "tests/tooling/fixtures/release_channel_operations_policy.json"
    )

    with pytest.raises(
        checker.ReleaseChannelPolicyError,
        match="stable\\.evidence\\.generated_evidence_artifacts\\[0\\] must be generated evidence",
    ):
        validate(policy)


def test_release_channel_operations_policy_rejects_schema_contract_drift() -> None:
    schema = deepcopy(load_schema())
    schema["properties"]["contract_id"]["const"] = "objc3c.release.channel.broken.v1"

    with pytest.raises(
        checker.ReleaseChannelPolicyError,
        match="schema contract_id const drifted",
    ):
        checker.validate_policy(load_policy(), schema)
