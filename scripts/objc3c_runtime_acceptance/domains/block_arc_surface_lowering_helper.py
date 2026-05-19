"""Block/ARC lowering helper surface builder."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.block_arc_surface_support import (
    authoritative_case_ids,
)

from ..c_api import RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH
from ..runtime_contract_block_arc import (
    BLOCK_ARC_RUNTIME_ABI_PROBE,
    RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID,
    RUNTIME_BLOCK_ARC_RUNTIME_ABI_SURFACE_CONTRACT_ID,
    RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID,
)


def build_runtime_block_arc_lowering_helper_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
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
        "authoritative_case_ids": authoritative_case_ids(
            results,
            {
                "block-arc-runtime-abi",
                "escaping-block-capture-legality",
                "block-storage-arc-automation-semantics",
                "block-helper-runtime-execution",
            },
        ),
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


__all__ = ["build_runtime_block_arc_lowering_helper_surface"]
