"""Owner contract for error propagation cleanup semantic acceptance."""

from __future__ import annotations

from dataclasses import dataclass


ERROR_PROPAGATION_CLEANUP_CONTRACT_ID = (
    "objc3c.error_handling.error.semantic.model.v1"
)
ERROR_PROPAGATION_CLEANUP_DEPENDENCY_CONTRACT_ID = (
    "objc3c.error_handling.error.source.closure.v1"
)
ERROR_PROPAGATION_CLEANUP_SURFACE_PATH = (
    "frontend.pipeline.semantic_surface.objc_error_handling_error_semantic_model"
)
ERROR_PROPAGATION_CLEANUP_OWNER_CONTRACT_ID = (
    "objc3c.runtime_acceptance.error_propagation_cleanup.semantic_contract.v1"
)


@dataclass(frozen=True)
class ErrorPropagationCleanupSemanticContract:
    owner_contract_id: str
    surface_path: str
    expected_fields: tuple[tuple[str, object], ...]
    expected_counts: tuple[tuple[str, int], ...]
    runtime_deferred_fields: tuple[str, ...]
    zero_runtime_placeholder_fields: tuple[str, ...]

    def expected_field_map(self) -> dict[str, object]:
        return dict(self.expected_fields)

    def expected_count_map(self) -> dict[str, int]:
        return dict(self.expected_counts)

    def payload(self) -> dict[str, object]:
        return {
            "owner_contract_id": self.owner_contract_id,
            "surface_path": self.surface_path,
            "expected_fields": self.expected_field_map(),
            "expected_counts": self.expected_count_map(),
            "runtime_deferred_fields": list(self.runtime_deferred_fields),
            "zero_runtime_placeholder_fields": list(
                self.zero_runtime_placeholder_fields
            ),
        }


ERROR_PROPAGATION_CLEANUP_SEMANTIC_CONTRACT = (
    ErrorPropagationCleanupSemanticContract(
        owner_contract_id=ERROR_PROPAGATION_CLEANUP_OWNER_CONTRACT_ID,
        surface_path=ERROR_PROPAGATION_CLEANUP_SURFACE_PATH,
        expected_fields=(
            ("contract_id", ERROR_PROPAGATION_CLEANUP_CONTRACT_ID),
            (
                "frontend_dependency_contract_id",
                ERROR_PROPAGATION_CLEANUP_DEPENDENCY_CONTRACT_ID,
            ),
            ("surface_path", ERROR_PROPAGATION_CLEANUP_SURFACE_PATH),
            ("throws_declaration_semantics_landed", True),
            ("result_carrier_profile_semantics_landed", True),
            ("ns_error_bridging_profile_semantics_landed", True),
            ("bridge_marker_semantics_landed", True),
            ("parser_fail_closed_boundary_required", True),
            ("parser_fail_closed_boundary_preserved", True),
            ("propagation_runtime_deferred", True),
            ("status_to_error_runtime_deferred", True),
            ("native_error_abi_deferred", True),
            ("placeholder_throws_summary_carried", True),
            ("deterministic", True),
            ("ready_for_lowering_and_runtime", False),
        ),
        expected_counts=(
            ("throws_declaration_sites", 1),
            ("result_like_sites", 7),
            ("ns_error_bridging_sites", 3),
            ("placeholder_throws_propagation_sites", 0),
            ("placeholder_unwind_cleanup_sites", 0),
        ),
        runtime_deferred_fields=(
            "propagation_runtime_deferred",
            "status_to_error_runtime_deferred",
            "native_error_abi_deferred",
        ),
        zero_runtime_placeholder_fields=(
            "placeholder_throws_propagation_sites",
            "placeholder_unwind_cleanup_sites",
        ),
    )
)


def error_propagation_cleanup_semantic_contract() -> dict[str, object]:
    return ERROR_PROPAGATION_CLEANUP_SEMANTIC_CONTRACT.payload()


def expected_error_propagation_cleanup_fields() -> dict[str, object]:
    return ERROR_PROPAGATION_CLEANUP_SEMANTIC_CONTRACT.expected_field_map()


def expected_error_propagation_cleanup_counts() -> dict[str, int]:
    return ERROR_PROPAGATION_CLEANUP_SEMANTIC_CONTRACT.expected_count_map()


__all__ = [
    "ERROR_PROPAGATION_CLEANUP_CONTRACT_ID",
    "ERROR_PROPAGATION_CLEANUP_DEPENDENCY_CONTRACT_ID",
    "ERROR_PROPAGATION_CLEANUP_OWNER_CONTRACT_ID",
    "ERROR_PROPAGATION_CLEANUP_SEMANTIC_CONTRACT",
    "ERROR_PROPAGATION_CLEANUP_SURFACE_PATH",
    "ErrorPropagationCleanupSemanticContract",
    "error_propagation_cleanup_semantic_contract",
    "expected_error_propagation_cleanup_counts",
    "expected_error_propagation_cleanup_fields",
]
