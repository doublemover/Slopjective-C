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
