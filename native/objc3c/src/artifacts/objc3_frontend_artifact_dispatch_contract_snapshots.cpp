#include "artifacts/objc3_frontend_artifact_dispatch_contract_snapshots.h"

#include "artifacts/objc3_frontend_runtime_import_artifacts.h"
#include "support/frontend_dispatch_contract_records.h"

namespace objc3::artifacts::frontend {

Objc3DispatchControlLoweringSnapshot BuildDispatchControlLoweringSnapshot(
    const Objc3DispatchDispatchControlLoweringContract &contract) {
  Objc3DispatchControlLoweringSnapshot snapshot;
  snapshot.direct_call_candidate_sites = contract.direct_call_candidate_sites;
  snapshot.direct_members_defaulted_sites =
      contract.direct_members_defaulted_sites;
  snapshot.dynamic_opt_out_sites = contract.dynamic_opt_out_sites;
  snapshot.final_container_sites = contract.final_container_sites;
  snapshot.sealed_container_sites = contract.sealed_container_sites;
  snapshot.override_legality_sites = contract.override_legality_sites;
  snapshot.metadata_preserved_callable_sites =
      contract.metadata_preserved_callable_sites;
  snapshot.metadata_preserved_container_sites =
      contract.metadata_preserved_container_sites;
  snapshot.guard_blocked_sites = contract.guard_blocked_sites;
  snapshot.contract_violation_sites = contract.contract_violation_sites;
  snapshot.deterministic = contract.deterministic;
  return snapshot;
}

Objc3DispatchMetadataPreservationSnapshot
BuildDispatchMetadataPreservationSnapshot(
    const Objc3DispatchDispatchMetadataInterfacePreservationSurfaceSummary
        &summary) {
  Objc3DispatchMetadataPreservationSnapshot snapshot;
  snapshot.replay_key = summary.replay_key;
  snapshot.local_direct_callable_record_count =
      summary.local_direct_callable_record_count;
  snapshot.local_final_callable_record_count =
      summary.local_final_callable_record_count;
  snapshot.local_final_container_record_count =
      summary.local_final_container_record_count;
  snapshot.local_sealed_container_record_count =
      summary.local_sealed_container_record_count;
  snapshot.imported_module_count = summary.imported_module_count;
  snapshot.imported_direct_callable_record_count =
      summary.imported_direct_callable_record_count;
  snapshot.imported_final_callable_record_count =
      summary.imported_final_callable_record_count;
  snapshot.imported_final_container_record_count =
      summary.imported_final_container_record_count;
  snapshot.imported_sealed_container_record_count =
      summary.imported_sealed_container_record_count;
  snapshot.runtime_import_artifact_ready =
      summary.runtime_import_artifact_ready;
  snapshot.separate_compilation_preservation_ready =
      summary.separate_compilation_preservation_ready;
  snapshot.deterministic = summary.deterministic;
  return snapshot;
}

Objc3PropertySynthesisIvarBindingSnapshot
BuildPropertySynthesisIvarBindingSnapshot(
    const Objc3PropertySynthesisIvarBindingContract &contract) {
  Objc3PropertySynthesisIvarBindingSnapshot snapshot;
  snapshot.property_synthesis_sites = contract.property_synthesis_sites;
  snapshot.property_synthesis_explicit_ivar_bindings =
      contract.property_synthesis_explicit_ivar_bindings;
  snapshot.property_synthesis_default_ivar_bindings =
      contract.property_synthesis_default_ivar_bindings;
  snapshot.interface_owned_property_synthesis_sites =
      contract.interface_owned_property_synthesis_sites;
  snapshot.implementation_property_redeclaration_sites =
      contract.implementation_property_redeclaration_sites;
  snapshot.ivar_binding_resolved = contract.ivar_binding_resolved;
  snapshot.deterministic = contract.deterministic;
  return snapshot;
}

Objc3IdClassSelObjectPointerTypecheckSnapshot
BuildIdClassSelObjectPointerTypecheckSnapshot(
    const Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  Objc3IdClassSelObjectPointerTypecheckSnapshot snapshot;
  snapshot.id_typecheck_sites = contract.id_typecheck_sites;
  snapshot.class_typecheck_sites = contract.class_typecheck_sites;
  snapshot.sel_typecheck_sites = contract.sel_typecheck_sites;
  snapshot.object_pointer_typecheck_sites =
      contract.object_pointer_typecheck_sites;
  snapshot.total_typecheck_sites = contract.total_typecheck_sites;
  snapshot.deterministic = contract.deterministic;
  return snapshot;
}

Objc3DispatchSurfaceClassificationSnapshot
BuildDispatchSurfaceClassificationSnapshot(
    const Objc3DispatchSurfaceClassificationContract &contract) {
  Objc3DispatchSurfaceClassificationSnapshot snapshot;
  snapshot.instance_dispatch_sites = contract.instance_dispatch_sites;
  snapshot.class_dispatch_sites = contract.class_dispatch_sites;
  snapshot.super_dispatch_sites = contract.super_dispatch_sites;
  snapshot.direct_dispatch_sites = contract.direct_dispatch_sites;
  snapshot.dynamic_dispatch_sites = contract.dynamic_dispatch_sites;
  snapshot.instance_entrypoint_family = contract.instance_entrypoint_family;
  snapshot.class_entrypoint_family = contract.class_entrypoint_family;
  snapshot.super_entrypoint_family = contract.super_entrypoint_family;
  snapshot.direct_entrypoint_family = contract.direct_entrypoint_family;
  snapshot.dynamic_entrypoint_family = contract.dynamic_entrypoint_family;
  snapshot.deterministic = contract.deterministic;
  return snapshot;
}

Objc3MessageSendSelectorLoweringSnapshot
BuildMessageSendSelectorLoweringSnapshot(
    const Objc3MessageSendSelectorLoweringContract &contract) {
  Objc3MessageSendSelectorLoweringSnapshot snapshot;
  snapshot.message_send_sites = contract.message_send_sites;
  snapshot.unary_selector_sites = contract.unary_selector_sites;
  snapshot.keyword_selector_sites = contract.keyword_selector_sites;
  snapshot.selector_piece_sites = contract.selector_piece_sites;
  snapshot.argument_expression_sites = contract.argument_expression_sites;
  snapshot.receiver_expression_sites = contract.receiver_expression_sites;
  snapshot.selector_literal_entries = contract.selector_literal_entries;
  snapshot.selector_literal_characters = contract.selector_literal_characters;
  snapshot.deterministic = contract.deterministic;
  return snapshot;
}

Objc3DispatchAbiMarshallingSnapshot BuildDispatchAbiMarshallingSnapshot(
    const Objc3DispatchAbiMarshallingContract &contract) {
  Objc3DispatchAbiMarshallingSnapshot snapshot;
  snapshot.message_send_sites = contract.message_send_sites;
  snapshot.receiver_slots_marshaled = contract.receiver_slots_marshaled;
  snapshot.selector_slots_marshaled = contract.selector_slots_marshaled;
  snapshot.argument_value_slots_marshaled =
      contract.argument_value_slots_marshaled;
  snapshot.argument_padding_slots_marshaled =
      contract.argument_padding_slots_marshaled;
  snapshot.argument_total_slots_marshaled =
      contract.argument_total_slots_marshaled;
  snapshot.total_marshaled_slots = contract.total_marshaled_slots;
  snapshot.runtime_dispatch_arg_slots = contract.runtime_dispatch_arg_slots;
  snapshot.deterministic = contract.deterministic;
  return snapshot;
}

Objc3NilReceiverSemanticsFoldabilitySnapshot
BuildNilReceiverSemanticsFoldabilitySnapshot(
    const Objc3NilReceiverSemanticsFoldabilityContract &contract) {
  Objc3NilReceiverSemanticsFoldabilitySnapshot snapshot;
  snapshot.message_send_sites = contract.message_send_sites;
  snapshot.receiver_nil_literal_sites = contract.receiver_nil_literal_sites;
  snapshot.nil_receiver_semantics_enabled_sites =
      contract.nil_receiver_semantics_enabled_sites;
  snapshot.nil_receiver_foldable_sites = contract.nil_receiver_foldable_sites;
  snapshot.nil_receiver_runtime_dispatch_required_sites =
      contract.nil_receiver_runtime_dispatch_required_sites;
  snapshot.non_nil_receiver_sites = contract.non_nil_receiver_sites;
  snapshot.contract_violation_sites = contract.contract_violation_sites;
  snapshot.deterministic = contract.deterministic;
  return snapshot;
}

Objc3SuperDispatchMethodFamilySnapshot BuildSuperDispatchMethodFamilySnapshot(
    const Objc3SuperDispatchMethodFamilyContract &contract) {
  Objc3SuperDispatchMethodFamilySnapshot snapshot;
  snapshot.message_send_sites = contract.message_send_sites;
  snapshot.receiver_super_identifier_sites =
      contract.receiver_super_identifier_sites;
  snapshot.super_dispatch_enabled_sites =
      contract.super_dispatch_enabled_sites;
  snapshot.super_dispatch_requires_class_context_sites =
      contract.super_dispatch_requires_class_context_sites;
  snapshot.method_family_init_sites = contract.method_family_init_sites;
  snapshot.method_family_copy_sites = contract.method_family_copy_sites;
  snapshot.method_family_mutable_copy_sites =
      contract.method_family_mutable_copy_sites;
  snapshot.method_family_new_sites = contract.method_family_new_sites;
  snapshot.method_family_none_sites = contract.method_family_none_sites;
  snapshot.method_family_returns_retained_result_sites =
      contract.method_family_returns_retained_result_sites;
  snapshot.method_family_returns_related_result_sites =
      contract.method_family_returns_related_result_sites;
  snapshot.contract_violation_sites = contract.contract_violation_sites;
  snapshot.deterministic = contract.deterministic;
  return snapshot;
}

Objc3RuntimeLinkHostLinkSnapshot BuildRuntimeLinkHostLinkSnapshot(
    const Objc3RuntimeLinkHostLinkContract &contract) {
  Objc3RuntimeLinkHostLinkSnapshot snapshot;
  snapshot.message_send_sites = contract.message_send_sites;
  snapshot.runtime_link_required_sites = contract.runtime_link_required_sites;
  snapshot.runtime_link_elided_sites = contract.runtime_link_elided_sites;
  snapshot.runtime_dispatch_arg_slots = contract.runtime_dispatch_arg_slots;
  snapshot.runtime_dispatch_declaration_parameter_count =
      contract.runtime_dispatch_declaration_parameter_count;
  snapshot.contract_violation_sites = contract.contract_violation_sites;
  snapshot.runtime_dispatch_symbol = contract.runtime_dispatch_symbol;
  snapshot.default_runtime_dispatch_symbol_binding =
      contract.default_runtime_dispatch_symbol_binding;
  snapshot.deterministic = contract.deterministic;
  return snapshot;
}

Objc3RuntimeDispatchLoweringAbiSnapshot
BuildRuntimeDispatchLoweringAbiSnapshot(
    const Objc3RuntimeDispatchLoweringAbiContract &contract) {
  Objc3RuntimeDispatchLoweringAbiSnapshot snapshot;
  snapshot.message_send_sites = contract.message_send_sites;
  snapshot.fixed_argument_slot_count = contract.fixed_argument_slot_count;
  snapshot.runtime_dispatch_parameter_count =
      contract.runtime_dispatch_parameter_count;
  snapshot.lowering_boundary_model = contract.lowering_boundary_model;
  snapshot.canonical_runtime_dispatch_symbol =
      contract.canonical_runtime_dispatch_symbol;
  snapshot.default_lowering_target_symbol =
      contract.default_lowering_target_symbol;
  snapshot.selector_lookup_symbol = contract.selector_lookup_symbol;
  snapshot.selector_handle_type = contract.selector_handle_type;
  snapshot.receiver_abi_type = contract.receiver_abi_type;
  snapshot.selector_abi_type = contract.selector_abi_type;
  snapshot.argument_abi_type = contract.argument_abi_type;
  snapshot.result_abi_type = contract.result_abi_type;
  snapshot.selector_operand_model = contract.selector_operand_model;
  snapshot.selector_handle_model = contract.selector_handle_model;
  snapshot.argument_padding_model = contract.argument_padding_model;
  snapshot.default_lowering_target_model =
      contract.default_lowering_target_model;
  snapshot.strict_dispatch_error_model =
      contract.strict_dispatch_error_model;
  snapshot.deferred_cases_model = contract.deferred_cases_model;
  snapshot.fail_closed = contract.fail_closed;
  snapshot.deterministic = contract.deterministic;
  return snapshot;
}

}  // namespace objc3::artifacts::frontend
