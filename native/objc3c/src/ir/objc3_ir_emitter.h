#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "lower/objc3_lowering_contract.h"
#include "parse/objc3_parser_contract.h"

struct Objc3IRFrontendMetadata {
  std::uint8_t language_version = 3u;
  std::string compatibility_mode = "canonical";
  bool migration_assist = false;
  std::size_t migration_legacy_yes = 0;
  std::size_t migration_legacy_no = 0;
  std::size_t migration_legacy_null = 0;
  std::size_t declared_interfaces = 0;
  std::size_t declared_implementations = 0;
  std::size_t resolved_interface_symbols = 0;
  std::size_t resolved_implementation_symbols = 0;
  std::size_t interface_method_symbols = 0;
  std::size_t implementation_method_symbols = 0;
  std::size_t linked_implementation_symbols = 0;
  bool deterministic_interface_implementation_handoff = false;
  std::size_t declared_protocols = 0;
  std::size_t declared_categories = 0;
  std::size_t resolved_protocol_symbols = 0;
  std::size_t resolved_category_symbols = 0;
  std::size_t protocol_method_symbols = 0;
  std::size_t category_method_symbols = 0;
  std::size_t linked_category_symbols = 0;
  bool deterministic_protocol_category_handoff = false;
  std::size_t declared_class_interfaces = 0;
  std::size_t declared_class_implementations = 0;
  std::size_t resolved_class_interfaces = 0;
  std::size_t resolved_class_implementations = 0;
  std::size_t linked_class_method_symbols = 0;
  std::size_t linked_category_method_symbols = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t protocol_composition_symbols = 0;
  std::size_t category_composition_sites = 0;
  std::size_t category_composition_symbols = 0;
  std::size_t invalid_protocol_composition_sites = 0;
  bool deterministic_class_protocol_category_linking_handoff = false;
  std::size_t selector_method_declaration_entries = 0;
  std::size_t selector_normalized_method_declarations = 0;
  std::size_t selector_piece_entries = 0;
  std::size_t selector_piece_parameter_links = 0;
  bool deterministic_selector_normalization_handoff = false;
  std::size_t property_declaration_entries = 0;
  std::size_t property_attribute_entries = 0;
  std::size_t property_attribute_value_entries = 0;
  std::size_t property_accessor_modifier_entries = 0;
  std::size_t property_getter_selector_entries = 0;
  std::size_t property_setter_selector_entries = 0;
  bool deterministic_property_attribute_handoff = false;
  std::string lowering_property_synthesis_ivar_binding_replay_key;
  std::string lowering_id_class_sel_object_pointer_typecheck_replay_key;
  std::size_t id_typecheck_sites = 0;
  std::size_t class_typecheck_sites = 0;
  std::size_t sel_typecheck_sites = 0;
  std::size_t object_pointer_typecheck_sites = 0;
  std::size_t id_class_sel_object_pointer_typecheck_sites_total = 0;
  bool deterministic_id_class_sel_object_pointer_typecheck_handoff = false;
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
  std::size_t nil_receiver_semantics_foldability_runtime_dispatch_required_sites = 0;
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
  std::string lowering_runtime_shim_host_link_replay_key;
  std::size_t runtime_shim_host_link_message_send_sites = 0;
  std::size_t runtime_shim_host_link_required_sites = 0;
  std::size_t runtime_shim_host_link_elided_sites = 0;
  std::size_t runtime_shim_host_link_runtime_dispatch_arg_slots = 0;
  std::size_t runtime_shim_host_link_runtime_dispatch_declaration_parameter_count = 0;
  std::size_t runtime_shim_host_link_contract_violation_sites = 0;
  std::string runtime_shim_host_link_runtime_dispatch_symbol;
  bool runtime_shim_host_link_default_runtime_dispatch_symbol_binding = true;
  bool deterministic_runtime_shim_host_link_handoff = false;
  std::size_t object_pointer_type_spellings = 0;
  std::size_t pointer_declarator_entries = 0;
  std::size_t pointer_declarator_depth_total = 0;
  std::size_t pointer_declarator_token_entries = 0;
  std::size_t nullability_suffix_entries = 0;
  std::size_t generic_suffix_entries = 0;
  std::size_t terminated_generic_suffix_entries = 0;
  std::size_t unterminated_generic_suffix_entries = 0;
  bool deterministic_object_pointer_nullability_generics_handoff = false;
  std::size_t global_symbol_nodes = 0;
  std::size_t function_symbol_nodes = 0;
  std::size_t interface_symbol_nodes = 0;
  std::size_t implementation_symbol_nodes = 0;
  std::size_t interface_property_symbol_nodes = 0;
  std::size_t implementation_property_symbol_nodes = 0;
  std::size_t interface_method_symbol_nodes = 0;
  std::size_t implementation_method_symbol_nodes = 0;
  std::size_t top_level_scope_symbols = 0;
  std::size_t nested_scope_symbols = 0;
  std::size_t scope_frames_total = 0;
  std::size_t implementation_interface_resolution_sites = 0;
  std::size_t implementation_interface_resolution_hits = 0;
  std::size_t implementation_interface_resolution_misses = 0;
  std::size_t method_resolution_sites = 0;
  std::size_t method_resolution_hits = 0;
  std::size_t method_resolution_misses = 0;
  bool deterministic_symbol_graph_handoff = false;
  bool deterministic_scope_resolution_handoff = false;
  std::string deterministic_symbol_graph_scope_resolution_handoff_key;

  std::size_t migration_legacy_total() const { return migration_legacy_yes + migration_legacy_no + migration_legacy_null; }
};

bool EmitObjc3IRText(const Objc3ParsedProgram &program,
                     const Objc3LoweringContract &lowering_contract,
                     const Objc3IRFrontendMetadata &frontend_metadata,
                     std::string &ir,
                     std::string &error);
