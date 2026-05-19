#pragma once

#include <cstddef>
#include <string>

struct Objc3IRFrontendDispatchExecutionMetadata {
  std::string lowering_dispatch_surface_classification_replay_key;
  std::size_t dispatch_surface_classification_instance_sites = 0;
  std::size_t dispatch_surface_classification_class_sites = 0;
  std::size_t dispatch_surface_classification_super_sites = 0;
  std::size_t dispatch_surface_classification_direct_sites = 0;
  std::size_t dispatch_surface_classification_dynamic_sites = 0;
  std::string dispatch_surface_classification_instance_entrypoint_family;
  std::string dispatch_surface_classification_class_entrypoint_family;
  std::string dispatch_surface_classification_super_entrypoint_family;
  std::string dispatch_surface_classification_direct_entrypoint_family;
  std::string dispatch_surface_classification_dynamic_entrypoint_family;
  bool deterministic_dispatch_surface_classification_handoff = false;
  std::string lowering_message_send_selector_lowering_replay_key;
  std::size_t message_send_selector_lowering_sites = 0;
  std::size_t message_send_selector_lowering_unary_sites = 0;
  std::size_t message_send_selector_lowering_keyword_sites = 0;
  std::size_t message_send_selector_lowering_selector_piece_sites = 0;
  std::size_t message_send_selector_lowering_argument_expression_sites = 0;
  std::size_t message_send_selector_lowering_receiver_sites = 0;
  std::size_t message_send_selector_lowering_selector_literal_entries = 0;
  std::size_t message_send_selector_lowering_selector_literal_characters = 0;
  bool deterministic_message_send_selector_lowering_handoff = false;
  std::string lowering_dispatch_abi_marshalling_replay_key;
  std::size_t dispatch_abi_marshalling_message_send_sites = 0;
  std::size_t dispatch_abi_marshalling_receiver_slots_marshaled = 0;
  std::size_t dispatch_abi_marshalling_selector_slots_marshaled = 0;
  std::size_t dispatch_abi_marshalling_argument_value_slots_marshaled = 0;
  std::size_t dispatch_abi_marshalling_argument_padding_slots_marshaled = 0;
  std::size_t dispatch_abi_marshalling_argument_total_slots_marshaled = 0;
  std::size_t dispatch_abi_marshalling_total_marshaled_slots = 0;
  std::size_t dispatch_abi_marshalling_runtime_dispatch_arg_slots = 0;
  bool deterministic_dispatch_abi_marshalling_handoff = false;
  std::string lowering_nil_receiver_semantics_foldability_replay_key;
  std::size_t nil_receiver_semantics_foldability_message_send_sites = 0;
  std::size_t nil_receiver_semantics_foldability_receiver_nil_literal_sites = 0;
  std::size_t nil_receiver_semantics_foldability_enabled_sites = 0;
  std::size_t nil_receiver_semantics_foldability_foldable_sites = 0;
  std::size_t nil_receiver_semantics_foldability_runtime_dispatch_required_sites =
      0;
  std::size_t nil_receiver_semantics_foldability_non_nil_receiver_sites = 0;
  std::size_t nil_receiver_semantics_foldability_contract_violation_sites = 0;
  bool deterministic_nil_receiver_semantics_foldability_handoff = false;
  std::string lowering_super_dispatch_method_family_replay_key;
  std::size_t super_dispatch_method_family_message_send_sites = 0;
  std::size_t super_dispatch_method_family_receiver_super_identifier_sites = 0;
  std::size_t super_dispatch_method_family_enabled_sites = 0;
  std::size_t super_dispatch_method_family_requires_class_context_sites = 0;
  std::size_t super_dispatch_method_family_init_sites = 0;
  std::size_t super_dispatch_method_family_copy_sites = 0;
  std::size_t super_dispatch_method_family_mutable_copy_sites = 0;
  std::size_t super_dispatch_method_family_new_sites = 0;
  std::size_t super_dispatch_method_family_none_sites = 0;
  std::size_t super_dispatch_method_family_returns_retained_result_sites = 0;
  std::size_t super_dispatch_method_family_returns_related_result_sites = 0;
  std::size_t super_dispatch_method_family_contract_violation_sites = 0;
  bool deterministic_super_dispatch_method_family_handoff = false;
  std::string lowering_runtime_link_host_link_replay_key;
  std::size_t runtime_link_host_link_message_send_sites = 0;
  std::size_t runtime_link_host_link_required_sites = 0;
  std::size_t runtime_link_host_link_elided_sites = 0;
  std::size_t runtime_link_host_link_runtime_dispatch_arg_slots = 0;
  std::size_t runtime_link_host_link_runtime_dispatch_declaration_parameter_count =
      0;
  std::size_t runtime_link_host_link_contract_violation_sites = 0;
  std::string runtime_link_host_link_runtime_dispatch_symbol;
  bool runtime_link_host_link_default_runtime_dispatch_symbol_binding = true;
  bool deterministic_runtime_link_host_link_handoff = false;
};
