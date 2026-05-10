#include "artifacts/objc3_frontend_artifact_dispatch_runtime_manifest_surfaces.h"

#include <ostream>

#include "lower/contracts/dispatch_abi_marshalling_contracts.h"
#include "lower/contracts/dispatch_surface_classification_contracts.h"
#include "lower/contracts/message_send_selector_lowering_contracts.h"
#include "lower/contracts/nil_receiver_semantics_foldability_contracts.h"
#include "lower/contracts/runtime_dispatch_abi_contracts.h"
#include "lower/contracts/super_dispatch_method_family_contracts.h"

namespace objc3::artifacts::frontend {

void WriteDispatchRuntimeAbiManifestSurfaces(
    std::ostream &manifest,
    const Objc3DispatchSurfaceClassificationContract
        &dispatch_surface_classification_contract,
    const std::string &dispatch_surface_classification_replay_key,
    const Objc3MessageSendSelectorLoweringContract
        &message_send_selector_lowering_contract,
    const std::string &message_send_selector_lowering_replay_key,
    const Objc3DispatchAbiMarshallingContract
        &dispatch_abi_marshalling_contract,
    const std::string &dispatch_abi_marshalling_replay_key,
    const Objc3NilReceiverSemanticsFoldabilityContract
        &nil_receiver_semantics_foldability_contract,
    const std::string &nil_receiver_semantics_foldability_replay_key,
    const Objc3SuperDispatchMethodFamilyContract
        &super_dispatch_method_family_contract,
    const std::string &super_dispatch_method_family_replay_key,
    const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract,
    const std::string &runtime_link_host_link_replay_key,
    const Objc3RuntimeDispatchLoweringAbiContract
        &runtime_dispatch_lowering_abi_contract,
    const std::string &runtime_dispatch_lowering_abi_replay_key) {
  manifest
      << ",\"objc_dispatch_surface_classification_surface\":{\"instance_dispatch_sites\":"
      << dispatch_surface_classification_contract.instance_dispatch_sites
      << ",\"class_dispatch_sites\":"
      << dispatch_surface_classification_contract.class_dispatch_sites
      << ",\"super_dispatch_sites\":"
      << dispatch_surface_classification_contract.super_dispatch_sites
      << ",\"direct_dispatch_sites\":"
      << dispatch_surface_classification_contract.direct_dispatch_sites
      << ",\"dynamic_dispatch_sites\":"
      << dispatch_surface_classification_contract.dynamic_dispatch_sites
      << ",\"instance_entrypoint_family\":\""
      << dispatch_surface_classification_contract.instance_entrypoint_family
      << "\",\"class_entrypoint_family\":\""
      << dispatch_surface_classification_contract.class_entrypoint_family
      << "\",\"super_entrypoint_family\":\""
      << dispatch_surface_classification_contract.super_entrypoint_family
      << "\",\"direct_entrypoint_family\":\""
      << dispatch_surface_classification_contract.direct_entrypoint_family
      << "\",\"dynamic_entrypoint_family\":\""
      << dispatch_surface_classification_contract.dynamic_entrypoint_family
      << "\",\"replay_key\":\""
      << dispatch_surface_classification_replay_key
      << "\",\"deterministic_handoff\":"
      << (dispatch_surface_classification_contract.deterministic ? "true"
                                                                 : "false")
      << "}"
      << ",\"objc_message_send_selector_lowering_surface\":{\"message_send_sites\":"
      << message_send_selector_lowering_contract.message_send_sites
      << ",\"unary_selector_sites\":"
      << message_send_selector_lowering_contract.unary_selector_sites
      << ",\"keyword_selector_sites\":"
      << message_send_selector_lowering_contract.keyword_selector_sites
      << ",\"selector_piece_sites\":"
      << message_send_selector_lowering_contract.selector_piece_sites
      << ",\"argument_expression_sites\":"
      << message_send_selector_lowering_contract.argument_expression_sites
      << ",\"receiver_expression_sites\":"
      << message_send_selector_lowering_contract.receiver_expression_sites
      << ",\"selector_literal_entries\":"
      << message_send_selector_lowering_contract.selector_literal_entries
      << ",\"selector_literal_characters\":"
      << message_send_selector_lowering_contract.selector_literal_characters
      << ",\"replay_key\":\""
      << message_send_selector_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (message_send_selector_lowering_contract.deterministic ? "true"
                                                                : "false")
      << "}"
      << ",\"objc_dispatch_abi_marshalling_surface\":{\"message_send_sites\":"
      << dispatch_abi_marshalling_contract.message_send_sites
      << ",\"receiver_slots_marshaled\":"
      << dispatch_abi_marshalling_contract.receiver_slots_marshaled
      << ",\"selector_slots_marshaled\":"
      << dispatch_abi_marshalling_contract.selector_slots_marshaled
      << ",\"argument_value_slots_marshaled\":"
      << dispatch_abi_marshalling_contract.argument_value_slots_marshaled
      << ",\"argument_padding_slots_marshaled\":"
      << dispatch_abi_marshalling_contract.argument_padding_slots_marshaled
      << ",\"argument_total_slots_marshaled\":"
      << dispatch_abi_marshalling_contract.argument_total_slots_marshaled
      << ",\"total_marshaled_slots\":"
      << dispatch_abi_marshalling_contract.total_marshaled_slots
      << ",\"runtime_dispatch_arg_slots\":"
      << dispatch_abi_marshalling_contract.runtime_dispatch_arg_slots
      << ",\"replay_key\":\""
      << dispatch_abi_marshalling_replay_key
      << "\",\"deterministic_handoff\":"
      << (dispatch_abi_marshalling_contract.deterministic ? "true" : "false")
      << "}"
      << ",\"objc_nil_receiver_semantics_foldability_surface\":{\"message_send_sites\":"
      << nil_receiver_semantics_foldability_contract.message_send_sites
      << ",\"receiver_nil_literal_sites\":"
      << nil_receiver_semantics_foldability_contract.receiver_nil_literal_sites
      << ",\"nil_receiver_semantics_enabled_sites\":"
      << nil_receiver_semantics_foldability_contract
             .nil_receiver_semantics_enabled_sites
      << ",\"nil_receiver_foldable_sites\":"
      << nil_receiver_semantics_foldability_contract.nil_receiver_foldable_sites
      << ",\"nil_receiver_runtime_dispatch_required_sites\":"
      << nil_receiver_semantics_foldability_contract
             .nil_receiver_runtime_dispatch_required_sites
      << ",\"non_nil_receiver_sites\":"
      << nil_receiver_semantics_foldability_contract.non_nil_receiver_sites
      << ",\"contract_violation_sites\":"
      << nil_receiver_semantics_foldability_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << nil_receiver_semantics_foldability_replay_key
      << "\",\"deterministic_handoff\":"
      << (nil_receiver_semantics_foldability_contract.deterministic ? "true"
                                                                    : "false")
      << "}"
      << ",\"objc_super_dispatch_method_family_surface\":{\"message_send_sites\":"
      << super_dispatch_method_family_contract.message_send_sites
      << ",\"receiver_super_identifier_sites\":"
      << super_dispatch_method_family_contract.receiver_super_identifier_sites
      << ",\"super_dispatch_enabled_sites\":"
      << super_dispatch_method_family_contract.super_dispatch_enabled_sites
      << ",\"super_dispatch_requires_class_context_sites\":"
      << super_dispatch_method_family_contract
             .super_dispatch_requires_class_context_sites
      << ",\"method_family_init_sites\":"
      << super_dispatch_method_family_contract.method_family_init_sites
      << ",\"method_family_copy_sites\":"
      << super_dispatch_method_family_contract.method_family_copy_sites
      << ",\"method_family_mutable_copy_sites\":"
      << super_dispatch_method_family_contract.method_family_mutable_copy_sites
      << ",\"method_family_new_sites\":"
      << super_dispatch_method_family_contract.method_family_new_sites
      << ",\"method_family_none_sites\":"
      << super_dispatch_method_family_contract.method_family_none_sites
      << ",\"method_family_returns_retained_result_sites\":"
      << super_dispatch_method_family_contract
             .method_family_returns_retained_result_sites
      << ",\"method_family_returns_related_result_sites\":"
      << super_dispatch_method_family_contract
             .method_family_returns_related_result_sites
      << ",\"contract_violation_sites\":"
      << super_dispatch_method_family_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << super_dispatch_method_family_replay_key
      << "\",\"deterministic_handoff\":"
      << (super_dispatch_method_family_contract.deterministic ? "true"
                                                              : "false")
      << "}"
      << ",\"objc_runtime_link_host_link_surface\":{\"message_send_sites\":"
      << runtime_link_host_link_contract.message_send_sites
      << ",\"runtime_link_required_sites\":"
      << runtime_link_host_link_contract.runtime_link_required_sites
      << ",\"runtime_link_elided_sites\":"
      << runtime_link_host_link_contract.runtime_link_elided_sites
      << ",\"runtime_dispatch_arg_slots\":"
      << runtime_link_host_link_contract.runtime_dispatch_arg_slots
      << ",\"runtime_dispatch_declaration_parameter_count\":"
      << runtime_link_host_link_contract
             .runtime_dispatch_declaration_parameter_count
      << ",\"runtime_dispatch_symbol\":\""
      << runtime_link_host_link_contract.runtime_dispatch_symbol
      << "\",\"default_runtime_dispatch_symbol_binding\":"
      << (runtime_link_host_link_contract.default_runtime_dispatch_symbol_binding
              ? "true"
              : "false")
      << ",\"contract_violation_sites\":"
      << runtime_link_host_link_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << runtime_link_host_link_replay_key
      << "\",\"deterministic_handoff\":"
      << (runtime_link_host_link_contract.deterministic ? "true" : "false")
      << "}"
      << ",\"objc_runtime_dispatch_lowering_abi_contract\":{\"message_send_sites\":"
      << runtime_dispatch_lowering_abi_contract.message_send_sites
      << ",\"fixed_argument_slot_count\":"
      << runtime_dispatch_lowering_abi_contract.fixed_argument_slot_count
      << ",\"runtime_dispatch_parameter_count\":"
      << runtime_dispatch_lowering_abi_contract.runtime_dispatch_parameter_count
      << ",\"lowering_boundary_model\":\""
      << runtime_dispatch_lowering_abi_contract.lowering_boundary_model
      << "\",\"canonical_runtime_dispatch_symbol\":\""
      << runtime_dispatch_lowering_abi_contract.canonical_runtime_dispatch_symbol
      << "\",\"default_lowering_target_symbol\":\""
      << runtime_dispatch_lowering_abi_contract.default_lowering_target_symbol
      << "\",\"selector_lookup_symbol\":\""
      << runtime_dispatch_lowering_abi_contract.selector_lookup_symbol
      << "\",\"selector_handle_type\":\""
      << runtime_dispatch_lowering_abi_contract.selector_handle_type
      << "\",\"receiver_abi_type\":\""
      << runtime_dispatch_lowering_abi_contract.receiver_abi_type
      << "\",\"selector_abi_type\":\""
      << runtime_dispatch_lowering_abi_contract.selector_abi_type
      << "\",\"argument_abi_type\":\""
      << runtime_dispatch_lowering_abi_contract.argument_abi_type
      << "\",\"result_abi_type\":\""
      << runtime_dispatch_lowering_abi_contract.result_abi_type
      << "\",\"selector_operand_model\":\""
      << runtime_dispatch_lowering_abi_contract.selector_operand_model
      << "\",\"selector_handle_model\":\""
      << runtime_dispatch_lowering_abi_contract.selector_handle_model
      << "\",\"argument_padding_model\":\""
      << runtime_dispatch_lowering_abi_contract.argument_padding_model
      << "\",\"default_lowering_target_model\":\""
      << runtime_dispatch_lowering_abi_contract.default_lowering_target_model
      << "\",\"strict_dispatch_error_model\":\""
      << runtime_dispatch_lowering_abi_contract.strict_dispatch_error_model
      << "\",\"deferred_cases_model\":\""
      << runtime_dispatch_lowering_abi_contract.deferred_cases_model
      << "\",\"replay_key\":\""
      << runtime_dispatch_lowering_abi_replay_key
      << "\",\"fail_closed\":"
      << (runtime_dispatch_lowering_abi_contract.fail_closed ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (runtime_dispatch_lowering_abi_contract.deterministic ? "true"
                                                               : "false")
      << "}";
}

}  // namespace objc3::artifacts::frontend
