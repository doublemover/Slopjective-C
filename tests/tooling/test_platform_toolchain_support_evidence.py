from __future__ import annotations

from copy import deepcopy
import json
from pathlib import Path

import pytest

from scripts.platform_hardening_contracts.report_payloads import build_support_matrix_payload
from scripts.platform_hardening_contracts.support_evidence import (
    load_hosted_runner_capability_summaries,
    load_platform_expansion_claim_contract,
    load_platform_toolchain_support_evidence,
    validate_platform_toolchain_support_evidence,
)

ROOT = Path(__file__).resolve().parents[2]
REQUIRED_TOOLCHAIN_COMPONENTS = [
    "llvm",
    "clang",
    "cmake",
    "ninja",
    "python",
    "node",
    "pwsh",
]


def load_fixture(relative_path: str) -> dict:
    return json.loads((ROOT / relative_path).read_text(encoding="utf-8"))


def validate_evidence(payload: dict) -> None:
    validate_platform_toolchain_support_evidence(
        payload,
        boundary=load_fixture("tests/tooling/fixtures/platform_hardening/boundary_inventory.json"),
        supported_platforms=load_fixture("tests/tooling/fixtures/packaging_channels/supported_platforms.json"),
        tier_policy=load_fixture("tests/tooling/fixtures/platform_hardening/platform_support_tier_policy.json"),
        unsupported_host_policy=load_fixture("tests/tooling/fixtures/platform_hardening/unsupported_host_fail_closed_policy.json"),
    )


