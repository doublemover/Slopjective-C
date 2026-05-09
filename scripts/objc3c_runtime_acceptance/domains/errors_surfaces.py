"""Error-handling runtime acceptance surface builders."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult

from ..core import (
    PRIVATE_ERROR_RUNTIME_ABI_BOUNDARY,
    PUBLIC_RUNTIME_ABI_BOUNDARY,
    RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
    RUNTIME_BRIDGING_FILTER_UNWIND_DIAGNOSTICS_SURFACE_CONTRACT_ID,
    RUNTIME_CATCH_FILTER_FINALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_ERROR_EXECUTION_CLEANUP_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_ERROR_LOWERING_UNWIND_BRIDGE_HELPER_SURFACE_CONTRACT_ID,
    RUNTIME_ERROR_PROPAGATION_CATCH_CLEANUP_RUNTIME_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_ERROR_PROPAGATION_CLEANUP_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_ERROR_RUNTIME_ABI_CLEANUP_SURFACE_CONTRACT_ID,
    RUNTIME_PUBLIC_HEADER_PATH,
)


def build_runtime_error_execution_cleanup_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id == "error-execution-cleanup-source"
    ]
    return {
        "contract_id": RUNTIME_ERROR_EXECUTION_CLEANUP_SOURCE_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "source_contract_ids": [
            "objc3c.error_handling.error.source.closure.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/parse/objc3_parser.cpp",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/error_source_closure_positive.objc3",
            "tests/tooling/fixtures/native/try_expression_fail_closed_negative.objc3",
            "tests/tooling/fixtures/native/throw_statement_fail_closed_negative.objc3",
            "tests/tooling/fixtures/native/do_catch_fail_closed_negative.objc3",
        ],
        "explicit_non_goals": [
            "no-milestone-specific-scaffolding",
            "no-sidecar-only-proof",
            "no-public-runtime-abi-widening",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": False,
    }


def build_runtime_catch_filter_finalization_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id == "catch-filter-finalization-source"
    ]
    return {
        "contract_id": RUNTIME_CATCH_FILTER_FINALIZATION_SOURCE_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "source_contract_ids": [
            "objc3c.error_handling.try.throw.do.catch.semantics.v1",
            "objc3c.error_handling.error.bridge.legality.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/parse/objc3_parser.cpp",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/try_do_catch_semantics_positive.objc3",
            "tests/tooling/fixtures/native/bridge_legality_positive.objc3",
            "tests/tooling/fixtures/native/try_requires_throwing_context_negative.objc3",
            "tests/tooling/fixtures/native/throw_requires_throws_or_catch_negative.objc3",
            "tests/tooling/fixtures/native/catch_after_catch_all_negative.objc3",
            "tests/tooling/fixtures/native/bridge_legality_throws_conflict_negative.objc3",
        ],
        "explicit_non_goals": [
            "no-milestone-specific-scaffolding",
            "no-sidecar-only-proof",
            "no-lowering-or-runtime-abi-claims",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": False,
    }


def build_runtime_error_propagation_cleanup_semantics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id == "error-propagation-cleanup-semantics"
    ]
    return {
        "contract_id": RUNTIME_ERROR_PROPAGATION_CLEANUP_SEMANTICS_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "source_contract_ids": [
            "objc3c.error_handling.error.semantic.model.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/sema/objc3_sema_contract.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/sema/objc3_semantic_passes.h",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/error_source_closure_positive.objc3",
            "tests/tooling/fixtures/native/error_bridge_marker_surface_positive.objc3",
            "tests/tooling/fixtures/native/status_code_attribute_missing_mapping_negative.objc3",
        ],
        "explicit_non_goals": [
            "no-milestone-specific-scaffolding",
            "no-sidecar-only-proof",
            "no-runtime-abi-claims-before-lane-d",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": False,
    }


def build_runtime_bridging_filter_unwind_diagnostics_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id == "bridging-filter-unwind-compatibility-diagnostics"
    ]
    return {
        "contract_id": RUNTIME_BRIDGING_FILTER_UNWIND_DIAGNOSTICS_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "source_contract_ids": [
            "objc3c.error_handling.error.bridge.legality.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/sema/objc3_sema_contract.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/sema/objc3_semantic_passes.h",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/bridge_legality_positive.objc3",
            "tests/tooling/fixtures/native/bridge_legality_native_fail_closed.objc3",
            "tests/tooling/fixtures/native/bridge_legality_nserror_missing_out_negative.objc3",
            "tests/tooling/fixtures/native/bridge_legality_nserror_bad_return_negative.objc3",
            "tests/tooling/fixtures/native/bridge_legality_throws_conflict_negative.objc3",
            "tests/tooling/fixtures/native/bridge_legality_marker_conflict_negative.objc3",
            "tests/tooling/fixtures/native/bridge_legality_bad_error_type_negative.objc3",
            "tests/tooling/fixtures/native/bridge_legality_missing_mapping_negative.objc3",
            "tests/tooling/fixtures/native/bridge_legality_bad_mapping_signature_negative.objc3",
            "tests/tooling/fixtures/native/bridge_legality_bad_status_return_negative.objc3",
        ],
        "explicit_non_goals": [
            "no-milestone-specific-scaffolding",
            "no-sidecar-only-proof",
            "no-runtime-abi-claims-before-lane-d",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": False,
    }


def build_runtime_error_lowering_unwind_bridge_helper_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id == "error-lowering-unwind-bridge-helper-surface"
    ]
    return {
        "contract_id": RUNTIME_ERROR_LOWERING_UNWIND_BRIDGE_HELPER_SURFACE_CONTRACT_ID,
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "source_contract_ids": [
            "objc3c.ns.error.bridging.lowering.v1",
            "objc3c.unwind.cleanup.lowering.v1",
            "objc3c.error_handling.throws.abi.propagation.lowering.v1",
            "objc3c.error_handling.result.and.bridging.artifact.replay.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/lower/objc3_lowering_contract.h",
            "native/objc3c/src/ir/objc3_ir_emitter.h",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/error_out_abi_positive.objc3",
            "tests/tooling/fixtures/native/error_runtime_bridge_helper_positive.objc3",
            "tests/tooling/runtime/error_runtime_bridge_helper_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-milestone-specific-scaffolding",
            "no-sidecar-only-proof",
            "no-runtime-helper-abi-claims-before-lane-d",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": False,
    }


def build_runtime_error_runtime_abi_cleanup_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id in {"error-runtime-abi-cleanup"}
    ]
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
        "authoritative_case_ids": authoritative_case_ids,
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
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {"error-runtime-abi-cleanup", "live-error-runtime-integration"}
    ]
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
        "authoritative_case_ids": authoritative_case_ids,
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }
