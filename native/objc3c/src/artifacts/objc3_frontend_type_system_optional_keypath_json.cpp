#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"

#include <sstream>
#include <string>

#include "io/objc3_json.h"
#include "lower/contracts/runtime_metadata_source_record_contracts.h"
#include "runtime/metadata/property_metadata.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

inline constexpr const char
    *kObjc3TypeSystemOptionalKeypathLoweringSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_type_system_optional_keypath_lowering_contract";

}  // namespace

std::string BuildTypeSystemOptionalKeypathLoweringContractJson(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract,
    const Objc3TypeSystemTypeSemanticModelSummary &semantic_summary,
    const std::string &semantic_summary_replay_key,
    const std::string &message_send_selector_lowering_replay_key,
    const std::string &dispatch_abi_marshalling_replay_key,
    const std::string &nil_receiver_semantics_foldability_replay_key,
    const std::string &replay_key) {
  std::ostringstream out;
  const bool source_semantic_model_ready = semantic_summary.deterministic;
  const bool typed_keypath_artifact_emission_deferred =
      contract.deferred_typed_keypath_sites != 0;
  const bool ready_for_native_optional_lowering =
      contract.deterministic && contract.contract_violation_sites == 0;
  const bool ready_for_typed_keypath_artifact_emission =
      contract.deterministic && contract.contract_violation_sites == 0 &&
      contract.live_typed_keypath_artifact_sites ==
          contract.typed_keypath_literal_sites;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathLoweringSurfacePath)
      << "\",\"source_semantic_contract_id\":\""
      << EscapeJsonString(semantic_summary.contract_id)
      << "\",\"optional_model\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathLoweringOptionalModel)
      << "\",\"typed_keypath_model\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathLoweringTypedKeypathModel)
      << "\",\"authority_model\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathLoweringAuthorityModel)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathLoweringFailClosedModel)
      << "\",\"optional_binding_sites\":"
      << contract.optional_binding_sites
      << ",\"optional_binding_clause_sites\":"
      << contract.optional_binding_clause_sites
      << ",\"optional_send_sites\":" << contract.optional_send_sites
      << ",\"nil_coalescing_sites\":" << contract.nil_coalescing_sites
      << ",\"typed_keypath_literal_sites\":"
      << contract.typed_keypath_literal_sites
      << ",\"typed_keypath_self_root_sites\":"
      << contract.typed_keypath_self_root_sites
      << ",\"typed_keypath_class_root_sites\":"
      << contract.typed_keypath_class_root_sites
      << ",\"live_optional_lowering_sites\":"
      << contract.live_optional_lowering_sites
      << ",\"single_evaluation_nil_short_circuit_sites\":"
      << contract.single_evaluation_nil_short_circuit_sites
      << ",\"live_typed_keypath_artifact_sites\":"
      << contract.live_typed_keypath_artifact_sites
      << ",\"deferred_typed_keypath_sites\":"
      << contract.deferred_typed_keypath_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"fail_closed\":true"
      << ",\"source_semantic_model_ready\":"
      << (source_semantic_model_ready ? "true" : "false")
      << ",\"message_send_selector_lowering_ready\":true"
      << ",\"dispatch_abi_marshalling_ready\":true"
      << ",\"nil_receiver_semantics_foldability_ready\":true"
      << ",\"live_optional_binding_lowering_landed\":true"
      << ",\"live_optional_send_lowering_landed\":true"
      << ",\"live_nil_coalescing_lowering_landed\":true"
      << ",\"single_evaluation_nil_short_circuit_landed\":true"
      << ",\"typed_keypath_artifact_emission_deferred\":"
      << (typed_keypath_artifact_emission_deferred ? "true" : "false")
      << ",\"ready_for_native_optional_lowering\":"
      << (ready_for_native_optional_lowering ? "true" : "false")
      << ",\"ready_for_typed_keypath_artifact_emission\":"
      << (ready_for_typed_keypath_artifact_emission ? "true" : "false")
      << ",\"semantic_summary_replay_key\":\""
      << EscapeJsonString(semantic_summary_replay_key)
      << "\",\"message_send_selector_lowering_replay_key\":\""
      << EscapeJsonString(message_send_selector_lowering_replay_key)
      << "\",\"dispatch_abi_marshalling_replay_key\":\""
      << EscapeJsonString(dispatch_abi_marshalling_replay_key)
      << "\",\"nil_receiver_semantics_foldability_replay_key\":\""
      << EscapeJsonString(nil_receiver_semantics_foldability_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key) << "\"}";
  return out.str();
}

