#include "ir/objc3_ir_module_metadata_publication_advanced_profiles_interop_meta.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRModuleMetadataInteropMetaprogrammingAdvancedProfilePublication(
    const Objc3IRFrontendMetadata &frontend_metadata_,
    std::ostringstream &out) {
  out << "; frontend_objc_interop_interop_lowering_profile = foreign_callable_sites="
      << frontend_metadata_.interop_interop_lowering_foreign_callable_sites
      << ", c_foreign_callable_sites="
      << frontend_metadata_.interop_interop_lowering_c_foreign_callable_sites
      << ", objc_runtime_parity_callable_sites="
      << frontend_metadata_.interop_interop_lowering_objc_runtime_parity_callable_sites
      << ", ownership_bridge_callable_sites="
      << frontend_metadata_.interop_interop_lowering_ownership_bridge_callable_sites
      << ", error_surface_sites="
      << frontend_metadata_.interop_interop_lowering_error_surface_sites
      << ", async_boundary_sites="
      << frontend_metadata_.interop_interop_lowering_async_boundary_sites
      << ", swift_concurrency_metadata_sites="
      << frontend_metadata_.interop_interop_lowering_swift_concurrency_metadata_sites
      << ", interface_preserved_foreign_callable_sites="
      << frontend_metadata_.interop_interop_lowering_interface_preserved_foreign_callable_sites
      << ", interface_preserved_metadata_annotation_sites="
      << frontend_metadata_.interop_interop_lowering_interface_preserved_metadata_annotation_sites
      << ", guard_blocked_sites="
      << frontend_metadata_.interop_interop_lowering_guard_blocked_sites
      << ", contract_violation_sites="
      << frontend_metadata_.interop_interop_lowering_contract_violation_sites
      << ", deterministic_interop_interop_lowering_handoff="
      << (frontend_metadata_.deterministic_interop_interop_lowering_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_metaprogramming_expansion_lowering_profile = derive_inventory_sites="
      << frontend_metadata_.metaprogramming_expansion_lowering_derive_inventory_sites
      << ", derived_selector_artifact_sites="
      << frontend_metadata_
             .metaprogramming_expansion_lowering_derived_selector_artifact_sites
      << ", macro_replay_visible_sites="
      << frontend_metadata_
             .metaprogramming_expansion_lowering_macro_replay_visible_sites
      << ", property_behavior_sites="
      << frontend_metadata_.metaprogramming_expansion_lowering_property_behavior_sites
      << ", synthesized_binding_sites="
      << frontend_metadata_.metaprogramming_expansion_lowering_synthesized_binding_sites
      << ", synthesized_getter_sites="
      << frontend_metadata_.metaprogramming_expansion_lowering_synthesized_getter_sites
      << ", synthesized_setter_sites="
      << frontend_metadata_.metaprogramming_expansion_lowering_synthesized_setter_sites
      << ", replay_visible_metadata_sites="
      << frontend_metadata_
             .metaprogramming_expansion_lowering_replay_visible_metadata_sites
      << ", guard_blocked_sites="
      << frontend_metadata_.metaprogramming_expansion_lowering_guard_blocked_sites
      << ", contract_violation_sites="
      << frontend_metadata_
             .metaprogramming_expansion_lowering_contract_violation_sites
      << ", deterministic_metaprogramming_expansion_lowering_handoff="
      << (frontend_metadata_.deterministic_metaprogramming_expansion_lowering_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_metaprogramming_synthesized_emission_profile = emitted_derive_method_sites="
      << frontend_metadata_.metaprogramming_synthesized_emitted_derive_method_sites
      << ", emitted_macro_artifact_sites="
      << frontend_metadata_.metaprogramming_synthesized_emitted_macro_artifact_sites
      << ", emitted_property_behavior_artifact_sites="
      << frontend_metadata_
             .metaprogramming_synthesized_emitted_property_behavior_artifact_sites
      << ", emitted_global_artifact_sites="
      << frontend_metadata_.metaprogramming_synthesized_emitted_global_artifact_sites
      << ", emitted_runtime_method_list_sites="
      << frontend_metadata_
             .metaprogramming_synthesized_emitted_runtime_method_list_sites
      << ", guard_blocked_sites="
      << frontend_metadata_.metaprogramming_synthesized_guard_blocked_sites
      << ", contract_violation_sites="
      << frontend_metadata_.metaprogramming_synthesized_contract_violation_sites
      << ", deterministic_metaprogramming_synthesized_emission_handoff="
      << (frontend_metadata_.deterministic_metaprogramming_synthesized_emission_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_dispatch_metadata_interface_profile = local_direct_callable_record_count="
      << frontend_metadata_
             .dispatch_dispatch_metadata_local_direct_callable_record_count
      << ", local_final_callable_record_count="
      << frontend_metadata_
             .dispatch_dispatch_metadata_local_final_callable_record_count
      << ", local_final_container_record_count="
      << frontend_metadata_
             .dispatch_dispatch_metadata_local_final_container_record_count
      << ", local_sealed_container_record_count="
      << frontend_metadata_
             .dispatch_dispatch_metadata_local_sealed_container_record_count
      << ", imported_module_count="
      << frontend_metadata_.dispatch_dispatch_metadata_imported_module_count
      << ", imported_direct_callable_record_count="
      << frontend_metadata_
             .dispatch_dispatch_metadata_imported_direct_callable_record_count
      << ", imported_final_callable_record_count="
      << frontend_metadata_
             .dispatch_dispatch_metadata_imported_final_callable_record_count
      << ", imported_final_container_record_count="
      << frontend_metadata_
             .dispatch_dispatch_metadata_imported_final_container_record_count
      << ", imported_sealed_container_record_count="
      << frontend_metadata_
             .dispatch_dispatch_metadata_imported_sealed_container_record_count
      << ", runtime_import_artifact_ready="
      << (frontend_metadata_
                  .dispatch_dispatch_metadata_runtime_import_artifact_ready
              ? "true"
              : "false")
      << ", separate_compilation_preservation_ready="
      << (frontend_metadata_
                  .dispatch_dispatch_metadata_separate_compilation_preservation_ready
              ? "true"
              : "false")
      << ", deterministic_dispatch_dispatch_metadata_interface_handoff="
      << (frontend_metadata_
                  .deterministic_dispatch_dispatch_metadata_interface_handoff
              ? "true"
              : "false")
      << "\n";
}
