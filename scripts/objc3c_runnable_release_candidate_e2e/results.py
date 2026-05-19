"""Probe result parsing and assertions for runnable release-candidate validation."""

from __future__ import annotations

from objc3c_tooling.probe_output import parse_key_value_output

from .assertions import expect


def parse_claim_probe_payload(result: object) -> dict[str, object]:
    return parse_key_value_output(result, "packaged release-candidate claim ABI probe")


def parse_evidence_probe_payload(result: object) -> dict[str, object]:
    return parse_key_value_output(result, "packaged release-candidate evidence probe")


def assert_claim_probe_payload(payload: dict[str, object]) -> None:
    expect(
        payload.get("copy_status") == 0
        and payload.get("claim_bundle_ready") == 1
        and payload.get("deterministic") == 1,
        "expected packaged release-candidate claim ABI probe to publish a ready deterministic snapshot",
    )
    expect(
        payload.get("dashboard_schema_path")
        == "schemas/objc3-conformance-dashboard-status-v1.schema.json"
        and payload.get("gate_script_path") == "scripts/check_release_evidence.py"
        and payload.get("runbook_reference_path")
        == "spec/conformance/release_evidence_gate_maintenance.md",
        "expected packaged release-candidate claim ABI probe to preserve the packaged release evidence references",
    )


def assert_evidence_probe_payload(payload: dict[str, object]) -> None:
    expect(
        payload.get("copy_status") == 0
        and payload.get("validation_artifact_ready") == 1
        and payload.get("release_evidence_operation_ready") == 1
        and payload.get("dashboard_status_ready") == 1
        and payload.get("advanced_feature_gate_ready") == 1
        and payload.get("release_candidate_matrix_ready") == 1
        and payload.get("deprecated_paths_shutdown") == 1
        and payload.get("deterministic") == 1,
        "expected packaged release-candidate evidence probe to publish a ready deterministic implementation snapshot",
    )


__all__ = [
    "assert_claim_probe_payload",
    "assert_evidence_probe_payload",
    "parse_claim_probe_payload",
    "parse_evidence_probe_payload",
]
