from runtime_acceptance_error_semantic_contract_support import (
    ERROR_PROPAGATION_CLEANUP_SEMANTIC_CONTRACT,
    EXPECTED_ERROR_SEMANTIC_CONTRACT_ID,
    EXPECTED_ERROR_SOURCE_CONTRACT_ID,
    EXPECTED_ZERO_RUNTIME_PLACEHOLDER_FIELDS,
    cleanup_expected_fields,
)


def error_propagation_cleanup_contract_preserves_deferred_runtime_boundary() -> None:
    fields = cleanup_expected_fields()

    assert fields["contract_id"] == EXPECTED_ERROR_SEMANTIC_CONTRACT_ID
    assert fields["frontend_dependency_contract_id"] == (
        EXPECTED_ERROR_SOURCE_CONTRACT_ID
    )
    assert fields["ready_for_lowering_and_runtime"] is False
    assert fields["propagation_runtime_deferred"] is True
    assert fields["status_to_error_runtime_deferred"] is True
    assert fields["native_error_abi_deferred"] is True
    assert (
        ERROR_PROPAGATION_CLEANUP_SEMANTIC_CONTRACT.zero_runtime_placeholder_fields
        == tuple(EXPECTED_ZERO_RUNTIME_PLACEHOLDER_FIELDS)
    )
