#include "artifacts/objc3_frontend_artifact_dispatch_metadata_application.h"

#include "ir/objc3_ir_frontend_metadata.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendDispatchMetadataSnapshots(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &dispatch_dispatch_control_lowering_replay_key,
    const Objc3DispatchControlLoweringSnapshot
        &dispatch_dispatch_control_lowering,
    const Objc3DispatchMetadataPreservationSnapshot
        &dispatch_dispatch_metadata_interface_preservation) {
  ir_frontend_metadata.lowering_dispatch_dispatch_control_replay_key =
      dispatch_dispatch_control_lowering_replay_key;
  ir_frontend_metadata
      .dispatch_dispatch_control_lowering_direct_call_candidate_sites =
      dispatch_dispatch_control_lowering.direct_call_candidate_sites;
  ir_frontend_metadata
      .dispatch_dispatch_control_lowering_direct_members_defaulted_sites =
      dispatch_dispatch_control_lowering.direct_members_defaulted_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_dynamic_opt_out_sites =
      dispatch_dispatch_control_lowering.dynamic_opt_out_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_final_container_sites =
      dispatch_dispatch_control_lowering.final_container_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_sealed_container_sites =
      dispatch_dispatch_control_lowering.sealed_container_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_override_legality_sites =
      dispatch_dispatch_control_lowering.override_legality_sites;
  ir_frontend_metadata
      .dispatch_dispatch_control_lowering_metadata_preserved_callable_sites =
      dispatch_dispatch_control_lowering.metadata_preserved_callable_sites;
  ir_frontend_metadata
      .dispatch_dispatch_control_lowering_metadata_preserved_container_sites =
      dispatch_dispatch_control_lowering.metadata_preserved_container_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_guard_blocked_sites =
      dispatch_dispatch_control_lowering.guard_blocked_sites;
  ir_frontend_metadata
      .dispatch_dispatch_control_lowering_contract_violation_sites =
      dispatch_dispatch_control_lowering.contract_violation_sites;
  ir_frontend_metadata.deterministic_dispatch_dispatch_control_lowering_handoff =
      dispatch_dispatch_control_lowering.deterministic;

  ir_frontend_metadata
      .lowering_dispatch_dispatch_metadata_interface_preservation_key =
      dispatch_dispatch_metadata_interface_preservation.replay_key;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_local_direct_callable_record_count =
      dispatch_dispatch_metadata_interface_preservation
          .local_direct_callable_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_local_final_callable_record_count =
      dispatch_dispatch_metadata_interface_preservation
          .local_final_callable_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_local_final_container_record_count =
      dispatch_dispatch_metadata_interface_preservation
          .local_final_container_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_local_sealed_container_record_count =
      dispatch_dispatch_metadata_interface_preservation
          .local_sealed_container_record_count;
  ir_frontend_metadata.dispatch_dispatch_metadata_imported_module_count =
      dispatch_dispatch_metadata_interface_preservation.imported_module_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_imported_direct_callable_record_count =
      dispatch_dispatch_metadata_interface_preservation
          .imported_direct_callable_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_imported_final_callable_record_count =
      dispatch_dispatch_metadata_interface_preservation
          .imported_final_callable_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_imported_final_container_record_count =
      dispatch_dispatch_metadata_interface_preservation
          .imported_final_container_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_imported_sealed_container_record_count =
      dispatch_dispatch_metadata_interface_preservation
          .imported_sealed_container_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_runtime_import_artifact_ready =
      dispatch_dispatch_metadata_interface_preservation
          .runtime_import_artifact_ready;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_separate_compilation_preservation_ready =
      dispatch_dispatch_metadata_interface_preservation
          .separate_compilation_preservation_ready;
  ir_frontend_metadata
      .deterministic_dispatch_dispatch_metadata_interface_handoff =
      dispatch_dispatch_metadata_interface_preservation.deterministic;
}