std::string BuildTypeSystemOptionalKeypathRuntimeHelperContractJson(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract,
    const Objc3RuntimeSupportLibraryLinkWiringSummary &runtime_link_wiring,
    const std::string &lowering_replay_key) {
  const bool optional_send_runtime_ready =
      runtime_link_wiring.ready_for_runtime_library_consumption &&
      contract.live_optional_lowering_sites >= contract.optional_send_sites;
  const bool typed_keypath_descriptor_handles_ready =
      contract.live_typed_keypath_artifact_sites ==
          contract.typed_keypath_literal_sites &&
      contract.deferred_typed_keypath_sites == 0;
  const bool typed_keypath_runtime_execution_helper_landed = true;
  const bool diagnostic_fail_closed_ready = true;
  std::ostringstream replay_key;
  replay_key << "contract="
             << kObjc3TypeSystemOptionalKeypathRuntimeHelperContractId
             << ";lowering_replay=" << lowering_replay_key
             << ";runtime_link_ready="
             << (runtime_link_wiring.ready_for_runtime_library_consumption
                     ? "true"
                     : "false")
             << ";optional_send_sites=" << contract.optional_send_sites
             << ";live_optional_lowering_sites="
             << contract.live_optional_lowering_sites
             << ";typed_keypath_literal_sites="
             << contract.typed_keypath_literal_sites
             << ";live_typed_keypath_artifact_sites="
             << contract.live_typed_keypath_artifact_sites;
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathRuntimeHelperContractId)
      << "\",\"source_lowering_contract_id\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathLoweringContractId)
      << "\",\"runtime_link_wiring_contract_id\":\""
      << EscapeJsonString(kObjc3RuntimeSupportLibraryLinkWiringContractId)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathRuntimeHelperSurfacePath)
      << "\",\"optional_send_helper_model\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathRuntimeHelperOptionalModel)
      << "\",\"typed_keypath_helper_model\":\""
      << EscapeJsonString(
             kObjc3TypeSystemOptionalKeypathRuntimeHelperTypedKeypathModel)
      << "\",\"diagnostic_fail_closed_model\":\""
      << EscapeJsonString(
             kObjc3TypeSystemOptionalKeypathRuntimeHelperDiagnosticModel)
      << "\",\"public_lookup_selector_symbol\":\""
      << EscapeJsonString(kObjc3RuntimeSupportLibraryLookupSelectorSymbol)
      << "\",\"public_dispatch_i32_symbol\":\""
      << EscapeJsonString(kObjc3RuntimeSupportLibraryDispatchI32Symbol)
      << "\",\"keypath_descriptor_section\":\""
      << EscapeJsonString(kObjc3RuntimeKeypathDescriptorLogicalSection)
      << "\",\"keypath_descriptor_aggregate_symbol\":\"__objc3_sec_keypath_descriptors\""
      << ",\"runtime_library_archive_available\":"
      << (runtime_link_wiring.runtime_library_archive_available ? "true"
                                                                : "false")
      << ",\"runtime_library_consumption_ready\":"
      << (runtime_link_wiring.ready_for_runtime_library_consumption ? "true"
                                                                    : "false")
      << ",\"optional_send_runtime_ready\":"
      << (optional_send_runtime_ready ? "true" : "false")
      << ",\"typed_keypath_descriptor_handles_ready\":"
      << (typed_keypath_descriptor_handles_ready ? "true" : "false")
      << ",\"typed_keypath_runtime_execution_helper_landed\":"
      << (typed_keypath_runtime_execution_helper_landed ? "true" : "false")
      << ",\"diagnostic_fail_closed_ready\":"
      << (diagnostic_fail_closed_ready ? "true" : "false")
      << ",\"fail_closed\":true"
      << ",\"replay_key\":\"" << EscapeJsonString(replay_key.str()) << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
