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
    LLVM_TOOL_CAPABILITY_OWNER_CONTRACTS,
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
