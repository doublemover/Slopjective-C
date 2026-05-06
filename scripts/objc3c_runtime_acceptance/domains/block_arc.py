"""Block ARC runtime acceptance domain."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.commands import run
from objc3c_runtime_acceptance.native_build import (
    NegativeDiagnosticExpectation,
    ROOT,
    compile_fixture,
    compile_fixture_outputs,
    compile_fixture_outputs_with_args,
    compile_fixture_with_args,
    compile_negative_diagnostic_batch,
    link_fixture_executable,
)
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

from ..core import (
    BLOCK_ARC_RUNTIME_ABI_BOUNDARY_MODEL,
    BLOCK_ARC_RUNTIME_ABI_PROBE,
    BLOCK_ARC_RUNTIME_ARC_MODEL,
    BLOCK_ARC_RUNTIME_BLOCK_MODEL,
    BLOCK_ARC_RUNTIME_FAIL_CLOSED_MODEL,
    PRIVATE_BLOCK_ARC_RUNTIME_ABI_BOUNDARY,
    PUBLIC_RUNTIME_ABI_BOUNDARY,
    RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID,
    RUNTIME_BLOCK_ARC_RUNTIME_ABI_SURFACE_CONTRACT_ID,
    RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
    RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_PUBLIC_HEADER_PATH,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_block_arc_unified_source_surface",
    "build_runtime_ownership_transfer_capture_family_source_surface",
    "build_runtime_block_arc_lowering_helper_surface",
    "build_runtime_block_arc_runtime_abi_surface",
    "check_escaping_block_capture_legality_case",
    "check_block_storage_arc_automation_semantics_case",
    "check_block_arc_runtime_abi_case",
    "check_block_helper_runtime_execution_case",
    "check_arc_property_helper_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)

def build_runtime_block_arc_unified_source_surface(results: list[CaseResult]) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "block-arc-runtime-abi",
            "escaping-block-capture-legality",
            "block-storage-arc-automation-semantics",
            "block-helper-runtime-execution",
            "arc-property-helper-abi",
        }
    ]
    return {
        "contract_id": RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID,
        "compile_manifest_artifact": "<emit-prefix>.manifest.json",
        "registration_manifest_artifact": "<emit-prefix>.runtime-registration-manifest.json",
        "registration_descriptor_artifact": "<emit-prefix>.runtime-registration-descriptor.json",
        "object_artifact": "<emit-prefix>.obj",
        "backend_artifact": "<emit-prefix>.ll",
        "source_surface_model": (
            "block-arc-unified-source-surface-freezes-live-frontend-sema-ir-and-runtime-entrypoints-before-generalized-ownership-automation-or-public-abi-widening"
        ),
        "source_contract_ids": [
            "objc3c.executable.block.source.closure.v1",
            "objc3c.executable.block.source.model.completion.v1",
            "objc3c.executable.block.source.storage.annotation.v1",
            "objc3c.executable.block.runtime.semantic.rules.v1",
            "objc3c.executable.block.capture.legality.escape.and.invocation.v1",
            "objc3c.executable.block.byref.copy.dispose.and.object.capture.ownership.v1",
            "objc3c.executable.block.object.and.invoke.thunk.lowering.v1",
            "objc3c.executable.block.byref.helper.lowering.v1",
            "objc3c.executable.block.escape.runtime.hook.lowering.v1",
            "objc3c.arc.source.mode.boundary.freeze.v1",
            "objc3c.arc.mode.handling.v1",
            "objc3c.arc.semantic.rules.v1",
            "objc3c.arc.inference.lifetime.v1",
            "objc3c.arc.interaction.semantics.v1",
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_source_fields": [
            "frontend.pipeline.semantic_surface.objc_arc_diagnostics_fixit_lowering_surface",
            "frontend.pipeline.semantic_surface.objc_block_literal_capture_lowering_surface",
            "frontend.pipeline.semantic_surface.objc_block_source_model_completion_surface",
            "frontend.pipeline.semantic_surface.objc_block_source_storage_annotation_surface",
            "frontend.pipeline.semantic_surface.objc_block_abi_invoke_trampoline_lowering_surface",
            "frontend.pipeline.semantic_surface.objc_block_storage_escape_lowering_surface",
            "frontend.pipeline.semantic_surface.objc_block_copy_dispose_lowering_surface",
            "llvm_ir_summary.executable_block_object_invoke_thunk_lowering",
            "llvm_ir_summary.executable_block_byref_helper_lowering",
            "llvm_ir_summary.executable_block_escape_runtime_hook_lowering",
            "llvm_ir_summary.runtime_block_api_object_layout",
            "llvm_ir_summary.runtime_block_allocation_copy_dispose_invoke_support",
            "llvm_ir_summary.runtime_block_byref_forwarding_heap_promotion_ownership_interop",
            "runtime_api.objc3_runtime_promote_block_i32",
            "runtime_api.objc3_runtime_invoke_block_i32",
            "runtime_api.objc3_runtime_copy_arc_debug_state_for_testing",
            "runtime_api.objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing",
        ],
        "block_runtime_boundary_model": (
            "source-only-sema-rejects-escaping-byref-and-owned-object-captures-before-runnable-block-ownership-lowering"
        ),
        "arc_runtime_boundary_model": (
            "weak-properties-and-nonowning-captures-stay-nonretaining-autorelease-returns-stay-profiled-and-synthesized-property-accessors-publish-owned-lifetime-packets-under-arc"
        ),
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/block_source_model_completion_positive.objc3",
            "tests/tooling/fixtures/native/block_source_storage_annotations_positive.objc3",
            "tests/tooling/fixtures/native/capture_legality_escape_invocation_bad_call.objc3",
            "tests/tooling/fixtures/native/capture_legality_escape_invocation_missing_capture.objc3",
            "tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3",
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_argument_positive.objc3",
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_return_positive.objc3",
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_byref_positive.objc3",
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_owned_capture_positive.objc3",
            "tests/tooling/fixtures/native/executable_block_object_invoke_thunk_positive.objc3",
            "tests/tooling/fixtures/native/arc_mode_handling_positive.objc3",
            "tests/tooling/fixtures/native/arc_inference_lifetime_positive.objc3",
            "tests/tooling/fixtures/native/arc_cleanup_scope_positive.objc3",
            "tests/tooling/fixtures/native/arc_implicit_cleanup_void_positive.objc3",
            "tests/tooling/fixtures/native/arc_autorelease_return_positive.objc3",
            "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/block_runtime_copy_dispose_invoke_probe.cpp",
            "tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp",
            "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
            BLOCK_ARC_RUNTIME_ABI_PROBE,
        ],
        "explicit_non_goals": [
            "no-public-block-object-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-sidecar-only-block-or-arc-proof",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }
def build_runtime_ownership_transfer_capture_family_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "escaping-block-capture-legality",
            "block-storage-arc-automation-semantics",
            "block-helper-runtime-execution",
            "arc-property-helper-abi",
        }
    ]
    return {
        "contract_id": (
            RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID
        ),
        "compile_manifest_artifact": "<emit-prefix>.manifest.json",
        "registration_manifest_artifact": "<emit-prefix>.runtime-registration-manifest.json",
        "registration_descriptor_artifact": "<emit-prefix>.runtime-registration-descriptor.json",
        "object_artifact": "<emit-prefix>.obj",
        "backend_artifact": "<emit-prefix>.ll",
        "block_arc_unified_source_surface_contract_id": (
            RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID
        ),
        "source_surface_model": (
            "ownership-transfer-and-capture-family-source-surface-freezes-sema-level-move-capture-explicit-capture-mode-and-retainable-family-truth-before-lowering-or-runtime-lifetime-expansion"
        ),
        "ownership_resource_move_use_after_move_surface_path": (
            "frontend.pipeline.semantic_surface.objc_ownership_resource_move_and_use_after_move_semantics"
        ),
        "ownership_capture_list_retainable_family_surface_path": (
            "frontend.pipeline.semantic_surface.objc_ownership_capture_list_and_retainable_family_legality_completion"
        ),
        "block_capture_ownership_contract_id": (
            "objc3c.executable.block.byref.copy.dispose.and.object.capture.ownership.v1"
        ),
        "arc_inference_lifetime_contract_id": "objc3c.arc.inference.lifetime.v1",
        "arc_interaction_semantics_contract_id": "objc3c.arc.interaction.semantics.v1",
        "block_capture_ownership_profile_field": (
            "Expr.block_runtime_capture_ownership_profile"
        ),
        "block_capture_owned_count_field": (
            "Expr.block_runtime_owned_object_capture_count"
        ),
        "block_capture_weak_count_field": (
            "Expr.block_runtime_weak_object_capture_count"
        ),
        "block_capture_unowned_count_field": (
            "Expr.block_runtime_unowned_object_capture_count"
        ),
        "cleanup_ownership_transfer_field": "cleanup_ownership_transfer_enforced",
        "explicit_capture_ownership_mode_field": (
            "explicit_capture_ownership_mode_enforced"
        ),
        "retainable_family_conflict_field": "retainable_family_conflict_enforced",
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/sema/objc3_semantic_passes.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
        ],
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/owned_object_capture_helper_positive.objc3",
            "tests/tooling/fixtures/native/owned_object_capture_runtime_positive.objc3",
            "tests/tooling/fixtures/native/nonowning_object_capture_helper_elided_positive.objc3",
            "tests/tooling/fixtures/native/nonowning_object_capture_runtime_positive.objc3",
            "tests/tooling/fixtures/native/weak_object_capture_mutation_negative.objc3",
            "tests/tooling/fixtures/native/unowned_object_capture_mutation_negative.objc3",
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_owned_capture_positive.objc3",
            "tests/tooling/fixtures/native/arc_inference_lifetime_positive.objc3",
            "tests/tooling/fixtures/native/arc_cleanup_scope_positive.objc3",
            "tests/tooling/fixtures/native/arc_implicit_cleanup_void_positive.objc3",
            "tests/tooling/fixtures/native/arc_block_autorelease_return_positive.objc3",
            "tests/tooling/fixtures/native/arc_autorelease_return_positive.objc3",
            "tests/tooling/fixtures/native/capture_list_and_retainable_family_legality_completion_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/block_runtime_copy_dispose_invoke_probe.cpp",
            "tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp",
            "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-parallel-semantics-path",
            "no-milestone-specific-scaffolding",
            "no-lowering-owned-reinterpretation-of-capture-family-truth",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }
def build_runtime_block_arc_lowering_helper_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "block-arc-runtime-abi",
            "escaping-block-capture-legality",
            "block-storage-arc-automation-semantics",
            "block-helper-runtime-execution",
        }
    ]
    return {
        "contract_id": RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID,
        "compile_manifest_artifact": "<emit-prefix>.manifest.json",
        "registration_manifest_artifact": "<emit-prefix>.runtime-registration-manifest.json",
        "registration_descriptor_artifact": "<emit-prefix>.runtime-registration-descriptor.json",
        "object_artifact": "<emit-prefix>.obj",
        "backend_artifact": "<emit-prefix>.ll",
        "block_arc_unified_source_surface_contract_id": (
            RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID
        ),
        "runtime_block_arc_runtime_abi_surface_contract_id": (
            RUNTIME_BLOCK_ARC_RUNTIME_ABI_SURFACE_CONTRACT_ID
        ),
        "ownership_transfer_capture_family_source_surface_contract_id": (
            RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID
        ),
        "block_object_invoke_thunk_lowering_contract_id": (
            "objc3c.executable.block.object.and.invoke.thunk.lowering.v1"
        ),
        "block_byref_helper_lowering_contract_id": (
            "objc3c.executable.block.byref.helper.lowering.v1"
        ),
        "block_escape_runtime_hook_lowering_contract_id": (
            "objc3c.executable.block.escape.runtime.hook.lowering.v1"
        ),
        "arc_mode_handling_contract_id": "objc3c.arc.mode.handling.v1",
        "arc_semantic_rules_contract_id": "objc3c.arc.semantic.rules.v1",
        "arc_inference_lifetime_contract_id": "objc3c.arc.inference.lifetime.v1",
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "lowering_helper_surface_model": (
            "block-arc-lowering-helper-surface-freezes-live-semantic-lowering-packets-manifest-replay-keys-llvm-helper-summaries-and-private-runtime-hooks-before-cross-module-or-public-abi-expansion"
        ),
        "semantic_surface_paths": [
            "frontend.pipeline.semantic_surface.objc_block_abi_invoke_trampoline_lowering_surface",
            "frontend.pipeline.semantic_surface.objc_block_storage_escape_lowering_surface",
            "frontend.pipeline.semantic_surface.objc_block_copy_dispose_lowering_surface",
            "frontend.pipeline.semantic_surface.objc_arc_diagnostics_fixit_lowering_surface",
        ],
        "manifest_lowering_paths": [
            "lowering_block_abi_invoke_trampoline",
            "lowering_block_storage_escape",
            "lowering_block_copy_dispose",
        ],
        "llvm_ir_summary_paths": [
            "llvm_ir_summary.executable_block_object_invoke_thunk_lowering",
            "llvm_ir_summary.executable_block_byref_helper_lowering",
            "llvm_ir_summary.executable_block_escape_runtime_hook_lowering",
            "llvm_ir_summary.arc_cleanup_weak_lifetime_hooks",
            "llvm_ir_summary.arc_block_autorelease_return_lowering",
        ],
        "runtime_api_paths": [
            "runtime_api.objc3_runtime_promote_block_i32",
            "runtime_api.objc3_runtime_invoke_block_i32",
            "runtime_api.objc3_runtime_copy_arc_debug_state_for_testing",
            "runtime_api.objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing",
        ],
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_code_paths": [
            "native/objc3c/src/ast/objc3_ast.h",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
            "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
            "native/objc3c/src/runtime/objc3_runtime.cpp",
        ],
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3",
            "tests/tooling/fixtures/native/owned_object_capture_runtime_positive.objc3",
            "tests/tooling/fixtures/native/nonowning_object_capture_runtime_positive.objc3",
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_argument_positive.objc3",
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_return_positive.objc3",
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_byref_positive.objc3",
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_owned_capture_positive.objc3",
            "tests/tooling/fixtures/native/executable_block_object_invoke_thunk_positive.objc3",
            "tests/tooling/fixtures/native/arc_mode_handling_positive.objc3",
            "tests/tooling/fixtures/native/arc_inference_lifetime_positive.objc3",
            "tests/tooling/fixtures/native/arc_cleanup_scope_positive.objc3",
            "tests/tooling/fixtures/native/arc_implicit_cleanup_void_positive.objc3",
            "tests/tooling/fixtures/native/arc_autorelease_return_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/block_runtime_copy_dispose_invoke_probe.cpp",
            "tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp",
            "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
            BLOCK_ARC_RUNTIME_ABI_PROBE,
        ],
        "explicit_non_goals": [
            "no-cross-module-packaging-claims",
            "no-public-block-abi-widening",
            "no-milestone-specific-scaffolding",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }
def build_runtime_block_arc_runtime_abi_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    authoritative_case_ids = [
        result.case_id
        for result in results
        if result.case_id
        in {
            "block-arc-runtime-abi",
            "block-helper-runtime-execution",
            "arc-property-helper-abi",
        }
    ]
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
        "arc_runtime_model": BLOCK_ARC_RUNTIME_ARC_MODEL,
        "fail_closed_model": BLOCK_ARC_RUNTIME_FAIL_CLOSED_MODEL,
        "authoritative_case_ids": authoritative_case_ids,
        "authoritative_probe_path": BLOCK_ARC_RUNTIME_ABI_PROBE,
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }
def check_escaping_block_capture_legality_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "escaping-block-capture-legality"

    argument_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "escaping_block_runtime_hook_argument_positive.objc3"
    )
    _, _, argument_manifest_path = compile_fixture_outputs(
        argument_fixture, case_dir / "argument-positive"
    )
    argument_manifest = json.loads(argument_manifest_path.read_text(encoding="utf-8"))
    argument_escape_surface = (
        argument_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_storage_escape_lowering_surface", {})
    )
    argument_copy_dispose_surface = (
        argument_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )

    return_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "escaping_block_runtime_hook_return_positive.objc3"
    )
    _, _, return_manifest_path = compile_fixture_outputs(
        return_fixture, case_dir / "return-positive"
    )
    return_manifest = json.loads(return_manifest_path.read_text(encoding="utf-8"))
    return_escape_surface = (
        return_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_storage_escape_lowering_surface", {})
    )
    return_copy_dispose_surface = (
        return_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )

    negative_batch = compile_negative_diagnostic_batch(
        case_id="escaping-block-capture-legality",
        out_dir=case_dir / "negative-diagnostics-batch",
        expectations=[
            NegativeDiagnosticExpectation(
                key="bad-call-negative",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "capture_legality_escape_invocation_bad_call.objc3",
                expected_snippets=[
                    "type mismatch: expected 'i32' argument for parameter 0 of callable 'closure', got 'bool'"
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="missing-capture-negative",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "capture_legality_escape_invocation_missing_capture.objc3",
                expected_snippets=["undefined capture 'seed' in block literal"],
                expected_codes=["O3S202"],
            ),
        ],
    )
    bad_call_negative = negative_batch["results"][0]
    missing_capture_negative = negative_batch["results"][1]
    byref_escape_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "escaping_block_runtime_hook_byref_positive.objc3"
    )
    _, _, byref_escape_manifest_path = compile_fixture_outputs(
        byref_escape_fixture, case_dir / "byref-escape-positive"
    )
    byref_escape_manifest = json.loads(byref_escape_manifest_path.read_text(encoding="utf-8"))
    byref_escape_surface = (
        byref_escape_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_storage_escape_lowering_surface", {})
    )
    byref_copy_dispose_surface = (
        byref_escape_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )

    owned_escape_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "escaping_block_runtime_hook_owned_capture_positive.objc3"
    )
    _, _, owned_escape_manifest_path = compile_fixture_outputs(
        owned_escape_fixture, case_dir / "owned-escape-positive"
    )
    owned_escape_manifest = json.loads(
        owned_escape_manifest_path.read_text(encoding="utf-8")
    )
    owned_escape_surface = (
        owned_escape_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_storage_escape_lowering_surface", {})
    )
    owned_escape_copy_dispose_surface = (
        owned_escape_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )

    expect(
        argument_escape_surface.get("escape_to_heap_sites") == 1
        and argument_escape_surface.get("requires_byref_cells_sites") == 0,
        "expected escaping argument fixture to publish one heap-promotion candidate without byref cells",
    )
    expect(
        argument_copy_dispose_surface.get("copy_helper_required_sites") == 0
        and argument_copy_dispose_surface.get("dispose_helper_required_sites") == 0,
        "expected escaping argument fixture to keep copy/dispose helpers elided",
    )
    expect(
        return_escape_surface.get("escape_to_heap_sites") == 1
        and return_escape_surface.get("requires_byref_cells_sites") == 0,
        "expected escaping return fixture to publish one heap-promotion candidate without byref cells",
    )
    expect(
        return_copy_dispose_surface.get("copy_helper_required_sites") == 0
        and return_copy_dispose_surface.get("dispose_helper_required_sites") == 0,
        "expected escaping return fixture to keep copy/dispose helpers elided",
    )
    expect(
        byref_escape_surface.get("escape_to_heap_sites") == 1
        and byref_escape_surface.get("requires_byref_cells_sites") == 1,
        "expected escaping byref fixture to publish one heap-promotion candidate with one byref-cell site",
    )
    expect(
        byref_copy_dispose_surface.get("copy_helper_required_sites") == 1
        and byref_copy_dispose_surface.get("dispose_helper_required_sites") == 1,
        "expected escaping byref fixture to require copy/dispose helpers",
    )
    expect(
        owned_escape_surface.get("escape_to_heap_sites") == 1
        and owned_escape_surface.get("requires_byref_cells_sites") == 0,
        "expected escaping owned-capture fixture to publish one heap-promotion candidate without byref cells",
    )
    expect(
        owned_escape_copy_dispose_surface.get("copy_helper_required_sites") == 1
        and owned_escape_copy_dispose_surface.get("dispose_helper_required_sites")
        == 1,
        "expected escaping owned-capture fixture to require copy/dispose helpers",
    )

    return CaseResult(
        case_id="escaping-block-capture-legality",
        probe="compile-manifest-diagnostics-and-llvm-ir",
        fixture="tests/tooling/fixtures/native/escaping_block_runtime_hook_argument_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "argument_escape_to_heap_sites": argument_escape_surface.get(
                "escape_to_heap_sites"
            ),
            "return_escape_to_heap_sites": return_escape_surface.get(
                "escape_to_heap_sites"
            ),
            "bad_call_diagnostic_count": bad_call_negative["diagnostic_count"],
            "missing_capture_diagnostic_count": missing_capture_negative[
                "diagnostic_count"
            ],
            "byref_escape_to_heap_sites": byref_escape_surface.get(
                "escape_to_heap_sites"
            ),
            "byref_copy_helper_required_sites": byref_copy_dispose_surface.get(
                "copy_helper_required_sites"
            ),
            "owned_escape_to_heap_sites": owned_escape_surface.get(
                "escape_to_heap_sites"
            ),
            "owned_copy_helper_required_sites": (
                owned_escape_copy_dispose_surface.get("copy_helper_required_sites")
            ),
            "negative_diagnostics_batch": negative_batch,
        },
    )
def check_block_storage_arc_automation_semantics_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / "block-storage-arc-automation-semantics"

    owned_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "owned_object_capture_helper_positive.objc3"
    )
    _, owned_ll_path, owned_manifest_path = compile_fixture_outputs(
        owned_fixture, case_dir / "owned-positive"
    )
    owned_manifest = json.loads(owned_manifest_path.read_text(encoding="utf-8"))
    owned_semantic_surface = (
        owned_manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {})
    )
    owned_copy_dispose_surface = owned_semantic_surface.get(
        "objc_block_copy_dispose_lowering_surface", {}
    )
    owned_escape_surface = owned_semantic_surface.get(
        "objc_block_storage_escape_lowering_surface", {}
    )
    owned_ll = owned_ll_path.read_text(encoding="utf-8")

    nonowning_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "nonowning_object_capture_helper_elided_positive.objc3"
    )
    _, nonowning_ll_path, nonowning_manifest_path = compile_fixture_outputs(
        nonowning_fixture, case_dir / "nonowning-positive"
    )
    nonowning_manifest = json.loads(
        nonowning_manifest_path.read_text(encoding="utf-8")
    )
    nonowning_semantic_surface = (
        nonowning_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
    )
    nonowning_copy_dispose_surface = nonowning_semantic_surface.get(
        "objc_block_copy_dispose_lowering_surface", {}
    )
    nonowning_arc_diagnostics_surface = nonowning_semantic_surface.get(
        "objc_arc_diagnostics_fixit_lowering_surface", {}
    )
    nonowning_ll = nonowning_ll_path.read_text(encoding="utf-8")

    arc_mode_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_mode_handling_positive.objc3"
    )
    compile_fixture_with_args(
        arc_mode_fixture, case_dir / "arc-mode-positive", extra_args=["-fobjc-arc"]
    )
    arc_mode_ll_path = case_dir / "arc-mode-positive" / "module.ll"
    arc_mode_manifest_path = case_dir / "arc-mode-positive" / "module.manifest.json"
    arc_mode_manifest = json.loads(arc_mode_manifest_path.read_text(encoding="utf-8"))
    arc_mode_ll = arc_mode_ll_path.read_text(encoding="utf-8")
    arc_mode_sema = arc_mode_manifest.get("frontend", {}).get("pipeline", {}).get(
        "sema_pass_manager", {}
    )
    arc_mode_block_copy_dispose_surface = (
        arc_mode_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )

    arc_inference_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_inference_lifetime_positive.objc3"
    )
    compile_fixture_with_args(
        arc_inference_fixture,
        case_dir / "arc-inference-positive",
        extra_args=["-fobjc-arc"],
    )
    arc_inference_ll_path = case_dir / "arc-inference-positive" / "module.ll"
    arc_inference_manifest_path = (
        case_dir / "arc-inference-positive" / "module.manifest.json"
    )
    arc_inference_manifest = json.loads(
        arc_inference_manifest_path.read_text(encoding="utf-8")
    )
    arc_inference_ll = arc_inference_ll_path.read_text(encoding="utf-8")
    arc_inference_sema = arc_inference_manifest.get("frontend", {}).get(
        "pipeline", {}
    ).get("sema_pass_manager", {})

    arc_cleanup_scope_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_cleanup_scope_positive.objc3"
    )
    compile_fixture_with_args(
        arc_cleanup_scope_fixture,
        case_dir / "arc-cleanup-scope-positive",
        extra_args=["-fobjc-arc"],
    )
    arc_cleanup_scope_manifest_path = (
        case_dir / "arc-cleanup-scope-positive" / "module.manifest.json"
    )
    arc_cleanup_scope_manifest = json.loads(
        arc_cleanup_scope_manifest_path.read_text(encoding="utf-8")
    )
    arc_cleanup_scope_sema = arc_cleanup_scope_manifest.get("frontend", {}).get(
        "pipeline", {}
    ).get("sema_pass_manager", {})

    arc_implicit_cleanup_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_implicit_cleanup_void_positive.objc3"
    )
    compile_fixture_with_args(
        arc_implicit_cleanup_fixture,
        case_dir / "arc-implicit-cleanup-positive",
        extra_args=["-fobjc-arc"],
    )
    arc_implicit_cleanup_manifest_path = (
        case_dir / "arc-implicit-cleanup-positive" / "module.manifest.json"
    )
    arc_implicit_cleanup_manifest = json.loads(
        arc_implicit_cleanup_manifest_path.read_text(encoding="utf-8")
    )
    arc_implicit_cleanup_sema = arc_implicit_cleanup_manifest.get(
        "frontend", {}
    ).get("pipeline", {}).get("sema_pass_manager", {})

    arc_autorelease_return_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_autorelease_return_positive.objc3"
    )
    compile_fixture_with_args(
        arc_autorelease_return_fixture,
        case_dir / "arc-autorelease-return-positive",
        extra_args=["-fobjc-arc"],
    )
    arc_autorelease_return_ll_path = (
        case_dir / "arc-autorelease-return-positive" / "module.ll"
    )
    arc_autorelease_return_manifest_path = (
        case_dir / "arc-autorelease-return-positive" / "module.manifest.json"
    )
    arc_autorelease_return_manifest = json.loads(
        arc_autorelease_return_manifest_path.read_text(encoding="utf-8")
    )
    arc_autorelease_return_ll = arc_autorelease_return_ll_path.read_text(
        encoding="utf-8"
    )
    arc_autorelease_return_sema = arc_autorelease_return_manifest.get(
        "frontend", {}
    ).get("pipeline", {}).get("sema_pass_manager", {})

    negative_batch = compile_negative_diagnostic_batch(
        case_id="block-storage-arc-automation-semantics",
        out_dir=case_dir / "negative-diagnostics-batch",
        expectations=[
            NegativeDiagnosticExpectation(
                key="weak-mutation-negative",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "weak_object_capture_mutation_negative.objc3",
                expected_snippets=[
                    "type mismatch: block mutated capture 'weakValue' requires owned runtime-backed storage"
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="unowned-mutation-negative",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "unowned_object_capture_mutation_negative.objc3",
                expected_snippets=[
                    "type mismatch: block mutated capture 'borrowedValue' requires owned runtime-backed storage"
                ],
                expected_codes=["O3S206"],
            ),
        ],
    )
    weak_negative = negative_batch["results"][0]
    unowned_negative = negative_batch["results"][1]

    expect(
        owned_manifest.get("runtime_block_arc_unified_source_surface", {}).get(
            "contract_id"
        )
        == RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID,
        "expected owned-capture helper fixture to publish the block/ARC unified source surface",
    )
    expect(
        owned_manifest.get(
            "runtime_ownership_transfer_capture_family_source_surface", {}
        ).get("contract_id")
        == RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID,
        "expected owned-capture helper fixture to publish the ownership-transfer/capture-family source surface",
    )
    expect(
        owned_copy_dispose_surface.get("copy_helper_required_sites") == 1
        and owned_copy_dispose_surface.get("dispose_helper_required_sites") == 1,
        "expected owned object capture fixture to require copy/dispose helpers",
    )
    expect(
        owned_copy_dispose_surface.get("copy_helper_symbolized_sites") == 1
        and owned_copy_dispose_surface.get("dispose_helper_symbolized_sites") == 1,
        "expected owned object capture fixture to symbolize copy/dispose helpers",
    )
    expect(
        owned_escape_surface.get("escape_to_heap_sites") == 1
        and owned_escape_surface.get("escape_analysis_enabled_sites") == 1,
        "expected owned object capture fixture to publish one escaping block path",
    )
    expect(
        "; runtime_block_allocation_copy_dispose_invoke_support = "
        "contract=objc3c.runtime.block.allocation.copy.dispose.invoke.support.v1"
        in owned_ll,
        "expected owned object capture fixture LLVM IR to publish the block allocation/copy/dispose/invoke support surface",
    )

    expect(
        nonowning_copy_dispose_surface.get("copy_helper_required_sites") == 0
        and nonowning_copy_dispose_surface.get("dispose_helper_required_sites") == 0,
        "expected non-owning object capture fixture to elide copy/dispose helpers",
    )
    expect(
        nonowning_copy_dispose_surface.get("copy_helper_symbolized_sites") == 0
        and nonowning_copy_dispose_surface.get("dispose_helper_symbolized_sites") == 0,
        "expected non-owning object capture fixture to publish zero helper symbols",
    )
    expect(
        nonowning_arc_diagnostics_surface.get(
            "ownership_arc_diagnostic_candidate_sites"
        )
        == 1
        and nonowning_arc_diagnostics_surface.get("ownership_arc_fixit_available_sites")
        == 1
        and nonowning_arc_diagnostics_surface.get("ownership_arc_profiled_sites")
        == 1,
        "expected non-owning object capture fixture to publish one ARC ownership diagnostic/fixit candidate",
    )
    expect(
        "; block_copy_dispose_lowering = " in nonowning_ll,
        "expected non-owning object capture fixture LLVM IR to publish the block copy/dispose lowering summary",
    )

    expect(
        arc_mode_sema.get("retain_release_operation_lowering_retain_insertion_sites")
        == 8
        and arc_mode_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 8,
        "expected arc mode handling fixture to publish eight retain and eight release insertions",
    )
    expect(
        arc_mode_block_copy_dispose_surface.get("copy_helper_required_sites") == 1
        and arc_mode_block_copy_dispose_surface.get("dispose_helper_required_sites")
        == 1,
        "expected arc mode handling fixture to keep block copy/dispose helper lowering enabled",
    )
    expect(
        "; arc_cleanup_weak_lifetime_hooks = "
        "contract=objc3c.arc.cleanup.weak.lifetime.hooks.v1" in arc_mode_ll,
        "expected arc mode handling fixture LLVM IR to publish the ARC cleanup/weak lifetime hooks surface",
    )

    expect(
        arc_inference_sema.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == 8
        and arc_inference_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 8
        and arc_inference_sema.get(
            "retain_release_operation_lowering_autorelease_insertion_sites"
        )
        == 0,
        "expected arc inference fixture to publish canonical retain/release insertion counts without autorelease insertion",
    )
    expect(
        arc_inference_sema.get(
            "weak_unowned_semantics_lowering_ownership_candidate_sites"
        )
        == 8
        and arc_inference_sema.get(
            "weak_unowned_semantics_lowering_weak_reference_sites"
        )
        == 0,
        "expected arc inference fixture to normalize eight ownership-qualified candidates without weak-reference lowering",
    )
    expect(
        "; arc_cleanup_weak_lifetime_hooks = "
        "contract=objc3c.arc.cleanup.weak.lifetime.hooks.v1" in arc_inference_ll,
        "expected arc inference fixture LLVM IR to publish the ARC cleanup/weak lifetime hooks surface",
    )
    expect(
        arc_cleanup_scope_sema.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == 1
        and arc_cleanup_scope_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 1
        and arc_cleanup_scope_sema.get(
            "retain_release_operation_lowering_autorelease_insertion_sites"
        )
        == 0,
        "expected ARC cleanup scope fixture to publish one retain/release transfer pair without autorelease insertion",
    )
    expect(
        arc_implicit_cleanup_sema.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == 1
        and arc_implicit_cleanup_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 1
        and arc_implicit_cleanup_sema.get(
            "retain_release_operation_lowering_autorelease_insertion_sites"
        )
        == 0,
        "expected ARC implicit cleanup fixture to publish one retain/release cleanup pair without autorelease insertion",
    )
    expect(
        arc_autorelease_return_sema.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == 0
        and arc_autorelease_return_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 0
        and arc_autorelease_return_sema.get(
            "retain_release_operation_lowering_autorelease_insertion_sites"
        )
        == 2,
        "expected ARC autorelease-return fixture to publish two autorelease insertions without retain/release insertion",
    )
    expect(
        "; arc_block_autorelease_return_lowering = " in arc_autorelease_return_ll,
        "expected ARC autorelease-return fixture LLVM IR to publish the ARC block/autorelease-return lowering summary",
    )

    return CaseResult(
        case_id="block-storage-arc-automation-semantics",
        probe="compile-manifest-diagnostics-and-llvm-ir",
        fixture="tests/tooling/fixtures/native/owned_object_capture_helper_positive.objc3",
        claim_class="compile-coupled-inspection",
        passed=True,
        summary={
            "owned_copy_helper_required_sites": owned_copy_dispose_surface.get(
                "copy_helper_required_sites"
            ),
            "nonowning_copy_helper_required_sites": nonowning_copy_dispose_surface.get(
                "copy_helper_required_sites"
            ),
            "arc_mode_retain_insertions": arc_mode_sema.get(
                "retain_release_operation_lowering_retain_insertion_sites"
            ),
            "arc_inference_retain_insertions": arc_inference_sema.get(
                "retain_release_operation_lowering_retain_insertion_sites"
            ),
            "arc_cleanup_scope_release_insertions": arc_cleanup_scope_sema.get(
                "retain_release_operation_lowering_release_insertion_sites"
            ),
            "arc_implicit_cleanup_release_insertions": arc_implicit_cleanup_sema.get(
                "retain_release_operation_lowering_release_insertion_sites"
            ),
            "arc_autorelease_return_autorelease_insertions": arc_autorelease_return_sema.get(
                "retain_release_operation_lowering_autorelease_insertion_sites"
            ),
            "weak_negative_diagnostic_count": weak_negative["diagnostic_count"],
            "unowned_negative_diagnostic_count": unowned_negative["diagnostic_count"],
            "negative_diagnostics_batch": negative_batch,
        },
    )
def check_block_arc_runtime_abi_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "block-arc-runtime-abi"
    probe = ROOT / Path(BLOCK_ARC_RUNTIME_ABI_PROBE)
    exe_path = case_dir / "block_arc_runtime_abi_probe.exe"
    compile_probe(clangxx, probe, exe_path, [])
    payload = parse_json_output(run_probe(exe_path), "block ARC runtime ABI probe")

    expected_string_fields = {
        "block_promote_symbol": "objc3_runtime_promote_block_i32",
        "block_invoke_symbol": "objc3_runtime_invoke_block_i32",
        "retain_symbol": "objc3_runtime_retain_i32",
        "release_symbol": "objc3_runtime_release_i32",
        "autorelease_symbol": "objc3_runtime_autorelease_i32",
        "autoreleasepool_push_symbol": "objc3_runtime_push_autoreleasepool_scope",
        "autoreleasepool_pop_symbol": "objc3_runtime_pop_autoreleasepool_scope",
        "current_property_read_symbol": "objc3_runtime_read_current_property_i32",
        "current_property_write_symbol": "objc3_runtime_write_current_property_i32",
        "current_property_exchange_symbol": "objc3_runtime_exchange_current_property_i32",
        "bind_current_property_context_symbol": (
            "objc3_runtime_bind_current_property_context_for_testing"
        ),
        "clear_current_property_context_symbol": (
            "objc3_runtime_clear_current_property_context_for_testing"
        ),
        "weak_current_property_load_symbol": (
            "objc3_runtime_load_weak_current_property_i32"
        ),
        "weak_current_property_store_symbol": (
            "objc3_runtime_store_weak_current_property_i32"
        ),
        "arc_debug_state_snapshot_symbol": (
            "objc3_runtime_copy_arc_debug_state_for_testing"
        ),
        "runtime_abi_boundary_model": BLOCK_ARC_RUNTIME_ABI_BOUNDARY_MODEL,
        "block_runtime_model": BLOCK_ARC_RUNTIME_BLOCK_MODEL,
        "arc_runtime_model": BLOCK_ARC_RUNTIME_ARC_MODEL,
        "fail_closed_model": BLOCK_ARC_RUNTIME_FAIL_CLOSED_MODEL,
    }
    for field, expected_value in expected_string_fields.items():
        expect(
            payload.get(field) == expected_value,
            f"expected block ARC runtime ABI probe to preserve {field}",
        )

    expected_integer_fields = {
        "abi_status": 0,
        "arc_status": 0,
        "retained": 77,
        "autoreleased": 77,
        "released": 77,
        "invoke_result": 17,
        "private_runtime_abi_ready": 1,
        "public_runtime_header_unchanged": 1,
        "deterministic": 1,
        "live_runtime_block_handle_count": 0,
        "block_promote_call_count": 1,
        "block_invoke_call_count": 1,
        "retain_call_count": 2,
        "release_call_count": 3,
        "autorelease_call_count": 1,
        "autoreleasepool_push_count": 1,
        "autoreleasepool_pop_count": 1,
        "current_property_read_count": 0,
        "current_property_write_count": 0,
        "current_property_exchange_count": 0,
        "weak_current_property_load_count": 0,
        "weak_current_property_store_count": 0,
        "last_promote_has_pointer_capture_storage": 1,
        "last_block_invoke_result": 17,
        "last_autorelease_value": 77,
        "arc_retain_call_count": 2,
        "arc_release_call_count": 3,
        "arc_autorelease_call_count": 1,
        "arc_autoreleasepool_push_count": 1,
        "arc_autoreleasepool_pop_count": 1,
    }
    for field, expected_value in expected_integer_fields.items():
        expect(
            payload.get(field) == expected_value,
            f"expected block ARC runtime ABI probe to preserve {field}",
        )

    handle = payload.get("handle")
    expect(isinstance(handle, int) and handle > 0, "expected block ARC runtime ABI probe to publish a live promoted block handle")
    expect(
        payload.get("retain_handle_result") == handle
        and payload.get("release_handle_result") == handle
        and payload.get("final_release_result") == handle,
        "expected block ARC runtime ABI probe to preserve block handle retain/release traffic",
    )
    expect(
        payload.get("last_promoted_block_handle") == handle
        and payload.get("last_invoked_block_handle") == handle,
        "expected block ARC runtime ABI probe to preserve the last promoted and invoked block handle",
    )
    expect(
        payload.get("last_retain_value") == handle
        and payload.get("last_release_value") == handle,
        "expected block ARC runtime ABI probe to preserve the last ARC retain/release value",
    )

    return CaseResult(
        case_id="block-arc-runtime-abi",
        probe=BLOCK_ARC_RUNTIME_ABI_PROBE,
        fixture=None,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "handle": handle,
            "invoke_result": payload.get("invoke_result"),
            "block_promote_call_count": payload.get("block_promote_call_count"),
            "block_invoke_call_count": payload.get("block_invoke_call_count"),
            "retain_call_count": payload.get("retain_call_count"),
            "release_call_count": payload.get("release_call_count"),
            "autorelease_call_count": payload.get("autorelease_call_count"),
        },
    )
def check_block_helper_runtime_execution_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "block-helper-runtime-execution"

    byref_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "byref_cell_copy_dispose_runtime_positive.objc3"
    )
    byref_obj, _, byref_manifest_path = compile_fixture_outputs(
        byref_fixture, case_dir / "byref-runtime-positive"
    )
    byref_manifest = json.loads(byref_manifest_path.read_text(encoding="utf-8"))
    byref_copy_dispose_surface = (
        byref_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )
    byref_exe = case_dir / "byref-runtime-positive" / "byref_runtime.exe"
    link_fixture_executable(clangxx, byref_obj, byref_exe)
    byref_run = run([str(byref_exe)])
    expect(
        byref_run.returncode == 14,
        f"expected byref runtime positive fixture to exit 14, saw {byref_run.returncode}",
    )
    expect(
        byref_copy_dispose_surface.get("copy_helper_required_sites") == 1
        and byref_copy_dispose_surface.get("dispose_helper_required_sites") == 1,
        "expected byref runtime positive fixture to require copy/dispose helpers",
    )
    byref_forwarding_probe = (
        ROOT
        / "tests"
        / "tooling"
        / "runtime"
        / "block_runtime_byref_forwarding_probe.cpp"
    )
    byref_forwarding_exe = (
        case_dir / "block_runtime_byref_forwarding_probe.exe"
    )
    compile_probe(clangxx, byref_forwarding_probe, byref_forwarding_exe, [])
    byref_forwarding_payload = parse_json_output(
        run_probe(byref_forwarding_exe),
        "block runtime byref forwarding probe",
    )
    expect(
        isinstance(byref_forwarding_payload.get("handle"), int)
        and byref_forwarding_payload.get("handle", 0) > 0,
        "expected byref forwarding probe to publish a positive runtime block handle",
    )
    expect(
        byref_forwarding_payload.get("copy_count_after_promotion") == 1,
        "expected byref forwarding probe to execute one copy helper during promotion",
    )
    expect(
        byref_forwarding_payload.get("first_invoke_result") == 23
        and byref_forwarding_payload.get("second_invoke_result") == 25,
        "expected byref forwarding probe to preserve runtime-owned forwarded cell state across invokes",
    )
    expect(
        byref_forwarding_payload.get("dispose_count_before_final_release") == 0
        and byref_forwarding_payload.get("dispose_count_after_final_release") == 1,
        "expected byref forwarding probe to defer dispose helper execution until final release",
    )
    expect(
        byref_forwarding_payload.get("last_disposed_value") == 11,
        "expected byref forwarding probe to dispose the original owned capture payload",
    )
    expect(
        byref_forwarding_payload.get("final_release_result")
        == byref_forwarding_payload.get("handle")
        and byref_forwarding_payload.get("invoke_after_release_result") == 0,
        "expected byref forwarding probe to release the block handle and reject post-release invocation",
    )

    owned_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "owned_object_capture_runtime_positive.objc3"
    )
    owned_obj, _, owned_manifest_path = compile_fixture_outputs(
        owned_fixture, case_dir / "owned-runtime-positive"
    )
    owned_manifest = json.loads(owned_manifest_path.read_text(encoding="utf-8"))
    owned_copy_dispose_surface = (
        owned_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )
    owned_exe = case_dir / "owned-runtime-positive" / "owned_runtime.exe"
    link_fixture_executable(clangxx, owned_obj, owned_exe)
    owned_run = run([str(owned_exe)])
    expect(
        owned_run.returncode == 11,
        f"expected owned object capture runtime positive fixture to exit 11, saw {owned_run.returncode}",
    )
    expect(
        owned_copy_dispose_surface.get("copy_helper_required_sites") == 1
        and owned_copy_dispose_surface.get("dispose_helper_required_sites") == 1,
        "expected owned runtime positive fixture to require copy/dispose helpers",
    )

    nonowning_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "nonowning_object_capture_runtime_positive.objc3"
    )
    nonowning_obj, _, nonowning_manifest_path = compile_fixture_outputs(
        nonowning_fixture, case_dir / "nonowning-runtime-positive"
    )
    nonowning_manifest = json.loads(
        nonowning_manifest_path.read_text(encoding="utf-8")
    )
    nonowning_copy_dispose_surface = (
        nonowning_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_block_copy_dispose_lowering_surface", {})
    )
    nonowning_exe = (
        case_dir / "nonowning-runtime-positive" / "nonowning_runtime.exe"
    )
    link_fixture_executable(clangxx, nonowning_obj, nonowning_exe)
    nonowning_run = run([str(nonowning_exe)])
    expect(
        nonowning_run.returncode == 9,
        f"expected non-owning object capture runtime positive fixture to exit 9, saw {nonowning_run.returncode}",
    )
    expect(
        nonowning_copy_dispose_surface.get("copy_helper_required_sites") == 0
        and nonowning_copy_dispose_surface.get("dispose_helper_required_sites") == 0,
        "expected non-owning runtime positive fixture to elide copy/dispose helpers",
    )

    arc_mode_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_mode_handling_positive.objc3"
    )
    arc_mode_obj, _, arc_mode_manifest_path = compile_fixture_outputs_with_args(
        arc_mode_fixture,
        case_dir / "arc-mode-runtime-positive",
        extra_args=["-fobjc-arc"],
    )
    arc_mode_manifest = json.loads(arc_mode_manifest_path.read_text(encoding="utf-8"))
    arc_mode_sema = arc_mode_manifest.get("frontend", {}).get("pipeline", {}).get(
        "sema_pass_manager", {}
    )
    arc_mode_exe = case_dir / "arc-mode-runtime-positive" / "arc_mode.exe"
    link_fixture_executable(clangxx, arc_mode_obj, arc_mode_exe)
    arc_mode_run = run([str(arc_mode_exe)])
    expect(
        arc_mode_run.returncode == 17,
        f"expected ARC mode runtime positive fixture to exit 17, saw {arc_mode_run.returncode}",
    )
    expect(
        arc_mode_sema.get("retain_release_operation_lowering_retain_insertion_sites")
        == 8
        and arc_mode_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 8,
        "expected ARC mode runtime positive fixture to preserve eight retain and eight release insertions",
    )

    arc_inference_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_inference_lifetime_positive.objc3"
    )
    arc_inference_obj, _, arc_inference_manifest_path = (
        compile_fixture_outputs_with_args(
            arc_inference_fixture,
            case_dir / "arc-inference-runtime-positive",
            extra_args=["-fobjc-arc"],
        )
    )
    arc_inference_manifest = json.loads(
        arc_inference_manifest_path.read_text(encoding="utf-8")
    )
    arc_inference_sema = arc_inference_manifest.get("frontend", {}).get(
        "pipeline", {}
    ).get("sema_pass_manager", {})
    arc_inference_exe = (
        case_dir / "arc-inference-runtime-positive" / "arc_inference.exe"
    )
    link_fixture_executable(clangxx, arc_inference_obj, arc_inference_exe)
    arc_inference_run = run([str(arc_inference_exe)])
    expect(
        arc_inference_run.returncode == 17,
        f"expected ARC inference runtime positive fixture to exit 17, saw {arc_inference_run.returncode}",
    )
    expect(
        arc_inference_sema.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == 8
        and arc_inference_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 8,
        "expected ARC inference runtime positive fixture to preserve eight retain and eight release insertions",
    )

    arc_cleanup_scope_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_cleanup_scope_positive.objc3"
    )
    arc_cleanup_scope_obj, _, arc_cleanup_scope_manifest_path = (
        compile_fixture_outputs_with_args(
            arc_cleanup_scope_fixture,
            case_dir / "arc-cleanup-scope-runtime-positive",
            extra_args=["-fobjc-arc"],
        )
    )
    arc_cleanup_scope_manifest = json.loads(
        arc_cleanup_scope_manifest_path.read_text(encoding="utf-8")
    )
    arc_cleanup_scope_sema = arc_cleanup_scope_manifest.get("frontend", {}).get(
        "pipeline", {}
    ).get("sema_pass_manager", {})
    arc_cleanup_scope_exe = (
        case_dir / "arc-cleanup-scope-runtime-positive" / "arc_cleanup_scope.exe"
    )
    link_fixture_executable(clangxx, arc_cleanup_scope_obj, arc_cleanup_scope_exe)
    arc_cleanup_scope_run = run([str(arc_cleanup_scope_exe)])
    expect(
        arc_cleanup_scope_run.returncode == 9,
        f"expected ARC cleanup scope runtime positive fixture to exit 9, saw {arc_cleanup_scope_run.returncode}",
    )
    expect(
        arc_cleanup_scope_sema.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == 1
        and arc_cleanup_scope_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 1,
        "expected ARC cleanup scope runtime positive fixture to preserve one retain/release cleanup pair",
    )

    arc_implicit_cleanup_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_implicit_cleanup_void_positive.objc3"
    )
    arc_implicit_cleanup_obj, _, arc_implicit_cleanup_manifest_path = (
        compile_fixture_outputs_with_args(
            arc_implicit_cleanup_fixture,
            case_dir / "arc-implicit-cleanup-runtime-positive",
            extra_args=["-fobjc-arc"],
        )
    )
    arc_implicit_cleanup_manifest = json.loads(
        arc_implicit_cleanup_manifest_path.read_text(encoding="utf-8")
    )
    arc_implicit_cleanup_sema = arc_implicit_cleanup_manifest.get(
        "frontend", {}
    ).get("pipeline", {}).get("sema_pass_manager", {})
    arc_implicit_cleanup_exe = (
        case_dir
        / "arc-implicit-cleanup-runtime-positive"
        / "arc_implicit_cleanup.exe"
    )
    link_fixture_executable(
        clangxx, arc_implicit_cleanup_obj, arc_implicit_cleanup_exe
    )
    arc_implicit_cleanup_run = run([str(arc_implicit_cleanup_exe)])
    expect(
        arc_implicit_cleanup_run.returncode == 0,
        f"expected ARC implicit cleanup runtime positive fixture to exit 0, saw {arc_implicit_cleanup_run.returncode}",
    )
    expect(
        arc_implicit_cleanup_sema.get(
            "retain_release_operation_lowering_retain_insertion_sites"
        )
        == 1
        and arc_implicit_cleanup_sema.get(
            "retain_release_operation_lowering_release_insertion_sites"
        )
        == 1,
        "expected ARC implicit cleanup runtime positive fixture to preserve one retain/release cleanup pair",
    )

    return CaseResult(
        case_id="block-helper-runtime-execution",
        probe="linked-fixture-main",
        fixture="tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "byref_runtime_exit_code": byref_run.returncode,
            "owned_runtime_exit_code": owned_run.returncode,
            "nonowning_runtime_exit_code": nonowning_run.returncode,
            "arc_mode_runtime_exit_code": arc_mode_run.returncode,
            "arc_inference_runtime_exit_code": arc_inference_run.returncode,
            "arc_cleanup_scope_runtime_exit_code": arc_cleanup_scope_run.returncode,
            "arc_implicit_cleanup_runtime_exit_code": arc_implicit_cleanup_run.returncode,
            "byref_forwarding_probe_handle": byref_forwarding_payload.get("handle"),
            "byref_forwarding_first_invoke_result": byref_forwarding_payload.get(
                "first_invoke_result"
            ),
            "byref_forwarding_second_invoke_result": byref_forwarding_payload.get(
                "second_invoke_result"
            ),
            "byref_copy_helper_required_sites": byref_copy_dispose_surface.get(
                "copy_helper_required_sites"
            ),
            "owned_copy_helper_required_sites": owned_copy_dispose_surface.get(
                "copy_helper_required_sites"
            ),
            "nonowning_copy_helper_required_sites": nonowning_copy_dispose_surface.get(
                "copy_helper_required_sites"
            ),
            "arc_mode_retain_insertions": arc_mode_sema.get(
                "retain_release_operation_lowering_retain_insertion_sites"
            ),
            "arc_inference_retain_insertions": arc_inference_sema.get(
                "retain_release_operation_lowering_retain_insertion_sites"
            ),
            "arc_cleanup_scope_release_insertions": arc_cleanup_scope_sema.get(
                "retain_release_operation_lowering_release_insertion_sites"
            ),
            "arc_implicit_cleanup_release_insertions": arc_implicit_cleanup_sema.get(
                "retain_release_operation_lowering_release_insertion_sites"
            ),
        },
    )
def check_arc_property_helper_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "arc-property-helper-abi"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_property_interaction_positive.objc3"
    )
    obj_path = compile_fixture(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "arc_debug_instrumentation_probe.cpp"
    exe_path = case_dir / "arc_debug_instrumentation_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "arc property helper probe")

    inside = payload.get("inside", {})
    after = payload.get("after", {})

    expect(payload.get("parent", 0) != 0 and payload.get("child", 0) != 0,
           "expected ArcBox runtime helper probe to allocate live receivers")
    expect(payload.get("bind_current_status") == 0,
           "expected strong-property current context binding to succeed")
    expect(payload.get("bind_weak_status") == 0,
           "expected weak-property current context binding to succeed")
    expect(payload.get("rebind_current_status") == 0 and payload.get("rebind_weak_status") == 0,
           "expected current-property helper rebinds to succeed")
    expect(payload.get("getter_value") == payload.get("child"),
           "expected current-property getter helper to read the stored child value")
    expect(payload.get("release_local_result") == payload.get("child"),
           "expected releasing the retained local to return the child handle")
    expect(payload.get("retained") == 9,
           "expected the retain helper to preserve the canonical retained payload")
    expect(payload.get("autoreleased") == 9,
           "expected the autorelease helper to preserve the canonical autoreleased payload")
    expect(payload.get("weak_set_result") == payload.get("child"),
           "expected weak-property helper write to preserve the child value")
    expect(payload.get("weak_inside_pool") == payload.get("child"),
           "expected weak-property helper read inside the pool to preserve the child value")
    expect(payload.get("weak_after_pool") == payload.get("child"),
           "expected weak-property helper read after pool pop to stay coherent")
    expect(payload.get("released") == 9,
           "expected release helper accounting to preserve the released payload")
    expect(payload.get("parent_release_result") == payload.get("parent"),
           "expected releasing the parent to return the parent handle")
    expect(payload.get("strong_set_result") == 0,
           "expected first strong-property exchange to replace an empty slot")
    expect(payload.get("clear_strong_result") == payload.get("child"),
           "expected clearing the strong property to return the previous child value")
    expect(inside.get("retain_call_count") == 1,
           "expected one retain helper call before the autoreleasepool drains")
    expect(inside.get("release_call_count") == 1,
           "expected one release helper call before the autoreleasepool drains")
    expect(inside.get("autorelease_call_count") == 1,
           "expected one autorelease helper call before the autoreleasepool drains")
    expect(inside.get("autoreleasepool_push_count") == 1,
           "expected one autoreleasepool push before the autoreleasepool drains")
    expect(inside.get("autoreleasepool_pop_count") == 0,
           "expected no autoreleasepool pop before the autoreleasepool drains")
    expect(inside.get("current_property_read_count") == 2,
           "expected live current-property reads to execute through the runtime helper ABI")
    expect(inside.get("current_property_write_count") == 1,
           "expected live current-property writes to execute through the runtime helper ABI")
    expect(inside.get("current_property_exchange_count") == 2,
           "expected strong ownership accessors to execute through exchange helper traffic")
    expect(inside.get("weak_current_property_load_count") == 1,
           "expected weak-property loads to execute through the runtime helper ABI")
    expect(inside.get("weak_current_property_store_count") == 1,
           "expected weak-property stores to execute through the runtime helper ABI")
    expect(inside.get("last_retain_value") == 9,
           "expected helper ABI debug state to preserve the retained payload")
    expect(inside.get("last_release_value") == payload.get("child"),
           "expected helper ABI debug state to preserve the pre-pool child release")
    expect(inside.get("last_autorelease_value") == 9,
           "expected helper ABI debug state to preserve the autoreleased payload")
    expect(inside.get("last_property_exchange_previous_value") == payload.get("child"),
           "expected helper ABI debug state to preserve the exchanged child handle")
    expect(inside.get("last_property_exchange_new_value") == 0,
           "expected helper ABI debug state to preserve the cleared strong slot")
    expect(inside.get("last_property_receiver") == payload.get("parent"),
           "expected helper ABI debug state to preserve the bound receiver")
    expect(inside.get("last_property_name") == "weakValue",
           "expected helper ABI debug state to report the bound weak property")
    expect(inside.get("last_property_owner_identity") == "implementation:ArcBox",
           "expected helper ABI debug state to report the ArcBox owner identity")
    expect(after.get("retain_call_count") == 1,
           "expected retain helper accounting to remain stable after the autoreleasepool drains")
    expect(after.get("release_call_count") == 3,
           "expected helper ABI probe to release the child, retained value, and parent exactly once each")
    expect(after.get("autorelease_call_count") == 1,
           "expected autorelease helper accounting to remain stable after the autoreleasepool drains")
    expect(after.get("autoreleasepool_push_count") == 1,
           "expected helper ABI probe to preserve a single autoreleasepool push")
    expect(after.get("autoreleasepool_pop_count") == 1,
           "expected helper ABI probe to pop one autorelease pool")
    expect(after.get("current_property_read_count") == 3,
           "expected post-pool helper accounting to include the final weak-property read")
    expect(after.get("current_property_write_count") == 1,
           "expected post-pool helper accounting to preserve one current-property write")
    expect(after.get("current_property_exchange_count") == 2,
           "expected post-pool helper accounting to preserve two strong-property exchanges")
    expect(after.get("weak_current_property_load_count") == 2,
           "expected post-pool helper accounting to preserve two weak-property loads")
    expect(after.get("weak_current_property_store_count") == 1,
           "expected post-pool helper accounting to preserve one weak-property store")
    expect(after.get("last_release_value") == payload.get("parent"),
           "expected helper ABI debug state to report the final parent release after pool drain")
    expect(after.get("last_property_name") == "weakValue",
           "expected post-pool helper ABI debug state to preserve the bound weak property")
    expect(after.get("last_property_owner_identity") == "implementation:ArcBox",
           "expected post-pool helper ABI debug state to preserve the ArcBox owner identity")

    return CaseResult(
        case_id="arc-property-helper-abi",
        probe="tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
        fixture="tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "parent": payload.get("parent"),
            "child": payload.get("child"),
            "release_local_result": payload.get("release_local_result"),
            "retained": payload.get("retained"),
            "autoreleased": payload.get("autoreleased"),
            "released": payload.get("released"),
            "getter_value": payload.get("getter_value"),
            "weak_after_pool": payload.get("weak_after_pool"),
            "inside_retain_call_count": inside.get("retain_call_count"),
            "inside_current_property_exchange_count": inside.get("current_property_exchange_count"),
            "after_autoreleasepool_pop_count": after.get("autoreleasepool_pop_count"),
            "after_release_call_count": after.get("release_call_count"),
        },
    )

__all__ = [
    "build_runtime_block_arc_unified_source_surface",
    "build_runtime_ownership_transfer_capture_family_source_surface",
    "build_runtime_block_arc_lowering_helper_surface",
    "build_runtime_block_arc_runtime_abi_surface",
    "check_escaping_block_capture_legality_case",
    "check_block_storage_arc_automation_semantics_case",
    "check_block_arc_runtime_abi_case",
    "check_block_helper_runtime_execution_case",
    "check_arc_property_helper_case",
    "exported_case_names",
]
