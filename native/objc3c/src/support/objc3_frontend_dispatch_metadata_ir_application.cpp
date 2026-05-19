#include "support/objc3_frontend_dispatch_metadata_ir_application.h"

#include "artifacts/objc3_frontend_artifact_dispatch_metadata_application.h"
#include "ir/objc3_ir_frontend_metadata.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3DispatchMetadataIrApplication(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3DispatchMetadataApplication &application) {
  const auto &dispatch_control =
      application.dispatch_dispatch_control_lowering;
  ir_frontend_metadata.lowering_dispatch_dispatch_control_replay_key =
      application.dispatch_dispatch_control_lowering_replay_key;
  ir_frontend_metadata
      .dispatch_dispatch_control_lowering_direct_call_candidate_sites =
      dispatch_control.direct_call_candidate_sites;
  ir_frontend_metadata
      .dispatch_dispatch_control_lowering_direct_members_defaulted_sites =
      dispatch_control.direct_members_defaulted_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_dynamic_opt_out_sites =
      dispatch_control.dynamic_opt_out_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_final_container_sites =
      dispatch_control.final_container_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_sealed_container_sites =
      dispatch_control.sealed_container_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_override_legality_sites =
      dispatch_control.override_legality_sites;
  ir_frontend_metadata
      .dispatch_dispatch_control_lowering_metadata_preserved_callable_sites =
      dispatch_control.metadata_preserved_callable_sites;
  ir_frontend_metadata
      .dispatch_dispatch_control_lowering_metadata_preserved_container_sites =
      dispatch_control.metadata_preserved_container_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_guard_blocked_sites =
      dispatch_control.guard_blocked_sites;
  ir_frontend_metadata
      .dispatch_dispatch_control_lowering_contract_violation_sites =
      dispatch_control.contract_violation_sites;
  ir_frontend_metadata.deterministic_dispatch_dispatch_control_lowering_handoff =
      dispatch_control.deterministic;

  const auto &dispatch_metadata =
      application.dispatch_dispatch_metadata_interface_preservation;
  ir_frontend_metadata
      .lowering_dispatch_dispatch_metadata_interface_preservation_key =
      dispatch_metadata.replay_key;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_local_direct_callable_record_count =
      dispatch_metadata.local_direct_callable_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_local_final_callable_record_count =
      dispatch_metadata.local_final_callable_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_local_final_container_record_count =
      dispatch_metadata.local_final_container_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_local_sealed_container_record_count =
      dispatch_metadata.local_sealed_container_record_count;
  ir_frontend_metadata.dispatch_dispatch_metadata_imported_module_count =
      dispatch_metadata.imported_module_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_imported_direct_callable_record_count =
      dispatch_metadata.imported_direct_callable_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_imported_final_callable_record_count =
      dispatch_metadata.imported_final_callable_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_imported_final_container_record_count =
      dispatch_metadata.imported_final_container_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_imported_sealed_container_record_count =
      dispatch_metadata.imported_sealed_container_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_runtime_import_artifact_ready =
      dispatch_metadata.runtime_import_artifact_ready;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_separate_compilation_preservation_ready =
      dispatch_metadata.separate_compilation_preservation_ready;
  ir_frontend_metadata
      .deterministic_dispatch_dispatch_metadata_interface_handoff =
      dispatch_metadata.deterministic;
}