def test_platform_toolchain_support_evidence_fixture_validates() -> None:
    evidence = load_platform_toolchain_support_evidence()
    hosted_summaries = load_hosted_runner_capability_summaries()
    expansion_contract = load_platform_expansion_claim_contract()
    unsupported_host_policy = load_fixture(
        "tests/tooling/fixtures/platform_hardening/unsupported_host_fail_closed_policy.json"
    )

    validate_evidence(evidence)

    assert evidence["network_policy"] == {
        "support_claims_require_live_network": False,
        "network_unavailable_result": "skip-no-support-claim",
        "unsupported_host_result": "fail-closed",
    }
    assert evidence["toolchain_evidence_requirements"] == {
        "required_components": REQUIRED_TOOLCHAIN_COMPONENTS,
        "support_claim_policy": "evidence-bound-current-probes-only",
        "unsupported_component_behavior": "fail-closed-no-range-claim",
    }
    assert evidence["host_evidence_contract"]["hosted_evidence_ingestion"] == {
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
        "host_promotion_contract_check_action": "check-platform-host-promotion-evidence",
        "ingestion_helper": "scripts/ingest_objc3c_platform_host_evidence.py",
        "generated_report_contract_id": "objc3c.platform.hosted-runner.evidence-report.v1",
        "generated_report_root": "tmp/reports/platform-host-evidence",
        "generated_only_result": "refuse-source-truth-promotion",
        "review_promotion_policy": "checked-in-source-truth-required",
        "candidate_evidence_record_ids": [
            "objc3c.evidence.hosted-ci.linux-x64.generated-host-run",
            "objc3c.evidence.hosted-ci.darwin-arm64.generated-host-run",
        ],
        "reviewed_source_truth_required": True,
        "support_rows_remain_fail_closed_until_reviewed": True,
    }
    assert {8206, 8228, 8229, 8230, 8231, 8232} <= set(evidence["roadmap_issue_refs"])
    assert hosted_summaries["support_claim_policy"] == "summary-only-no-support-promotion"
    assert {
        summary["summary_id"]: summary["publication_allowed"]
        for summary in hosted_summaries["summaries"]
    } == {
        "objc3c.hosted.windows-x64.supported.current": True,
        "objc3c.hosted.linux-x64.unsupported": False,
        "objc3c.hosted.darwin-arm64.unsupported": False,
        "objc3c.hosted.sanitizer.address.reserved": False,
        "objc3c.hosted.sanitizer.undefined.reserved": False,
        "objc3c.hosted.toolchain.missing-llc.fail-closed": False,
        "objc3c.hosted.toolchain.mixed-root.fail-closed": False,
        "objc3c.hosted.toolchain.mismatched-version.fail-closed": False,
    }
    assert expansion_contract["source_authority"] == {
        "support_claims_require_checked_source": True,
        "generated_reports_are_source_truth": False,
        "support_claims_require_live_network": False,
        "publication_boundary": "windows-x64-only",
        "package_variant_identity_source": "checked-in-package-variant-rows",
        "sanitizer_reports_are_support_truth": False,
    }
    assert {
        case["platform_id"]: case["artifact_contract"]["object_format"]
        for case in expansion_contract["platform_claim_cases"]
    } == {
        "linux-x64": "ELF",
        "darwin-arm64": "Mach-O",
    }
    assert all(
        case["artifact_contract"]["object_emission_alone_supports_platform"] is False
        for case in expansion_contract["platform_claim_cases"]
    )
    assert {
        case["summary_id"]: case["publication_allowed"]
        for case in expansion_contract["hosted_runner_projection_cases"]
    } == {
        "objc3c.hosted.windows-x64.supported.current": True,
        "objc3c.hosted.linux-x64.unsupported": False,
        "objc3c.hosted.darwin-arm64.unsupported": False,
        "objc3c.hosted.sanitizer.address.reserved": False,
        "objc3c.hosted.sanitizer.undefined.reserved": False,
        "objc3c.hosted.toolchain.missing-llc.fail-closed": False,
        "objc3c.hosted.toolchain.mixed-root.fail-closed": False,
        "objc3c.hosted.toolchain.mismatched-version.fail-closed": False,
    }
    assert {
        case["summary_id"]: case["hosted_runner"]
        for case in expansion_contract["hosted_runner_projection_cases"]
        if case["summary_kind"] == "platform"
    } == {
        "objc3c.hosted.windows-x64.supported.current": "windows-latest",
        "objc3c.hosted.linux-x64.unsupported": "ubuntu-24.04",
        "objc3c.hosted.darwin-arm64.unsupported": "macos-15",
    }
    assert "native-object-emission-unavailable" in {
        failure_class["failure_id"]
        for failure_class in unsupported_host_policy["hard_fail_classes"]
    }
    assert "mixed-toolchain-root" in {
        failure_class["failure_id"]
        for failure_class in unsupported_host_policy["hard_fail_classes"]
    }
    assert "unsupported-toolchain-version" in {
        failure_class["failure_id"]
        for failure_class in unsupported_host_policy["hard_fail_classes"]
    }
    assert (
        "native object emission requires llc --filetype=obj and has no clang substitute success path"
        in unsupported_host_policy["required_claims"]
    )
    assert [row["row_id"] for row in evidence["support_rows"]] == [
        "objc3c.platform.windows-x64.tier1",
        "objc3c.platform.linux-x64.unsupported",
        "objc3c.platform.darwin-arm64.unsupported",
    ]
    assert {
        row["platform_id"]: row["issue_ref"]
        for row in evidence["support_rows"]
    } == {
        "windows-x64": 8206,
        "linux-x64": 8228,
        "darwin-arm64": 8229,
    }
    assert {
        row["platform_id"]: row["host_triples"]
        for row in evidence["support_rows"]
    } == {
        "windows-x64": ["x86_64-pc-windows-msvc"],
        "linux-x64": ["x86_64-unknown-linux-gnu"],
        "darwin-arm64": ["aarch64-apple-darwin"],
    }
    assert [row["row_id"] for row in evidence["package_variant_rows"]] == [
        "objc3c.package.runtime.windows-x64.release",
        "objc3c.package.runtime.linux-x64.release.fail-closed",
        "objc3c.package.runtime.darwin-arm64.release.fail-closed",
        "objc3c.package.sanitizer.asan.reserved",
        "objc3c.package.sanitizer.ubsan.reserved",
    ]
    llvm_matrix = evidence["llvm_version_support_matrix"]
    assert llvm_matrix["contract_id"] == "objc3c.llvm.version-support-matrix.source.v1"
    assert llvm_matrix["issue_ref"] == 8232
    assert llvm_matrix["support_claim_policy"] == "capability-probed-fail-closed"
    assert llvm_matrix["native_object_emission_contract"] == {
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
        "task_hygiene_behavior": (
            "skip-no-success-claim-when-native-object-emission-unavailable"
        ),
        "required_conformance_minima_env": (
            "OBJC3C_REQUIRE_HOSTED_NATIVE_OBJECT_EMISSION"
        ),
        "fallback_policy": "no-clang-fallback-success-claim",
        "coherent_toolchain_policy": "no-mixed-root-or-mismatched-version-success-claim",
    }
    assert {tool["tool_name"] for tool in llvm_matrix["required_tools"]} == {
        "clang",
        "clang++",
        "llc",
        "llvm-ar",
        "headers-libs",
    }
    assert all(tool["claim_state"] == "required" for tool in llvm_matrix["required_tools"])
    assert {
        tool["tool_name"]: tool["version_source"]
        for tool in llvm_matrix["required_tools"]
    } == {
        "clang": "probe",
        "clang++": "native-build-resolution",
        "llc": "probe",
        "llvm-ar": "probe",
        "headers-libs": "probe-or-install-root",
    }
    assert [entry["entry_id"] for entry in llvm_matrix["matrix_entries"]] == [
        "objc3c.llvm.windows-x64.current-probed-22"
    ]


