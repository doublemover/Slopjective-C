#include "ir/objc3_ir_lowering_extension_metadata_publication.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRInteropLoweringMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!108 = !{!\""
      << EscapeCStringLiteral(metadata.lowering_interop_interop_replay_key)
      << "\", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_interop_lowering_foreign_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_interop_lowering_c_foreign_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_interop_lowering_objc_runtime_parity_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_interop_lowering_ownership_bridge_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_interop_lowering_error_surface_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_interop_lowering_async_boundary_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_interop_lowering_swift_concurrency_metadata_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_interop_lowering_interface_preserved_foreign_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .interop_interop_lowering_interface_preserved_metadata_annotation_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_interop_lowering_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_interop_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_interop_interop_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!109 = !{!\""
      << EscapeCStringLiteral(metadata.lowering_interop_foreign_call_lifetime_replay_key)
      << "\", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_foreign_call_lifetime_lowering_foreign_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_foreign_call_lifetime_lowering_c_foreign_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .interop_foreign_call_lifetime_lowering_objc_runtime_parity_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_foreign_call_lifetime_lowering_ownership_bridge_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_foreign_call_lifetime_lowering_lifetime_bridge_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_foreign_call_lifetime_lowering_metadata_preservation_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_foreign_call_lifetime_lowering_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_foreign_call_lifetime_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_interop_foreign_call_lifetime_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!110 = !{!\""
      << EscapeCStringLiteral(
             metadata.lowering_interop_ffi_metadata_interface_preservation_key)
      << "\", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .interop_ffi_metadata_interface_preservation_local_foreign_callable_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .interop_ffi_metadata_interface_preservation_local_metadata_preservation_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .interop_ffi_metadata_interface_preservation_local_interface_annotation_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.interop_ffi_metadata_interface_preservation_imported_module_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .interop_ffi_metadata_interface_preservation_imported_foreign_callable_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .interop_ffi_metadata_interface_preservation_imported_metadata_preservation_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .interop_ffi_metadata_interface_preservation_imported_interface_annotation_sites)
      << ", i1 "
      << (metadata
                  .interop_ffi_metadata_interface_preservation_runtime_import_artifact_ready
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .interop_ffi_metadata_interface_preservation_separate_compilation_preservation_ready
              ? 1
              : 0)
      << ", i1 "
      << (metadata.deterministic_interop_ffi_metadata_interface_preservation_handoff
              ? 1
              : 0)
      << "}\n\n";
  out << "!111 = !{!\""
      << EscapeCStringLiteral(Objc3InteropBridgePackagingToolchainSummary())
      << "\"}\n\n";
  out << "!112 = !{!\""
      << EscapeCStringLiteral(
             Objc3InteropHeaderModuleBridgeGenerationBoundarySummary())
      << "\"}\n\n";
}

void EmitObjc3IRMetaprogrammingLoweringMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!104 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_expansion_lowering_derive_inventory_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_expansion_lowering_derived_selector_artifact_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_expansion_lowering_macro_replay_visible_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_expansion_lowering_property_behavior_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_expansion_lowering_synthesized_binding_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_expansion_lowering_synthesized_getter_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_expansion_lowering_synthesized_setter_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_expansion_lowering_replay_visible_metadata_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_expansion_lowering_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_expansion_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_metaprogramming_expansion_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!105 = !{!\""
      << EscapeCStringLiteral(
             metadata.lowering_metaprogramming_synthesized_emission_replay_key)
      << "\", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_synthesized_emitted_derive_method_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_synthesized_emitted_macro_artifact_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .metaprogramming_synthesized_emitted_property_behavior_artifact_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_synthesized_emitted_global_artifact_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_synthesized_emitted_runtime_method_list_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_synthesized_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_synthesized_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_metaprogramming_synthesized_emission_handoff ? 1 : 0)
      << "}\n\n";
  out << "!106 = !{!\""
      << EscapeCStringLiteral(
             metadata.lowering_metaprogramming_module_interface_replay_preservation_key)
      << "\", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_module_replay_local_derive_method_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_module_replay_local_macro_artifact_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .metaprogramming_module_replay_local_interface_property_behavior_artifact_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .metaprogramming_module_replay_local_implementation_property_behavior_artifact_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_module_replay_local_runtime_method_list_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_module_replay_imported_module_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_module_replay_imported_derive_method_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_module_replay_imported_macro_artifact_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .metaprogramming_module_replay_imported_interface_property_behavior_artifact_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .metaprogramming_module_replay_imported_implementation_property_behavior_artifact_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_module_replay_imported_runtime_method_list_count)
      << ", i1 "
      << (metadata.metaprogramming_module_replay_runtime_import_artifact_ready ? 1 : 0)
      << ", i1 "
      << (metadata
                  .metaprogramming_module_replay_separate_compilation_preservation_ready
              ? 1
              : 0)
      << ", i1 "
      << (metadata.deterministic_metaprogramming_module_interface_replay_handoff ? 1 : 0)
      << "}\n\n";
  out << "!107 = !{!\""
      << EscapeCStringLiteral(Objc3MetaprogrammingExpansionHostRuntimeBoundarySummary())
      << "\", i1 1, i1 0, i1 0, i1 0, i1 1}\n\n";
}

