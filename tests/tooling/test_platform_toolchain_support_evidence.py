from __future__ import annotations

from copy import deepcopy
import json
from pathlib import Path

import pytest

from scripts.platform_hardening_contracts.report_payloads import build_support_matrix_payload
from scripts.platform_hardening_contracts.support_evidence import (
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
    assert [row["row_id"] for row in evidence["support_rows"]] == [
        "objc3c.platform.windows-x64.tier1",
        "objc3c.platform.linux-x64.unsupported",
        "objc3c.platform.darwin-arm64.unsupported",
    ]
    llvm_matrix = evidence["llvm_version_support_matrix"]
    assert llvm_matrix["contract_id"] == "objc3c.llvm.version-support-matrix.source.v1"
    assert llvm_matrix["issue_ref"] == 8232
    assert llvm_matrix["support_claim_policy"] == "capability-probed-fail-closed"
    assert {tool["tool_name"] for tool in llvm_matrix["required_tools"]} == {
        "clang",
        "clang++",
        "llc",
        "llvm-ar",
        "llvm-config",
        "headers-libs",
    }
    assert [entry["entry_id"] for entry in llvm_matrix["matrix_entries"]] == [
        "objc3c.llvm.windows-x64.current-probed-19"
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
    assert llvm_matrix["matrix_entries"][0] == {
        "entry_id": "objc3c.llvm.windows-x64.current-probed-19",
        "platform_id": "windows-x64",
        "llvm_version_claim": "19.1.0-current-probed-only",
        "support_status": "evidence-bound",
        "object_emission_capability": "supported",
        "package_capability": "supported",
        "native_execution_capability": "supported",
        "evidence_ids": [
            "objc3c.evidence.toolchain.llvm.current-probe",
            "objc3c.evidence.toolchain.clang-cmake-ninja.native-build-resolution",
            "objc3c.evidence.platform.windows-x64.execution.native-smoke",
        ],
        "unsupported_version_behavior": "fail-closed-no-range-claim",
    }
    assert {
        rule["rule_id"]
        for rule in llvm_matrix["rejection_rules"]
    } == {
        "objc3c.llvm.reject.missing-llc",
        "objc3c.llvm.reject.mixed-toolchain",
        "objc3c.llvm.reject.unsupported-range",
    }
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
    assert payload["toolchain_support"]["sanitizer_variants"] == [
        {
            "variant_id": "objc3c.toolchain.sanitizer.address",
            "claim_state": "reserved",
            "platform_ids": [],
            "evidence_ids": [
                "objc3c.evidence.toolchain.sanitizer.policy.reserved",
            ],
            "diagnostic": "AddressSanitizer support is reserved until package install and native execution evidence exists for the sanitizer build.",
        },
        {
            "variant_id": "objc3c.toolchain.sanitizer.undefined",
            "claim_state": "reserved",
            "platform_ids": [],
            "evidence_ids": [
                "objc3c.evidence.toolchain.sanitizer.policy.reserved",
            ],
            "diagnostic": "UBSan support is reserved until package install and native execution evidence exists for the sanitizer build.",
        },
    ]


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


def test_platform_toolchain_support_evidence_rejects_llvm_range_compatibility_claim() -> None:
    evidence = deepcopy(load_platform_toolchain_support_evidence())
    evidence["llvm_version_support_matrix"]["matrix_entries"][0][
        "llvm_version_claim"
    ] = "compatible with all LLVM 19 installs"

    with pytest.raises(RuntimeError, match="LLVM matrix entry used unsupported compatibility language"):
        validate_evidence(evidence)
