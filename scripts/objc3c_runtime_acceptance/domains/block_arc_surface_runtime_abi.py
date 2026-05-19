"""Block/ARC runtime ABI surface builder."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.block_arc_surface_support import (
    authoritative_case_ids,
)

from ..c_api import (
    BLOCK_ARC_RUNTIME_ABI_BOUNDARY_MODEL,
    BLOCK_ARC_RUNTIME_ARC_MODEL,
    BLOCK_ARC_RUNTIME_BLOCK_MODEL,
    BLOCK_ARC_RUNTIME_DESCRIPTOR_MODEL,
    BLOCK_ARC_RUNTIME_FAIL_CLOSED_MODEL,
    BLOCK_ARC_RUNTIME_INVOKE_THUNK_MODEL,
    PRIVATE_BLOCK_ARC_RUNTIME_ABI_BOUNDARY,
    PUBLIC_RUNTIME_ABI_BOUNDARY,
    RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
    RUNTIME_PUBLIC_HEADER_PATH,
)
from ..runtime_contract_block_arc import (
    BLOCK_ARC_RUNTIME_ABI_PROBE,
    RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID,
    RUNTIME_BLOCK_ARC_RUNTIME_ABI_SURFACE_CONTRACT_ID,
    RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID,
)


def build_runtime_block_arc_runtime_abi_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    return {
        "contract_id": RUNTIME_BLOCK_ARC_RUNTIME_ABI_SURFACE_CONTRACT_ID,
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "block_arc_unified_source_surface_contract_id": (
            RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID
        ),
        "block_arc_lowering_helper_surface_contract_id": (
            RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_block_arc_runtime_abi_boundary": PRIVATE_BLOCK_ARC_RUNTIME_ABI_BOUNDARY,
        "block_arc_runtime_abi_snapshot_symbol": (
            "objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing"
        ),
        "arc_debug_state_snapshot_symbol": (
            "objc3_runtime_copy_arc_debug_state_for_testing"
        ),
        "runtime_abi_boundary_model": BLOCK_ARC_RUNTIME_ABI_BOUNDARY_MODEL,
        "block_runtime_model": BLOCK_ARC_RUNTIME_BLOCK_MODEL,
        "block_descriptor_model": BLOCK_ARC_RUNTIME_DESCRIPTOR_MODEL,
        "block_invoke_thunk_model": BLOCK_ARC_RUNTIME_INVOKE_THUNK_MODEL,
        "arc_runtime_model": BLOCK_ARC_RUNTIME_ARC_MODEL,
        "fail_closed_model": BLOCK_ARC_RUNTIME_FAIL_CLOSED_MODEL,
        "authoritative_case_ids": authoritative_case_ids(
            results,
            {
                "block-arc-runtime-abi",
                "block-helper-runtime-execution",
                "arc-property-helper-abi",
            },
        ),
        "authoritative_probe_path": BLOCK_ARC_RUNTIME_ABI_PROBE,
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


__all__ = ["build_runtime_block_arc_runtime_abi_surface"]