void EmitObjc3IRActorDispatchControlMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!97 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_actor_interface_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_actor_method_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_actor_metadata_record_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_nonisolated_entry_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_executor_affinity_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_actor_hop_artifact_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_actor_isolation_thunk_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_replay_proof_dependency_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_race_guard_dependency_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_task_handoff_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.actor_lowering_metadata_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_actor_lowering_metadata_handoff ? 1 : 0)
      << "}\n\n";
  out << "!102 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_control_lowering_direct_call_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_control_lowering_direct_members_defaulted_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_control_lowering_dynamic_opt_out_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_control_lowering_final_container_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_control_lowering_sealed_container_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_control_lowering_override_legality_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .dispatch_dispatch_control_lowering_metadata_preserved_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .dispatch_dispatch_control_lowering_metadata_preserved_container_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_control_lowering_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_control_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_dispatch_dispatch_control_lowering_handoff ? 1 : 0)
      << "}\n\n";
}

void EmitObjc3IRDispatchMetadataPreservationNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!103 = !{!\""
      << EscapeCStringLiteral(
             metadata.lowering_dispatch_dispatch_metadata_interface_preservation_key)
      << "\", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_metadata_local_direct_callable_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_metadata_local_final_callable_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_metadata_local_final_container_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_metadata_local_sealed_container_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_metadata_imported_module_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_metadata_imported_direct_callable_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_metadata_imported_final_callable_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_metadata_imported_final_container_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_dispatch_metadata_imported_sealed_container_record_count)
      << ", i1 "
      << (metadata.dispatch_dispatch_metadata_runtime_import_artifact_ready ? 1 : 0)
      << ", i1 "
      << (metadata.dispatch_dispatch_metadata_separate_compilation_preservation_ready
              ? 1
              : 0)
      << ", i1 "
      << (metadata.deterministic_dispatch_dispatch_metadata_interface_handoff ? 1 : 0)
      << "}\n\n";
}

void EmitObjc3IROwnershipExtensionMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!98 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_system_extension_lowering_cleanup_hook_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_system_extension_lowering_resource_local_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_system_extension_lowering_cleanup_owned_local_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_system_extension_lowering_resource_move_capture_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_system_extension_lowering_borrowed_parameter_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_system_extension_lowering_borrowed_return_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_system_extension_lowering_borrowed_escape_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_system_extension_lowering_explicit_capture_item_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_system_extension_lowering_retainable_family_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .ownership_system_extension_lowering_retainable_family_operation_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .ownership_system_extension_lowering_retainable_family_alias_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_system_extension_lowering_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_system_extension_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_ownership_system_extension_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!99 = !{!\""
      << EscapeCStringLiteral(kObjc3OwnershipBorrowedRetainableAbiCompletionContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipBorrowedRetainableAbiCompletionSurfacePath)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.ownership_borrowed_retainable_abi_completion_replay_key)
      << "\", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_borrowed_retainable_returns_borrowed_attribute_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_borrowed_retainable_family_retain_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_borrowed_retainable_family_release_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_borrowed_retainable_family_autorelease_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_borrowed_retainable_compatibility_returns_retained_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .ownership_borrowed_retainable_compatibility_returns_not_retained_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_borrowed_retainable_compatibility_consumed_sites)
      << ", i1 "
      << (metadata
                  .deterministic_ownership_borrowed_retainable_abi_completion_handoff
              ? 1
              : 0)
      << "}\n\n";
  out << "!100 = !{!\""
      << EscapeCStringLiteral(kObjc3OwnershipSystemHelperRuntimeContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipSystemExtensionLoweringContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipBorrowedRetainableAbiCompletionContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
      << "\", !\"objc3_runtime_copy_memory_management_state_for_testing\", !\"objc3_runtime_copy_arc_debug_state_for_testing\"}\n\n";
  out << "!101 = !{!\""
      << EscapeCStringLiteral(kObjc3OwnershipLiveCleanupRetainableIntegrationContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipSystemHelperRuntimeContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipBorrowedRetainableAbiCompletionContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
      << "\", !\"objc3_runtime_copy_memory_management_state_for_testing\", !\"objc3_runtime_copy_arc_debug_state_for_testing\"}\n\n";
}
