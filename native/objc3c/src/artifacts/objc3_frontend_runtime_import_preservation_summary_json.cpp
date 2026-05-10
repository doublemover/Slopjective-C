#include "artifacts/objc3_frontend_runtime_import_artifacts.h"

#include <sstream>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {

using objc3::io::EscapeJsonString;

std::string BuildRuntimeStorageReflectionArtifactPreservationSummaryJson(
    const Objc3RuntimeStorageReflectionArtifactPreservationSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(summary.source_contract_id)
      << "\",\"dispatch_and_synthesized_accessor_lowering_surface_contract_id\":\""
      << EscapeJsonString(
             summary.dispatch_and_synthesized_accessor_lowering_surface_contract_id)
      << "\",\"executable_property_accessor_layout_lowering_contract_id\":\""
      << EscapeJsonString(
             summary.executable_property_accessor_layout_lowering_contract_id)
      << "\",\"executable_ivar_layout_emission_contract_id\":\""
      << EscapeJsonString(
             summary.executable_ivar_layout_emission_contract_id)
      << "\",\"executable_synthesized_accessor_property_lowering_contract_id\":\""
      << EscapeJsonString(
             summary.executable_synthesized_accessor_property_lowering_contract_id)
      << "\",\"surface_path\":\""
      << EscapeJsonString(summary.surface_path)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(summary.import_artifact_member_name)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"preservation_model\":\""
      << EscapeJsonString(summary.preservation_model)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"local_property_descriptor_count\":"
      << summary.local_property_descriptor_count
      << ",\"local_ivar_descriptor_count\":"
      << summary.local_ivar_descriptor_count
      << ",\"implementation_owned_property_entries\":"
      << summary.implementation_owned_property_entries
      << ",\"synthesized_accessor_owner_entries\":"
      << summary.synthesized_accessor_owner_entries
      << ",\"synthesized_getter_entries\":"
      << summary.synthesized_getter_entries
      << ",\"synthesized_setter_entries\":"
      << summary.synthesized_setter_entries
      << ",\"synthesized_accessor_entries\":"
      << summary.synthesized_accessor_entries
      << ",\"current_property_read_entries\":"
      << summary.current_property_read_entries
      << ",\"current_property_write_entries\":"
      << summary.current_property_write_entries
      << ",\"current_property_exchange_entries\":"
      << summary.current_property_exchange_entries
      << ",\"weak_current_property_load_entries\":"
      << summary.weak_current_property_load_entries
      << ",\"weak_current_property_store_entries\":"
      << summary.weak_current_property_store_entries
      << ",\"ivar_layout_entries\":" << summary.ivar_layout_entries
      << ",\"ivar_layout_owner_entries\":"
      << summary.ivar_layout_owner_entries
      << ",\"runtime_import_artifact_ready\":"
      << (summary.runtime_import_artifact_ready ? "true" : "false")
      << ",\"separate_compilation_preservation_ready\":"
      << (summary.separate_compilation_preservation_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildRuntimeBlockOwnershipArtifactPreservationSummaryJson(
    const Objc3RuntimeBlockOwnershipArtifactPreservationSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(summary.source_contract_id)
      << "\",\"block_object_invoke_thunk_lowering_contract_id\":\""
      << EscapeJsonString(
             summary.block_object_invoke_thunk_lowering_contract_id)
      << "\",\"block_byref_helper_lowering_contract_id\":\""
      << EscapeJsonString(summary.block_byref_helper_lowering_contract_id)
      << "\",\"block_escape_runtime_hook_lowering_contract_id\":\""
      << EscapeJsonString(
             summary.block_escape_runtime_hook_lowering_contract_id)
      << "\",\"runtime_support_library_link_wiring_contract_id\":\""
      << EscapeJsonString(
             summary.runtime_support_library_link_wiring_contract_id)
      << "\",\"surface_path\":\""
      << EscapeJsonString(summary.surface_path)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(summary.import_artifact_member_name)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"preservation_model\":\""
      << EscapeJsonString(summary.preservation_model)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"local_block_literal_sites\":"
      << summary.local_block_literal_sites
      << ",\"local_invoke_trampoline_symbolized_sites\":"
      << summary.local_invoke_trampoline_symbolized_sites
      << ",\"local_copy_helper_required_sites\":"
      << summary.local_copy_helper_required_sites
      << ",\"local_dispose_helper_required_sites\":"
      << summary.local_dispose_helper_required_sites
      << ",\"local_copy_helper_symbolized_sites\":"
      << summary.local_copy_helper_symbolized_sites
      << ",\"local_dispose_helper_symbolized_sites\":"
      << summary.local_dispose_helper_symbolized_sites
      << ",\"local_escape_to_heap_sites\":"
      << summary.local_escape_to_heap_sites
      << ",\"local_byref_layout_symbolized_sites\":"
      << summary.local_byref_layout_symbolized_sites
      << ",\"runtime_import_artifact_ready\":"
      << (summary.runtime_import_artifact_ready ? "true" : "false")
      << ",\"separate_compilation_preservation_ready\":"
      << (summary.separate_compilation_preservation_ready ? "true" : "false")
      << ",\"runtime_support_library_link_wiring_ready\":"
      << (summary.runtime_support_library_link_wiring_ready ? "true"
                                                            : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
