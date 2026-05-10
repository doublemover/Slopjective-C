#pragma once

#include <cstddef>
#include <string>

struct Objc3DispatchAbiMarshallingContract;
struct Objc3DispatchDispatchControlLoweringContract;
struct Objc3DispatchSurfaceClassificationContract;
struct Objc3IdClassSelObjectPointerTypecheckContract;
struct Objc3MessageSendSelectorLoweringContract;
struct Objc3NilReceiverSemanticsFoldabilityContract;
struct Objc3PropertySynthesisIvarBindingContract;
struct Objc3RuntimeDispatchLoweringAbiContract;
struct Objc3RuntimeLinkHostLinkContract;
struct Objc3SuperDispatchMethodFamilyContract;

namespace objc3::artifacts::frontend {

struct Objc3DispatchDispatchMetadataInterfacePreservationSurfaceSummary;

struct Objc3DispatchControlLoweringSnapshot {
  std::size_t direct_call_candidate_sites = 0;
  std::size_t direct_members_defaulted_sites = 0;
  std::size_t dynamic_opt_out_sites = 0;
  std::size_t final_container_sites = 0;
  std::size_t sealed_container_sites = 0;
  std::size_t override_legality_sites = 0;
  std::size_t metadata_preserved_callable_sites = 0;
  std::size_t metadata_preserved_container_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3DispatchMetadataPreservationSnapshot {
  std::string replay_key;
  std::size_t local_direct_callable_record_count = 0;
  std::size_t local_final_callable_record_count = 0;
  std::size_t local_final_container_record_count = 0;
  std::size_t local_sealed_container_record_count = 0;
  std::size_t imported_module_count = 0;
  std::size_t imported_direct_callable_record_count = 0;
  std::size_t imported_final_callable_record_count = 0;
  std::size_t imported_final_container_record_count = 0;
  std::size_t imported_sealed_container_record_count = 0;
  bool runtime_import_artifact_ready = false;
  bool separate_compilation_preservation_ready = false;
  bool deterministic = false;
};

struct Objc3PropertySynthesisIvarBindingSnapshot {
  std::size_t property_synthesis_sites = 0;
  std::size_t property_synthesis_explicit_ivar_bindings = 0;
  std::size_t property_synthesis_default_ivar_bindings = 0;
  std::size_t interface_owned_property_synthesis_sites = 0;
  std::size_t implementation_property_redeclaration_sites = 0;
  std::size_t ivar_binding_resolved = 0;
  bool deterministic = true;
};

struct Objc3IdClassSelObjectPointerTypecheckSnapshot {
  std::size_t id_typecheck_sites = 0;
  std::size_t class_typecheck_sites = 0;
  std::size_t sel_typecheck_sites = 0;
  std::size_t object_pointer_typecheck_sites = 0;
  std::size_t total_typecheck_sites = 0;
  bool deterministic = true;
};

struct Objc3DispatchSurfaceClassificationSnapshot {
  std::size_t instance_dispatch_sites = 0;
  std::size_t class_dispatch_sites = 0;
  std::size_t super_dispatch_sites = 0;
  std::size_t direct_dispatch_sites = 0;
  std::size_t dynamic_dispatch_sites = 0;
  std::string instance_entrypoint_family;
  std::string class_entrypoint_family;
  std::string super_entrypoint_family;
  std::string direct_entrypoint_family;
  std::string dynamic_entrypoint_family;
  bool deterministic = true;
};

struct Objc3MessageSendSelectorLoweringSnapshot {
  std::size_t message_send_sites = 0;
  std::size_t unary_selector_sites = 0;
  std::size_t keyword_selector_sites = 0;
  std::size_t selector_piece_sites = 0;
  std::size_t argument_expression_sites = 0;
  std::size_t receiver_expression_sites = 0;
  std::size_t selector_literal_entries = 0;
  std::size_t selector_literal_characters = 0;
  bool deterministic = true;
};

struct Objc3DispatchAbiMarshallingSnapshot {
  std::size_t message_send_sites = 0;
  std::size_t receiver_slots_marshaled = 0;
  std::size_t selector_slots_marshaled = 0;
  std::size_t argument_value_slots_marshaled = 0;
  std::size_t argument_padding_slots_marshaled = 0;
  std::size_t argument_total_slots_marshaled = 0;
  std::size_t total_marshaled_slots = 0;
  std::size_t runtime_dispatch_arg_slots = 0;
  bool deterministic = true;
};

struct Objc3NilReceiverSemanticsFoldabilitySnapshot {
  std::size_t message_send_sites = 0;
  std::size_t receiver_nil_literal_sites = 0;
  std::size_t nil_receiver_semantics_enabled_sites = 0;
  std::size_t nil_receiver_foldable_sites = 0;
  std::size_t nil_receiver_runtime_dispatch_required_sites = 0;
  std::size_t non_nil_receiver_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3SuperDispatchMethodFamilySnapshot {
  std::size_t message_send_sites = 0;
  std::size_t receiver_super_identifier_sites = 0;
  std::size_t super_dispatch_enabled_sites = 0;
  std::size_t super_dispatch_requires_class_context_sites = 0;
  std::size_t method_family_init_sites = 0;
  std::size_t method_family_copy_sites = 0;
  std::size_t method_family_mutable_copy_sites = 0;
  std::size_t method_family_new_sites = 0;
  std::size_t method_family_none_sites = 0;
  std::size_t method_family_returns_retained_result_sites = 0;
  std::size_t method_family_returns_related_result_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3RuntimeLinkHostLinkSnapshot {
  std::size_t message_send_sites = 0;
  std::size_t runtime_link_required_sites = 0;
  std::size_t runtime_link_elided_sites = 0;
  std::size_t runtime_dispatch_arg_slots = 0;
  std::size_t runtime_dispatch_declaration_parameter_count = 0;
  std::size_t contract_violation_sites = 0;
  std::string runtime_dispatch_symbol;
  bool default_runtime_dispatch_symbol_binding = true;
  bool deterministic = true;
};

struct Objc3RuntimeDispatchLoweringAbiSnapshot {
  std::size_t message_send_sites = 0;
  std::size_t fixed_argument_slot_count = 0;
  std::size_t runtime_dispatch_parameter_count = 0;
  std::string lowering_boundary_model;
  std::string canonical_runtime_dispatch_symbol;
  std::string default_lowering_target_symbol;
  std::string selector_lookup_symbol;
  std::string selector_handle_type;
  std::string receiver_abi_type;
  std::string selector_abi_type;
  std::string argument_abi_type;
  std::string result_abi_type;
  std::string selector_operand_model;
  std::string selector_handle_model;
  std::string argument_padding_model;
  std::string default_lowering_target_model;
  std::string strict_dispatch_error_model;
  std::string deferred_cases_model;
  bool fail_closed = true;
  bool deterministic = true;
};

[[nodiscard]] Objc3DispatchControlLoweringSnapshot
BuildDispatchControlLoweringSnapshot(
    const Objc3DispatchDispatchControlLoweringContract &contract);
[[nodiscard]] Objc3DispatchMetadataPreservationSnapshot
BuildDispatchMetadataPreservationSnapshot(
    const Objc3DispatchDispatchMetadataInterfacePreservationSurfaceSummary
        &summary);
[[nodiscard]] Objc3PropertySynthesisIvarBindingSnapshot
BuildPropertySynthesisIvarBindingSnapshot(
    const Objc3PropertySynthesisIvarBindingContract &contract);
[[nodiscard]] Objc3IdClassSelObjectPointerTypecheckSnapshot
BuildIdClassSelObjectPointerTypecheckSnapshot(
    const Objc3IdClassSelObjectPointerTypecheckContract &contract);
[[nodiscard]] Objc3DispatchSurfaceClassificationSnapshot
BuildDispatchSurfaceClassificationSnapshot(
    const Objc3DispatchSurfaceClassificationContract &contract);
[[nodiscard]] Objc3MessageSendSelectorLoweringSnapshot
BuildMessageSendSelectorLoweringSnapshot(
    const Objc3MessageSendSelectorLoweringContract &contract);
[[nodiscard]] Objc3DispatchAbiMarshallingSnapshot
BuildDispatchAbiMarshallingSnapshot(
    const Objc3DispatchAbiMarshallingContract &contract);
[[nodiscard]] Objc3NilReceiverSemanticsFoldabilitySnapshot
BuildNilReceiverSemanticsFoldabilitySnapshot(
    const Objc3NilReceiverSemanticsFoldabilityContract &contract);
[[nodiscard]] Objc3SuperDispatchMethodFamilySnapshot
BuildSuperDispatchMethodFamilySnapshot(
    const Objc3SuperDispatchMethodFamilyContract &contract);
[[nodiscard]] Objc3RuntimeLinkHostLinkSnapshot
BuildRuntimeLinkHostLinkSnapshot(
    const Objc3RuntimeLinkHostLinkContract &contract);
[[nodiscard]] Objc3RuntimeDispatchLoweringAbiSnapshot
BuildRuntimeDispatchLoweringAbiSnapshot(
    const Objc3RuntimeDispatchLoweringAbiContract &contract);

}  // namespace objc3::artifacts::frontend