def test_platform_support_matrix_publishes_issue_owned_evidence_sections() -> None:
    payload = build_support_matrix_payload()

    assert payload["support_evidence_contract"] == {
        "contract_id": "objc3c.platform.toolchain.support.evidence.v1",
        "schema_version": 1,
        "issue": "OBJ3-NEXT-024",
        "schema_path": "schemas/objc3c-platform-toolchain-support-evidence-v1.schema.json",
        "source_path": "tests/tooling/fixtures/platform_hardening/platform_toolchain_support_evidence.json",
        "network_policy": {
            "support_claims_require_live_network": False,
            "network_unavailable_result": "skip-no-support-claim",
            "unsupported_host_result": "fail-closed",
        },
    }
    supported_rows = [
        row for row in payload["platform_support_rows"] if row["support_state"] == "supported"
    ]
    assert [row["platform_id"] for row in supported_rows] == ["windows-x64"]
    windows_row = supported_rows[0]
    assert windows_row["required_toolchain_components"] == REQUIRED_TOOLCHAIN_COMPONENTS
    assert windows_row["evidence"] == {
        "build": "objc3c.evidence.platform.windows-x64.build.native-binaries",
        "package": "objc3c.evidence.platform.windows-x64.package.runnable-toolchain",
        "install": "objc3c.evidence.platform.windows-x64.install.packaging-e2e",
        "execution": "objc3c.evidence.platform.windows-x64.execution.native-smoke",
    }
    assert windows_row["toolchain_evidence_ids"] == [
        "objc3c.evidence.toolchain.llvm.current-probe",
        "objc3c.evidence.toolchain.clang-cmake-ninja.native-build-resolution",
        "objc3c.evidence.toolchain.python-node-pwsh.package-bridge",
    ]
    toolchain_ranges = payload["toolchain_support"]["toolchain_ranges"]
    assert payload["toolchain_support"]["toolchain_evidence_requirements"] == {
        "required_components": REQUIRED_TOOLCHAIN_COMPONENTS,
        "support_claim_policy": "evidence-bound-current-probes-only",
        "unsupported_component_behavior": "fail-closed-no-range-claim",
    }
    assert {row["component"] for row in toolchain_ranges} == set(REQUIRED_TOOLCHAIN_COMPONENTS)
    assert {
        row["component"]: row["range_claim"]
        for row in toolchain_ranges
    } == {
        "llvm": "current-probed-executable-only",
        "clang": "current-clangxx-executable-only",
        "cmake": "current-cmake-executable-only",
        "ninja": "current-ninja-executable-only",
        "python": "checked-in-package-bridge-current-major-lines-only",
        "node": "current-node-executable-used-by-public-npm-bridge-only",
        "pwsh": "current-pwsh-executable-used-by-packaging-scripts-only",
    }
    assert all(row["required_evidence_classes"] == ["toolchain"] for row in toolchain_ranges)
    llvm_matrix = payload["toolchain_support"]["llvm_version_support_matrix"]
    assert llvm_matrix["support_claim_policy"] == "capability-probed-fail-closed"
    assert llvm_matrix["minimum_supported_version"] == "19.1.0"
    assert "22.1.6" in llvm_matrix["known_good_versions"]
    assert llvm_matrix["matrix_entries"][0] == {
        "entry_id": "objc3c.llvm.windows-x64.current-probed-22",
        "platform_id": "windows-x64",
        "llvm_version_claim": "22.1.6-current-probed-only",
        "support_status": "evidence-bound",
        "object_emission_capability": "supported",
        "package_capability": "supported",
        "native_execution_capability": "supported",
        "evidence_ids": [
            "objc3c.evidence.toolchain.llvm.current-probe",
            "objc3c.evidence.toolchain.clang-cmake-ninja.native-build-resolution",
            "objc3c.evidence.platform.windows-x64.package.runnable-toolchain",
            "objc3c.evidence.clean-room.local-offline-install",
            "objc3c.evidence.platform.windows-x64.execution.native-smoke",
        ],
        "unsupported_version_behavior": "fail-closed-no-range-claim",
    }
    assert {
        rule["rule_id"]
        for rule in llvm_matrix["rejection_rules"]
    } == {
        "objc3c.llvm.reject.missing-llc",
        "objc3c.llvm.reject.missing-archive-tool",
        "objc3c.llvm.reject.missing-headers-libs",
        "objc3c.llvm.reject.mixed-toolchain",
        "objc3c.llvm.reject.mismatched-tool-version",
        "objc3c.llvm.reject.unresolved-tool-version",
        "objc3c.llvm.reject.unsupported-range",
    }
    missing_llc_rule = next(
        rule
        for rule in llvm_matrix["rejection_rules"]
        if rule["rule_id"] == "objc3c.llvm.reject.missing-llc"
    )
    assert missing_llc_rule["failure_status"] == "native_object_emission_missing_llc"
    assert missing_llc_rule["hosted_runner_behavior"] == "fail-closed-no-native-object-success-claim"
    assert missing_llc_rule["conformance_minima_behavior"] == "fail-closed-before-cross-lane-runtime-proof"
    assert missing_llc_rule["fallback_policy"] == "no-clang-fallback-success-claim"
    clean_room_record = next(
        record
        for record in payload["evidence_records"]
        if record["evidence_id"] == "objc3c.evidence.clean-room.local-offline-install"
    )
    assert clean_room_record["replay_commands"] == [
        "npm run objc3c -- validate-package-install-distribution --from-nothing"
    ]
    assert (
        "tmp/reports/package-ecosystem/install-distribution-credibility-summary.json"
        in clean_room_record["generated_report_paths"]
    )
    assert (
        "tmp/artifacts/package-ecosystem/install-validation/objc3c-install-distribution-verification.json"
        in clean_room_record["generated_report_paths"]
    )
    assert {
        row["platform_id"]: row["failure_id"]
        for row in payload["platform_support_rows"]
        if row["support_state"] == "unsupported"
    } == {
        "linux-x64": "unsupported-host-linux-x64-denied",
        "darwin-arm64": "unsupported-host-darwin-arm64-denied",
    }
    assert "objc3c.evidence.toolchain.llvm.current-probe" in payload["support_evidence_ids"]
    assert {
        "objc3c.evidence.hosted-ci.linux-x64.generated-host-run",
        "objc3c.evidence.hosted-ci.darwin-arm64.generated-host-run",
    } <= set(payload["support_evidence_ids"])
    generated_host_records = {
        record["evidence_id"]: record
        for record in payload["evidence_records"]
        if record["evidence_id"].startswith("objc3c.evidence.hosted-ci.")
        and record["evidence_id"].endswith(".generated-host-run")
    }
    assert {
        record_id: record["claim_weight"]
        for record_id, record in generated_host_records.items()
    } == {
        "objc3c.evidence.hosted-ci.linux-x64.generated-host-run": "policy",
        "objc3c.evidence.hosted-ci.darwin-arm64.generated-host-run": "policy",
    }
    assert all(
        not record["supports_platform_ids"]
        and "scripts/ingest_objc3c_platform_host_evidence.py" in record["source_paths"]
        and any(
            path.startswith("tmp/reports/platform-host-evidence/")
            for path in record["generated_report_paths"]
        )
        for record in generated_host_records.values()
    )
    package_rows = {
        row["row_id"]: row
        for row in payload["package_variant_rows"]
    }
    assert package_rows["objc3c.package.runtime.windows-x64.release"]["claim_state"] == "evidence-bound"
    assert package_rows["objc3c.package.runtime.windows-x64.release"]["platform_ids"] == ["windows-x64"]
    assert package_rows["objc3c.package.runtime.linux-x64.release.fail-closed"]["claim_state"] == "fail-closed"
    assert package_rows["objc3c.package.runtime.linux-x64.release.fail-closed"][
        "required_missing_evidence_classes"
    ] == ["build", "package", "install", "execution"]
    assert package_rows["objc3c.package.runtime.darwin-arm64.release.fail-closed"]["platform_ids"] == []
    assert package_rows["objc3c.package.runtime.darwin-arm64.release.fail-closed"][
        "required_missing_evidence_classes"
    ] == ["build", "package", "install", "execution"]
    assert package_rows["objc3c.package.runtime.linux-x64.release.fail-closed"][
        "artifact_identity_contract"
    ]["object_format"] == "ELF"
    assert package_rows["objc3c.package.runtime.linux-x64.release.fail-closed"][
        "artifact_identity_contract"
    ]["runtime_library_names"] == ["libobjc3-runtime.so"]
    assert package_rows["objc3c.package.runtime.darwin-arm64.release.fail-closed"][
        "artifact_identity_contract"
    ]["object_format"] == "Mach-O"
    assert package_rows["objc3c.package.runtime.darwin-arm64.release.fail-closed"][
        "artifact_identity_contract"
    ]["runtime_library_names"] == ["libobjc3-runtime.dylib"]
    assert set(
        package_rows["objc3c.package.runtime.linux-x64.release.fail-closed"][
            "promotion_gate_contract"
        ]["blocked_publication_surfaces"]
    ) == {"package", "install", "execution", "publication"}
    assert package_rows["objc3c.package.sanitizer.asan.reserved"]["claim_state"] == "evidence-bound"
    assert package_rows["objc3c.package.sanitizer.asan.reserved"][
        "required_missing_evidence_classes"
    ] == []
    assert package_rows["objc3c.package.sanitizer.ubsan.reserved"]["platform_ids"] == ["windows-x64"]
    assert package_rows["objc3c.package.sanitizer.ubsan.reserved"][
        "required_missing_evidence_classes"
    ] == []
    assert all(
        row["metadata_freshness_guard"] == {
            "metadata_source": (
                "tests/tooling/fixtures/platform_hardening/"
                f"platform_toolchain_support_evidence.json#{row_id}"
            ),
            "generated_metadata_allowed": False,
            "stale_package_metadata_behavior": "fail-closed-before-publication",
            "blocks_publication_on_stale": True,
        }
        for row_id, row in package_rows.items()
    )
    assert package_rows["objc3c.package.sanitizer.asan.reserved"][
        "runtime_library_contract"
    ] == {
        "runtime_library_ids": ["objc3-runtime", "clang_rt.asan"],
        "missing_runtime_behavior": "fail-closed-before-package-install",
        "mixed_runtime_behavior": "fail-closed",
    }
    assert package_rows["objc3c.package.sanitizer.ubsan.reserved"][
        "runtime_library_contract"
    ] == {
        "runtime_library_ids": ["objc3-runtime", "clang_rt.ubsan"],
        "missing_runtime_behavior": "fail-closed-before-package-install",
        "mixed_runtime_behavior": "fail-closed",
    }
    expansion = payload["platform_expansion_claim_contract"]
    assert expansion["contract_id"] == "objc3c.platform.expansion.claim.contract.v1"
    assert expansion["issue_refs"] == [8228, 8229, 8230, 8231, 8232]
    assert expansion["platform_claim_case_ids"] == [
        "objc3c.platform.linux-x64.fail-closed.claim-case",
        "objc3c.platform.darwin-arm64.fail-closed.claim-case",
    ]
    assert {
        "objc3c.package.identity.sanitizer.asan.reserved",
        "objc3c.package.identity.sanitizer.ubsan.reserved",
    } <= set(expansion["package_variant_identity_ids"])
    assert {
        "objc3c.object-emission.reject.missing-llc",
        "objc3c.object-emission.reject.mixed-toolchain-root",
        "objc3c.object-emission.reject.mismatched-tool-version",
    } <= set(expansion["object_emission_truth_case_ids"])

    sanitizer_rows = {
        row["variant_id"]: row
        for row in payload["toolchain_support"]["sanitizer_variants"]
    }
    assert sanitizer_rows["objc3c.toolchain.sanitizer.address"]["issue_ref"] == 8230
    assert sanitizer_rows["objc3c.toolchain.sanitizer.address"]["sanitizer"] == "address"
    assert sanitizer_rows["objc3c.toolchain.sanitizer.address"]["claim_state"] == "evidence-bound"
    assert sanitizer_rows["objc3c.toolchain.sanitizer.address"]["platform_ids"] == ["windows-x64"]
    assert sanitizer_rows["objc3c.toolchain.sanitizer.address"][
        "required_promotion_evidence"
    ] == ["package", "install", "execution"]
    assert sanitizer_rows["objc3c.toolchain.sanitizer.address"][
        "required_missing_evidence_classes"
    ] == []
    assert sanitizer_rows["objc3c.toolchain.sanitizer.address"]["package_variant_row_id"] == "objc3c.package.sanitizer.asan.reserved"
    assert "-fsanitize=address" in sanitizer_rows["objc3c.toolchain.sanitizer.address"]["build_contract"]["compiler_flags"]
    assert sanitizer_rows["objc3c.toolchain.sanitizer.address"]["install_guard"] == {
        "release_channel_policy": "asan packages are opt-in and blocked from default release runtime installs",
        "unsupported_platform_behavior": "fail-closed",
        "missing_runtime_behavior": "fail-closed-before-package-install",
        "mixed_runtime_behavior": "fail-closed",
        "stale_package_metadata_behavior": "fail-closed-before-publication",
    }
    assert sanitizer_rows["objc3c.toolchain.sanitizer.address"][
        "package_runtime_contract"
    ]["runtime_probe_required"] is True
    assert sanitizer_rows["objc3c.toolchain.sanitizer.address"][
        "package_runtime_contract"
    ]["default_release_channel_allowed"] is False
    assert sanitizer_rows["objc3c.toolchain.sanitizer.undefined"]["issue_ref"] == 8231
    assert sanitizer_rows["objc3c.toolchain.sanitizer.undefined"]["sanitizer"] == "undefined"
    assert sanitizer_rows["objc3c.toolchain.sanitizer.undefined"]["claim_state"] == "evidence-bound"
    assert sanitizer_rows["objc3c.toolchain.sanitizer.undefined"]["platform_ids"] == ["windows-x64"]
    assert sanitizer_rows["objc3c.toolchain.sanitizer.undefined"][
        "required_promotion_evidence"
    ] == ["package", "install", "execution"]
    assert sanitizer_rows["objc3c.toolchain.sanitizer.undefined"][
        "required_missing_evidence_classes"
    ] == []
    assert sanitizer_rows["objc3c.toolchain.sanitizer.undefined"]["package_variant_row_id"] == "objc3c.package.sanitizer.ubsan.reserved"
    assert "-fsanitize=undefined" in sanitizer_rows["objc3c.toolchain.sanitizer.undefined"]["build_contract"]["compiler_flags"]
    assert sanitizer_rows["objc3c.toolchain.sanitizer.undefined"]["install_guard"] == {
        "release_channel_policy": "ubsan packages are opt-in and blocked from default release runtime installs",
        "unsupported_platform_behavior": "fail-closed",
        "missing_runtime_behavior": "fail-closed-before-package-install",
        "mixed_runtime_behavior": "fail-closed",
        "stale_package_metadata_behavior": "fail-closed-before-publication",
    }
    assert sanitizer_rows["objc3c.toolchain.sanitizer.undefined"][
        "package_runtime_contract"
    ]["mixed_release_sanitizer_runtime_behavior"] == "fail-closed"


