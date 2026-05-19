"""Expected cross-module Block/ARC preservation contracts."""

from __future__ import annotations

from ..runtime_contract_block_arc import (
    RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID,
    RUNTIME_CROSS_MODULE_BLOCK_OWNERSHIP_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
)


BLOCK_OBJECT_INVOKE_THUNK_LOWERING_CONTRACT_ID = (
    "objc3c.executable.block.object.and.invoke.thunk.lowering.v1"
)
BLOCK_BYREF_HELPER_LOWERING_CONTRACT_ID = (
    "objc3c.executable.block.byref.helper.lowering.v1"
)
BLOCK_ESCAPE_RUNTIME_HOOK_LOWERING_CONTRACT_ID = (
    "objc3c.executable.block.escape.runtime.hook.lowering.v1"
)
RUNTIME_SUPPORT_LIBRARY_LINK_WIRING_CONTRACT_ID = (
    "objc3c.runtime.support.library.link.wiring.v1"
)
BLOCK_OWNERSHIP_PRESERVATION_MODEL = (
    "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-"
    "preserve-block-ownership-lowering-helper-and-runtime-link-facts-beyond-local-"
    "ir-object-emission"
)

PROVIDER_BLOCK_OWNERSHIP_FIELDS = {
    "contract_id": RUNTIME_CROSS_MODULE_BLOCK_OWNERSHIP_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
    "source_contract_id": RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID,
    "block_object_invoke_thunk_lowering_contract_id": BLOCK_OBJECT_INVOKE_THUNK_LOWERING_CONTRACT_ID,
    "block_byref_helper_lowering_contract_id": BLOCK_BYREF_HELPER_LOWERING_CONTRACT_ID,
    "block_escape_runtime_hook_lowering_contract_id": BLOCK_ESCAPE_RUNTIME_HOOK_LOWERING_CONTRACT_ID,
    "runtime_support_library_link_wiring_contract_id": RUNTIME_SUPPORT_LIBRARY_LINK_WIRING_CONTRACT_ID,
    "surface_path": "frontend.pipeline.semantic_surface.objc_runtime_block_ownership_artifact_preservation",
    "import_artifact_member_name": "objc_runtime_block_ownership_artifact_preservation",
    "source_model": "runtime-block-lowering-helper-surfaces-preserve-invoke-thunk-byref-copy-dispose-escape-and-runtime-link-facts-for-separate-compilation",
    "preservation_model": BLOCK_OWNERSHIP_PRESERVATION_MODEL,
    "fail_closed_model": "missing-or-drifted-block-ownership-preservation-packets-disable-cross-module-block-ownership-claims",
}

PROVIDER_BLOCK_OWNERSHIP_COUNTS = (
    ("local_block_literal_sites", 1),
    ("local_invoke_trampoline_symbolized_sites", 1),
    ("local_copy_helper_required_sites", 1),
    ("local_dispose_helper_required_sites", 1),
    ("local_copy_helper_symbolized_sites", 1),
    ("local_dispose_helper_symbolized_sites", 1),
    ("local_escape_to_heap_sites", 1),
    ("local_byref_layout_symbolized_sites", 1),
)

LINK_PLAN_CONTRACT_FIELDS = (
    (
        "runtime_cross_module_block_ownership_artifact_preservation_surface_contract_id",
        RUNTIME_CROSS_MODULE_BLOCK_OWNERSHIP_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
    ),
    (
        "runtime_block_arc_lowering_helper_surface_contract_id",
        RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID,
    ),
    (
        "block_object_invoke_thunk_lowering_contract_id",
        BLOCK_OBJECT_INVOKE_THUNK_LOWERING_CONTRACT_ID,
    ),
    (
        "block_byref_helper_lowering_contract_id",
        BLOCK_BYREF_HELPER_LOWERING_CONTRACT_ID,
    ),
    (
        "block_escape_runtime_hook_lowering_contract_id",
        BLOCK_ESCAPE_RUNTIME_HOOK_LOWERING_CONTRACT_ID,
    ),
    (
        "block_runtime_support_library_link_wiring_contract_id",
        RUNTIME_SUPPORT_LIBRARY_LINK_WIRING_CONTRACT_ID,
    ),
    (
        "block_ownership_artifact_preservation_model",
        BLOCK_OWNERSHIP_PRESERVATION_MODEL,
    ),
)

