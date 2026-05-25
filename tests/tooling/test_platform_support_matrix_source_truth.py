from __future__ import annotations

from copy import deepcopy
import json
from pathlib import Path

import pytest

from scripts.objc3c_package_channels.model import (
    RELEASE_PACKAGE_TARGET_PLATFORM_CHOICES,
    release_package_layout_for_platform,
)
from scripts.check_objc3c_platform_support_matrix import (
    SOURCE_TRUTH_PATH,
    validate_platform_support_source_truth,
)

ROOT = Path(__file__).resolve().parents[2]
EXPECTED_RELEASE_PACKAGE_ROOT_LAYOUTS = {
    platform_id: release_package_layout_for_platform(platform_id)
    for platform_id in RELEASE_PACKAGE_TARGET_PLATFORM_CHOICES
}
INSTALL_PREFIX_PACKAGE_LAYOUT_ROOTS = ("bin/", "lib/", "include/")
EXPECTED_UNSUPPORTED_PROMOTION_CLOSURE = {
    "linux-x64": {
        "capability_status": "unsupported",
        "support_claim_allowed": False,
        "support_claim_blocked_until": ["build", "package", "install", "execution"],
        "package_variant_claim_state": "fail-closed",
        "package_variant_platform_ids": [],
        "package_artifact_evidence": {
            "package_root_layout": EXPECTED_RELEASE_PACKAGE_ROOT_LAYOUTS["linux-x64"],
            "runtime_library_names": ["libobjc3-runtime.so"],
            "object_format": "ELF",
            "debug_format": "DWARF",
            "loader_path_proof_required": True,
            "install_receipt_required": True,
            "installed_root_execution_required": True,
            "offline_installed_root_execution_required": True,
            "installed_root_execution_summary_path": "tmp/reports/platform-host-evidence/linux-x64/install/end-to-end-summary.json",
            "native_execution_evidence_required": True,
        },
        "host_promotion_constraints": {
            "promotion_policy": "real-host-execution-required",
            "real_host_execution_required": True,
            "generated_host_evidence_support_truth": False,
            "reviewed_source_truth_required": True,
            "hosted_runner_summary_behavior": "summary-only-no-support-promotion",
        },
        "runner_toolchain_requirements": {
            "runner_label": "ubuntu-24.04",
            "required_toolchain_components": [
                "llvm",
                "clang",
                "cmake",
                "ninja",
                "python",
                "node",
                "pwsh",
            ],
            "native_object_emission_required": True,
            "missing_native_execution_behavior": "fail-closed-no-support-promotion",
            "unsupported_toolchain_result": "fail-closed-no-range-claim",
        },
        "fail_closed_requirements": {
            "unsupported_behavior": "fail-closed",
            "unsupported_evidence_claim_weight": "policy",
            "blocked_publication_surfaces": [
                "package",
                "install",
                "execution",
                "publication",
            ],
        },
    },
    "darwin-arm64": {
        "capability_status": "unsupported",
        "support_claim_allowed": False,
        "support_claim_blocked_until": ["build", "package", "install", "execution"],
        "package_variant_claim_state": "fail-closed",
        "package_variant_platform_ids": [],
        "package_artifact_evidence": {
            "package_root_layout": EXPECTED_RELEASE_PACKAGE_ROOT_LAYOUTS["darwin-arm64"],
            "runtime_library_names": ["libobjc3-runtime.dylib"],
            "object_format": "Mach-O",
            "debug_format": "DWARF/dSYM",
            "loader_path_proof_required": True,
            "install_receipt_required": True,
            "installed_root_execution_required": True,
            "offline_installed_root_execution_required": True,
            "installed_root_execution_summary_path": "tmp/reports/platform-host-evidence/darwin-arm64/install/end-to-end-summary.json",
            "native_execution_evidence_required": True,
        },
        "host_promotion_constraints": {
            "promotion_policy": "real-host-execution-required",
            "real_host_execution_required": True,
            "generated_host_evidence_support_truth": False,
            "reviewed_source_truth_required": True,
            "hosted_runner_summary_behavior": "summary-only-no-support-promotion",
        },
        "runner_toolchain_requirements": {
            "runner_label": "macos-15",
            "required_toolchain_components": [
                "llvm",
                "clang",
                "cmake",
                "ninja",
                "python",
                "node",
                "pwsh",
            ],
            "native_object_emission_required": True,
            "missing_native_execution_behavior": "fail-closed-no-support-promotion",
            "unsupported_toolchain_result": "fail-closed-no-range-claim",
        },
        "fail_closed_requirements": {
            "unsupported_behavior": "fail-closed",
            "unsupported_evidence_claim_weight": "policy",
            "blocked_publication_surfaces": [
                "package",
                "install",
                "execution",
                "publication",
            ],
        },
    },
}


