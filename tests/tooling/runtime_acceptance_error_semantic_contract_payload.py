from runtime_acceptance_error_semantic_contract_support import (
    ERROR_PROPAGATION_CLEANUP_OWNER_CONTRACT_ID,
    ERROR_PROPAGATION_CLEANUP_SURFACE_PATH,
    EXPECTED_RUNTIME_DEFERRED_FIELDS,
    EXPECTED_ZERO_RUNTIME_PLACEHOLDER_FIELDS,
    cleanup_expected_counts,
    cleanup_expected_fields,
    cleanup_semantic_payload,
)


def error_propagation_cleanup_semantic_contract_is_owner_typed() -> None:
    payload = cleanup_semantic_payload()

    assert payload["owner_contract_id"] == ERROR_PROPAGATION_CLEANUP_OWNER_CONTRACT_ID
    assert payload["surface_path"] == ERROR_PROPAGATION_CLEANUP_SURFACE_PATH
    assert payload["expected_fields"] == cleanup_expected_fields()
    assert payload["expected_counts"] == cleanup_expected_counts()
    assert payload["runtime_deferred_fields"] == EXPECTED_RUNTIME_DEFERRED_FIELDS
    assert (
        payload["zero_runtime_placeholder_fields"]
        == EXPECTED_ZERO_RUNTIME_PLACEHOLDER_FIELDS
    )
