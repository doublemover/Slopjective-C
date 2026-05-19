#include "artifacts/objc3_frontend_runtime_bootstrap_artifacts.h"

#include <sstream>
#include <string>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

std::string BuildRuntimeBootstrapLoweringSummaryJson(
    const Objc3RuntimeBootstrapLoweringSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"registration_manifest_contract_id\":\""
      << EscapeJsonString(summary.registration_manifest_contract_id)
      << "\",\"bootstrap_semantics_contract_id\":\""
      << EscapeJsonString(summary.bootstrap_semantics_contract_id)
      << "\",\"registration_descriptor_frontend_closure_contract_id\":\""
      << EscapeJsonString(
             summary.registration_descriptor_frontend_closure_contract_id)
      << "\",\"registration_descriptor_artifact\":\""
      << EscapeJsonString(summary.registration_descriptor_artifact)
      << "\",\"bootstrap_surface_path\":\""
      << EscapeJsonString(summary.bootstrap_surface_path)
      << "\",\"lowering_boundary_model\":\""
      << EscapeJsonString(summary.lowering_boundary_model)
      << "\",\"registration_descriptor_handoff_model\":\""
      << EscapeJsonString(summary.registration_descriptor_handoff_model)
      << "\",\"constructor_root_symbol\":\""
      << EscapeJsonString(summary.constructor_root_symbol)
      << "\",\"constructor_init_stub_symbol_prefix\":\""
      << EscapeJsonString(summary.constructor_init_stub_symbol_prefix)
      << "\",\"registration_table_symbol_prefix\":\""
      << EscapeJsonString(summary.registration_table_symbol_prefix)
      << "\",\"image_local_init_state_symbol_prefix\":\""
      << EscapeJsonString(summary.image_local_init_state_symbol_prefix)
      << "\",\"registration_entrypoint_symbol\":\""
      << EscapeJsonString(summary.registration_entrypoint_symbol)
      << "\",\"global_ctor_list_model\":\""
      << EscapeJsonString(summary.global_ctor_list_model)
      << "\",\"registration_table_layout_model\":\""
      << EscapeJsonString(summary.registration_table_layout_model)
      << "\",\"image_local_initialization_model\":\""
      << EscapeJsonString(summary.image_local_initialization_model)
      << "\",\"registration_table_abi_version\":"
      << summary.registration_table_abi_version
      << ",\"registration_table_pointer_field_count\":"
      << summary.registration_table_pointer_field_count
      << ",\"constructor_root_emission_state\":\""
      << EscapeJsonString(summary.constructor_root_emission_state)
      << "\",\"init_stub_emission_state\":\""
      << EscapeJsonString(summary.init_stub_emission_state)
      << "\",\"registration_table_emission_state\":\""
      << EscapeJsonString(summary.registration_table_emission_state)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeBootstrapLoweringSummary(summary) ? "true"
                                                               : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"registration_manifest_contract_ready\":"
      << (summary.registration_manifest_contract_ready ? "true" : "false")
      << ",\"bootstrap_semantics_contract_ready\":"
      << (summary.bootstrap_semantics_contract_ready ? "true" : "false")
      << ",\"registration_descriptor_frontend_closure_contract_ready\":"
      << (summary.registration_descriptor_frontend_closure_contract_ready
              ? "true"
              : "false")
      << ",\"lowering_contract_published\":"
      << (summary.lowering_contract_published ? "true" : "false")
      << ",\"manifest_authority_preserved\":"
      << (summary.manifest_authority_preserved ? "true" : "false")
      << ",\"no_bootstrap_ir_materialization_yet\":"
      << (summary.no_bootstrap_ir_materialization_yet ? "true" : "false")
      << ",\"bootstrap_ir_materialization_landed\":"
      << (summary.bootstrap_ir_materialization_landed ? "true" : "false")
      << ",\"image_local_initialization_landed\":"
      << (summary.image_local_initialization_landed ? "true" : "false")
      << ",\"ready_for_bootstrap_materialization\":"
      << (summary.ready_for_bootstrap_materialization ? "true" : "false")
      << ",\"registration_manifest_replay_key\":\""
      << EscapeJsonString(summary.registration_manifest_replay_key)
      << "\",\"bootstrap_semantics_replay_key\":\""
      << EscapeJsonString(summary.bootstrap_semantics_replay_key)
      << "\",\"registration_descriptor_frontend_closure_replay_key\":\""
      << EscapeJsonString(
             summary.registration_descriptor_frontend_closure_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
