#include "driver/objc3_driver_cross_module_imported_input_block.h"

void PopulateObjc3DriverCrossModuleRuntimeImportedInputBlockOwnership(
    Objc3CrossModuleRuntimeLinkPlanImportedInput &imported_input,
    const Objc3ImportedRuntimeModuleSurface &imported_surface) {
  imported_input.block_ownership_artifact_preservation_present =
      imported_surface.block_ownership_artifact_preservation_present;
  imported_input.block_ownership_runtime_import_artifact_ready =
      imported_surface.block_ownership_runtime_import_artifact_ready;
  imported_input.block_ownership_separate_compilation_preservation_ready =
      imported_surface.block_ownership_separate_compilation_preservation_ready;
  imported_input.block_ownership_runtime_support_library_link_wiring_ready =
      imported_surface.block_ownership_runtime_support_library_link_wiring_ready;
  imported_input.block_ownership_deterministic =
      imported_surface.block_ownership_deterministic;
  imported_input.block_ownership_contract_id =
      imported_surface.block_ownership_contract_id;
  imported_input.block_ownership_source_contract_id =
      imported_surface.block_ownership_source_contract_id;
  imported_input.block_ownership_object_invoke_thunk_lowering_contract_id =
      imported_surface.block_ownership_object_invoke_thunk_lowering_contract_id;
  imported_input.block_ownership_byref_helper_lowering_contract_id =
      imported_surface.block_ownership_byref_helper_lowering_contract_id;
  imported_input.block_ownership_escape_runtime_hook_lowering_contract_id =
      imported_surface.block_ownership_escape_runtime_hook_lowering_contract_id;
  imported_input
      .block_ownership_runtime_support_library_link_wiring_contract_id =
      imported_surface
          .block_ownership_runtime_support_library_link_wiring_contract_id;
  imported_input
      .block_ownership_retain_release_operation_lowering_contract_id =
      imported_surface
          .block_ownership_retain_release_operation_lowering_contract_id;
  imported_input.block_ownership_autoreleasepool_scope_lowering_contract_id =
      imported_surface
          .block_ownership_autoreleasepool_scope_lowering_contract_id;
  imported_input.block_ownership_replay_key =
      imported_surface.block_ownership_replay_key;
  imported_input.block_ownership_retain_release_operation_lowering_replay_key =
      imported_surface
          .block_ownership_retain_release_operation_lowering_replay_key;
  imported_input.block_ownership_autoreleasepool_scope_lowering_replay_key =
      imported_surface.block_ownership_autoreleasepool_scope_lowering_replay_key;
  imported_input.block_ownership_local_block_literal_sites =
      imported_surface.block_ownership_local_block_literal_sites;
  imported_input.block_ownership_local_invoke_trampoline_symbolized_sites =
      imported_surface.block_ownership_local_invoke_trampoline_symbolized_sites;
  imported_input.block_ownership_local_copy_helper_required_sites =
      imported_surface.block_ownership_local_copy_helper_required_sites;
  imported_input.block_ownership_local_dispose_helper_required_sites =
      imported_surface.block_ownership_local_dispose_helper_required_sites;
  imported_input.block_ownership_local_copy_helper_symbolized_sites =
      imported_surface.block_ownership_local_copy_helper_symbolized_sites;
  imported_input.block_ownership_local_dispose_helper_symbolized_sites =
      imported_surface.block_ownership_local_dispose_helper_symbolized_sites;
  imported_input.block_ownership_local_escape_to_heap_sites =
      imported_surface.block_ownership_local_escape_to_heap_sites;
  imported_input.block_ownership_local_byref_layout_symbolized_sites =
      imported_surface.block_ownership_local_byref_layout_symbolized_sites;
  imported_input.block_ownership_local_arc_ownership_qualified_sites =
      imported_surface.block_ownership_local_arc_ownership_qualified_sites;
  imported_input.block_ownership_local_arc_retain_insertion_sites =
      imported_surface.block_ownership_local_arc_retain_insertion_sites;
  imported_input.block_ownership_local_arc_release_insertion_sites =
      imported_surface.block_ownership_local_arc_release_insertion_sites;
  imported_input.block_ownership_local_arc_autorelease_insertion_sites =
      imported_surface.block_ownership_local_arc_autorelease_insertion_sites;
  imported_input.block_ownership_local_arc_contract_violation_sites =
      imported_surface.block_ownership_local_arc_contract_violation_sites;
  imported_input.block_ownership_local_autoreleasepool_scope_sites =
      imported_surface.block_ownership_local_autoreleasepool_scope_sites;
  imported_input
      .block_ownership_local_autoreleasepool_scope_symbolized_sites =
      imported_surface
          .block_ownership_local_autoreleasepool_scope_symbolized_sites;
  imported_input.block_ownership_local_autoreleasepool_max_scope_depth =
      imported_surface.block_ownership_local_autoreleasepool_max_scope_depth;
  imported_input
      .block_ownership_local_autoreleasepool_scope_entry_transition_sites =
      imported_surface
          .block_ownership_local_autoreleasepool_scope_entry_transition_sites;
  imported_input
      .block_ownership_local_autoreleasepool_scope_exit_transition_sites =
      imported_surface
          .block_ownership_local_autoreleasepool_scope_exit_transition_sites;
  imported_input
      .block_ownership_local_autoreleasepool_contract_violation_sites =
      imported_surface
          .block_ownership_local_autoreleasepool_contract_violation_sites;
  imported_input.block_ownership_arc_cleanup_preservation_ready =
      imported_surface.block_ownership_arc_cleanup_preservation_ready;
}
