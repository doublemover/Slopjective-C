#pragma once

#include <string>
#include <vector>

#include "ast/objc3_ast_value_type.h"
#include "token/objc3_token_contract.h"

struct FuncParam {
  std::string name;
  ValueType type = ValueType::I32;
  bool vector_spelling = false;
  std::string vector_base_spelling;
  unsigned vector_lane_count = 1;
  bool id_spelling = false;
  bool class_spelling = false;
  bool sel_spelling = false;
  bool instancetype_spelling = false;
  bool object_pointer_type_spelling = false;
  std::string object_pointer_type_name;
  std::string typecheck_family_symbol;
  bool has_generic_suffix = false;
  bool generic_suffix_terminated = true;
  std::string generic_suffix_text;
  unsigned generic_line = 1;
  unsigned generic_column = 1;
  bool lightweight_generic_constraint_profile_is_normalized = false;
  std::string lightweight_generic_constraint_profile;
  bool nullability_flow_profile_is_normalized = false;
  std::string nullability_flow_profile;
  bool protocol_qualified_object_type_profile_is_normalized = false;
  std::string protocol_qualified_object_type_profile;
  bool variance_bridge_cast_profile_is_normalized = false;
  std::string variance_bridge_cast_profile;
  bool generic_metadata_abi_profile_is_normalized = false;
  std::string generic_metadata_abi_profile;
  bool module_import_graph_profile_is_normalized = false;
  std::string module_import_graph_profile;
  bool namespace_collision_shadowing_profile_is_normalized = false;
  std::string namespace_collision_shadowing_profile;
  bool public_private_api_partition_profile_is_normalized = false;
  std::string public_private_api_partition_profile;
  bool incremental_module_cache_invalidation_profile_is_normalized = false;
  std::string incremental_module_cache_invalidation_profile;
  bool cross_module_conformance_profile_is_normalized = false;
  std::string cross_module_conformance_profile;
  bool has_pointer_declarator = false;
  unsigned pointer_declarator_depth = 0;
  std::vector<Objc3SemaTokenMetadata> pointer_declarator_tokens;
  std::vector<Objc3SemaTokenMetadata> nullability_suffix_tokens;
  bool has_ownership_qualifier = false;
  std::string ownership_qualifier_spelling;
  std::string ownership_qualifier_symbol;
  std::vector<Objc3SemaTokenMetadata> ownership_qualifier_tokens;
  bool ownership_insert_retain = false;
  bool ownership_insert_release = false;
  bool ownership_insert_autorelease = false;
  std::string ownership_operation_profile;
  bool ownership_is_weak_reference = false;
  bool ownership_is_unowned_reference = false;
  bool ownership_is_unowned_safe_reference = false;
  std::string ownership_lifetime_profile;
  std::string ownership_runtime_hook_profile;
  bool ownership_arc_diagnostic_candidate = false;
  bool ownership_arc_fixit_available = false;
  std::string ownership_arc_diagnostic_profile;
  std::string ownership_arc_fixit_hint;
  bool borrowed_pointer_qualified = false;
  std::string borrowed_pointer_profile;
  unsigned line = 1;
  unsigned column = 1;
};
