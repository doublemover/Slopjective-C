from scripts.objc3c_runtime_acceptance.domains.errors_semantic_propagation_contract import (
    ERROR_PROPAGATION_CLEANUP_OWNER_CONTRACT_ID,
    ERROR_PROPAGATION_CLEANUP_SEMANTIC_CONTRACT,
    ERROR_PROPAGATION_CLEANUP_SURFACE_PATH,
    error_propagation_cleanup_semantic_contract,
    expected_error_propagation_cleanup_counts,
    expected_error_propagation_cleanup_fields,
)

EXPECTED_RUNTIME_DEFERRED_FIELDS = [
    "propagation_runtime_deferred",
    "status_to_error_runtime_deferred",
    "native_error_abi_deferred",
]
EXPECTED_ZERO_RUNTIME_PLACEHOLDER_FIELDS = [
    "placeholder_throws_propagation_sites",
    "placeholder_unwind_cleanup_sites",
]
EXPECTED_ERROR_SEMANTIC_CONTRACT_ID = "objc3c.error_handling.error.semantic.model.v1"
EXPECTED_ERROR_SOURCE_CONTRACT_ID = "objc3c.error_handling.error.source.closure.v1"


def cleanup_semantic_payload() -> dict:
    return error_propagation_cleanup_semantic_contract()


def cleanup_expected_counts() -> dict:
    return expected_error_propagation_cleanup_counts()


def cleanup_expected_fields() -> dict:
    return expected_error_propagation_cleanup_fields()
