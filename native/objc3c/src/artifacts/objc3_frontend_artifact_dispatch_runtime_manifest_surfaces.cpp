#include "artifacts/objc3_frontend_artifact_dispatch_runtime_manifest_surfaces.h"

#include <ostream>

namespace objc3::artifacts::frontend {

void WriteDispatchRuntimeAbiManifestSurfaces(
    std::ostream &manifest,
    const Objc3DispatchSurfaceClassificationSnapshot
        &dispatch_surface_classification,
    const std::string &dispatch_surface_classification_replay_key,
    const Objc3MessageSendSelectorLoweringSnapshot
        &message_send_selector_lowering,
    const std::string &message_send_selector_lowering_replay_key,
    const Objc3DispatchAbiMarshallingSnapshot &dispatch_abi_marshalling,
    const std::string &dispatch_abi_marshalling_replay_key,
    const Objc3NilReceiverSemanticsFoldabilitySnapshot
        &nil_receiver_semantics_foldability,
    const std::string &nil_receiver_semantics_foldability_replay_key,
    const Objc3SuperDispatchMethodFamilySnapshot &super_dispatch_method_family,
    const std::string &super_dispatch_method_family_replay_key,
    const Objc3RuntimeLinkHostLinkSnapshot &runtime_link_host_link,
    const std::string &runtime_link_host_link_replay_key,
    const Objc3RuntimeDispatchLoweringAbiSnapshot
        &runtime_dispatch_lowering_abi,
    const std::string &runtime_dispatch_lowering_abi_replay_key) {
  manifest
      << ",\"objc_dispatch_surface_classification_surface\":{\"instance_dispatch_sites\":"
      << dispatch_surface_classification.instance_dispatch_sites
      << ",\"class_dispatch_sites\":"
      << dispatch_surface_classification.class_dispatch_sites
      << ",\"super_dispatch_sites\":"
      << dispatch_surface_classification.super_dispatch_sites
      << ",\"direct_dispatch_sites\":"
      << dispatch_surface_classification.direct_dispatch_sites
      << ",\"dynamic_dispatch_sites\":"
      << dispatch_surface_classification.dynamic_dispatch_sites
      << ",\"instance_entrypoint_family\":\""
      << dispatch_surface_classification.instance_entrypoint_family
      << "\",\"class_entrypoint_family\":\""
      << dispatch_surface_classification.class_entrypoint_family
      << "\",\"super_entrypoint_family\":\""
      << dispatch_surface_classification.super_entrypoint_family
      << "\",\"direct_entrypoint_family\":\""
      << dispatch_surface_classification.direct_entrypoint_family
      << "\",\"dynamic_entrypoint_family\":\""
      << dispatch_surface_classification.dynamic_entrypoint_family
      << "\",\"replay_key\":\""
      << dispatch_surface_classification_replay_key
      << "\",\"deterministic_handoff\":"
      << (dispatch_surface_classification.deterministic ? "true" : "false")
      << "}"
      << ",\"objc_message_send_selector_lowering_surface\":{\"message_send_sites\":"
      << message_send_selector_lowering.message_send_sites
      << ",\"unary_selector_sites\":"
      << message_send_selector_lowering.unary_selector_sites
      << ",\"keyword_selector_sites\":"
      << message_send_selector_lowering.keyword_selector_sites
      << ",\"selector_piece_sites\":"
      << message_send_selector_lowering.selector_piece_sites
      << ",\"argument_expression_sites\":"
      << message_send_selector_lowering.argument_expression_sites
      << ",\"receiver_expression_sites\":"
      << message_send_selector_lowering.receiver_expression_sites
      << ",\"selector_literal_entries\":"
      << message_send_selector_lowering.selector_literal_entries
      << ",\"selector_literal_characters\":"
      << message_send_selector_lowering.selector_literal_characters
      << ",\"replay_key\":\""
      << message_send_selector_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (message_send_selector_lowering.deterministic ? "true" : "false")
      << "}"
      << ",\"objc_dispatch_abi_marshalling_surface\":{\"message_send_sites\":"
      << dispatch_abi_marshalling.message_send_sites
      << ",\"receiver_slots_marshaled\":"
      << dispatch_abi_marshalling.receiver_slots_marshaled
      << ",\"selector_slots_marshaled\":"
      << dispatch_abi_marshalling.selector_slots_marshaled
      << ",\"argument_value_slots_marshaled\":"
      << dispatch_abi_marshalling.argument_value_slots_marshaled
      << ",\"argument_padding_slots_marshaled\":"
      << dispatch_abi_marshalling.argument_padding_slots_marshaled
      << ",\"argument_total_slots_marshaled\":"
      << dispatch_abi_marshalling.argument_total_slots_marshaled
      << ",\"total_marshaled_slots\":"
      << dispatch_abi_marshalling.total_marshaled_slots
      << ",\"runtime_dispatch_arg_slots\":"
      << dispatch_abi_marshalling.runtime_dispatch_arg_slots
      << ",\"replay_key\":\""
      << dispatch_abi_marshalling_replay_key
      << "\",\"deterministic_handoff\":"
      << (dispatch_abi_marshalling.deterministic ? "true" : "false")
      << "}"
      << ",\"objc_nil_receiver_semantics_foldability_surface\":{\"message_send_sites\":"
      << nil_receiver_semantics_foldability.message_send_sites
      << ",\"receiver_nil_literal_sites\":"
      << nil_receiver_semantics_foldability.receiver_nil_literal_sites
      << ",\"nil_receiver_semantics_enabled_sites\":"
      << nil_receiver_semantics_foldability
             .nil_receiver_semantics_enabled_sites
      << ",\"nil_receiver_foldable_sites\":"
      << nil_receiver_semantics_foldability.nil_receiver_foldable_sites
      << ",\"nil_receiver_runtime_dispatch_required_sites\":"
      << nil_receiver_semantics_foldability
             .nil_receiver_runtime_dispatch_required_sites
      << ",\"non_nil_receiver_sites\":"
      << nil_receiver_semantics_foldability.non_nil_receiver_sites
      << ",\"contract_violation_sites\":"
      << nil_receiver_semantics_foldability.contract_violation_sites
      << ",\"replay_key\":\""
      << nil_receiver_semantics_foldability_replay_key
      << "\",\"deterministic_handoff\":"
      << (nil_receiver_semantics_foldability.deterministic ? "true" : "false")
      << "}"
      << ",\"objc_super_dispatch_method_family_surface\":{\"message_send_sites\":"
      << super_dispatch_method_family.message_send_sites
      << ",\"receiver_super_identifier_sites\":"
      << super_dispatch_method_family.receiver_super_identifier_sites
      << ",\"super_dispatch_enabled_sites\":"
      << super_dispatch_method_family.super_dispatch_enabled_sites
      << ",\"super_dispatch_requires_class_context_sites\":"
      << super_dispatch_method_family.super_dispatch_requires_class_context_sites
      << ",\"method_family_alloc_sites\":"
      << super_dispatch_method_family.method_family_alloc_sites
      << ",\"method_family_init_sites\":"
      << super_dispatch_method_family.method_family_init_sites
      << ",\"method_family_copy_sites\":"
      << super_dispatch_method_family.method_family_copy_sites
      << ",\"method_family_mutable_copy_sites\":"
      << super_dispatch_method_family.method_family_mutable_copy_sites
      << ",\"method_family_new_sites\":"
      << super_dispatch_method_family.method_family_new_sites
      << ",\"method_family_none_sites\":"
      << super_dispatch_method_family.method_family_none_sites
      << ",\"method_family_returns_retained_result_sites\":"
      << super_dispatch_method_family.method_family_returns_retained_result_sites
      << ",\"method_family_returns_related_result_sites\":"
      << super_dispatch_method_family.method_family_returns_related_result_sites
      << ",\"contract_violation_sites\":"
      << super_dispatch_method_family.contract_violation_sites
      << ",\"replay_key\":\""
      << super_dispatch_method_family_replay_key
      << "\",\"deterministic_handoff\":"
      << (super_dispatch_method_family.deterministic ? "true" : "false")
      << "}"
      << ",\"objc_runtime_link_host_link_surface\":{\"message_send_sites\":"
      << runtime_link_host_link.message_send_sites
      << ",\"runtime_link_required_sites\":"
      << runtime_link_host_link.runtime_link_required_sites
      << ",\"runtime_link_elided_sites\":"
      << runtime_link_host_link.runtime_link_elided_sites
      << ",\"runtime_dispatch_arg_slots\":"
      << runtime_link_host_link.runtime_dispatch_arg_slots
      << ",\"runtime_dispatch_declaration_parameter_count\":"
      << runtime_link_host_link.runtime_dispatch_declaration_parameter_count
      << ",\"runtime_dispatch_symbol\":\""
      << runtime_link_host_link.runtime_dispatch_symbol
      << "\",\"default_runtime_dispatch_symbol_binding\":"
      << (runtime_link_host_link.default_runtime_dispatch_symbol_binding
              ? "true"
              : "false")
      << ",\"contract_violation_sites\":"
      << runtime_link_host_link.contract_violation_sites
      << ",\"replay_key\":\""
      << runtime_link_host_link_replay_key
      << "\",\"deterministic_handoff\":"
      << (runtime_link_host_link.deterministic ? "true" : "false")
      << "}"
      << ",\"objc_runtime_dispatch_lowering_abi_contract\":{\"message_send_sites\":"
      << runtime_dispatch_lowering_abi.message_send_sites
      << ",\"fixed_argument_slot_count\":"
      << runtime_dispatch_lowering_abi.fixed_argument_slot_count
      << ",\"runtime_dispatch_parameter_count\":"
      << runtime_dispatch_lowering_abi.runtime_dispatch_parameter_count
      << ",\"lowering_boundary_model\":\""
      << runtime_dispatch_lowering_abi.lowering_boundary_model
      << "\",\"canonical_runtime_dispatch_symbol\":\""
      << runtime_dispatch_lowering_abi.canonical_runtime_dispatch_symbol
      << "\",\"default_lowering_target_symbol\":\""
      << runtime_dispatch_lowering_abi.default_lowering_target_symbol
      << "\",\"selector_lookup_symbol\":\""
      << runtime_dispatch_lowering_abi.selector_lookup_symbol
      << "\",\"selector_handle_type\":\""
      << runtime_dispatch_lowering_abi.selector_handle_type
      << "\",\"receiver_abi_type\":\""
      << runtime_dispatch_lowering_abi.receiver_abi_type
      << "\",\"selector_abi_type\":\""
      << runtime_dispatch_lowering_abi.selector_abi_type
      << "\",\"argument_abi_type\":\""
      << runtime_dispatch_lowering_abi.argument_abi_type
      << "\",\"result_abi_type\":\""
      << runtime_dispatch_lowering_abi.result_abi_type
      << "\",\"selector_operand_model\":\""
      << runtime_dispatch_lowering_abi.selector_operand_model
      << "\",\"selector_handle_model\":\""
      << runtime_dispatch_lowering_abi.selector_handle_model
      << "\",\"argument_padding_model\":\""
      << runtime_dispatch_lowering_abi.argument_padding_model
      << "\",\"default_lowering_target_model\":\""
      << runtime_dispatch_lowering_abi.default_lowering_target_model
      << "\",\"strict_dispatch_error_model\":\""
      << runtime_dispatch_lowering_abi.strict_dispatch_error_model
      << "\",\"deferred_cases_model\":\""
      << runtime_dispatch_lowering_abi.deferred_cases_model
      << "\",\"replay_key\":\""
      << runtime_dispatch_lowering_abi_replay_key
      << "\",\"fail_closed\":"
      << (runtime_dispatch_lowering_abi.fail_closed ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (runtime_dispatch_lowering_abi.deterministic ? "true" : "false")
      << "}";
}

}  // namespace objc3::artifacts::frontend
