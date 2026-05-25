from __future__ import annotations

from pathlib import Path
from typing import Any

import pytest

from scripts import review_objc3c_platform_host_evidence as review


def _artifact(path: str) -> dict[str, Any]:
    return {
        "path": path,
        "exists": True,
        "size_bytes": 1,
        "sha256": "a" * 64,
    }


def _object_identity_record() -> dict[str, Any]:
    return {
        "record_id": "objc3c.object-identity.linux-x64.release.missing",
        "platform_id": "linux-x64",
        "target_triple": "x86_64-unknown-linux-gnu",
        "arch": "x64",
        "object_format": "ELF",
    }


def _object_identity_payload() -> dict[str, Any]:
    return {
        "contract_id": "objc3c.platform.hosted-object-identity.generated.v1",
        "schema_version": 1,
        "platform_id": "linux-x64",
        "issue_ref": 8228,
        "record_id": "objc3c.object-identity.linux-x64.release.missing",
        "generated_report_path": (
            "tmp/reports/platform-host-evidence/linux-x64/build/object-identity.json"
        ),
        "reviewed_source_required": True,
        "support_truth": False,
        "promotion_allowed_from_generated_evidence": False,
        "status": review.REQUIRED_GENERATED_ARTIFACT_STATUS,
        "expected_identity": {
            "target_platform_id": "linux-x64",
            "target_triple": "x86_64-unknown-linux-gnu",
            "arch": "x64",
            "object_format": "ELF",
        },
        "actual_identity": {
            "target_platform_id": "linux-x64",
            "target_triple": "x86_64-unknown-linux-gnu",
            "object_format": "ELF",
        },
        "source_artifacts": [
            _artifact(review.NATIVE_BUILD_SUMMARY_PATH),
            _artifact(review.NATIVE_EXECUTION_SMOKE_SUMMARY_PATH),
        ],
    }


def _installed_root_execution(channel_id: str) -> dict[str, Any]:
    return {
        "contract_id": review.REQUIRED_INSTALLED_ROOT_EXECUTION_CONTRACT_ID,
        "status": "PASS",
        "channel_id": channel_id,
        "execution_source": "installed-root",
        "repo_temp_dependency": False,
        "preexisting_artifacts_dependency": False,
        "expected_exit_code": review.REQUIRED_INSTALLED_ROOT_USAGE_EXIT_CODE,
        "returncode": review.REQUIRED_INSTALLED_ROOT_USAGE_EXIT_CODE,
        "usage_banner_seen": True,
        "target_platform_id": "linux-x64",
    }


def _package_install_record() -> dict[str, Any]:
    return {
        "record_id": "objc3c.package-install-identity.linux-x64.release.missing",
        "platform_id": "linux-x64",
        "package_root_layout": [
            "artifacts/package/objc3c-runnable-toolchain-package.json",
            "artifacts/bin/objc3c-native",
            "artifacts/lib/libobjc3-runtime.so",
        ],
    }


def _runtime_record() -> dict[str, Any]:
    return {
        "record_id": "objc3c.runtime-load-link.linux-x64.release.missing",
        "platform_id": "linux-x64",
        "runtime_library_names": ["libobjc3-runtime.so"],
        "loader_policy": "ELF rpath, RUNPATH, or package-root loader resolution",
    }


def _install_receipt_payload() -> dict[str, Any]:
    source_receipt_path = (
        "tmp/artifacts/package-ecosystem/install-validation/clean-root/"
        "objc3c-install-receipt.json"
    )
    return {
        "contract_id": "objc3c.platform.hosted-install-receipt.generated.v1",
        "schema_version": 1,
        "platform_id": "linux-x64",
        "issue_ref": 8228,
        "record_id": "objc3c.package-install-identity.linux-x64.release.missing",
        "generated_report_path": (
            "tmp/reports/platform-host-evidence/linux-x64/install/install-receipt.json"
        ),
        "reviewed_source_required": True,
        "support_truth": False,
        "native_execution_claimed": False,
        "promotion_allowed_from_generated_evidence": False,
        "status": review.REQUIRED_GENERATED_ARTIFACT_STATUS,
        "target_platform_id": "linux-x64",
        "target_triple": "x86_64-unknown-linux-gnu",
        "package_root_layout": _package_install_record()["package_root_layout"],
        "package_manifest": review.RUNNABLE_PACKAGE_MANIFEST_PATH,
        "package_manifest_artifact": _artifact(review.RUNNABLE_PACKAGE_MANIFEST_PATH),
        "package_channels_summary_artifact": _artifact(
            review.PACKAGE_CHANNELS_END_TO_END_SUMMARY_PATH
        ),
        "source_install_receipt_path": source_receipt_path,
        "source_install_receipt_artifact": _artifact(source_receipt_path),
        "source_install_receipt": {
            "contract_id": review.PACKAGE_INSTALL_RECEIPT_CONTRACT_ID,
            "target_platform_id": "linux-x64",
            "package_runtime_model": {
                "target_platform_id": "linux-x64",
                "package_root_layout": _package_install_record()["package_root_layout"],
            },
        },
        "installed_root_execution": _installed_root_execution("local-installer"),
        "offline_installed_root_execution": _installed_root_execution("offline-bundle"),
        "installed_root_execution_status": "PASS",
        "offline_installed_root_execution_status": "PASS",
        "source_artifacts": [
            _artifact(review.PACKAGE_CHANNELS_END_TO_END_SUMMARY_PATH),
            _artifact(review.RUNNABLE_PACKAGE_MANIFEST_PATH),
            _artifact(source_receipt_path),
        ],
    }


