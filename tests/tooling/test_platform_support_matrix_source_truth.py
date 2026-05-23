from __future__ import annotations

from copy import deepcopy
import json
from pathlib import Path

import pytest

from scripts.check_objc3c_platform_support_matrix import (
    SOURCE_TRUTH_PATH,
    validate_platform_support_source_truth,
)

ROOT = Path(__file__).resolve().parents[2]


def load_source_truth() -> dict:
    return json.loads(SOURCE_TRUTH_PATH.read_text(encoding="utf-8"))


def write_source_truth(tmp_path: Path, payload: dict) -> Path:
    path = tmp_path / "source_truth_matrix.json"
    path.write_text(json.dumps(payload, indent=2, sort_keys=True), encoding="utf-8")
    return path


def test_platform_support_source_truth_validates_checked_matrix() -> None:
    source_truth = load_source_truth()
    summary = validate_platform_support_source_truth()

    assert summary["status"] == "PASS"
    assert summary["issue"] == "OBJ3-NEXT-024"
    assert summary["supported_platform_ids"] == ["windows-x64"]
    assert summary["unsupported_platform_ids"] == ["darwin-arm64", "linux-x64"]
    assert summary["required_supported_evidence_classes"] == [
        "build",
        "package",
        "install",
        "execution",
    ]
    assert summary["required_auxiliary_evidence_classes"] == [
        "toolchain",
        "hosted_ci",
        "clean_room",
    ]
    assert summary["required_toolchain_components"] == [
        "llvm",
        "clang",
        "cmake",
        "ninja",
        "python",
        "node",
        "pwsh",
    ]
    assert summary["package_variant_row_ids"] == [
        "objc3c.package.runtime.darwin-arm64.release.fail-closed",
        "objc3c.package.runtime.linux-x64.release.fail-closed",
        "objc3c.package.runtime.windows-x64.release",
        "objc3c.package.sanitizer.asan.reserved",
        "objc3c.package.sanitizer.ubsan.reserved",
    ]
    assert summary["sanitizer_variant_ids"] == [
        "objc3c.toolchain.sanitizer.address",
        "objc3c.toolchain.sanitizer.undefined",
    ]
    assert summary["umbrella_readiness"] == {
        "umbrella_issue_ref": 8206,
        "closure_state": "source-owned-fail-closed-ready",
        "support_claim_boundary": "windows-x64-only",
        "child_issue_refs": [8228, 8229, 8230, 8231, 8232],
        "native_object_emission_statuses": [
            "native_object_emission_supported",
            "native_object_emission_missing_llc",
            "native_object_emission_filetype_obj_unavailable",
            "native_object_emission_mixed_toolchain_root",
            "native_object_emission_mismatched_tool_versions",
            "native_object_emission_unsupported_tool_version",
            "native_object_emission_unresolved_tool_version",
        ],
    }
    umbrella = source_truth["umbrella_readiness_contract"]
    assert umbrella["supported_platform_row_ids"] == ["objc3c.platform.windows-x64.tier1"]
    assert {
        row["issue_ref"]: row["claim_state"]
        for row in umbrella["child_issue_contracts"]
    } == {
        8228: "unsupported",
        8229: "unsupported",
        8230: "reserved",
        8231: "reserved",
        8232: "toolchain-prerequisite-fail-closed",
    }
    assert umbrella["native_object_emission_contract"] == {
        "contract_id": "objc3c.llvm.native-object-emission.fail-closed.v1",
        "issue_ref": 8232,
        "required_tool": "llc",
        "required_probe": "llc --filetype=obj",
        "success_status": "native_object_emission_supported",
        "missing_llc_status": "native_object_emission_missing_llc",
        "missing_filetype_status": "native_object_emission_filetype_obj_unavailable",
        "mixed_toolchain_status": "native_object_emission_mixed_toolchain_root",
        "mismatched_version_status": "native_object_emission_mismatched_tool_versions",
        "unsupported_version_status": "native_object_emission_unsupported_tool_version",
        "unresolved_version_status": "native_object_emission_unresolved_tool_version",
        "hosted_runner_behavior": "fail-closed-no-native-object-success-claim",
        "conformance_minima_behavior": "fail-closed-before-cross-lane-runtime-proof",
        "fallback_policy": "no-clang-fallback-success-claim",
        "coherent_toolchain_policy": "no-mixed-root-or-mismatched-version-success-claim",
    }
    assert {
        row["platform_id"]: row["host_triples"]
        for row in [*source_truth["supported_rows"], *source_truth["unsupported_rows"]]
    } == {
        "windows-x64": ["x86_64-pc-windows-msvc"],
        "linux-x64": ["x86_64-unknown-linux-gnu"],
        "darwin-arm64": ["aarch64-apple-darwin"],
    }
    assert all(
        row["metadata_freshness_guard"]["generated_metadata_allowed"] is False
        and row["metadata_freshness_guard"]["blocks_publication_on_stale"] is True
        and row["metadata_freshness_guard"]["stale_package_metadata_behavior"]
        == "fail-closed-before-publication"
        and row["promotion_gate_contract"]["promotion_source"]
        == "checked-in-package-variant-row"
        and row["promotion_gate_contract"]["hosted_runner_summary_behavior"]
        == "summary-only-no-support-promotion"
        for row in source_truth["package_variant_rows"]
    )
    package_rows = {
        row["row_id"]: row
        for row in source_truth["package_variant_rows"]
    }
    assert package_rows["objc3c.package.runtime.linux-x64.release.fail-closed"][
        "artifact_identity_contract"
    ]["runtime_library_names"] == ["libobjc3-runtime.so"]
    assert package_rows["objc3c.package.runtime.darwin-arm64.release.fail-closed"][
        "artifact_identity_contract"
    ]["runtime_library_names"] == ["libobjc3-runtime.dylib"]
    assert set(
        package_rows["objc3c.package.sanitizer.asan.reserved"][
            "promotion_gate_contract"
        ]["blocked_publication_surfaces"]
    ) == {"package", "install", "execution", "publication"}
    assert all(
        row["install_guard"]["missing_runtime_behavior"]
        == "fail-closed-before-package-install"
        and row["install_guard"]["stale_package_metadata_behavior"]
        == "fail-closed-before-publication"
        and row["package_runtime_contract"]["runtime_probe_required"] is True
        and row["package_runtime_contract"]["report_artifact_support_truth"] is False
        for row in source_truth["sanitizer_variant_rows"]
    )