def test_platform_toolchain_support_evidence_rejects_network_backed_support_claim() -> None:
    evidence = deepcopy(load_platform_toolchain_support_evidence())
    evidence["evidence_records"][0]["requires_network"] = True

    with pytest.raises(RuntimeError, match="requires network"):
        validate_evidence(evidence)


def test_platform_toolchain_support_evidence_rejects_unsupported_host_widening() -> None:
    evidence = deepcopy(load_platform_toolchain_support_evidence())
    for record in evidence["evidence_records"]:
        if record["evidence_id"] == "objc3c.evidence.unsupported.linux-x64.fail-closed":
            record["supports_platform_ids"] = ["linux-x64"]
            break

    with pytest.raises(RuntimeError, match="widened support outside"):
        validate_evidence(evidence)


def test_platform_toolchain_support_evidence_rejects_missing_required_toolchain_component() -> None:
    evidence = deepcopy(load_platform_toolchain_support_evidence())
    evidence["toolchain_ranges"] = [
        row
        for row in evidence["toolchain_ranges"]
        if row["component"] != "node"
    ]

    with pytest.raises(RuntimeError, match="missing required node toolchain range"):
        validate_evidence(evidence)


def test_platform_toolchain_support_evidence_rejects_toolchain_compatibility_claim() -> None:
    evidence = deepcopy(load_platform_toolchain_support_evidence())
    evidence["toolchain_ranges"][0]["range_claim"] = "compatible fallback LLVM versions"

    with pytest.raises(RuntimeError, match="unsupported compatibility language"):
        validate_evidence(evidence)


