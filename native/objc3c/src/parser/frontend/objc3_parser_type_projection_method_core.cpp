#include "parser/frontend/objc3_parser_type_projection_internal.h"

namespace objc3c::parse {

void CopyObjc3MethodReturnTypeCoreProjection(const FunctionDecl &source,
                                             Objc3MethodDecl &target) {
  target.return_type = source.return_type;
  target.return_vector_spelling = source.return_vector_spelling;
  target.return_vector_base_spelling = source.return_vector_base_spelling;
  target.return_vector_lane_count = source.return_vector_lane_count;
  target.return_id_spelling = source.return_id_spelling;
  target.return_class_spelling = source.return_class_spelling;
  target.return_sel_spelling = source.return_sel_spelling;
  target.return_instancetype_spelling = source.return_instancetype_spelling;
  target.return_object_pointer_type_spelling = source.return_object_pointer_type_spelling;
  target.return_object_pointer_type_name = source.return_object_pointer_type_name;
  target.return_value_optional = source.return_value_optional;
  target.return_typecheck_family_symbol = source.return_typecheck_family_symbol;
  target.has_return_generic_suffix = source.has_return_generic_suffix;
  target.return_generic_suffix_terminated = source.return_generic_suffix_terminated;
  target.return_generic_suffix_text = source.return_generic_suffix_text;
  target.return_generic_line = source.return_generic_line;
  target.return_generic_column = source.return_generic_column;
  target.return_lightweight_generic_constraint_profile_is_normalized =
      source.return_lightweight_generic_constraint_profile_is_normalized;
  target.return_lightweight_generic_constraint_profile =
      source.return_lightweight_generic_constraint_profile;
  target.return_nullability_flow_profile_is_normalized =
      source.return_nullability_flow_profile_is_normalized;
  target.return_nullability_flow_profile =
      source.return_nullability_flow_profile;
  target.return_protocol_qualified_object_type_profile_is_normalized =
      source.return_protocol_qualified_object_type_profile_is_normalized;
  target.return_protocol_qualified_object_type_profile =
      source.return_protocol_qualified_object_type_profile;
  target.return_variance_bridge_cast_profile_is_normalized =
      source.return_variance_bridge_cast_profile_is_normalized;
  target.return_variance_bridge_cast_profile =
      source.return_variance_bridge_cast_profile;
  target.return_generic_metadata_abi_profile_is_normalized =
      source.return_generic_metadata_abi_profile_is_normalized;
  target.return_generic_metadata_abi_profile =
      source.return_generic_metadata_abi_profile;
  target.return_module_import_graph_profile_is_normalized =
      source.return_module_import_graph_profile_is_normalized;
  target.return_module_import_graph_profile =
      source.return_module_import_graph_profile;
  target.return_namespace_collision_shadowing_profile_is_normalized =
      source.return_namespace_collision_shadowing_profile_is_normalized;
  target.return_namespace_collision_shadowing_profile =
      source.return_namespace_collision_shadowing_profile;
  target.return_public_private_api_partition_profile_is_normalized =
      source.return_public_private_api_partition_profile_is_normalized;
  target.return_public_private_api_partition_profile =
      source.return_public_private_api_partition_profile;
  target.return_incremental_module_cache_invalidation_profile_is_normalized =
      source.return_incremental_module_cache_invalidation_profile_is_normalized;
  target.return_incremental_module_cache_invalidation_profile =
      source.return_incremental_module_cache_invalidation_profile;
  target.return_cross_module_conformance_profile_is_normalized =
      source.return_cross_module_conformance_profile_is_normalized;
  target.return_cross_module_conformance_profile =
      source.return_cross_module_conformance_profile;
  target.has_return_pointer_declarator = source.has_return_pointer_declarator;
  target.return_pointer_declarator_depth = source.return_pointer_declarator_depth;
  target.return_pointer_declarator_tokens = source.return_pointer_declarator_tokens;
  target.return_nullability_suffix_tokens = source.return_nullability_suffix_tokens;
  target.has_return_ownership_qualifier = source.has_return_ownership_qualifier;
  target.return_ownership_qualifier_spelling = source.return_ownership_qualifier_spelling;
  target.return_ownership_qualifier_symbol = source.return_ownership_qualifier_symbol;
  target.return_ownership_qualifier_tokens = source.return_ownership_qualifier_tokens;
  target.return_ownership_insert_retain = source.return_ownership_insert_retain;
  target.return_ownership_insert_release = source.return_ownership_insert_release;
  target.return_ownership_insert_autorelease = source.return_ownership_insert_autorelease;
  target.return_ownership_operation_profile = source.return_ownership_operation_profile;
  target.return_ownership_is_weak_reference = source.return_ownership_is_weak_reference;
  target.return_ownership_is_unowned_reference = source.return_ownership_is_unowned_reference;
  target.return_ownership_is_unowned_safe_reference = source.return_ownership_is_unowned_safe_reference;
  target.return_ownership_lifetime_profile = source.return_ownership_lifetime_profile;
  target.return_ownership_runtime_hook_profile = source.return_ownership_runtime_hook_profile;
  target.return_ownership_arc_diagnostic_candidate = source.return_ownership_arc_diagnostic_candidate;
  target.return_ownership_arc_fixit_available = source.return_ownership_arc_fixit_available;
  target.return_ownership_arc_diagnostic_profile = source.return_ownership_arc_diagnostic_profile;
  target.return_ownership_arc_fixit_hint = source.return_ownership_arc_fixit_hint;
  target.return_borrowed_pointer_qualified = source.return_borrowed_pointer_qualified;
  target.objc_returns_borrowed_declared = source.objc_returns_borrowed_declared;
  target.objc_returns_borrowed_owner_index = source.objc_returns_borrowed_owner_index;
  target.returns_borrowed_profile = source.returns_borrowed_profile;
  target.throws_declared = source.throws_declared;
  target.typed_throws_declared = source.typed_throws_declared;
  target.typed_throws_payload = source.typed_throws_payload;
  target.throws_declaration_profile_is_normalized =
      source.throws_declaration_profile_is_normalized;
  target.throws_declaration_profile = source.throws_declaration_profile;
}

}  // namespace objc3c::parse
