"""Error-handling source surface builders."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.errors_surface_support import (
    authoritative_case_ids,
)

from ..runtime_contract_errors import (
    RUNTIME_CATCH_FILTER_FINALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_ERROR_EXECUTION_CLEANUP_SOURCE_SURFACE_CONTRACT_ID,
)


def build_runtime_error_execution_cleanup_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
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
        "authoritative_case_ids": authoritative_case_ids(
            results,
            {"error-execution-cleanup-source"},
        ),
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
        "authoritative_case_ids": authoritative_case_ids(
            results,
            {"catch-filter-finalization-source"},
        ),
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/try_do_catch_semantics_positive.objc3",
            "tests/tooling/fixtures/native/throw_local_handler_positive.objc3",
            "tests/tooling/fixtures/native/bridge_legality_positive.objc3",
            "tests/tooling/fixtures/native/try_requires_throwing_context_negative.objc3",
            "tests/tooling/fixtures/native/throw_requires_throws_or_catch_negative.objc3",
            "tests/tooling/fixtures/native/throwing_call_requires_try_negative.objc3",
            "tests/tooling/fixtures/native/rethrow_requires_throws_or_local_handler_negative.objc3",
            "tests/tooling/fixtures/native/catch_body_return_type_negative.objc3",
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


__all__ = [
    "build_runtime_catch_filter_finalization_source_surface",
    "build_runtime_error_execution_cleanup_source_surface",
]
