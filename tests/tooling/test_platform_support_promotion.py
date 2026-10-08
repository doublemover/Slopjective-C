from __future__ import annotations

from copy import deepcopy
import json
from pathlib import Path

import pytest

from scripts import promote_objc3c_platform_support as promotion


ROOT = Path(__file__).resolve().parents[2]


SOURCE_FIXTURES = {
    "reviewed": ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "platform_hardening"
    / "host_promotion_reviewed_source_inputs.json",
    "source_truth": ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "platform_support"
    / "source_truth_matrix.json",
    "platform_evidence": ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "platform_hardening"
    / "platform_toolchain_support_evidence.json",
    "boundary": ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "platform_hardening"
    / "boundary_inventory.json",
    "supported_platforms": ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "packaging_channels"
    / "supported_platforms.json",
    "tier_policy": ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "platform_hardening"
    / "platform_support_tier_policy.json",
}


def _load(path: Path) -> dict[str, object]:
    return json.loads(path.read_text(encoding="utf-8"))


def _write(path: Path, payload: dict[str, object]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def _copy_fixture_set(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> dict[str, Path]:
    paths: dict[str, Path] = {}
    for key, source_path in SOURCE_FIXTURES.items():
        target_path = tmp_path / source_path.relative_to(ROOT)
        _write(target_path, _load(source_path))
        paths[key] = target_path

    monkeypatch.setattr(promotion, "REVIEWED_SOURCE_INPUTS_PATH", paths["reviewed"])
    monkeypatch.setattr(promotion, "PLATFORM_SUPPORT_SOURCE_TRUTH_PATH", paths["source_truth"])
    monkeypatch.setattr(
        promotion,
        "PLATFORM_TOOLCHAIN_SUPPORT_EVIDENCE_PATH",
        paths["platform_evidence"],
    )
    monkeypatch.setattr(promotion, "BOUNDARY_INVENTORY_PATH", paths["boundary"])
    monkeypatch.setattr(promotion, "SUPPORTED_PLATFORMS_PATH", paths["supported_platforms"])
    monkeypatch.setattr(promotion, "SUPPORT_TIER_POLICY_PATH", paths["tier_policy"])
    return paths


def _row_by_id(rows: list[object], field_name: str, value: str) -> dict[str, object]:
    for row in rows:
        if isinstance(row, dict) and row.get(field_name) == value:
            return row
    raise AssertionError(f"missing {field_name}={value}")


def _platform_artifact_paths(platform_id: str) -> list[str]:
    return [
        f"tmp/reports/platform-host-evidence/{platform_id}/host-evidence-report.json",
        f"tmp/reports/platform-host-evidence/{platform_id}/promotion-readiness-requirements.json",
        f"tmp/reports/platform-host-evidence/{platform_id}/review-candidate-source-truth.json",
        f"tmp/reports/platform-host-evidence/{platform_id}/llvm-capabilities.json",
        f"tmp/reports/platform-host-evidence/{platform_id}/build/native_build_summary.json",
        f"tmp/reports/platform-host-evidence/{platform_id}/build/object-identity.json",
        f"tmp/reports/platform-host-evidence/{platform_id}/build/debug-identity.json",
        f"tmp/reports/platform-host-evidence/{platform_id}/package/objc3c-runnable-toolchain-package.json",
        f"tmp/reports/platform-host-evidence/{platform_id}/package/runtime-library-manifest.json",
        f"tmp/reports/platform-host-evidence/{platform_id}/install/install-receipt.json",
        f"tmp/reports/platform-host-evidence/{platform_id}/install/end-to-end-summary.json",
        f"tmp/reports/platform-host-evidence/{platform_id}/install/install-distribution-credibility-summary.json",
        f"tmp/reports/platform-host-evidence/{platform_id}/install/install-distribution-verification.json",
        f"tmp/reports/platform-host-evidence/{platform_id}/install/clean-install-distribution-receipt.json",
        f"tmp/reports/platform-host-evidence/{platform_id}/execution/runtime-load-probe.json",
        f"tmp/reports/platform-host-evidence/{platform_id}/execution/hosted-execution-smoke-summary.json",
        f"tmp/reports/platform-host-evidence/{platform_id}/execution/native-execution-smoke-summary.json",
    ]


def _reviewed_record_id(old_record_id: str) -> str:
    return old_record_id.replace(".fail-closed", ".reviewed-source").replace(
        ".missing",
        ".reviewed-source",
    )


def _mark_platform_promotion_ready(payload: dict[str, object], platform_id: str) -> None:
    profile = promotion.PLATFORM_PROFILES[platform_id]
    platforms = payload["platforms"]
    assert isinstance(platforms, list)
    platform = _row_by_id(platforms, "platform_id", platform_id)
    required_ids = platform["required_record_ids"]
    assert isinstance(required_ids, dict)

    platform["support_row_id"] = profile["supported_row_id"]
    platform["package_variant_row_id"] = profile["package_row_id"]
    platform["review_decision"] = "reviewed-source-promotion-ready"
    platform["not_promotion_ready_record_types"] = []
    platform["remaining_blockers"] = []
    platform["promotion_allowed"] = True
    platform["support_truth"] = True

    artifact_paths = _platform_artifact_paths(platform_id)
    for record_type in promotion.REQUIRED_RECORD_TYPES:
        section_name = promotion.RECORD_SECTION_BY_TYPE[record_type]
        id_field = promotion.RECORD_ID_FIELD_BY_TYPE[record_type]
        old_record_id = str(required_ids[id_field])
        new_record_id = _reviewed_record_id(old_record_id)
        section = payload[section_name]
        assert isinstance(section, list)
        record = _row_by_id(section, "record_id", old_record_id)
        record["record_id"] = new_record_id
        record["support_row_id"] = profile["supported_row_id"]
        record["claim_state"] = "reviewed-source"
        record["promotion_allowed"] = True
        record["support_truth"] = True
        record["generated_report_support_truth"] = False
        record["platform_ids"] = [platform_id]
        record["review_status"] = "reviewed-current-source"
        record["stale_evidence_allowed"] = False
        record["prose_only_evidence"] = False
        record["local_temp_evidence_claim"] = False
        record["hosted_runner_artifact_paths"] = artifact_paths
        record["toolchain_artifact_paths"] = artifact_paths
        record["package_artifact_paths"] = artifact_paths
        required_ids[id_field] = new_record_id


def test_platform_support_promotion_refuses_non_ready_reviewed_source(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    _copy_fixture_set(tmp_path, monkeypatch)

    with pytest.raises(promotion.PromotionError, match="linux-x64 reviewed source row is not promotion-ready"):
        promotion.promote_platform(
            "linux-x64",
            apply=False,
            summary_path=tmp_path / "summary.json",
        )

    assert not (tmp_path / "summary.json").exists()


def test_platform_support_promotion_dry_run_does_not_rewrite_source(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    paths = _copy_fixture_set(tmp_path, monkeypatch)
    reviewed = _load(paths["reviewed"])
    _mark_platform_promotion_ready(reviewed, "linux-x64")
    _write(paths["reviewed"], reviewed)
    before = {key: path.read_text(encoding="utf-8") for key, path in paths.items()}

    summary = promotion.promote_platform(
        "linux-x64",
        apply=False,
        summary_path=tmp_path / "summary.json",
    )

    assert summary["status"] == "DRY_RUN_READY"
    assert summary["generated_reports_are_source_truth"] is False
    assert _load(tmp_path / "summary.json")["status"] == "DRY_RUN_READY"
    assert {key: path.read_text(encoding="utf-8") for key, path in paths.items()} == before


def test_platform_support_promotion_applies_tiered_linux_source_truth(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    paths = _copy_fixture_set(tmp_path, monkeypatch)
    reviewed = _load(paths["reviewed"])
    _mark_platform_promotion_ready(reviewed, "linux-x64")
    _write(paths["reviewed"], reviewed)

    summary = promotion.promote_platform(
        "linux-x64",
        apply=True,
        summary_path=tmp_path / "summary.json",
    )

    assert summary == {
        "contract_id": "objc3c.platform.support-promotion.source-truth-application.v1",
        "status": "APPLIED",
        "platform_id": "linux-x64",
        "issue_ref": 8228,
        "tier_id": "tier-2",
        "generated_reports_are_source_truth": False,
        "reviewed_source_input": str(paths["reviewed"].as_posix()),
        "changed_source_paths": [str(paths[key].as_posix()) for key in (
            "boundary",
            "supported_platforms",
            "tier_policy",
            "platform_evidence",
            "source_truth",
        )],
        "supported_row_id": "objc3c.platform.linux-x64.tier2",
        "package_variant_row_id": "objc3c.package.runtime.linux-x64.release",
        "public_capability_id": "platform.linux-x64.tier2",
    }

    boundary = _load(paths["boundary"])
    assert boundary["supported_platform_ids"] == ["windows-x64", "linux-x64"]

    supported_platforms = _load(paths["supported_platforms"])
    assert {
        row["platform_id"] for row in supported_platforms["supported_platforms"]
    } == {"windows-x64", "linux-x64"}
    assert "linux-x64" not in {
        row["platform_id"] for row in supported_platforms["unsupported_platforms"]
    }

    tier_policy = _load(paths["tier_policy"])
    tier2 = _row_by_id(tier_policy["tiers"], "tier_id", "tier-2")
    assert "linux-x64" in tier2["platform_ids"]

    evidence = _load(paths["platform_evidence"])
    linux_support = _row_by_id(evidence["support_rows"], "platform_id", "linux-x64")
    assert linux_support["row_id"] == "objc3c.platform.linux-x64.tier2"
    assert linux_support["claim_class"] == "supported-but-not-default"
    assert linux_support["required_missing_evidence_classes"] == []
    assert linux_support["package_variant_row_ids"] == [
        "objc3c.package.runtime.linux-x64.release"
    ]
    assert set(linux_support["reviewed_source_record_ids"]) == set(
        promotion.REQUIRED_RECORD_TYPES
    )
    assert all(
        record_id.endswith(".reviewed-source")
        for record_id in linux_support["reviewed_source_record_ids"].values()
    )

    package = _row_by_id(
        evidence["package_variant_rows"],
        "row_id",
        "objc3c.package.runtime.linux-x64.release",
    )
    assert package["claim_state"] == "evidence-bound"
    assert package["platform_ids"] == ["linux-x64"]
    assert package["required_missing_evidence_classes"] == []
    assert package["artifact_identity_contract"]["object_format"] == "ELF"

    evidence_ids = {record["evidence_id"] for record in evidence["evidence_records"]}
    assert {
        "objc3c.evidence.reviewed-source.linux-x64.build",
        "objc3c.evidence.reviewed-source.linux-x64.package",
        "objc3c.evidence.reviewed-source.linux-x64.install",
        "objc3c.evidence.reviewed-source.linux-x64.execution",
        "objc3c.evidence.reviewed-source.linux-x64.toolchain.llvm",
        "objc3c.evidence.reviewed-source.linux-x64.toolchain.native-build-resolution",
        "objc3c.evidence.reviewed-source.linux-x64.toolchain.package-bridge",
    } <= evidence_ids

    source_truth = _load(paths["source_truth"])
    assert "linux-x64" in {
        row["platform_id"] for row in source_truth["supported_rows"]
    }
    assert "linux-x64" not in {
        row["platform_id"] for row in source_truth["unsupported_rows"]
    }
    assert source_truth["umbrella_readiness_contract"]["support_claim_boundary"] == (
        "source-owned:linux-x64,windows-x64"
    )
    assert "objc3c.platform.linux-x64.tier2" in source_truth[
        "umbrella_readiness_contract"
    ]["supported_platform_row_ids"]
