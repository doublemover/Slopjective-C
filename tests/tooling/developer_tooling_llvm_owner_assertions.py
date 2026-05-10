from __future__ import annotations

from typing import Any


def owner_contract_projection(contract: Any) -> dict[str, object]:
    return {
        "owner": contract.owner,
        "proof_source": contract.proof_source,
        "claim_scope": contract.claim_scope,
        "requires_hosted_probe_summary": contract.requires_hosted_probe_summary,
        "fails_closed_without_live_capability": (
            contract.fails_closed_without_live_capability
        ),
        "unsupported_claims": list(contract.unsupported_claims),
    }


def fixture_contract_projection(contract: dict[str, Any]) -> dict[str, object]:
    return {
        "owner": contract["owner"],
        "proof_source": contract["proof_source"],
        "claim_scope": contract["claim_scope"],
        "requires_hosted_probe_summary": contract["requires_hosted_probe_summary"],
        "fails_closed_without_live_capability": contract[
            "fails_closed_without_live_capability"
        ],
        "unsupported_claims": contract["unsupported_claims"],
    }


def assert_local_diagnostic_truth_payload(payload: dict[str, Any]) -> None:
    assert payload["local_probe_diagnostic_only"] is True
    assert payload["hosted_execution_supported"] is False
    assert payload["hosted_source_parity_supported"] is False
    assert payload["fail_closed_without_live_capability"] is True
    assert "local LLVM probe summary is diagnostic-only" in payload["failure_reasons"]


def assert_object_emission_required_payload(payload: dict[str, Any]) -> None:
    assert payload["local_probe_diagnostic_only"] is False
    assert payload["hosted_execution_supported"] is False
    assert payload["hosted_source_parity_supported"] is False
    assert payload["fail_closed_without_live_capability"] is True
    assert "llc --filetype=obj support missing" in payload["failure_reasons"]


def assert_hosted_success_payload(
    payload: dict[str, Any],
    *,
    expected_source_kind: str,
) -> None:
    assert payload["source_kind"] == expected_source_kind
    assert payload["local_probe_diagnostic_only"] is False
    assert payload["hosted_execution_supported"] is True
    assert payload["hosted_source_parity_supported"] is True
    assert payload["fail_closed_without_live_capability"] is False
    assert payload["failure_reasons"] == []