void ApplyObjc3FrontendObjectDispatchMetadataSnapshots(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &property_synthesis_ivar_binding_replay_key,
    const Objc3PropertySynthesisIvarBindingSnapshot
        &property_synthesis_ivar_binding,
    const std::string &id_class_sel_object_pointer_typecheck_replay_key,
    const Objc3IdClassSelObjectPointerTypecheckSnapshot
        &id_class_sel_object_pointer_typecheck,
    const std::string &dispatch_surface_classification_replay_key,
    const Objc3DispatchSurfaceClassificationSnapshot
        &dispatch_surface_classification,
    const std::string &message_send_selector_lowering_replay_key,
    const Objc3MessageSendSelectorLoweringSnapshot
        &message_send_selector_lowering,
    const std::string &dispatch_abi_marshalling_replay_key,
    const Objc3DispatchAbiMarshallingSnapshot &dispatch_abi_marshalling,
    const std::string &nil_receiver_semantics_foldability_replay_key,
    const Objc3NilReceiverSemanticsFoldabilitySnapshot
        &nil_receiver_semantics_foldability,
    const std::string &super_dispatch_method_family_replay_key,
    const Objc3SuperDispatchMethodFamilySnapshot
        &super_dispatch_method_family,
    const std::string &runtime_link_host_link_replay_key,
    const Objc3RuntimeLinkHostLinkSnapshot &runtime_link_host_link) {
  ir_frontend_metadata.lowering_property_synthesis_ivar_binding_replay_key =
      property_synthesis_ivar_binding_replay_key;
  ir_frontend_metadata.lowering_property_synthesis_sites =
      property_synthesis_ivar_binding.property_synthesis_sites;
  ir_frontend_metadata.lowering_property_synthesis_explicit_ivar_bindings =
      property_synthesis_ivar_binding
          .property_synthesis_explicit_ivar_bindings;
  ir_frontend_metadata.lowering_property_synthesis_default_ivar_bindings =
      property_synthesis_ivar_binding
          .property_synthesis_default_ivar_bindings;
  ir_frontend_metadata.lowering_interface_owned_property_synthesis_sites =
      property_synthesis_ivar_binding
          .interface_owned_property_synthesis_sites;
  ir_frontend_metadata.lowering_implementation_property_redeclaration_sites =
      property_synthesis_ivar_binding
          .implementation_property_redeclaration_sites;
  ir_frontend_metadata.lowering_property_synthesis_ivar_binding_resolved =
      property_synthesis_ivar_binding.ivar_binding_resolved;
  ir_frontend_metadata.lowering_property_synthesis_deterministic_handoff =
      property_synthesis_ivar_binding.deterministic;

  ir_frontend_metadata.lowering_id_class_sel_object_pointer_typecheck_replay_key =
      id_class_sel_object_pointer_typecheck_replay_key;
  ir_frontend_metadata.id_typecheck_sites =
      id_class_sel_object_pointer_typecheck.id_typecheck_sites;
  ir_frontend_metadata.class_typecheck_sites =
      id_class_sel_object_pointer_typecheck.class_typecheck_sites;
  ir_frontend_metadata.sel_typecheck_sites =
      id_class_sel_object_pointer_typecheck.sel_typecheck_sites;
  ir_frontend_metadata.object_pointer_typecheck_sites =
      id_class_sel_object_pointer_typecheck.object_pointer_typecheck_sites;
  ir_frontend_metadata.id_class_sel_object_pointer_typecheck_sites_total =
      id_class_sel_object_pointer_typecheck.total_typecheck_sites;
  ir_frontend_metadata
      .deterministic_id_class_sel_object_pointer_typecheck_handoff =
      id_class_sel_object_pointer_typecheck.deterministic;

  ir_frontend_metadata.lowering_dispatch_surface_classification_replay_key =
      dispatch_surface_classification_replay_key;
  ir_frontend_metadata.dispatch_surface_classification_instance_sites =
      dispatch_surface_classification.instance_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_class_sites =
      dispatch_surface_classification.class_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_super_sites =
      dispatch_surface_classification.super_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_direct_sites =
      dispatch_surface_classification.direct_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_dynamic_sites =
      dispatch_surface_classification.dynamic_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_instance_entrypoint_family =
      dispatch_surface_classification.instance_entrypoint_family;
  ir_frontend_metadata.dispatch_surface_classification_class_entrypoint_family =
      dispatch_surface_classification.class_entrypoint_family;
  ir_frontend_metadata.dispatch_surface_classification_super_entrypoint_family =
      dispatch_surface_classification.super_entrypoint_family;
  ir_frontend_metadata.dispatch_surface_classification_direct_entrypoint_family =
      dispatch_surface_classification.direct_entrypoint_family;
  ir_frontend_metadata.dispatch_surface_classification_dynamic_entrypoint_family =
      dispatch_surface_classification.dynamic_entrypoint_family;
  ir_frontend_metadata
      .deterministic_dispatch_surface_classification_handoff =
      dispatch_surface_classification.deterministic;

  ir_frontend_metadata.lowering_message_send_selector_lowering_replay_key =
      message_send_selector_lowering_replay_key;
  ir_frontend_metadata.message_send_selector_lowering_sites =
      message_send_selector_lowering.message_send_sites;
  ir_frontend_metadata.message_send_selector_lowering_unary_sites =
      message_send_selector_lowering.unary_selector_sites;
  ir_frontend_metadata.message_send_selector_lowering_keyword_sites =
      message_send_selector_lowering.keyword_selector_sites;
  ir_frontend_metadata.message_send_selector_lowering_selector_piece_sites =
      message_send_selector_lowering.selector_piece_sites;
  ir_frontend_metadata
      .message_send_selector_lowering_argument_expression_sites =
      message_send_selector_lowering.argument_expression_sites;
  ir_frontend_metadata.message_send_selector_lowering_receiver_sites =
      message_send_selector_lowering.receiver_expression_sites;
  ir_frontend_metadata
      .message_send_selector_lowering_selector_literal_entries =
      message_send_selector_lowering.selector_literal_entries;
  ir_frontend_metadata
      .message_send_selector_lowering_selector_literal_characters =
      message_send_selector_lowering.selector_literal_characters;
  ir_frontend_metadata.deterministic_message_send_selector_lowering_handoff =
      message_send_selector_lowering.deterministic;

  ir_frontend_metadata.lowering_dispatch_abi_marshalling_replay_key =
      dispatch_abi_marshalling_replay_key;
  ir_frontend_metadata.dispatch_abi_marshalling_message_send_sites =
      dispatch_abi_marshalling.message_send_sites;
  ir_frontend_metadata.dispatch_abi_marshalling_receiver_slots_marshaled =
      dispatch_abi_marshalling.receiver_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_selector_slots_marshaled =
      dispatch_abi_marshalling.selector_slots_marshaled;
  ir_frontend_metadata
      .dispatch_abi_marshalling_argument_value_slots_marshaled =
      dispatch_abi_marshalling.argument_value_slots_marshaled;
  ir_frontend_metadata
      .dispatch_abi_marshalling_argument_padding_slots_marshaled =
      dispatch_abi_marshalling.argument_padding_slots_marshaled;
  ir_frontend_metadata
      .dispatch_abi_marshalling_argument_total_slots_marshaled =
      dispatch_abi_marshalling.argument_total_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_total_marshaled_slots =
      dispatch_abi_marshalling.total_marshaled_slots;
  ir_frontend_metadata.dispatch_abi_marshalling_runtime_dispatch_arg_slots =
      dispatch_abi_marshalling.runtime_dispatch_arg_slots;
  ir_frontend_metadata.deterministic_dispatch_abi_marshalling_handoff =
      dispatch_abi_marshalling.deterministic;

  ir_frontend_metadata.lowering_nil_receiver_semantics_foldability_replay_key =
      nil_receiver_semantics_foldability_replay_key;
  ir_frontend_metadata.nil_receiver_semantics_foldability_message_send_sites =
      nil_receiver_semantics_foldability.message_send_sites;
  ir_frontend_metadata
      .nil_receiver_semantics_foldability_receiver_nil_literal_sites =
      nil_receiver_semantics_foldability.receiver_nil_literal_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_enabled_sites =
      nil_receiver_semantics_foldability.nil_receiver_semantics_enabled_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_foldable_sites =
      nil_receiver_semantics_foldability.nil_receiver_foldable_sites;
  ir_frontend_metadata
      .nil_receiver_semantics_foldability_runtime_dispatch_required_sites =
      nil_receiver_semantics_foldability
          .nil_receiver_runtime_dispatch_required_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_non_nil_receiver_sites =
      nil_receiver_semantics_foldability.non_nil_receiver_sites;
  ir_frontend_metadata
      .nil_receiver_semantics_foldability_contract_violation_sites =
      nil_receiver_semantics_foldability.contract_violation_sites;
  ir_frontend_metadata.deterministic_nil_receiver_semantics_foldability_handoff =
      nil_receiver_semantics_foldability.deterministic;

  ir_frontend_metadata.lowering_super_dispatch_method_family_replay_key =
      super_dispatch_method_family_replay_key;
  ir_frontend_metadata.super_dispatch_method_family_message_send_sites =
      super_dispatch_method_family.message_send_sites;
  ir_frontend_metadata
      .super_dispatch_method_family_receiver_super_identifier_sites =
      super_dispatch_method_family.receiver_super_identifier_sites;
  ir_frontend_metadata.super_dispatch_method_family_enabled_sites =
      super_dispatch_method_family.super_dispatch_enabled_sites;
  ir_frontend_metadata
      .super_dispatch_method_family_requires_class_context_sites =
      super_dispatch_method_family.super_dispatch_requires_class_context_sites;
  ir_frontend_metadata.super_dispatch_method_family_init_sites =
      super_dispatch_method_family.method_family_init_sites;
  ir_frontend_metadata.super_dispatch_method_family_copy_sites =
      super_dispatch_method_family.method_family_copy_sites;
  ir_frontend_metadata.super_dispatch_method_family_mutable_copy_sites =
      super_dispatch_method_family.method_family_mutable_copy_sites;
  ir_frontend_metadata.super_dispatch_method_family_new_sites =
      super_dispatch_method_family.method_family_new_sites;
  ir_frontend_metadata.super_dispatch_method_family_none_sites =
      super_dispatch_method_family.method_family_none_sites;
  ir_frontend_metadata
      .super_dispatch_method_family_returns_retained_result_sites =
      super_dispatch_method_family
          .method_family_returns_retained_result_sites;
  ir_frontend_metadata
      .super_dispatch_method_family_returns_related_result_sites =
      super_dispatch_method_family
          .method_family_returns_related_result_sites;
  ir_frontend_metadata.super_dispatch_method_family_contract_violation_sites =
      super_dispatch_method_family.contract_violation_sites;
  ir_frontend_metadata.deterministic_super_dispatch_method_family_handoff =
      super_dispatch_method_family.deterministic;

  ir_frontend_metadata.lowering_runtime_link_host_link_replay_key =
      runtime_link_host_link_replay_key;
  ir_frontend_metadata.runtime_link_host_link_message_send_sites =
      runtime_link_host_link.message_send_sites;
  ir_frontend_metadata.runtime_link_host_link_required_sites =
      runtime_link_host_link.runtime_link_required_sites;
  ir_frontend_metadata.runtime_link_host_link_elided_sites =
      runtime_link_host_link.runtime_link_elided_sites;
  ir_frontend_metadata.runtime_link_host_link_runtime_dispatch_arg_slots =
      runtime_link_host_link.runtime_dispatch_arg_slots;
  ir_frontend_metadata
      .runtime_link_host_link_runtime_dispatch_declaration_parameter_count =
      runtime_link_host_link.runtime_dispatch_declaration_parameter_count;
  ir_frontend_metadata.runtime_link_host_link_contract_violation_sites =
      runtime_link_host_link.contract_violation_sites;
  ir_frontend_metadata.runtime_link_host_link_runtime_dispatch_symbol =
      runtime_link_host_link.runtime_dispatch_symbol;
  ir_frontend_metadata
      .runtime_link_host_link_default_runtime_dispatch_symbol_binding =
      runtime_link_host_link.default_runtime_dispatch_symbol_binding;
  ir_frontend_metadata.deterministic_runtime_link_host_link_handoff =
      runtime_link_host_link.deterministic;
}

}  // namespace objc3::artifacts::frontend