void ApplyObjc3ObjectDispatchMetadataIrApplication(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3ObjectDispatchMetadataApplication &application) {
  const auto &property_synthesis =
      application.property_synthesis_ivar_binding;
  ir_frontend_metadata.lowering_property_synthesis_ivar_binding_replay_key =
      application.property_synthesis_ivar_binding_replay_key;
  ir_frontend_metadata.lowering_property_synthesis_sites =
      property_synthesis.property_synthesis_sites;
  ir_frontend_metadata.lowering_property_synthesis_explicit_ivar_bindings =
      property_synthesis.property_synthesis_explicit_ivar_bindings;
  ir_frontend_metadata.lowering_property_synthesis_default_ivar_bindings =
      property_synthesis.property_synthesis_default_ivar_bindings;
  ir_frontend_metadata.lowering_interface_owned_property_synthesis_sites =
      property_synthesis.interface_owned_property_synthesis_sites;
  ir_frontend_metadata.lowering_implementation_property_redeclaration_sites =
      property_synthesis.implementation_property_redeclaration_sites;
  ir_frontend_metadata.lowering_property_synthesis_ivar_binding_resolved =
      property_synthesis.ivar_binding_resolved;
  ir_frontend_metadata.lowering_property_synthesis_deterministic_handoff =
      property_synthesis.deterministic;

  const auto &typecheck = application.id_class_sel_object_pointer_typecheck;
  ir_frontend_metadata.lowering_id_class_sel_object_pointer_typecheck_replay_key =
      application.id_class_sel_object_pointer_typecheck_replay_key;
  ir_frontend_metadata.id_typecheck_sites = typecheck.id_typecheck_sites;
  ir_frontend_metadata.class_typecheck_sites = typecheck.class_typecheck_sites;
  ir_frontend_metadata.sel_typecheck_sites = typecheck.sel_typecheck_sites;
  ir_frontend_metadata.object_pointer_typecheck_sites =
      typecheck.object_pointer_typecheck_sites;
  ir_frontend_metadata.id_class_sel_object_pointer_typecheck_sites_total =
      typecheck.total_typecheck_sites;
  ir_frontend_metadata
      .deterministic_id_class_sel_object_pointer_typecheck_handoff =
      typecheck.deterministic;

  const auto &dispatch_surface = application.dispatch_surface_classification;
  ir_frontend_metadata.lowering_dispatch_surface_classification_replay_key =
      application.dispatch_surface_classification_replay_key;
  ir_frontend_metadata.dispatch_surface_classification_instance_sites =
      dispatch_surface.instance_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_class_sites =
      dispatch_surface.class_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_super_sites =
      dispatch_surface.super_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_direct_sites =
      dispatch_surface.direct_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_dynamic_sites =
      dispatch_surface.dynamic_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_instance_entrypoint_family =
      dispatch_surface.instance_entrypoint_family;
  ir_frontend_metadata.dispatch_surface_classification_class_entrypoint_family =
      dispatch_surface.class_entrypoint_family;
  ir_frontend_metadata.dispatch_surface_classification_super_entrypoint_family =
      dispatch_surface.super_entrypoint_family;
  ir_frontend_metadata.dispatch_surface_classification_direct_entrypoint_family =
      dispatch_surface.direct_entrypoint_family;
  ir_frontend_metadata.dispatch_surface_classification_dynamic_entrypoint_family =
      dispatch_surface.dynamic_entrypoint_family;
  ir_frontend_metadata.deterministic_dispatch_surface_classification_handoff =
      dispatch_surface.deterministic;

  const auto &selector_lowering = application.message_send_selector_lowering;
  ir_frontend_metadata.lowering_message_send_selector_lowering_replay_key =
      application.message_send_selector_lowering_replay_key;
  ir_frontend_metadata.message_send_selector_lowering_sites =
      selector_lowering.message_send_sites;
  ir_frontend_metadata.message_send_selector_lowering_unary_sites =
      selector_lowering.unary_selector_sites;
  ir_frontend_metadata.message_send_selector_lowering_keyword_sites =
      selector_lowering.keyword_selector_sites;
  ir_frontend_metadata.message_send_selector_lowering_selector_piece_sites =
      selector_lowering.selector_piece_sites;
  ir_frontend_metadata
      .message_send_selector_lowering_argument_expression_sites =
      selector_lowering.argument_expression_sites;
  ir_frontend_metadata.message_send_selector_lowering_receiver_sites =
      selector_lowering.receiver_expression_sites;
  ir_frontend_metadata
      .message_send_selector_lowering_selector_literal_entries =
      selector_lowering.selector_literal_entries;
  ir_frontend_metadata
      .message_send_selector_lowering_selector_literal_characters =
      selector_lowering.selector_literal_characters;
  ir_frontend_metadata.deterministic_message_send_selector_lowering_handoff =
      selector_lowering.deterministic;

  const auto &abi_marshalling = application.dispatch_abi_marshalling;
  ir_frontend_metadata.lowering_dispatch_abi_marshalling_replay_key =
      application.dispatch_abi_marshalling_replay_key;
  ir_frontend_metadata.dispatch_abi_marshalling_message_send_sites =
      abi_marshalling.message_send_sites;
  ir_frontend_metadata.dispatch_abi_marshalling_receiver_slots_marshaled =
      abi_marshalling.receiver_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_selector_slots_marshaled =
      abi_marshalling.selector_slots_marshaled;
  ir_frontend_metadata
      .dispatch_abi_marshalling_argument_value_slots_marshaled =
      abi_marshalling.argument_value_slots_marshaled;
  ir_frontend_metadata
      .dispatch_abi_marshalling_argument_padding_slots_marshaled =
      abi_marshalling.argument_padding_slots_marshaled;
  ir_frontend_metadata
      .dispatch_abi_marshalling_argument_total_slots_marshaled =
      abi_marshalling.argument_total_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_total_marshaled_slots =
      abi_marshalling.total_marshaled_slots;
  ir_frontend_metadata.dispatch_abi_marshalling_runtime_dispatch_arg_slots =
      abi_marshalling.runtime_dispatch_arg_slots;
  ir_frontend_metadata.deterministic_dispatch_abi_marshalling_handoff =
      abi_marshalling.deterministic;

  const auto &nil_receiver =
      application.nil_receiver_semantics_foldability;
  ir_frontend_metadata.lowering_nil_receiver_semantics_foldability_replay_key =
      application.nil_receiver_semantics_foldability_replay_key;
  ir_frontend_metadata.nil_receiver_semantics_foldability_message_send_sites =
      nil_receiver.message_send_sites;
  ir_frontend_metadata
      .nil_receiver_semantics_foldability_receiver_nil_literal_sites =
      nil_receiver.receiver_nil_literal_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_enabled_sites =
      nil_receiver.nil_receiver_semantics_enabled_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_foldable_sites =
      nil_receiver.nil_receiver_foldable_sites;
  ir_frontend_metadata
      .nil_receiver_semantics_foldability_runtime_dispatch_required_sites =
      nil_receiver.nil_receiver_runtime_dispatch_required_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_non_nil_receiver_sites =
      nil_receiver.non_nil_receiver_sites;
  ir_frontend_metadata
      .nil_receiver_semantics_foldability_contract_violation_sites =
      nil_receiver.contract_violation_sites;
  ir_frontend_metadata.deterministic_nil_receiver_semantics_foldability_handoff =
      nil_receiver.deterministic;

  const auto &super_dispatch = application.super_dispatch_method_family;
  ir_frontend_metadata.lowering_super_dispatch_method_family_replay_key =
      application.super_dispatch_method_family_replay_key;
  ir_frontend_metadata.super_dispatch_method_family_message_send_sites =
      super_dispatch.message_send_sites;
  ir_frontend_metadata
      .super_dispatch_method_family_receiver_super_identifier_sites =
      super_dispatch.receiver_super_identifier_sites;
  ir_frontend_metadata.super_dispatch_method_family_enabled_sites =
      super_dispatch.super_dispatch_enabled_sites;
  ir_frontend_metadata
      .super_dispatch_method_family_requires_class_context_sites =
      super_dispatch.super_dispatch_requires_class_context_sites;
  ir_frontend_metadata.super_dispatch_method_family_init_sites =
      super_dispatch.method_family_init_sites;
  ir_frontend_metadata.super_dispatch_method_family_copy_sites =
      super_dispatch.method_family_copy_sites;
  ir_frontend_metadata.super_dispatch_method_family_mutable_copy_sites =
      super_dispatch.method_family_mutable_copy_sites;
  ir_frontend_metadata.super_dispatch_method_family_new_sites =
      super_dispatch.method_family_new_sites;
  ir_frontend_metadata.super_dispatch_method_family_none_sites =
      super_dispatch.method_family_none_sites;
  ir_frontend_metadata
      .super_dispatch_method_family_returns_retained_result_sites =
      super_dispatch.method_family_returns_retained_result_sites;
  ir_frontend_metadata
      .super_dispatch_method_family_returns_related_result_sites =
      super_dispatch.method_family_returns_related_result_sites;
  ir_frontend_metadata.super_dispatch_method_family_contract_violation_sites =
      super_dispatch.contract_violation_sites;
  ir_frontend_metadata.deterministic_super_dispatch_method_family_handoff =
      super_dispatch.deterministic;

  const auto &runtime_link = application.runtime_link_host_link;
  ir_frontend_metadata.lowering_runtime_link_host_link_replay_key =
      application.runtime_link_host_link_replay_key;
  ir_frontend_metadata.runtime_link_host_link_message_send_sites =
      runtime_link.message_send_sites;
  ir_frontend_metadata.runtime_link_host_link_required_sites =
      runtime_link.runtime_link_required_sites;
  ir_frontend_metadata.runtime_link_host_link_elided_sites =
      runtime_link.runtime_link_elided_sites;
  ir_frontend_metadata.runtime_link_host_link_runtime_dispatch_arg_slots =
      runtime_link.runtime_dispatch_arg_slots;
  ir_frontend_metadata
      .runtime_link_host_link_runtime_dispatch_declaration_parameter_count =
      runtime_link.runtime_dispatch_declaration_parameter_count;
  ir_frontend_metadata.runtime_link_host_link_contract_violation_sites =
      runtime_link.contract_violation_sites;
  ir_frontend_metadata.runtime_link_host_link_runtime_dispatch_symbol =
      runtime_link.runtime_dispatch_symbol;
  ir_frontend_metadata
      .runtime_link_host_link_default_runtime_dispatch_symbol_binding =
      runtime_link.default_runtime_dispatch_symbol_binding;
  ir_frontend_metadata.deterministic_runtime_link_host_link_handoff =
      runtime_link.deterministic;
}

}  // namespace objc3::artifacts::frontend
