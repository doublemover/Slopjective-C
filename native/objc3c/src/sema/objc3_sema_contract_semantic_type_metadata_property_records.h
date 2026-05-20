#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "sema/objc3_sema_contract_type_handoff_canonical_records.h"

struct Objc3SemanticPropertyTypeMetadata {
  std::string name;
  ValueType type = ValueType::Unknown;
  Objc3SemanticCanonicalType canonical_type;
  bool is_vector = false;
  std::string vector_base_spelling;
  unsigned vector_lane_count = 1;
  bool id_spelling = false;
  bool class_spelling = false;
  bool instancetype_spelling = false;
  bool object_pointer_type_spelling = false;
  bool has_generic_suffix = false;
  bool has_pointer_declarator = false;
  bool has_nullability_suffix = false;
  bool has_ownership_qualifier = false;
  bool has_invalid_generic_suffix = false;
  bool has_invalid_pointer_declarator = false;
  bool has_invalid_nullability_suffix = false;
  bool has_invalid_ownership_qualifier = false;
  bool has_invalid_type_suffix = false;
  bool ownership_insert_retain = false;
  bool ownership_insert_release = false;
  bool ownership_insert_autorelease = false;
  bool ownership_is_weak_reference = false;
  bool ownership_is_unowned_reference = false;
  bool ownership_is_unowned_safe_reference = false;
  bool ownership_arc_diagnostic_candidate = false;
  bool ownership_arc_fixit_available = false;
  std::string ownership_arc_diagnostic_profile;
  std::string ownership_arc_fixit_hint;
  std::string ownership_lifetime_profile;
  std::string ownership_runtime_hook_profile;
  std::size_t attribute_entries = 0;
  std::vector<std::string> attribute_names_lexicographic;
  bool is_readonly = false;
  bool is_readwrite = false;
  bool is_atomic = false;
  bool is_nonatomic = false;
  bool is_copy = false;
  bool is_retain = false;
  bool is_strong = false;
  bool is_weak = false;
  bool is_unowned = false;
  bool is_unsafe_unretained = false;
  bool is_assign = false;
  bool is_nullable = false;
  bool is_nonnull = false;
  bool is_null_resettable = false;
  bool is_class = false;
  bool is_direct = false;
  bool has_getter = false;
  bool has_setter = false;
  std::string getter_selector;
  std::string setter_selector;
  std::string ivar_binding_symbol;
  std::string executable_synthesized_binding_kind;
  std::string executable_synthesized_binding_symbol;
  std::string property_attribute_profile;
  bool property_behavior_declared = false;
  std::string property_behavior_name;
  std::string effective_getter_selector;
  bool effective_setter_available = false;
  std::string effective_setter_selector;
  std::string accessor_ownership_profile;
  std::string executable_ivar_layout_symbol;
  std::size_t executable_ivar_layout_slot_index = 0;
  std::size_t executable_ivar_layout_size_bytes = 0;
  std::size_t executable_ivar_layout_alignment_bytes = 0;
  std::size_t executable_ivar_layout_offset_bytes = 0;
  std::size_t executable_ivar_layout_padding_bytes = 0;
  std::size_t executable_ivar_layout_inherited_slot_count = 0;
  std::size_t executable_ivar_layout_inherited_size_bytes = 0;
  std::size_t executable_ivar_layout_owner_size_bytes = 0;
  std::size_t executable_ivar_init_order_index = 0;
  std::size_t executable_ivar_destroy_order_index = 0;
  bool executable_ivar_layout_valid = false;
  std::string executable_ivar_layout_replay_key;
  std::size_t invalid_attribute_entries = 0;
  std::size_t property_contract_violations = 0;
  bool has_unknown_attribute = false;
  bool has_duplicate_attribute = false;
  bool has_readwrite_conflict = false;
  bool has_atomicity_conflict = false;
  bool has_ownership_conflict = false;
  bool has_weak_unowned_conflict = false;
  bool has_accessor_selector_contract_violation = false;
  bool has_invalid_attribute_contract = false;
};