def load_source_truth() -> dict:
    return json.loads(SOURCE_TRUTH_PATH.read_text(encoding="utf-8"))


def write_source_truth(tmp_path: Path, payload: dict) -> Path:
    path = tmp_path / "source_truth_matrix.json"
    path.write_text(json.dumps(payload, indent=2, sort_keys=True), encoding="utf-8")
    return path


def assert_release_package_root_layout(platform_id: str, layout: list[str]) -> None:
    assert layout == EXPECTED_RELEASE_PACKAGE_ROOT_LAYOUTS[platform_id]
    assert not any(path.startswith(INSTALL_PREFIX_PACKAGE_LAYOUT_ROOTS) for path in layout)


def assert_unsupported_promotion_closure(row: dict) -> None:
    platform_id = row["platform_id"]
    closure = row["promotion_closure_contract"]
    assert closure == EXPECTED_UNSUPPORTED_PROMOTION_CLOSURE[platform_id]
    assert closure["support_claim_blocked_until"] == row["required_missing_evidence_classes"]
    assert closure["runner_toolchain_requirements"]["required_toolchain_components"] == row[
        "required_toolchain_components"
    ]


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
            "native_object_emission_target_object_unavailable",
            "native_object_emission_mixed_toolchain_root",
            "native_object_emission_mismatched_tool_versions",
            "native_object_emission_unsupported_tool_version",
            "native_object_emission_unresolved_tool_version",
        ],
    }
    umbrella = source_truth["umbrella_readiness_contract"]
    host_ingestion = source_truth["host_promotion_architecture"]["hosted_evidence_ingestion"]
    assert host_ingestion == {
        "workflow_path": ".github/workflows/platform-host-evidence.yml",
        "accepted_workflow_paths": [
            ".github/workflows/platform-host-evidence.yml",
            ".github/workflows/conformance-minima.yml",
        ],
        "dispatch_gateway_workflow_paths": [
            ".github/workflows/conformance-minima.yml",
        ],
        "runner_labels": {
            "linux-x64": "ubuntu-24.04",
            "darwin-arm64": "macos-15",
        },
        "ingestion_action": "ingest-platform-host-evidence",
        "review_action": "review-platform-host-evidence",
        "support_promotion_action": "review-platform-support-promotion",
        "ingestion_helper": "scripts/ingest_objc3c_platform_host_evidence.py",
        "review_helper": "scripts/review_objc3c_platform_host_evidence.py",
        "support_promotion_helper": "scripts/promote_objc3c_platform_support.py",
        "host_promotion_contract_check_action": "check-platform-host-promotion-evidence",
        "generated_report_contract_id": "objc3c.platform.hosted-runner.evidence-report.v1",
        "generated_report_root": "tmp/reports/platform-host-evidence",
        "review_candidate_source_truth_path": (
            "tmp/reports/platform-host-evidence/<platform>/"
            "review-candidate-source-truth.json"
        ),
        "reviewed_source_proposal_path": (
            "tmp/reports/platform-host-evidence/<platform>/"
            "reviewed-source-inputs.proposed.json"
        ),
        "review_staging_summary_path": (
            "tmp/reports/platform-host-evidence/<platform>/"
            "reviewed-source-staging-summary.json"
        ),
        "generated_only_result": "refuse-source-truth-promotion",
        "review_promotion_policy": "checked-in-source-truth-required",
        "candidate_evidence_record_ids": [
            "objc3c.evidence.hosted-ci.linux-x64.generated-host-run",
            "objc3c.evidence.hosted-ci.darwin-arm64.generated-host-run",
        ],
        "reviewed_source_truth_required": True,
        "support_rows_remain_fail_closed_until_reviewed": True,
    }
    assert umbrella["supported_platform_row_ids"] == ["objc3c.platform.windows-x64.tier1"]
    assert {
        row["issue_ref"]: row["claim_state"]
        for row in umbrella["child_issue_contracts"]
        } == {
            8228: "unsupported",
            8229: "unsupported",
            8230: "evidence-bound",
            8231: "evidence-bound",
            8232: "toolchain-prerequisite-fail-closed",
        }
    assert umbrella["native_object_emission_contract"] == {
        "contract_id": "objc3c.llvm.native-object-emission.fail-closed.v1",
        "issue_ref": 8232,
        "required_tool": "llc",
        "required_probe": "llc --filetype=obj",
        "required_target_probe": "llc --filetype=obj --mtriple=<target> emits a non-empty object",
        "success_status": "native_object_emission_supported",
        "missing_llc_status": "native_object_emission_missing_llc",
        "missing_filetype_status": "native_object_emission_filetype_obj_unavailable",
        "target_object_status": "native_object_emission_target_object_unavailable",
        "mixed_toolchain_status": "native_object_emission_mixed_toolchain_root",
        "mismatched_version_status": "native_object_emission_mismatched_tool_versions",
        "unsupported_version_status": "native_object_emission_unsupported_tool_version",
        "unresolved_version_status": "native_object_emission_unresolved_tool_version",
        "hosted_runner_behavior": "fail-closed-no-native-object-success-claim",
        "conformance_minima_behavior": "fail-closed-before-cross-lane-runtime-proof",
        "task_hygiene_behavior": (
            "skip-no-success-claim-when-native-object-emission-unavailable"
        ),
        "required_conformance_minima_env": (
            "OBJC3C_REQUIRE_HOSTED_NATIVE_OBJECT_EMISSION"
        ),
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
    for row in source_truth["unsupported_rows"]:
        assert_unsupported_promotion_closure(row)
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
    for row_id in (
        "objc3c.package.runtime.windows-x64.release",
        "objc3c.package.runtime.linux-x64.release.fail-closed",
        "objc3c.package.runtime.darwin-arm64.release.fail-closed",
    ):
        row = package_rows[row_id]
        assert_release_package_root_layout(
            row["target_platform_id"],
            row["artifact_identity_contract"]["package_root_layout"],
        )
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


def test_platform_support_source_truth_rejects_unsupported_capability_promotion(tmp_path: Path) -> None:
    payload = deepcopy(load_source_truth())
    payload["unsupported_rows"][0]["promotion_closure_contract"]["capability_status"] = "supported"

    with pytest.raises(Exception, match="schema validation|was expected|const"):
        validate_platform_support_source_truth(write_source_truth(tmp_path, payload))


def test_platform_support_source_truth_rejects_unsupported_runner_label_drift(tmp_path: Path) -> None:
    payload = deepcopy(load_source_truth())
    payload["unsupported_rows"][0]["promotion_closure_contract"]["runner_toolchain_requirements"][
        "runner_label"
    ] = "ubuntu-latest"

    with pytest.raises(Exception, match="schema validation|was expected|const"):
        validate_platform_support_source_truth(write_source_truth(tmp_path, payload))


def test_platform_support_source_truth_rejects_darwin_package_artifact_drift(tmp_path: Path) -> None:
    payload = deepcopy(load_source_truth())
    payload["unsupported_rows"][1]["promotion_closure_contract"]["package_artifact_evidence"][
        "package_root_layout"
    ] = EXPECTED_RELEASE_PACKAGE_ROOT_LAYOUTS["linux-x64"]

    with pytest.raises(Exception, match="schema validation|was expected|const"):
        validate_platform_support_source_truth(write_source_truth(tmp_path, payload))


def test_platform_support_source_truth_rejects_fail_open_runner_toolchain_policy(tmp_path: Path) -> None:
    payload = deepcopy(load_source_truth())
    payload["unsupported_rows"][1]["promotion_closure_contract"]["runner_toolchain_requirements"][
        "unsupported_toolchain_result"
    ] = "best-effort"

    with pytest.raises(Exception, match="schema validation|was expected|const"):
        validate_platform_support_source_truth(write_source_truth(tmp_path, payload))


def test_platform_support_source_truth_rejects_release_install_prefix_layout(tmp_path: Path) -> None:
    payload = deepcopy(load_source_truth())
    for row in payload["package_variant_rows"]:
        if row["row_id"] == "objc3c.package.runtime.windows-x64.release":
            row["artifact_identity_contract"]["package_root_layout"] = [
                "bin/objc3c-native.exe",
                "lib/objc3-runtime.lib",
                "include/objc3/runtime",
            ]
            break

    with pytest.raises(RuntimeError, match="package_root_layout"):
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


def test_platform_support_source_truth_rejects_generated_host_evidence_widening(tmp_path: Path) -> None:
    payload = deepcopy(load_source_truth())
    upstream_path = tmp_path / "platform_toolchain_support_evidence.json"
    upstream = json.loads(
        (ROOT / payload["upstream_sources"]["platform_toolchain_support_evidence"]).read_text(
            encoding="utf-8"
        )
    )
    for record in upstream["evidence_records"]:
        if record["evidence_id"] == "objc3c.evidence.hosted-ci.darwin-arm64.generated-host-run":
            record["supports_platform_ids"] = ["darwin-arm64"]
            break
    upstream_path.write_text(json.dumps(upstream, indent=2, sort_keys=True), encoding="utf-8")
    payload["upstream_sources"]["platform_toolchain_support_evidence"] = str(upstream_path)

    with pytest.raises(RuntimeError, match="policy evidence widened support|generated host evidence widened support"):
        validate_platform_support_source_truth(write_source_truth(tmp_path, payload))
