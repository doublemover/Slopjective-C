from __future__ import annotations

from scripts.objc3c_runtime_acceptance.domains.errors_semantic_propagation_contract import (
    ERROR_PROPAGATION_CLEANUP_OWNER_CONTRACT_ID,
    ERROR_PROPAGATION_CLEANUP_SEMANTIC_CONTRACT,
    ERROR_PROPAGATION_CLEANUP_SURFACE_PATH,
    error_propagation_cleanup_semantic_contract,
    expected_error_propagation_cleanup_counts,
    expected_error_propagation_cleanup_fields,
)


def test_error_propagation_cleanup_semantic_contract_is_owner_typed() -> None:
    payload = error_propagation_cleanup_semantic_contract()

    assert payload["owner_contract_id"] == ERROR_PROPAGATION_CLEANUP_OWNER_CONTRACT_ID
    assert payload["surface_path"] == ERROR_PROPAGATION_CLEANUP_SURFACE_PATH
    assert payload["expected_fields"] == expected_error_propagation_cleanup_fields()
    assert payload["expected_counts"] == expected_error_propagation_cleanup_counts()
    assert payload["runtime_deferred_fields"] == [
        "propagation_runtime_deferred",
        "status_to_error_runtime_deferred",
        "native_error_abi_deferred",
    ]
    assert payload["zero_runtime_placeholder_fields"] == [
        "placeholder_throws_propagation_sites",
        "placeholder_unwind_cleanup_sites",
    ]


def test_error_propagation_cleanup_contract_preserves_strict_counts() -> None:
    counts = expected_error_propagation_cleanup_counts()

    assert counts["throws_declaration_sites"] == 1
    assert counts["result_like_sites"] == 7
    assert counts["ns_error_bridging_sites"] == 3
    assert counts["placeholder_throws_propagation_sites"] == 0
    assert counts["placeholder_unwind_cleanup_sites"] == 0


def test_error_propagation_cleanup_contract_preserves_deferred_runtime_boundary() -> None:
    fields = expected_error_propagation_cleanup_fields()

    assert fields["contract_id"] == "objc3c.error_handling.error.semantic.model.v1"
    assert fields["frontend_dependency_contract_id"] == (
        "objc3c.error_handling.error.source.closure.v1"
    )
    assert fields["ready_for_lowering_and_runtime"] is False
    assert fields["propagation_runtime_deferred"] is True
    assert fields["status_to_error_runtime_deferred"] is True
    assert fields["native_error_abi_deferred"] is True
    assert (
        ERROR_PROPAGATION_CLEANUP_SEMANTIC_CONTRACT.zero_runtime_placeholder_fields
        == (
            "placeholder_throws_propagation_sites",
            "placeholder_unwind_cleanup_sites",
        )
    )
