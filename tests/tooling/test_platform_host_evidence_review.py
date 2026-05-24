from __future__ import annotations

from pathlib import Path
from typing import Any

import pytest

from scripts import review_objc3c_platform_host_evidence as review


def test_parse_args_keeps_reviewed_source_apply_explicit(tmp_path: Path) -> None:
    evidence_root = tmp_path / "evidence"
    source_inputs = tmp_path / "source.json"
    output = tmp_path / "proposal.json"

    args = review.parse_args(
        [
            "--platform-id",
            "linux-x64",
            "--evidence-root",
            str(evidence_root),
            "--source-inputs",
            str(source_inputs),
            "--output",
            str(output),
        ]
    )

    assert args.platform_id == "linux-x64"
    assert args.evidence_root == evidence_root
    assert args.source_inputs == source_inputs
    assert args.output == output
    assert args.apply_reviewed_source_truth is False

    applied_args = review.parse_args(
        [
            "--platform-id",
            "linux-x64",
            "--apply-reviewed-source-truth",
        ]
    )

    assert applied_args.apply_reviewed_source_truth is True


def test_configured_evidence_root_still_returns_canonical_source_paths(
    tmp_path: Path,
) -> None:
    downloaded_root = tmp_path / "downloaded-artifact"
    artifact = downloaded_root / "build" / "object-identity.json"
    artifact.parent.mkdir(parents=True)
    artifact.write_text("{}", encoding="utf-8")

    assert review.require_report_path_scope(
        "linux-x64",
        downloaded_root,
        "build/object-identity.json",
    ) == "tmp/reports/platform-host-evidence/linux-x64/build/object-identity.json"

    with pytest.raises(review.ReviewError, match="left platform scope"):
        review.evidence_path(downloaded_root, "../host-evidence-report.json")


def test_main_validates_proposal_before_any_write(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    source_inputs = tmp_path / "source.json"
    output = tmp_path / "proposal.json"
    payload = {"contract_id": "example.reviewed-source-inputs"}
    events: list[tuple[str, Any]] = []

    def fake_build(args: Any) -> tuple[dict[str, Any], dict[str, Any]]:
        events.append(("build", args.apply_reviewed_source_truth))
        return payload, {
            "contract_id": "example.summary",
            "status": "PROPOSED_REVIEWED_SOURCE_READY",
            "platform_id": args.platform_id,
            "applied_to_source": bool(args.apply_reviewed_source_truth),
        }

    def fake_validate(candidate: dict[str, Any], *, owner: str) -> dict[str, Any]:
        events.append(("validate", owner, candidate is payload))
        return {
            "contract_id": "objc3c.platform.host-promotion.reviewed-source-inputs.v1",
            "status": "PASS",
            "promotion_allowed_platform_ids": ["linux-x64"],
            "required_record_types_before_promotion_allowed": ["host_identity"],
        }

    writes: list[tuple[Path, dict[str, Any]]] = []

    def fake_write(path: Path, written_payload: dict[str, Any]) -> None:
        events.append(("write", Path(path).name))
        writes.append((Path(path), dict(written_payload)))

    monkeypatch.setattr(review, "build_reviewed_source_payload", fake_build)
    monkeypatch.setattr(review, "validate_reviewed_source_payload", fake_validate)
    monkeypatch.setattr(review, "write_json", fake_write)

    assert review.main(
        [
            "--platform-id",
            "linux-x64",
            "--evidence-root",
            str(tmp_path),
            "--source-inputs",
            str(source_inputs),
            "--output",
            str(output),
        ]
    ) == 0

    assert events[:2] == [
        ("build", False),
        ("validate", "proposed reviewed-source payload", True),
    ]
    assert [event for event in events if event[0] == "write"] == [
        ("write", "proposal.json"),
        ("write", "reviewed-source-staging-summary.json"),
    ]
    assert source_inputs not in [path for path, _ in writes]
    summary = writes[-1][1]
    assert summary["source_owned_validation"] == {
        "validator": review.SOURCE_OWNED_REVIEW_VALIDATOR,
        "status": "PASS",
        "source_contract_id": (
            "objc3c.platform.host-promotion.reviewed-source-inputs.v1"
        ),
        "proposed_promotion_allowed_platform_ids": ["linux-x64"],
        "required_record_types_before_promotion_allowed": ["host_identity"],
    }
    assert "promotion_allowed_platform_ids" not in summary


def test_main_revalidates_before_explicit_apply(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path: Path,
) -> None:
    source_inputs = tmp_path / "source.json"
    output = tmp_path / "proposal.json"
    payload = {"contract_id": "example.reviewed-source-inputs"}
    events: list[tuple[str, Any]] = []

    def fake_build(args: Any) -> tuple[dict[str, Any], dict[str, Any]]:
        events.append(("build", args.apply_reviewed_source_truth))
        return payload, {
            "contract_id": "example.summary",
            "status": "PROPOSED_REVIEWED_SOURCE_READY",
            "platform_id": args.platform_id,
            "applied_to_source": bool(args.apply_reviewed_source_truth),
        }

    def fake_validate(candidate: dict[str, Any], *, owner: str) -> dict[str, Any]:
        events.append(("validate", owner, candidate is payload))
        return {
            "contract_id": "objc3c.platform.host-promotion.reviewed-source-inputs.v1",
            "status": "PASS",
            "promotion_allowed_platform_ids": ["linux-x64"],
            "required_record_types_before_promotion_allowed": ["host_identity"],
        }

    writes: list[tuple[Path, dict[str, Any]]] = []

    def fake_write(path: Path, written_payload: dict[str, Any]) -> None:
        events.append(("write", Path(path).name))
        writes.append((Path(path), dict(written_payload)))

    monkeypatch.setattr(review, "build_reviewed_source_payload", fake_build)
    monkeypatch.setattr(review, "validate_reviewed_source_payload", fake_validate)
    monkeypatch.setattr(review, "write_json", fake_write)

    assert review.main(
        [
            "--platform-id",
            "linux-x64",
            "--evidence-root",
            str(tmp_path),
            "--source-inputs",
            str(source_inputs),
            "--output",
            str(output),
            "--apply-reviewed-source-truth",
        ]
    ) == 0

    assert events == [
        ("build", True),
        ("validate", "proposed reviewed-source payload", True),
        ("write", "proposal.json"),
        ("validate", "applied reviewed-source payload", True),
        ("write", "source.json"),
        ("write", "reviewed-source-staging-summary.json"),
    ]
    summary = writes[-1][1]
    assert summary["status"] == "APPLIED_REVIEWED_SOURCE_TRUTH"
    assert summary["applied_source_owned_validation"][
        "applied_promotion_allowed_platform_ids"
    ] == ["linux-x64"]


def test_validate_reviewed_source_payload_wraps_source_owned_failures(
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    def fake_validate(_: dict[str, Any]) -> dict[str, Any]:
        raise RuntimeError("missing hosted promotion artifact paths")

    monkeypatch.setattr(
        review,
        "validate_host_promotion_reviewed_source_inputs",
        fake_validate,
    )

    with pytest.raises(
        review.ReviewError,
        match=(
            "proposed reviewed-source payload failed source-owned host promotion "
            "validation: missing hosted promotion artifact paths"
        ),
    ):
        review.validate_reviewed_source_payload(
            {"contract_id": "example"},
            owner="proposed reviewed-source payload",
        )