def _runtime_manifest_payload() -> dict[str, Any]:
    return {
        "contract_id": "objc3c.platform.hosted-runtime-library-manifest.generated.v1",
        "schema_version": 1,
        "platform_id": "linux-x64",
        "issue_ref": 8228,
        "generated_report_path": (
            "tmp/reports/platform-host-evidence/linux-x64/package/"
            "runtime-library-manifest.json"
        ),
        "support_truth": False,
        "native_execution_claimed": False,
        "promotion_allowed_from_generated_evidence": False,
        "status": review.REQUIRED_GENERATED_ARTIFACT_STATUS,
        "target_platform_id": "linux-x64",
        "target_triple": "x86_64-unknown-linux-gnu",
        "runtime_library_names": ["libobjc3-runtime.so"],
        "loader_path_policy": "ELF rpath, RUNPATH, or package-root loader resolution",
        "package_root_layout": _package_install_record()["package_root_layout"],
        "package_manifest_artifact": _artifact(review.RUNNABLE_PACKAGE_MANIFEST_PATH),
        "runtime_library_artifacts": [_artifact("artifacts/lib/libobjc3-runtime.so")],
        "source_artifacts": [
            _artifact(review.RUNNABLE_PACKAGE_MANIFEST_PATH),
            _artifact(review.NATIVE_BUILD_SUMMARY_PATH),
        ],
    }


def _runtime_load_payload() -> dict[str, Any]:
    return {
        "contract_id": "objc3c.platform.hosted-runtime-load-probe.generated.v1",
        "schema_version": 1,
        "platform_id": "linux-x64",
        "issue_ref": 8228,
        "record_id": "objc3c.runtime-load-link.linux-x64.release.missing",
        "generated_report_path": (
            "tmp/reports/platform-host-evidence/linux-x64/execution/runtime-load-probe.json"
        ),
        "reviewed_source_required": True,
        "support_truth": False,
        "native_execution_claimed": False,
        "promotion_allowed_from_generated_evidence": False,
        "status": review.REQUIRED_GENERATED_ARTIFACT_STATUS,
        "target_platform_id": "linux-x64",
        "target_triple": "x86_64-unknown-linux-gnu",
        "runtime_library_names": ["libobjc3-runtime.so"],
        "loader_path_policy": "ELF rpath, RUNPATH, or package-root loader resolution",
        "load_probe_exit_code": 0,
        "resolved_runtime_paths": ["artifacts/lib/libobjc3-runtime.so"],
        "hosted_execution_status": "SKIPPED",
        "native_execution_status": "PASS",
        "skip_reason": "",
        "runtime_library": "artifacts/lib/libobjc3-runtime.so",
        "source_artifacts": [
            _artifact(review.HOSTED_EXECUTION_SMOKE_SUMMARY_PATH),
            _artifact(review.NATIVE_EXECUTION_SMOKE_SUMMARY_PATH),
            _artifact(review.RUNNABLE_PACKAGE_MANIFEST_PATH),
        ],
    }


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


def test_review_rejects_status_only_object_identity_promotion() -> None:
    payload = _object_identity_payload()
    payload["actual_identity"]["target_platform_id"] = "windows-x64"

    with pytest.raises(
        review.ReviewError,
        match="object identity actual_identity.target_platform_id drifted",
    ):
        review.require_identity_payload_matches_record(
            payload,
            _object_identity_record(),
            platform_id="linux-x64",
            suffix="build/object-identity.json",
            contract_id="objc3c.platform.hosted-object-identity.generated.v1",
            record_id="objc3c.object-identity.linux-x64.release.missing",
            identity_kind="object",
            identity_field_names=("object_format",),
        )


