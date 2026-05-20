"""Unified Block/ARC source surface builder."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.block_arc_surface_support import (
    authoritative_case_ids,
)

from ..runtime_contract_block_arc import (
    BLOCK_ARC_RUNTIME_ABI_PROBE,
    RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID,
)


def build_runtime_block_arc_unified_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
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
        "authoritative_case_ids": authoritative_case_ids(
            results,
            {
                "block-arc-runtime-abi",
                "escaping-block-capture-legality",
                "block-storage-arc-automation-semantics",
                "block-helper-runtime-execution",
                "arc-property-helper-abi",
            },
        ),
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
            "tests/tooling/fixtures/native/execution/positive/escaping_owned_object_block_copy_dispose.objc3",
            "tests/tooling/fixtures/native/execution/negative/escaping_owned_object_block_conflicting_capture.objc3",
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
            "tests/tooling/runtime/block_runtime_owned_capture_lifetime_probe.cpp",
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


__all__ = ["build_runtime_block_arc_unified_source_surface"]
