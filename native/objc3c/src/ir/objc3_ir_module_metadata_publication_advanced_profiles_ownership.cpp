#include "ir/objc3_ir_module_metadata_publication_advanced_profiles_ownership.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRModuleMetadataOwnershipAdvancedProfilePublication(
    const Objc3IRFrontendMetadata &frontend_metadata_,
    std::ostringstream &out) {
  out << "; frontend_objc_system_extension_lowering_profile = cleanup_hook_sites="
      << frontend_metadata_.ownership_system_extension_lowering_cleanup_hook_sites
      << ", resource_local_sites="
      << frontend_metadata_.ownership_system_extension_lowering_resource_local_sites
      << ", cleanup_owned_local_sites="
      << frontend_metadata_
             .ownership_system_extension_lowering_cleanup_owned_local_sites
      << ", resource_move_capture_sites="
      << frontend_metadata_
             .ownership_system_extension_lowering_resource_move_capture_sites
      << ", borrowed_parameter_sites="
      << frontend_metadata_
             .ownership_system_extension_lowering_borrowed_parameter_sites
      << ", borrowed_return_callable_sites="
      << frontend_metadata_
             .ownership_system_extension_lowering_borrowed_return_callable_sites
      << ", borrowed_escape_candidate_sites="
      << frontend_metadata_
             .ownership_system_extension_lowering_borrowed_escape_candidate_sites
      << ", explicit_capture_item_sites="
      << frontend_metadata_
             .ownership_system_extension_lowering_explicit_capture_item_sites
      << ", retainable_family_callable_sites="
      << frontend_metadata_
             .ownership_system_extension_lowering_retainable_family_callable_sites
      << ", retainable_family_operation_callable_sites="
      << frontend_metadata_
             .ownership_system_extension_lowering_retainable_family_operation_callable_sites
      << ", retainable_family_alias_callable_sites="
      << frontend_metadata_
             .ownership_system_extension_lowering_retainable_family_alias_callable_sites
      << ", guard_blocked_sites="
      << frontend_metadata_.ownership_system_extension_lowering_guard_blocked_sites
      << ", contract_violation_sites="
      << frontend_metadata_
             .ownership_system_extension_lowering_contract_violation_sites
      << ", deterministic_ownership_system_extension_lowering_handoff="
      << (frontend_metadata_
                  .deterministic_ownership_system_extension_lowering_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_ownership_borrowed_retainable_abi_profile = returns_borrowed_attribute_sites="
      << frontend_metadata_
             .ownership_borrowed_retainable_returns_borrowed_attribute_sites
      << ", family_retain_sites="
      << frontend_metadata_.ownership_borrowed_retainable_family_retain_sites
      << ", family_release_sites="
      << frontend_metadata_.ownership_borrowed_retainable_family_release_sites
      << ", family_autorelease_sites="
      << frontend_metadata_.ownership_borrowed_retainable_family_autorelease_sites
      << ", compatibility_returns_retained_sites="
      << frontend_metadata_
             .ownership_borrowed_retainable_compatibility_returns_retained_sites
      << ", compatibility_returns_not_retained_sites="
      << frontend_metadata_
             .ownership_borrowed_retainable_compatibility_returns_not_retained_sites
      << ", compatibility_consumed_sites="
      << frontend_metadata_
             .ownership_borrowed_retainable_compatibility_consumed_sites
      << ", deterministic_ownership_borrowed_retainable_abi_completion_handoff="
      << (frontend_metadata_
                  .deterministic_ownership_borrowed_retainable_abi_completion_handoff
              ? "true"
              : "false")
      << "\n";
}