def test_review_rejects_pass_status_without_generated_artifact_presence() -> None:
    payload = _object_identity_payload()
    payload["status"] = "PASS"

    with pytest.raises(
        review.ReviewError,
        match="object identity generated artifact is not complete: PASS",
    ):
        review.require_identity_payload_matches_record(
            payload,
            _object_identity_record(),
            platform_id="linux-x64",
            suffix="build/object-identity.json",
            contract_id="objc3c.platform.hosted-object-identity.generated.v1",
            record_id="objc3c.object-identity.linux-x64.release.missing",
            identity_kind="object",
            identity_field_names=("object_format",),
        )


def test_review_rejects_generated_payload_schema_version_drift() -> None:
    payload = _object_identity_payload()
    payload["schema_version"] = 2

    with pytest.raises(
        review.ReviewError,
        match="object identity schema_version drifted",
    ):
        review.require_identity_payload_matches_record(
            payload,
            _object_identity_record(),
            platform_id="linux-x64",
            suffix="build/object-identity.json",
            contract_id="objc3c.platform.hosted-object-identity.generated.v1",
            record_id="objc3c.object-identity.linux-x64.release.missing",
            identity_kind="object",
            identity_field_names=("object_format",),
        )


def test_review_rejects_generated_payload_wrong_platform_issue_ref() -> None:
    payload = _object_identity_payload()
    payload["issue_ref"] = 8229

    with pytest.raises(
        review.ReviewError,
        match="object identity issue_ref drifted",
    ):
        review.require_identity_payload_matches_record(
            payload,
            _object_identity_record(),
            platform_id="linux-x64",
            suffix="build/object-identity.json",
            contract_id="objc3c.platform.hosted-object-identity.generated.v1",
            record_id="objc3c.object-identity.linux-x64.release.missing",
            identity_kind="object",
            identity_field_names=("object_format",),
        )


def test_review_rejects_generated_payload_without_source_artifacts() -> None:
    payload = _object_identity_payload()
    payload["source_artifacts"] = []

    with pytest.raises(
        review.ReviewError,
        match="object identity field source_artifacts must not be empty",
    ):
        review.require_identity_payload_matches_record(
            payload,
            _object_identity_record(),
            platform_id="linux-x64",
            suffix="build/object-identity.json",
            contract_id="objc3c.platform.hosted-object-identity.generated.v1",
            record_id="objc3c.object-identity.linux-x64.release.missing",
            identity_kind="object",
            identity_field_names=("object_format",),
        )


def test_review_accepts_object_identity_producer_timing_source_artifacts() -> None:
    payload = _object_identity_payload()
    payload["source_artifacts"] = [_artifact(review.NATIVE_BUILD_SUMMARY_PATH)]

    review.require_identity_payload_matches_record(
        payload,
        _object_identity_record(),
        platform_id="linux-x64",
        suffix="build/object-identity.json",
        contract_id="objc3c.platform.hosted-object-identity.generated.v1",
        record_id="objc3c.object-identity.linux-x64.release.missing",
        identity_kind="object",
        identity_field_names=("object_format",),
    )


def test_review_accepts_runtime_manifest_producer_timing_source_artifacts() -> None:
    payload = _runtime_manifest_payload()
    payload["source_artifacts"] = [
        _artifact(review.RUNNABLE_PACKAGE_MANIFEST_PATH),
        _artifact("artifacts/lib/libobjc3-runtime.so"),
    ]

    review.require_runtime_manifest_payload(
        payload,
        {
            "runtime_library_names": ["libobjc3-runtime.so"],
            "package_root_layout": _package_install_record()["package_root_layout"],
            "loader_path_policy": (
                "ELF rpath, RUNPATH, or package-root loader resolution"
            ),
        },
        platform_id="linux-x64",
    )


def test_review_accepts_runtime_load_producer_timing_source_artifacts() -> None:
    payload = _runtime_load_payload()
    payload["source_artifacts"] = [_artifact(review.NATIVE_EXECUTION_SMOKE_SUMMARY_PATH)]

    review.require_runtime_load_payload(
        payload,
        _runtime_record(),
        _object_identity_record(),
        platform_id="linux-x64",
        record_id="objc3c.runtime-load-link.linux-x64.release.missing",
    )


def test_review_rejects_runtime_manifest_native_execution_claim() -> None:
    payload = _runtime_manifest_payload()
    payload["native_execution_claimed"] = True

    with pytest.raises(
        review.ReviewError,
        match="runtime library manifest native_execution_claimed drifted",
    ):
        review.require_runtime_manifest_payload(
            payload,
            {
                "runtime_library_names": ["libobjc3-runtime.so"],
                "package_root_layout": _package_install_record()["package_root_layout"],
                "loader_path_policy": (
                    "ELF rpath, RUNPATH, or package-root loader resolution"
                ),
            },
            platform_id="linux-x64",
        )