def test_platform_toolchain_support_evidence_rejects_missing_llc_matrix_tool() -> None:
    evidence = deepcopy(load_platform_toolchain_support_evidence())
    evidence["llvm_version_support_matrix"]["required_tools"] = [
        tool
        for tool in evidence["llvm_version_support_matrix"]["required_tools"]
        if tool["tool_name"] != "llc"
    ]

    with pytest.raises(RuntimeError, match="required tools drifted"):
        validate_evidence(evidence)


def test_platform_toolchain_support_evidence_rejects_reserved_archive_matrix_tool() -> None:
    evidence = deepcopy(load_platform_toolchain_support_evidence())
    for tool in evidence["llvm_version_support_matrix"]["required_tools"]:
        if tool["tool_name"] == "llvm-ar":
            tool["claim_state"] = "reserved"
            tool["version_source"] = "reserved"

    with pytest.raises(RuntimeError, match="llvm-ar must be required"):
        validate_evidence(evidence)


def test_platform_toolchain_support_evidence_rejects_supported_row_with_missing_evidence() -> None:
    evidence = deepcopy(load_platform_toolchain_support_evidence())
    evidence["support_rows"][0]["required_missing_evidence_classes"] = ["package"]

    with pytest.raises(RuntimeError, match="supported row cannot list missing evidence classes"):
        validate_evidence(evidence)