def test_platform_support_source_truth_rejects_source_only_support_row(tmp_path: Path) -> None:
    payload = deepcopy(load_source_truth())
    payload["supported_rows"][0]["required_evidence"].pop("package")

    with pytest.raises(Exception, match="required property|schema validation"):
        validate_platform_support_source_truth(write_source_truth(tmp_path, payload))


def test_platform_support_source_truth_rejects_unregistered_public_command(tmp_path: Path) -> None:
    payload = deepcopy(load_source_truth())
    payload["supported_rows"][0]["public_replay_commands"].append(
        "npm run objc3c -- invented-platform-support"
    )

    with pytest.raises(RuntimeError, match="not in ACTION_SPECS"):
        validate_platform_support_source_truth(write_source_truth(tmp_path, payload))


def test_platform_support_source_truth_rejects_unsupported_host_widening(tmp_path: Path) -> None:
    payload = deepcopy(load_source_truth())
    payload["unsupported_rows"][0]["platform_id"] = "windows-x64"

    with pytest.raises(RuntimeError, match="both supported and unsupported"):
        validate_platform_support_source_truth(write_source_truth(tmp_path, payload))


def test_platform_support_source_truth_rejects_generated_source_truth(tmp_path: Path) -> None:
    payload = deepcopy(load_source_truth())
    upstream_path = tmp_path / "platform_toolchain_support_evidence.json"
    upstream = json.loads(
        (ROOT / payload["upstream_sources"]["platform_toolchain_support_evidence"]).read_text(
            encoding="utf-8"
        )
    )
    upstream["evidence_records"][0]["source_paths"] = [
        "tmp/reports/platform-matrix/generated-input.json"
    ]
    upstream_path.write_text(json.dumps(upstream, indent=2, sort_keys=True), encoding="utf-8")
    payload["upstream_sources"]["platform_toolchain_support_evidence"] = str(upstream_path)

    with pytest.raises(RuntimeError, match="used generated output as source truth"):
        validate_platform_support_source_truth(write_source_truth(tmp_path, payload))


def test_platform_support_source_truth_rejects_umbrella_support_widening(tmp_path: Path) -> None:
    payload = deepcopy(load_source_truth())
    payload["umbrella_readiness_contract"]["support_claim_boundary"] = "all-platforms"

    with pytest.raises(Exception, match="schema validation|was expected|const"):
        validate_platform_support_source_truth(write_source_truth(tmp_path, payload))


def test_platform_support_source_truth_rejects_native_object_clang_fallback(tmp_path: Path) -> None:
    payload = deepcopy(load_source_truth())
    payload["umbrella_readiness_contract"]["native_object_emission_contract"][
        "fallback_policy"
    ] = "clang-fallback-success-claim"

    with pytest.raises(Exception, match="schema validation|was expected|const"):
        validate_platform_support_source_truth(write_source_truth(tmp_path, payload))