IMPORTED_MODULE_FIELDS = (
    ("block_ownership_artifact_preservation_present", True),
    ("block_ownership_runtime_import_artifact_ready", True),
    ("block_ownership_separate_compilation_preservation_ready", True),
    ("block_ownership_runtime_support_library_link_wiring_ready", True),
    ("block_ownership_deterministic", True),
    (
        "block_ownership_contract_id",
        RUNTIME_CROSS_MODULE_BLOCK_OWNERSHIP_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
    ),
    (
        "block_ownership_source_contract_id",
        RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID,
    ),
    (
        "block_ownership_object_invoke_thunk_lowering_contract_id",
        BLOCK_OBJECT_INVOKE_THUNK_LOWERING_CONTRACT_ID,
    ),
    (
        "block_ownership_byref_helper_lowering_contract_id",
        BLOCK_BYREF_HELPER_LOWERING_CONTRACT_ID,
    ),
    (
        "block_ownership_escape_runtime_hook_lowering_contract_id",
        BLOCK_ESCAPE_RUNTIME_HOOK_LOWERING_CONTRACT_ID,
    ),
    (
        "block_ownership_runtime_support_library_link_wiring_contract_id",
        RUNTIME_SUPPORT_LIBRARY_LINK_WIRING_CONTRACT_ID,
    ),
    ("block_ownership_local_block_literal_sites", 1),
    ("block_ownership_local_invoke_trampoline_symbolized_sites", 1),
    ("block_ownership_local_copy_helper_required_sites", 1),
    ("block_ownership_local_dispose_helper_required_sites", 1),
    ("block_ownership_local_copy_helper_symbolized_sites", 1),
    ("block_ownership_local_dispose_helper_symbolized_sites", 1),
    ("block_ownership_local_escape_to_heap_sites", 1),
    ("block_ownership_local_byref_layout_symbolized_sites", 1),
)

LOCAL_BLOCK_OWNERSHIP_COUNTS = {
    "local_block_ownership_block_literal_sites": 0,
    "local_block_ownership_invoke_trampoline_symbolized_sites": 0,
    "local_block_ownership_copy_helper_required_sites": 0,
    "local_block_ownership_dispose_helper_required_sites": 0,
    "local_block_ownership_copy_helper_symbolized_sites": 0,
    "local_block_ownership_dispose_helper_symbolized_sites": 0,
    "local_block_ownership_escape_to_heap_sites": 0,
    "local_block_ownership_byref_layout_symbolized_sites": 0,
}
IMPORTED_BLOCK_OWNERSHIP_COUNTS = {
    "imported_block_ownership_block_literal_sites": 1,
    "imported_block_ownership_invoke_trampoline_symbolized_sites": 1,
    "imported_block_ownership_copy_helper_required_sites": 1,
    "imported_block_ownership_dispose_helper_required_sites": 1,
    "imported_block_ownership_copy_helper_symbolized_sites": 1,
    "imported_block_ownership_dispose_helper_symbolized_sites": 1,
    "imported_block_ownership_escape_to_heap_sites": 1,
    "imported_block_ownership_byref_layout_symbolized_sites": 1,
}
TRANSITIVE_BLOCK_OWNERSHIP_COUNTS = {
    "transitive_block_ownership_block_literal_sites": 1,
    "transitive_block_ownership_invoke_trampoline_symbolized_sites": 1,
    "transitive_block_ownership_copy_helper_required_sites": 1,
    "transitive_block_ownership_dispose_helper_required_sites": 1,
    "transitive_block_ownership_copy_helper_symbolized_sites": 1,
    "transitive_block_ownership_dispose_helper_symbolized_sites": 1,
    "transitive_block_ownership_escape_to_heap_sites": 1,
    "transitive_block_ownership_byref_layout_symbolized_sites": 1,
}


__all__ = [
    "IMPORTED_BLOCK_OWNERSHIP_COUNTS",
    "IMPORTED_MODULE_FIELDS",
    "LINK_PLAN_CONTRACT_FIELDS",
    "LOCAL_BLOCK_OWNERSHIP_COUNTS",
    "PROVIDER_BLOCK_OWNERSHIP_COUNTS",
    "PROVIDER_BLOCK_OWNERSHIP_FIELDS",
    "TRANSITIVE_BLOCK_OWNERSHIP_COUNTS",
]