def test_platform_toolchain_support_evidence_rejects_sanitizer_missing_promotion_evidence() -> None:
    evidence = deepcopy(load_platform_toolchain_support_evidence())
    evidence["sanitizer_variants"][0]["required_promotion_evidence"] = ["package", "install"]

    with pytest.raises(RuntimeError, match="sanitizer promotion prerequisites"):
        validate_evidence(evidence)


def test_platform_toolchain_support_evidence_rejects_expansion_package_identity_drift() -> None:
    evidence = deepcopy(load_platform_toolchain_support_evidence())
    for row in evidence["package_variant_rows"]:
        if row["row_id"] == "objc3c.package.sanitizer.asan.reserved":
            row["package_id"] = "org.objc3c.runtime:objc3c-runtime-release"
            break

    with pytest.raises(RuntimeError, match="package_id drifted"):
        validate_evidence(evidence)


def test_platform_toolchain_support_evidence_rejects_package_promotion_gate_drift() -> None:
    evidence = deepcopy(load_platform_toolchain_support_evidence())
    for row in evidence["package_variant_rows"]:
        if row["row_id"] == "objc3c.package.runtime.linux-x64.release.fail-closed":
            row["promotion_gate_contract"]["blocked_publication_surfaces"] = ["publication"]
            break

    with pytest.raises(RuntimeError, match="did not block package install execution and publication"):
        validate_evidence(evidence)


