from __future__ import annotations

from copy import deepcopy

import pytest

from scripts.check_platform_host_promotion_evidence import (
    REQUIRED_BLOCKER_FAILURE_CLASSES,
    REQUIRED_GATE_CLASSES,
    REQUIRED_PLATFORM_IDS,
    load_host_promotion_evidence_contract,
    validate_host_promotion_evidence_contract,
)


def test_platform_host_promotion_evidence_fixture_validates() -> None:
    payload = load_host_promotion_evidence_contract()

    summary = validate_host_promotion_evidence_contract(payload)

    assert summary == {
        "contract_id": "objc3c.platform.host-promotion.evidence.summary.v1",
        "status": "PASS",
        "source_contract_id": "objc3c.platform.host-promotion.evidence.contract.v1",
        "source_path": (
            "tests/tooling/fixtures/platform_hardening/"
            "platform_host_promotion_evidence_contract.json"
        ),
        "platform_ids": list(REQUIRED_PLATFORM_IDS),
        "required_gate_classes": list(REQUIRED_GATE_CLASSES),
        "required_source_record_types": [
            "host_identity",
            "toolchain_probe",
            "package_root",
            "install_receipt",
            "native_execution",
            "object_identity",
            "debug_identity",
            "package_install_identity",
            "runtime_load_link_proof",
        ],
        "required_blocker_failure_classes": list(REQUIRED_BLOCKER_FAILURE_CLASSES),
        "required_hosted_evidence": True,
        "prose_only_platform_support_claims_allowed": False,
        "summary_path": "tmp/reports/platform-hardening/host-promotion-evidence-summary.json",
        "promotion_allowed": False,
    }

    platforms = {row["platform_id"]: row for row in payload["platforms"]}
    assert set(platforms) == {"linux-x64", "darwin-arm64"}
    assert platforms["linux-x64"]["object_debug_identity"] == {
        "object_format": "ELF",
        "debug_format": "DWARF",
        "target_triple": "x86_64-unknown-linux-gnu",
        "arch": "x64",
        "object_identity_report_path": (
            "tmp/reports/platform-host-evidence/linux-x64/build/object-identity.json"
        ),
        "debug_identity_report_path": (
            "tmp/reports/platform-host-evidence/linux-x64/build/debug-identity.json"
        ),
        "llvm_toolchain_record_id": "objc3c.host.linux-x64.toolchain-probes.fail-closed",
        "wrong_format_behavior": "fail-closed-before-package-publication",
        "wrong_arch_behavior": "fail-closed-before-install",
        "support_truth_without_package_install_execution": False,
    }
    assert platforms["darwin-arm64"]["object_debug_identity"] == {
        "object_format": "Mach-O",
        "debug_format": "DWARF/dSYM",
        "target_triple": "aarch64-apple-darwin",
        "arch": "arm64",
        "object_identity_report_path": (
            "tmp/reports/platform-host-evidence/darwin-arm64/build/object-identity.json"
        ),
        "debug_identity_report_path": (
            "tmp/reports/platform-host-evidence/darwin-arm64/build/debug-identity.json"
        ),
        "llvm_toolchain_record_id": (
            "objc3c.host.darwin-arm64.toolchain-probes.fail-closed"
        ),
        "wrong_format_behavior": "fail-closed-before-package-publication",
        "wrong_arch_behavior": "fail-closed-before-install",
        "support_truth_without_package_install_execution": False,
    }


def test_platform_host_promotion_evidence_rejects_missing_linux_row() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"] = [
        row for row in payload["platforms"] if row["platform_id"] != "linux-x64"
    ]

    with pytest.raises(RuntimeError, match="missing required platform rows: linux-x64"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_generated_promotion() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][0]["generated_hosted_evidence"]["support_truth"] = True

    with pytest.raises(RuntimeError, match="linux-x64 generated evidence promoted support"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_unrequired_hosted_evidence() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][0]["generated_hosted_evidence"]["required"] = False

    with pytest.raises(RuntimeError, match="linux-x64 hosted evidence is not required"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_prose_only_support_claims() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][0]["support_claim_policy"][
        "prose_only_platform_support_claims_allowed"
    ] = True

    with pytest.raises(RuntimeError, match="linux-x64 allowed prose-only support claims"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_non_tmp_report_output() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][0]["generated_hosted_evidence"][
        "generated_report_root"
    ] = "artifacts/platform-host-evidence/linux-x64"

    with pytest.raises(RuntimeError, match="generated report root must write under tmp/reports"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_missing_reviewed_source_truth() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][0]["reviewed_source_truth_evidence"]["required"] = False

    with pytest.raises(RuntimeError, match="linux-x64 reviewed source truth is not required"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_missing_execution_gate() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][0]["reviewed_source_truth_evidence"]["missing_record_classes"] = [
        "build",
        "package",
        "install",
    ]

    with pytest.raises(
        RuntimeError,
        match="missing build/package/install/execution gates: execution",
    ):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_linux_object_format_drift() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][0]["object_debug_identity"]["object_format"] = "COFF"

    with pytest.raises(RuntimeError, match="linux-x64 object/debug identity object_format drifted"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_darwin_debug_format_drift() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][1]["object_debug_identity"]["debug_format"] = "DWARF"

    with pytest.raises(
        RuntimeError,
        match="darwin-arm64 object/debug identity debug_format drifted",
    ):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_missing_runtime_load_requirement() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][1]["runtime_link_load_identity"]["runtime_load_probe_required"] = False

    with pytest.raises(RuntimeError, match="darwin-arm64 runtime load probe is not mandatory"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_missing_package_root_layout() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][0]["package_install_native_execution_evidence"][
        "package_root_layout"
    ] = []

    with pytest.raises(RuntimeError, match="package_root_layout must not be empty"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_missing_install_receipt_requirement() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][0]["package_install_native_execution_evidence"][
        "install_receipt_required"
    ] = False

    with pytest.raises(RuntimeError, match="linux-x64 install receipt is not mandatory"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_missing_blocker_case() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["common_fail_closed_blockers"] = [
        row
        for row in payload["common_fail_closed_blockers"]
        if row["failure_class"] != "sanitizer-variant-leakage"
    ]

    with pytest.raises(
        RuntimeError,
        match="missing required blocker failure classes: sanitizer-variant-leakage",
    ):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_sanitizer_leakage_boundary_drift() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][0]["sanitizer_variant_boundary"][
        "release_runtime_allows_sanitizer_artifacts"
    ] = True

    with pytest.raises(RuntimeError, match="linux-x64 release runtime allowed sanitizer artifacts"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_public_command_drift() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["future_checker_contract"][
        "must_register_public_command"
    ] = "ingest-platform-host-evidence"

    with pytest.raises(RuntimeError, match="future checker public command drifted"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_integration_parent_drift() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["future_checker_contract"][
        "must_be_child_of_public_action"
    ] = "validate-packaging-channels"

    with pytest.raises(RuntimeError, match="future checker integration parent action drifted"):
        validate_host_promotion_evidence_contract(payload)
