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
    assert payload["hosted_native_object_emission_supported"] is False
    assert payload["hosted_package_archive_supported"] is False
    assert payload["hosted_headers_libraries_supported"] is False
    assert payload["hosted_execution_supported"] is False
    assert payload["hosted_source_parity_supported"] is False
    assert payload["fail_closed_without_live_capability"] is True
    assert payload["native_object_emission_status"] == "native_object_emission_diagnostic_only"
    assert payload["hosted_runner_behavior"] == "fail-closed-no-native-object-success-claim"
    assert payload["conformance_minima_behavior"] == "fail-closed-before-cross-lane-runtime-proof"
    assert "local LLVM probe summary is diagnostic-only" in payload["failure_reasons"]


def assert_object_emission_required_payload(payload: dict[str, Any]) -> None:
    assert payload["local_probe_diagnostic_only"] is False
    assert payload["hosted_native_object_emission_supported"] is False
    assert payload["hosted_package_archive_supported"] is False
    assert payload["hosted_headers_libraries_supported"] is False
    assert payload["hosted_execution_supported"] is False
    assert payload["hosted_source_parity_supported"] is False
    assert payload["fail_closed_without_live_capability"] is True
    assert payload["native_object_emission_status"] == "native_object_emission_filetype_obj_unavailable"
    assert payload["hosted_runner_behavior"] == "fail-closed-no-native-object-success-claim"
    assert payload["conformance_minima_behavior"] == "fail-closed-before-cross-lane-runtime-proof"
    assert "llc --filetype=obj support missing" in payload["failure_reasons"]


def assert_hosted_success_payload(
    payload: dict[str, Any],
    *,
    expected_source_kind: str,
) -> None:
    assert payload["source_kind"] == expected_source_kind
    assert payload["local_probe_diagnostic_only"] is False
    assert payload["clangxx_found"] is True
    assert payload["llc_supports_target_object_emission"] is True
    assert payload["llvm_ar_found"] is True
    assert payload["llvm_config_found"] is True
    assert payload["headers_libraries_discovered"] is True
    assert payload["toolchain_identity_claimable"] is True
    assert payload["hosted_native_object_emission_supported"] is True
    assert payload["hosted_package_archive_supported"] is True
    assert payload["hosted_headers_libraries_supported"] is True
    assert payload["hosted_execution_supported"] is True
    assert payload["hosted_source_parity_supported"] is True
    assert payload["fail_closed_without_live_capability"] is False
    assert payload["native_object_emission_status"] == "native_object_emission_supported"
    assert payload["hosted_runner_behavior"] == "native-object-emission-supported"
    assert payload["conformance_minima_behavior"] == "native-object-emission-may-enter-conformance-minima"
    assert payload["failure_reasons"] == []
