"""Error-handling runtime acceptance domain."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import (
    NegativeDiagnosticExpectation,
    ROOT,
    compile_fixture_outputs,
    compile_fixture_outputs_with_args,
    compile_negative_diagnostic_batch,
)
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.domains.errors_lowering_cases import (
    check_cross_module_error_metadata_replay_preservation_case,
    check_error_lowering_unwind_bridge_helper_surface_case,
    check_executable_throw_catch_cleanup_lowering_case,
)
from objc3c_runtime_acceptance.domains.errors_runtime_cases import (
    check_error_runtime_abi_cleanup_case,
    check_live_error_runtime_integration_case,
)

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

_EXPORTED_CASE_NAMES = [
    "build_runtime_error_execution_cleanup_source_surface",
    "build_runtime_catch_filter_finalization_source_surface",
    "build_runtime_error_propagation_cleanup_semantics_surface",
    "build_runtime_bridging_filter_unwind_diagnostics_surface",
    "build_runtime_error_lowering_unwind_bridge_helper_surface",
    "build_runtime_error_runtime_abi_cleanup_surface",
    "build_runtime_error_propagation_catch_cleanup_runtime_implementation_surface",
    "check_error_execution_cleanup_source_case",
    "check_catch_filter_finalization_source_case",
    "check_error_propagation_cleanup_semantics_case",
    "check_executable_try_throw_do_catch_semantics_case",
    "check_bridging_filter_unwind_compatibility_diagnostics_case",
    "check_error_lowering_unwind_bridge_helper_surface_case",
    "check_executable_throw_catch_cleanup_lowering_case",
    "check_cross_module_error_metadata_replay_preservation_case",
    "check_error_runtime_abi_cleanup_case",
    "check_live_error_runtime_integration_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)

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


def check_error_execution_cleanup_source_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "error-execution-cleanup-source"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "error_source_closure_positive.objc3"
    )
    _, _, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    surface = (
        manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_error_source_closure", {})
    )
    expect(
        isinstance(surface, dict),
        "expected error source closure fixture to publish objc_error_handling_error_source_closure",
    )
    expected_fields = {
        "contract_id": "objc3c.error_handling.error.source.closure.v1",
        "frontend_surface_path": "frontend.pipeline.semantic_surface.objc_error_handling_error_source_closure",
        "throws_declaration_source_supported": True,
        "result_carrier_source_supported": True,
        "ns_error_bridging_source_supported": True,
        "error_bridge_marker_source_supported": True,
        "try_keyword_reserved": True,
        "throw_keyword_reserved": True,
        "catch_keyword_reserved": True,
        "try_fail_closed": True,
        "throw_fail_closed": True,
        "do_catch_fail_closed": True,
        "deterministic_handoff": True,
        "ready_for_semantic_expansion": True,
    }
    for field_name, expected_value in expected_fields.items():
        expect(
            surface.get(field_name) == expected_value,
            f"expected error source closure surface to preserve {field_name}",
        )
    expect(
        surface.get("function_throws_declaration_sites") == 1,
        "expected error source closure surface to publish one function throws declaration site",
    )
    expect(
        surface.get("result_like_sites") == 7
        and surface.get("result_success_sites") == 1
        and surface.get("result_failure_sites") == 2
        and surface.get("result_branch_sites") == 4
        and surface.get("result_payload_sites") == 3,
        "expected error source closure surface to preserve the result carrier source counts",
    )
    expect(
        surface.get("ns_error_bridging_sites") == 3
        and surface.get("ns_error_out_parameter_sites") == 1
        and surface.get("ns_error_bridge_path_sites") == 1,
        "expected error source closure surface to preserve the NSError bridge source counts",
    )
    return CaseResult(
        case_id="error-execution-cleanup-source",
        probe="compile-manifest-source-surface",
        fixture="tests/tooling/fixtures/native/error_source_closure_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "throws_declaration_sites": surface.get("function_throws_declaration_sites"),
            "result_like_sites": surface.get("result_like_sites"),
            "ns_error_bridging_sites": surface.get("ns_error_bridging_sites"),
            "ready_for_semantic_expansion": surface.get("ready_for_semantic_expansion"),
        },
    )


def check_catch_filter_finalization_source_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "catch-filter-finalization-source"
    try_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "try_do_catch_semantics_positive.objc3"
    )
    _, _, try_manifest_path = compile_fixture_outputs(try_fixture, case_dir / "try")
    try_manifest = json.loads(try_manifest_path.read_text(encoding="utf-8"))
    try_surface = (
        try_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_try_do_catch_semantics", {})
    )
    expect(
        isinstance(try_surface, dict),
        "expected try/do/catch fixture to publish objc_error_handling_try_do_catch_semantics",
    )
    expected_try_fields = {
        "contract_id": "objc3c.error_handling.try.throw.do.catch.semantics.v1",
        "dependency_contract_id": "objc3c.error_handling.error.semantic.model.v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_error_handling_try_do_catch_semantics",
        "try_surface_landed": True,
        "throw_surface_landed": True,
        "do_catch_surface_landed": True,
        "throwing_context_legality_enforced": True,
        "native_emit_remains_fail_closed": True,
        "deterministic": True,
        "ready_for_lowering_and_runtime": False,
    }
    for field_name, expected_value in expected_try_fields.items():
        expect(
            try_surface.get(field_name) == expected_value,
            f"expected try/do/catch source surface to preserve {field_name}",
        )
    expect(
        try_surface.get("try_expression_sites") == 3
        and try_surface.get("throw_statement_sites") == 1
        and try_surface.get("do_catch_sites") == 1
        and try_surface.get("catch_clause_sites") == 2
        and try_surface.get("catch_all_sites") == 1,
        "expected try/do/catch source surface to preserve the catch/finalization source counts",
    )

    bridge_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "bridge_legality_positive.objc3"
    )
    _, _, bridge_manifest_path = compile_fixture_outputs(bridge_fixture, case_dir / "bridge")
    bridge_manifest = json.loads(bridge_manifest_path.read_text(encoding="utf-8"))
    bridge_surface = (
        bridge_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_error_bridge_legality", {})
    )
    expect(
        isinstance(bridge_surface, dict),
        "expected bridge legality fixture to publish objc_error_handling_error_bridge_legality",
    )
    expected_bridge_fields = {
        "contract_id": "objc3c.error_handling.error.bridge.legality.v1",
        "dependency_contract_id": "objc3c.error_handling.try.throw.do.catch.semantics.v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_error_handling_error_bridge_legality",
        "bridge_legality_landed": True,
        "try_bridge_filter_landed": True,
        "unsupported_combinations_fail_closed": True,
        "native_emit_remains_fail_closed": True,
        "deterministic": True,
        "ready_for_lowering_and_runtime": False,
    }
    for field_name, expected_value in expected_bridge_fields.items():
        expect(
            bridge_surface.get(field_name) == expected_value,
            f"expected bridge legality source surface to preserve {field_name}",
        )
    expect(
        bridge_surface.get("bridge_callable_sites") == 2
        and bridge_surface.get("semantically_valid_bridge_callable_sites") == 2
        and bridge_surface.get("try_eligible_bridge_callable_sites") == 2
        and bridge_surface.get("unsupported_combination_sites") == 0
        and bridge_surface.get("throws_bridge_conflict_sites") == 0,
        "expected bridge legality source surface to preserve the catch-filter eligibility counts",
    )

    return CaseResult(
        case_id="catch-filter-finalization-source",
        probe="compile-manifest-source-surface",
        fixture="tests/tooling/fixtures/native/try_do_catch_semantics_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "try_expression_sites": try_surface.get("try_expression_sites"),
            "catch_clause_sites": try_surface.get("catch_clause_sites"),
            "bridge_callable_sites": bridge_surface.get("bridge_callable_sites"),
            "try_eligible_bridge_callable_sites": bridge_surface.get(
                "try_eligible_bridge_callable_sites"
            ),
        },
    )


def check_error_propagation_cleanup_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "error-propagation-cleanup-semantics"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "error_source_closure_positive.objc3"
    )
    _, _, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    surface = (
        manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_error_semantic_model", {})
    )
    expect(
        isinstance(surface, dict),
        "expected error semantic model fixture to publish objc_error_handling_error_semantic_model",
    )
    expected_fields = {
        "contract_id": "objc3c.error_handling.error.semantic.model.v1",
        "frontend_dependency_contract_id": "objc3c.error_handling.error.source.closure.v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_error_handling_error_semantic_model",
        "throws_declaration_semantics_landed": True,
        "result_carrier_profile_semantics_landed": True,
        "ns_error_bridging_profile_semantics_landed": True,
        "bridge_marker_semantics_landed": True,
        "parser_fail_closed_boundary_required": True,
        "parser_fail_closed_boundary_preserved": True,
        "propagation_runtime_deferred": True,
        "status_to_error_runtime_deferred": True,
        "native_error_abi_deferred": True,
        "placeholder_throws_summary_carried": True,
        "deterministic": True,
        "ready_for_lowering_and_runtime": False,
    }
    for field_name, expected_value in expected_fields.items():
        expect(
            surface.get(field_name) == expected_value,
            f"expected error semantic model to preserve {field_name}",
        )
    expect(
        surface.get("throws_declaration_sites") == 1
        and surface.get("result_like_sites") == 7
        and surface.get("ns_error_bridging_sites") == 3
        and surface.get("placeholder_throws_propagation_sites") == 0
        and surface.get("placeholder_unwind_cleanup_sites") == 0,
        "expected error semantic model to preserve propagation and cleanup counts",
    )

    return CaseResult(
        case_id="error-propagation-cleanup-semantics",
        probe="compile-manifest-semantic-surface",
        fixture="tests/tooling/fixtures/native/error_source_closure_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "throws_declaration_sites": surface.get("throws_declaration_sites"),
            "result_like_sites": surface.get("result_like_sites"),
            "ns_error_bridging_sites": surface.get("ns_error_bridging_sites"),
            "propagation_runtime_deferred": surface.get("propagation_runtime_deferred"),
        },
    )


def check_executable_try_throw_do_catch_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "executable-try-throw-do-catch-semantics"
    positive_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "try_do_catch_semantics_positive.objc3"
    )
    _, _, manifest_path = compile_fixture_outputs(positive_fixture, case_dir / "positive")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    surface = (
        manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_try_do_catch_semantics", {})
    )
    expect(
        isinstance(surface, dict),
        "expected try/do/catch positive fixture to publish objc_error_handling_try_do_catch_semantics",
    )
    expected_fields = {
        "contract_id": "objc3c.error_handling.try.throw.do.catch.semantics.v1",
        "dependency_contract_id": "objc3c.error_handling.error.semantic.model.v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_error_handling_try_do_catch_semantics",
        "try_surface_landed": True,
        "throw_surface_landed": True,
        "do_catch_surface_landed": True,
        "throwing_context_legality_enforced": True,
        "native_emit_remains_fail_closed": True,
        "deterministic": True,
        "ready_for_lowering_and_runtime": False,
    }
    for field_name, expected_value in expected_fields.items():
        expect(
            surface.get(field_name) == expected_value,
            f"expected executable try/do/catch semantics to preserve {field_name}",
        )
    expect(
        surface.get("try_expression_sites") == 3
        and surface.get("try_propagating_sites") == 1
        and surface.get("try_optional_sites") == 1
        and surface.get("try_forced_sites") == 1
        and surface.get("throw_statement_sites") == 1
        and surface.get("do_catch_sites") == 1
        and surface.get("catch_clause_sites") == 2
        and surface.get("catch_binding_sites") == 1
        and surface.get("catch_all_sites") == 1
        and surface.get("throwing_callable_try_sites") == 2
        and surface.get("bridged_callable_try_sites") == 1
        and surface.get("caller_propagation_sites") == 1
        and surface.get("local_handler_sites") == 0
        and surface.get("rethrow_sites") == 0
        and surface.get("contract_violation_sites") == 0,
        "expected executable try/do/catch semantics to preserve the positive semantic counts",
    )

    negatives = [
        (
            "try_requires_throwing_context_negative.objc3",
            ["propagating try requires a throws function or an enclosing do/catch"],
            ["O3S272"],
        ),
        (
            "try_requires_throwing_or_bridged_operand_negative.objc3",
            ["try operand must be a throwing or NSError-bridged call surface"],
            ["O3S271"],
        ),
        (
            "throw_requires_throws_or_catch_negative.objc3",
            ["throw statements require a throws function or a catch body"],
            ["O3S274"],
        ),
        (
            "catch_after_catch_all_negative.objc3",
            ["catch clauses after a catch-all are unreachable"],
            ["O3S269"],
        ),
    ]
    negative_batch = compile_negative_diagnostic_batch(
        case_id="executable-try-throw-do-catch-semantics",
        out_dir=case_dir / "negative-diagnostics-batch",
        expectations=[
            NegativeDiagnosticExpectation(
                key=Path(fixture_name).stem,
                fixture=ROOT / "tests" / "tooling" / "fixtures" / "native" / fixture_name,
                expected_snippets=expected_snippets,
                expected_codes=expected_codes,
            )
            for fixture_name, expected_snippets, expected_codes in negatives
        ],
    )
    negative_summaries = [
        {
            "fixture": entry["fixture"],
            "diagnostic_codes": entry["diagnostic_codes"],
            "duration_seconds": entry["duration_seconds"],
        }
        for entry in negative_batch["results"]
    ]

    native_fail_closed_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "try_do_catch_native_fail_closed.objc3"
    )
    _, _, native_manifest_path = compile_fixture_outputs(
        native_fail_closed_fixture, case_dir / "native-fail-closed"
    )
    native_manifest = json.loads(native_manifest_path.read_text(encoding="utf-8"))
    native_surface = (
        native_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_try_do_catch_semantics", {})
    )
    expect(
        isinstance(native_surface, dict),
        "expected native fail-closed fixture to publish objc_error_handling_try_do_catch_semantics",
    )
    expect(
        native_surface.get("native_emit_remains_fail_closed") is True
        and native_surface.get("try_expression_sites") == 1
        and native_surface.get("do_catch_sites") == 1
        and native_surface.get("bridged_callable_try_sites") == 1,
        "expected native fail-closed fixture to preserve the semantic fail-closed lowering boundary",
    )

    return CaseResult(
        case_id="executable-try-throw-do-catch-semantics",
        probe="compile-manifest-semantic-surface-plus-fail-closed-diagnostics",
        fixture="tests/tooling/fixtures/native/try_do_catch_semantics_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "try_expression_sites": surface.get("try_expression_sites"),
            "catch_clause_sites": surface.get("catch_clause_sites"),
            "bridged_callable_try_sites": surface.get("bridged_callable_try_sites"),
            "native_fail_closed_fixture": {
                "fixture": str(native_fail_closed_fixture.relative_to(ROOT)).replace("\\", "/"),
                "native_emit_remains_fail_closed": native_surface.get(
                    "native_emit_remains_fail_closed"
                ),
            },
            "negative_fixtures": negative_summaries,
            "negative_diagnostics_batch": negative_batch,
        },
    )


def check_bridging_filter_unwind_compatibility_diagnostics_case(
    run_dir: Path,
) -> CaseResult:
    case_dir = run_dir / "bridging-filter-unwind-compatibility-diagnostics"
    positive_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "bridge_legality_positive.objc3"
    )
    _, _, manifest_path = compile_fixture_outputs(positive_fixture, case_dir / "positive")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    surface = (
        manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_error_bridge_legality", {})
    )
    expect(
        isinstance(surface, dict),
        "expected bridge legality positive fixture to publish objc_error_handling_error_bridge_legality",
    )
    expected_fields = {
        "contract_id": "objc3c.error_handling.error.bridge.legality.v1",
        "dependency_contract_id": "objc3c.error_handling.try.throw.do.catch.semantics.v1",
        "surface_path": "frontend.pipeline.semantic_surface.objc_error_handling_error_bridge_legality",
        "bridge_legality_landed": True,
        "try_bridge_filter_landed": True,
        "unsupported_combinations_fail_closed": True,
        "native_emit_remains_fail_closed": True,
        "deterministic": True,
        "ready_for_lowering_and_runtime": False,
    }
    for field_name, expected_value in expected_fields.items():
        expect(
            surface.get(field_name) == expected_value,
            f"expected bridge legality diagnostics to preserve {field_name}",
        )
    expect(
        surface.get("bridge_callable_sites") == 2
        and surface.get("objc_nserror_callable_sites") == 1
        and surface.get("objc_status_code_callable_sites") == 1
        and surface.get("semantically_valid_bridge_callable_sites") == 2
        and surface.get("try_eligible_bridge_callable_sites") == 2
        and surface.get("unsupported_combination_sites") == 0
        and surface.get("contract_violation_sites") == 0,
        "expected bridge legality diagnostics to preserve the positive bridge counts",
    )

    native_fail_closed_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "bridge_legality_native_fail_closed.objc3"
    )
    _, _, native_manifest_path = compile_fixture_outputs(
        native_fail_closed_fixture, case_dir / "native-fail-closed"
    )
    native_manifest = json.loads(native_manifest_path.read_text(encoding="utf-8"))
    native_surface = (
        native_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_error_handling_error_bridge_legality", {})
    )
    expect(
        isinstance(native_surface, dict),
        "expected bridge native fail-closed fixture to publish objc_error_handling_error_bridge_legality",
    )
    expect(
        native_surface.get("native_emit_remains_fail_closed") is True
        and native_surface.get("bridge_callable_sites") == 1
        and native_surface.get("objc_status_code_callable_sites") == 1
        and native_surface.get("try_eligible_bridge_callable_sites") == 1,
        "expected bridge native fail-closed fixture to preserve the lowered bridge boundary",
    )

    negatives = [
        (
            "bridge_legality_nserror_missing_out_negative.objc3",
            ["objc_nserror requires an NSError out parameter"],
            ["O3S275"],
        ),
        (
            "bridge_legality_nserror_bad_return_negative.objc3",
            ["objc_nserror currently requires a BOOL-like success return"],
            ["O3S276"],
        ),
        (
            "bridge_legality_throws_conflict_negative.objc3",
            ["NSError/status bridge markers cannot currently be combined with throws"],
            ["O3S277"],
        ),
        (
            "bridge_legality_marker_conflict_negative.objc3",
            ["objc_nserror and objc_status_code cannot appear on the same callable"],
            ["O3S278"],
        ),
        (
            "bridge_legality_bad_error_type_negative.objc3",
            ["objc_status_code currently requires error_type: NSError"],
            ["O3S280"],
        ),
        (
            "bridge_legality_missing_mapping_negative.objc3",
            ["objc_status_code mapping symbol must resolve to a declared function"],
            ["O3S282"],
        ),
        (
            "bridge_legality_bad_mapping_signature_negative.objc3",
            ["objc_status_code mapping function must accept one matching status parameter and return NSError"],
            ["O3S283"],
        ),
        (
            "bridge_legality_bad_status_return_negative.objc3",
            ["objc_status_code requires a BOOL-like or integer status return"],
            ["O3S281"],
        ),
    ]
    negative_batch = compile_negative_diagnostic_batch(
        case_id="bridging-filter-unwind-compatibility-diagnostics",
        out_dir=case_dir / "negative-diagnostics-batch",
        expectations=[
            NegativeDiagnosticExpectation(
                key=Path(fixture_name).stem,
                fixture=ROOT / "tests" / "tooling" / "fixtures" / "native" / fixture_name,
                expected_snippets=expected_snippets,
                expected_codes=expected_codes,
            )
            for fixture_name, expected_snippets, expected_codes in negatives
        ],
    )
    negative_summaries = [
        {
            "fixture": entry["fixture"],
            "diagnostic_codes": entry["diagnostic_codes"],
            "duration_seconds": entry["duration_seconds"],
        }
        for entry in negative_batch["results"]
    ]

    return CaseResult(
        case_id="bridging-filter-unwind-compatibility-diagnostics",
        probe="compile-manifest-semantic-surface-plus-compatibility-diagnostics",
        fixture="tests/tooling/fixtures/native/bridge_legality_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "bridge_callable_sites": surface.get("bridge_callable_sites"),
            "try_eligible_bridge_callable_sites": surface.get(
                "try_eligible_bridge_callable_sites"
            ),
            "native_fail_closed_fixture": {
                "fixture": str(native_fail_closed_fixture.relative_to(ROOT)).replace("\\", "/"),
                "native_emit_remains_fail_closed": native_surface.get(
                    "native_emit_remains_fail_closed"
                ),
            },
            "negative_fixtures": negative_summaries,
            "negative_diagnostics_batch": negative_batch,
        },
    )


__all__ = [
    "build_runtime_error_execution_cleanup_source_surface",
    "build_runtime_catch_filter_finalization_source_surface",
    "build_runtime_error_propagation_cleanup_semantics_surface",
    "build_runtime_bridging_filter_unwind_diagnostics_surface",
    "build_runtime_error_lowering_unwind_bridge_helper_surface",
    "build_runtime_error_runtime_abi_cleanup_surface",
    "build_runtime_error_propagation_catch_cleanup_runtime_implementation_surface",
    "check_error_execution_cleanup_source_case",
    "check_catch_filter_finalization_source_case",
    "check_error_propagation_cleanup_semantics_case",
    "check_executable_try_throw_do_catch_semantics_case",
    "check_bridging_filter_unwind_compatibility_diagnostics_case",
    "check_error_lowering_unwind_bridge_helper_surface_case",
    "check_executable_throw_catch_cleanup_lowering_case",
    "check_cross_module_error_metadata_replay_preservation_case",
    "check_error_runtime_abi_cleanup_case",
    "check_live_error_runtime_integration_case",
    "exported_case_names",
]