def test_platform_toolchain_support_evidence_rejects_llvm_range_compatibility_claim() -> None:
    evidence = deepcopy(load_platform_toolchain_support_evidence())
    evidence["llvm_version_support_matrix"]["matrix_entries"][0][
        "llvm_version_claim"
    ] = "compatible with all LLVM 19 installs"

    with pytest.raises(RuntimeError, match="LLVM matrix entry used unsupported compatibility language"):
        validate_evidence(evidence)


def test_platform_toolchain_support_evidence_rejects_missing_native_object_policy_class() -> None:
    evidence = deepcopy(load_platform_toolchain_support_evidence())
    unsupported_host_policy = load_fixture(
        "tests/tooling/fixtures/platform_hardening/unsupported_host_fail_closed_policy.json"
    )
    unsupported_host_policy["hard_fail_classes"] = [
        failure_class
        for failure_class in unsupported_host_policy["hard_fail_classes"]
        if failure_class["failure_id"] != "native-object-emission-unavailable"
    ]

    with pytest.raises(RuntimeError, match="hard-fail classes drifted"):
        validate_platform_toolchain_support_evidence(
            evidence,
            boundary=load_fixture("tests/tooling/fixtures/platform_hardening/boundary_inventory.json"),
            supported_platforms=load_fixture("tests/tooling/fixtures/packaging_channels/supported_platforms.json"),
            tier_policy=load_fixture("tests/tooling/fixtures/platform_hardening/platform_support_tier_policy.json"),
            unsupported_host_policy=unsupported_host_policy,
        )


def test_platform_toolchain_support_evidence_rejects_generated_host_run_promotion() -> None:
    evidence = deepcopy(load_platform_toolchain_support_evidence())
    for record in evidence["evidence_records"]:
        if record["evidence_id"] == "objc3c.evidence.hosted-ci.linux-x64.generated-host-run":
            record["supports_platform_ids"] = ["linux-x64"]
            break

    with pytest.raises(RuntimeError, match="generated host evidence widened support"):
        validate_evidence(evidence)
