"""Ownership transfer and capture-family source surface builder."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.block_arc_surface_support import (
    authoritative_case_ids,
)

from ..runtime_contract_block_arc import (
    RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID,
)


def build_runtime_ownership_transfer_capture_family_source_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
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
        "authoritative_case_ids": authoritative_case_ids(
            results,
            {
                "escaping-block-capture-legality",
                "block-storage-arc-automation-semantics",
                "block-helper-runtime-execution",
                "arc-property-helper-abi",
            },
        ),
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
            "tests/tooling/fixtures/native/execution/positive/escaping_owned_object_block_copy_dispose.objc3",
            "tests/tooling/fixtures/native/execution/negative/escaping_owned_object_block_conflicting_capture.objc3",
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
            "tests/tooling/runtime/block_runtime_owned_capture_lifetime_probe.cpp",
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


__all__ = ["build_runtime_ownership_transfer_capture_family_source_surface"]