def test_review_rejects_installed_root_repo_temp_dependency() -> None:
    payload = _install_receipt_payload()
    payload["installed_root_execution"]["repo_temp_dependency"] = True

    with pytest.raises(
        review.ReviewError,
        match="install receipt installed_root_execution depended on repo temp output",
    ):
        review.require_install_receipt_payload(
            payload,
            _package_install_record(),
            platform_id="linux-x64",
            record_id="objc3c.package-install-identity.linux-x64.release.missing",
        )


def test_review_rejects_install_receipt_status_coherence_drift() -> None:
    payload = _install_receipt_payload()
    payload["installed_root_execution_status"] = "SKIPPED"

    with pytest.raises(
        review.ReviewError,
        match="install receipt installed_root_execution_status drifted",
    ):
        review.require_install_receipt_payload(
            payload,
            _package_install_record(),
            platform_id="linux-x64",
            record_id="objc3c.package-install-identity.linux-x64.release.missing",
        )


def test_review_rejects_install_receipt_runtime_model_layout_drift() -> None:
    payload = _install_receipt_payload()
    payload["source_install_receipt"]["package_runtime_model"][
        "package_root_layout"
    ] = ["artifacts/package/objc3c-runnable-toolchain-package.json"]

    with pytest.raises(
        review.ReviewError,
        match="install receipt source receipt runtime model package_root_layout drifted",
    ):
        review.require_install_receipt_payload(
            payload,
            _package_install_record(),
            platform_id="linux-x64",
            record_id="objc3c.package-install-identity.linux-x64.release.missing",
        )


def test_review_rejects_runtime_load_with_skip_reason() -> None:
    payload = _runtime_load_payload()
    payload["skip_reason"] = "native execution skipped on hosted runner"

    with pytest.raises(
        review.ReviewError,
        match="runtime load probe skip_reason was present",
    ):
        review.require_runtime_load_payload(
            payload,
            _runtime_record(),
            _object_identity_record(),
            platform_id="linux-x64",
            record_id="objc3c.runtime-load-link.linux-x64.release.missing",
        )


def test_review_allows_runtime_load_nonpass_hosted_status_when_native_proof_passes() -> None:
    payload = _runtime_load_payload()
    payload["hosted_execution_status"] = "SKIPPED"

    review.require_runtime_load_payload(
        payload,
        _runtime_record(),
        _object_identity_record(),
        platform_id="linux-x64",
        record_id="objc3c.runtime-load-link.linux-x64.release.missing",
    )


def test_review_rejects_toolchain_capability_without_native_object_emission() -> None:
    payload = {
        "ok": True,
        "native_object_emission_status": review.REQUIRED_NATIVE_OBJECT_EMISSION_STATUS,
        "llc_filetype_obj_available": True,
        "llc_target_object_emission_available": False,
        "coherent_toolchain_root": True,
        "host_platform_support_gate": {"platform_id": "linux-x64"},
        "llvm_support_matrix": {
            "toolchain_matrix_entries": [
                {
                    "host_platform_id": "linux-x64",
                    "support_status": "supported",
                    "object_emission_capability": "supported",
                    "package_capability": "supported",
                    "native_execution_capability": "supported",
                }
            ]
        },
    }

    with pytest.raises(
        review.ReviewError,
        match="toolchain capabilities llc_target_object_emission_available did not pass",
    ):
        review.require_toolchain_capabilities_payload(payload, platform_id="linux-x64")


def test_review_rejects_installed_root_without_usage_path() -> None:
    payload = _install_receipt_payload()
    payload["installed_root_execution"]["usage_banner_seen"] = False

    with pytest.raises(
        review.ReviewError,
        match="install receipt installed_root_execution did not reach usage path",
    ):
        review.require_install_receipt_payload(
            payload,
            _package_install_record(),
            platform_id="linux-x64",
            record_id="objc3c.package-install-identity.linux-x64.release.missing",
        )


def test_review_rejects_toolchain_matrix_for_wrong_host_platform() -> None:
    payload = {
        "ok": True,
        "native_object_emission_status": review.REQUIRED_NATIVE_OBJECT_EMISSION_STATUS,
        "llc_filetype_obj_available": True,
        "llc_target_object_emission_available": True,
        "coherent_toolchain_root": True,
        "host_platform_support_gate": {"platform_id": "linux-x64"},
        "llvm_support_matrix": {
            "toolchain_matrix_entries": [
                {
                    "host_platform_id": "darwin-arm64",
                    "support_status": "supported",
                    "object_emission_capability": "supported",
                    "package_capability": "supported",
                    "native_execution_capability": "supported",
                }
            ]
        },
    }

    with pytest.raises(
        review.ReviewError,
        match="toolchain capabilities host_platform_id drifted",
    ):
        review.require_toolchain_capabilities_payload(payload, platform_id="linux-x64")


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
