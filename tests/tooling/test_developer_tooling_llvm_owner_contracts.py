from __future__ import annotations

import json
from pathlib import Path

from scripts.objc3c_workflow.actions import (
    developer_tooling_llvm_hosted,
    developer_tooling_llvm_parity,
    hosted_llvm_summary,
)
from scripts.objc3c_workflow.actions.developer_tooling_llvm_contracts import (
    CHECK_HOSTED_LLVM_CAPABILITIES_ACTION,
    HOSTED_LLVM_CAPABILITY_TRUTH_SOURCE,
    LOCAL_LLVM_DIAGNOSTIC_SOURCE,
    LLVM_TOOL_CAPABILITY_OWNER_CONTRACTS,
    hosted_llvm_capability_truth_from_summary,
)

ROOT = Path(__file__).resolve().parents[2]
OWNER_CONTRACT_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "developer_tooling"
    / "hosted_llvm_tool_capability_owner_contract.json"
)


def _capable_llvm_summary() -> dict[str, object]:
    return {
        "mode": "objc3c-llvm-capabilities-v2",
        "ok": True,
        "clang": {"found": True},
        "llc": {"found": True},
        "llc_features": {"supports_filetype_obj": True},
    }


def test_llvm_tool_capability_owner_contracts_match_machine_readable_fixture() -> None:
    fixture = json.loads(OWNER_CONTRACT_FIXTURE.read_text(encoding="utf-8"))

    actual = {
        contract.action: {
            "owner": contract.owner,
            "proof_source": contract.proof_source,
            "claim_scope": contract.claim_scope,
            "requires_hosted_probe_summary": contract.requires_hosted_probe_summary,
            "fails_closed_without_live_capability": (
                contract.fails_closed_without_live_capability
            ),
            "unsupported_claims": list(contract.unsupported_claims),
        }
        for contract in LLVM_TOOL_CAPABILITY_OWNER_CONTRACTS
    }
    expected = {
        contract["action"]: {
            "owner": contract["owner"],
            "proof_source": contract["proof_source"],
            "claim_scope": contract["claim_scope"],
            "requires_hosted_probe_summary": contract[
                "requires_hosted_probe_summary"
            ],
            "fails_closed_without_live_capability": contract[
                "fails_closed_without_live_capability"
            ],
            "unsupported_claims": contract["unsupported_claims"],
        }
        for contract in fixture["owner_contracts"]
    }

    assert actual == expected


def test_hosted_llvm_contract_forbids_clang_only_support_claim() -> None:
    contracts_by_action = {
        contract.action: contract for contract in LLVM_TOOL_CAPABILITY_OWNER_CONTRACTS
    }
    hosted_contract = contracts_by_action[CHECK_HOSTED_LLVM_CAPABILITIES_ACTION]

    assert hosted_contract.requires_hosted_probe_summary is True
    assert hosted_contract.fails_closed_without_live_capability is True
    assert "clang-only hosted execution" in hosted_contract.unsupported_claims


def test_local_llvm_probe_payload_remains_diagnostic_not_capability_truth() -> None:
    truth = hosted_llvm_capability_truth_from_summary(
        _capable_llvm_summary(),
        source_kind=LOCAL_LLVM_DIAGNOSTIC_SOURCE,
    )
    payload = truth.as_payload()

    assert payload["local_probe_diagnostic_only"] is True
    assert payload["hosted_execution_supported"] is False
    assert payload["hosted_source_parity_supported"] is False
    assert payload["fail_closed_without_live_capability"] is True
    assert "local LLVM probe summary is diagnostic-only" in payload["failure_reasons"]


def test_hosted_llvm_truth_payload_requires_object_emission() -> None:
    summary = _capable_llvm_summary()
    summary["llc_features"] = {"supports_filetype_obj": False}

    truth = hosted_llvm_capability_truth_from_summary(
        summary,
        source_kind=HOSTED_LLVM_CAPABILITY_TRUTH_SOURCE,
    )
    payload = truth.as_payload()

    assert payload["local_probe_diagnostic_only"] is False
    assert payload["hosted_execution_supported"] is False
    assert payload["hosted_source_parity_supported"] is False
    assert payload["fail_closed_without_live_capability"] is True
    assert "llc --filetype=obj support missing" in payload["failure_reasons"]


def test_hosted_llvm_truth_payload_publishes_success_fields() -> None:
    payload = hosted_llvm_capability_truth_from_summary(
        _capable_llvm_summary(),
        source_kind=HOSTED_LLVM_CAPABILITY_TRUTH_SOURCE,
    ).as_payload()

    assert payload["source_kind"] == HOSTED_LLVM_CAPABILITY_TRUTH_SOURCE
    assert payload["local_probe_diagnostic_only"] is False
    assert payload["hosted_execution_supported"] is True
    assert payload["hosted_source_parity_supported"] is True
    assert payload["fail_closed_without_live_capability"] is False
    assert payload["failure_reasons"] == []


def test_hosted_llvm_action_fails_closed_without_object_emission(monkeypatch) -> None:
    monkeypatch.setattr(
        developer_tooling_llvm_hosted,
        "run_hosted_llvm_probe",
        lambda: (
            1,
            {
                "mode": "objc3c-llvm-capabilities-v2",
                "ok": True,
                "clang": {"found": True},
                "llc": {"found": True},
                "llc_features": {"supports_filetype_obj": False},
            },
        ),
    )

    assert developer_tooling_llvm_hosted.action_check_hosted_llvm_capabilities([]) == 1


def test_capability_routed_parity_fails_closed_when_hosted_route_is_missing(
    monkeypatch,
) -> None:
    monkeypatch.setattr(
        developer_tooling_llvm_parity,
        "hosted_llc_object_emission_available",
        lambda: False,
    )

    assert developer_tooling_llvm_parity.action_test_capability_routed_source_parity([]) == 1


def test_hosted_summary_truth_requires_mode_ok_clang_and_object_emission(
    monkeypatch,
) -> None:
    monkeypatch.setattr(
        hosted_llvm_summary,
        "hosted_llvm_summary",
        lambda: {
            "mode": "objc3c-llvm-capabilities-v2",
            "ok": True,
            "clang": {"found": False},
            "llc": {"found": True},
            "llc_features": {"supports_filetype_obj": True},
        },
    )

    assert hosted_llvm_summary.hosted_llc_object_emission_available() is False
