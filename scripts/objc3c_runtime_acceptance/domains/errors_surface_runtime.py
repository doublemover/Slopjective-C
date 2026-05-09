"""Error runtime ABI and implementation surface builders."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.errors_surface_support import (
    authoritative_case_ids,
)

from ..c_api import (
    PRIVATE_ERROR_RUNTIME_ABI_BOUNDARY,
    PUBLIC_RUNTIME_ABI_BOUNDARY,
    RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
    RUNTIME_PUBLIC_HEADER_PATH,
)
from ..runtime_contract_errors import (
    RUNTIME_ERROR_LOWERING_UNWIND_BRIDGE_HELPER_SURFACE_CONTRACT_ID,
    RUNTIME_ERROR_PROPAGATION_CATCH_CLEANUP_RUNTIME_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_ERROR_RUNTIME_ABI_CLEANUP_SURFACE_CONTRACT_ID,
)


def build_runtime_error_runtime_abi_cleanup_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    return {
        "contract_id": RUNTIME_ERROR_RUNTIME_ABI_CLEANUP_SURFACE_CONTRACT_ID,
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "error_lowering_unwind_bridge_helper_surface_contract_id": (
            RUNTIME_ERROR_LOWERING_UNWIND_BRIDGE_HELPER_SURFACE_CONTRACT_ID
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_error_runtime_abi_boundary": PRIVATE_ERROR_RUNTIME_ABI_BOUNDARY,
        "error_store_symbol": "objc3_runtime_store_thrown_error_i32",
        "error_load_symbol": "objc3_runtime_load_thrown_error_i32",
        "error_status_bridge_symbol": "objc3_runtime_bridge_status_error_i32",
        "error_nserror_bridge_symbol": "objc3_runtime_bridge_nserror_error_i32",
        "error_catch_match_symbol": "objc3_runtime_catch_matches_error_i32",
        "error_bridge_state_snapshot_symbol": (
            "objc3_runtime_copy_error_bridge_state_for_testing"
        ),
        "runtime_abi_boundary_model": (
            "private-runtime-abi-exposes-thrown-error-storage-status-bridge-"
            "nserror-bridge-and-catch-match-helpers-through-stable-testable-"
            "bootstrap-internal-entrypoints"
        ),
        "cleanup_runtime_model": (
            "lowered-throw-and-catch-paths-share-one-runtime-error-bridge-state-"
            "snapshot-surface-for-store-load-bridge-and-catch-match-observation"
        ),
        "fail_closed_model": (
            "public-header-surface-stays-unchanged-while-private-error-runtime-abi-"
            "remains-test-only-and-explicitly-versioned-through-the-runtime-probe"
        ),
        "authoritative_case_ids": authoritative_case_ids(
            results,
            {"error-runtime-abi-cleanup"},
        ),
        "authoritative_probe_path": (
            "tests/tooling/runtime/error_runtime_bridge_helper_probe.cpp"
        ),
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


def build_runtime_error_propagation_catch_cleanup_runtime_implementation_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    return {
        "contract_id": (
            RUNTIME_ERROR_PROPAGATION_CATCH_CLEANUP_RUNTIME_IMPLEMENTATION_SURFACE_CONTRACT_ID
        ),
        "error_runtime_abi_cleanup_surface_contract_id": (
            RUNTIME_ERROR_RUNTIME_ABI_CLEANUP_SURFACE_CONTRACT_ID
        ),
        "error_lowering_unwind_bridge_helper_surface_contract_id": (
            RUNTIME_ERROR_LOWERING_UNWIND_BRIDGE_HELPER_SURFACE_CONTRACT_ID
        ),
        "public_runtime_abi_boundary": PUBLIC_RUNTIME_ABI_BOUNDARY,
        "private_error_runtime_abi_boundary": PRIVATE_ERROR_RUNTIME_ABI_BOUNDARY,
        "authoritative_code_paths": [
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/live_error_runtime_integration_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/error_runtime_bridge_helper_probe.cpp",
            "tests/tooling/runtime/live_error_runtime_integration_probe.cpp",
        ],
        "runtime_implementation_model": (
            "lowered-throw-catch-and-status-bridge-paths-execute-through-the-live-"
            "error-runtime-helpers-and-publish-observable-bridge-state-snapshots"
        ),
        "fail_closed_model": (
            "runtime-integration-remains-private-and-testable-through-runtime-probes-"
            "until-a-public-error-abi-is-explicitly-claimed"
        ),
        "authoritative_case_ids": authoritative_case_ids(
            results,
            {"error-runtime-abi-cleanup", "live-error-runtime-integration"},
        ),
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


__all__ = [
    "build_runtime_error_propagation_catch_cleanup_runtime_implementation_surface",
    "build_runtime_error_runtime_abi_cleanup_surface",
]
