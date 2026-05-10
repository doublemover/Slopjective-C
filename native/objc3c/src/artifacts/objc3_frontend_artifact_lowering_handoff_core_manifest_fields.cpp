#include "artifacts/objc3_frontend_artifact_lowering_handoff_manifest_field_modules.h"

#include <string>

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactLoweringHandoffCoreManifestFields(
    std::ostringstream &manifest,
    const Objc3FrontendArtifactCoreLoweringPlan &core_lowering_plan) {
  const bool property_synthesis_ivar_binding_handoff_deterministic =
      core_lowering_plan.property_synthesis_ivar_binding_handoff_deterministic;
  const Objc3PropertySynthesisIvarBindingSummary
      &property_synthesis_ivar_binding_summary =
          core_lowering_plan.property_synthesis_ivar_binding_summary;
  const std::string &property_synthesis_ivar_binding_replay_key =
      core_lowering_plan.property_synthesis_ivar_binding_replay_key;
  const Objc3IdClassSelObjectPointerTypecheckContract
      &id_class_sel_object_pointer_typecheck_contract =
          core_lowering_plan.id_class_sel_object_pointer_typecheck_contract;
  const std::string &id_class_sel_object_pointer_typecheck_replay_key =
      core_lowering_plan.id_class_sel_object_pointer_typecheck_replay_key;
  const Objc3MessageSendSelectorLoweringContract
      &message_send_selector_lowering_contract =
          core_lowering_plan.message_send_selector_lowering_contract;
  const std::string &message_send_selector_lowering_replay_key =
      core_lowering_plan.message_send_selector_lowering_replay_key;
  const Objc3DispatchAbiMarshallingContract &dispatch_abi_marshalling_contract =
      core_lowering_plan.dispatch_abi_marshalling_contract;
  const std::string &dispatch_abi_marshalling_replay_key =
      core_lowering_plan.dispatch_abi_marshalling_replay_key;
  const Objc3NilReceiverSemanticsFoldabilityContract
      &nil_receiver_semantics_foldability_contract =
          core_lowering_plan.nil_receiver_semantics_foldability_contract;
  const std::string &nil_receiver_semantics_foldability_replay_key =
      core_lowering_plan.nil_receiver_semantics_foldability_replay_key;
  const Objc3SuperDispatchMethodFamilyContract
      &super_dispatch_method_family_contract =
          core_lowering_plan.super_dispatch_method_family_contract;
  const std::string &super_dispatch_method_family_replay_key =
      core_lowering_plan.super_dispatch_method_family_replay_key;
  const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract =
      core_lowering_plan.runtime_link_host_link_contract;
  const std::string &runtime_link_host_link_replay_key =
      core_lowering_plan.runtime_link_host_link_replay_key;

  manifest << ",\"deterministic_property_synthesis_ivar_binding_handoff\":"
           << (property_synthesis_ivar_binding_handoff_deterministic ? "true" : "false")
           << ",\"property_synthesis_sites\":"
           << property_synthesis_ivar_binding_summary.property_synthesis_sites
           << ",\"property_synthesis_explicit_ivar_bindings\":"
           << property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings
           << ",\"property_synthesis_default_ivar_bindings\":"
           << property_synthesis_ivar_binding_summary.property_synthesis_default_ivar_bindings
           << ",\"interface_owned_property_synthesis_sites\":"
           << property_synthesis_ivar_binding_summary.interface_owned_property_synthesis_sites
           << ",\"implementation_property_redeclaration_sites\":"
           << property_synthesis_ivar_binding_summary.implementation_property_redeclaration_sites
           << ",\"ivar_binding_sites\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_sites
           << ",\"ivar_binding_resolved\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_resolved
           << ",\"ivar_binding_missing\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_missing
           << ",\"ivar_binding_conflicts\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_conflicts
           << ",\"lowering_property_synthesis_ivar_binding_replay_key\":\""
           << property_synthesis_ivar_binding_replay_key
           << "\""
           << ",\"deterministic_id_class_sel_object_pointer_typecheck_handoff\":"
           << (id_class_sel_object_pointer_typecheck_contract.deterministic ? "true" : "false")
           << ",\"id_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.id_typecheck_sites
           << ",\"class_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.class_typecheck_sites
           << ",\"sel_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.sel_typecheck_sites
           << ",\"object_pointer_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.object_pointer_typecheck_sites
           << ",\"id_class_sel_object_pointer_typecheck_sites_total\":"
           << id_class_sel_object_pointer_typecheck_contract.total_typecheck_sites
           << ",\"lowering_id_class_sel_object_pointer_typecheck_replay_key\":\""
           << id_class_sel_object_pointer_typecheck_replay_key
           << "\""
           << ",\"deterministic_message_send_selector_lowering_handoff\":"
           << (message_send_selector_lowering_contract.deterministic ? "true" : "false")
           << ",\"message_send_selector_lowering_sites\":"
           << message_send_selector_lowering_contract.message_send_sites
           << ",\"message_send_selector_lowering_unary_sites\":"
           << message_send_selector_lowering_contract.unary_selector_sites
           << ",\"message_send_selector_lowering_keyword_sites\":"
           << message_send_selector_lowering_contract.keyword_selector_sites
           << ",\"message_send_selector_lowering_selector_piece_sites\":"
           << message_send_selector_lowering_contract.selector_piece_sites
           << ",\"message_send_selector_lowering_argument_expression_sites\":"
           << message_send_selector_lowering_contract.argument_expression_sites
           << ",\"message_send_selector_lowering_receiver_sites\":"
           << message_send_selector_lowering_contract.receiver_expression_sites
           << ",\"message_send_selector_lowering_selector_literal_entries\":"
           << message_send_selector_lowering_contract.selector_literal_entries
           << ",\"message_send_selector_lowering_selector_literal_characters\":"
           << message_send_selector_lowering_contract.selector_literal_characters
           << ",\"lowering_message_send_selector_lowering_replay_key\":\""
           << message_send_selector_lowering_replay_key
           << "\""
           << ",\"deterministic_dispatch_abi_marshalling_handoff\":"
           << (dispatch_abi_marshalling_contract.deterministic ? "true" : "false")
           << ",\"dispatch_abi_marshalling_message_send_sites\":"
           << dispatch_abi_marshalling_contract.message_send_sites
           << ",\"dispatch_abi_marshalling_receiver_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.receiver_slots_marshaled
           << ",\"dispatch_abi_marshalling_selector_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.selector_slots_marshaled
           << ",\"dispatch_abi_marshalling_argument_value_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_value_slots_marshaled
           << ",\"dispatch_abi_marshalling_argument_padding_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_padding_slots_marshaled
           << ",\"dispatch_abi_marshalling_argument_total_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_total_slots_marshaled
           << ",\"dispatch_abi_marshalling_total_marshaled_slots\":"
           << dispatch_abi_marshalling_contract.total_marshaled_slots
           << ",\"dispatch_abi_marshalling_runtime_dispatch_arg_slots\":"
           << dispatch_abi_marshalling_contract.runtime_dispatch_arg_slots
           << ",\"lowering_dispatch_abi_marshalling_replay_key\":\""
           << dispatch_abi_marshalling_replay_key
           << "\""
           << ",\"deterministic_nil_receiver_semantics_foldability_handoff\":"
           << (nil_receiver_semantics_foldability_contract.deterministic ? "true" : "false")
           << ",\"nil_receiver_semantics_foldability_message_send_sites\":"
           << nil_receiver_semantics_foldability_contract.message_send_sites
           << ",\"nil_receiver_semantics_foldability_receiver_nil_literal_sites\":"
           << nil_receiver_semantics_foldability_contract.receiver_nil_literal_sites
           << ",\"nil_receiver_semantics_foldability_enabled_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_semantics_enabled_sites
           << ",\"nil_receiver_semantics_foldability_foldable_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_foldable_sites
           << ",\"nil_receiver_semantics_foldability_runtime_dispatch_required_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_runtime_dispatch_required_sites
           << ",\"nil_receiver_semantics_foldability_non_nil_receiver_sites\":"
           << nil_receiver_semantics_foldability_contract.non_nil_receiver_sites
           << ",\"nil_receiver_semantics_foldability_contract_violation_sites\":"
           << nil_receiver_semantics_foldability_contract.contract_violation_sites
           << ",\"lowering_nil_receiver_semantics_foldability_replay_key\":\""
           << nil_receiver_semantics_foldability_replay_key
           << "\""
           << ",\"deterministic_super_dispatch_method_family_handoff\":"
           << (super_dispatch_method_family_contract.deterministic ? "true" : "false")
           << ",\"super_dispatch_method_family_message_send_sites\":"
           << super_dispatch_method_family_contract.message_send_sites
           << ",\"super_dispatch_method_family_receiver_super_identifier_sites\":"
           << super_dispatch_method_family_contract.receiver_super_identifier_sites
           << ",\"super_dispatch_method_family_enabled_sites\":"
           << super_dispatch_method_family_contract.super_dispatch_enabled_sites
           << ",\"super_dispatch_method_family_requires_class_context_sites\":"
           << super_dispatch_method_family_contract.super_dispatch_requires_class_context_sites
           << ",\"super_dispatch_method_family_init_sites\":"
           << super_dispatch_method_family_contract.method_family_init_sites
           << ",\"super_dispatch_method_family_copy_sites\":"
           << super_dispatch_method_family_contract.method_family_copy_sites
           << ",\"super_dispatch_method_family_mutable_copy_sites\":"
           << super_dispatch_method_family_contract.method_family_mutable_copy_sites
           << ",\"super_dispatch_method_family_new_sites\":"
           << super_dispatch_method_family_contract.method_family_new_sites
           << ",\"super_dispatch_method_family_none_sites\":"
           << super_dispatch_method_family_contract.method_family_none_sites
           << ",\"super_dispatch_method_family_returns_retained_result_sites\":"
           << super_dispatch_method_family_contract.method_family_returns_retained_result_sites
           << ",\"super_dispatch_method_family_returns_related_result_sites\":"
           << super_dispatch_method_family_contract.method_family_returns_related_result_sites
           << ",\"super_dispatch_method_family_contract_violation_sites\":"
           << super_dispatch_method_family_contract.contract_violation_sites
           << ",\"lowering_super_dispatch_method_family_replay_key\":\""
           << super_dispatch_method_family_replay_key
           << "\""
           << ",\"deterministic_runtime_link_host_link_handoff\":"
           << (runtime_link_host_link_contract.deterministic ? "true" : "false")
           << ",\"runtime_link_host_link_message_send_sites\":"
           << runtime_link_host_link_contract.message_send_sites
           << ",\"runtime_link_host_link_required_runtime_link_sites\":"
           << runtime_link_host_link_contract.runtime_link_required_sites
           << ",\"runtime_link_host_link_elided_runtime_link_sites\":"
           << runtime_link_host_link_contract.runtime_link_elided_sites
           << ",\"runtime_link_host_link_runtime_dispatch_arg_slots\":"
           << runtime_link_host_link_contract.runtime_dispatch_arg_slots
           << ",\"runtime_link_host_link_runtime_dispatch_declaration_parameter_count\":"
           << runtime_link_host_link_contract.runtime_dispatch_declaration_parameter_count
           << ",\"runtime_link_host_link_runtime_dispatch_symbol\":\""
           << runtime_link_host_link_contract.runtime_dispatch_symbol
           << "\""
           << ",\"runtime_link_host_link_default_runtime_dispatch_symbol_binding\":"
           << (runtime_link_host_link_contract.default_runtime_dispatch_symbol_binding ? "true" : "false")
           << ",\"runtime_link_host_link_contract_violation_sites\":"
           << runtime_link_host_link_contract.contract_violation_sites
           << ",\"lowering_runtime_link_host_link_replay_key\":\""
           << runtime_link_host_link_replay_key
           << "\"";
}

}  // namespace objc3::artifacts::frontend
