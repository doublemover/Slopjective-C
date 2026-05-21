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
    assert [row["row_id"] for row in evidence["support_rows"]] == [
        "objc3c.platform.windows-x64.tier1",
        "objc3c.platform.linux-x64.unsupported",
        "objc3c.platform.darwin-arm64.unsupported",
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
    assert windows_row["evidence"] == {
        "build": "objc3c.evidence.platform.windows-x64.build.native-binaries",
        "package": "objc3c.evidence.platform.windows-x64.package.runnable-toolchain",
        "install": "objc3c.evidence.platform.windows-x64.install.packaging-e2e",
        "execution": "objc3c.evidence.platform.windows-x64.execution.native-smoke",
    }
    clean_room_record = next(
        record
        for record in payload["evidence_records"]
        if record["evidence_id"] == "objc3c.evidence.clean-room.local-offline-install"
    )
    assert clean_room_record["replay_commands"] == [
        "python scripts/check_objc3c_package_install_distribution_credibility.py --from-nothing"
    ]
    assert (
        "tmp/reports/package-ecosystem/install-distribution-credibility-summary.json"
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
