#include "parser/frontend/objc3_parser_type_projection.h"

namespace objc3c::parse {

void CopyObjc3PropertyTypeFromParam(const FuncParam &source,
                                    Objc3PropertyDecl &target) {
  target.type = source.type;
  target.vector_spelling = source.vector_spelling;
  target.vector_base_spelling = source.vector_base_spelling;
  target.vector_lane_count = source.vector_lane_count;
  target.id_spelling = source.id_spelling;
  target.class_spelling = source.class_spelling;
  target.sel_spelling = source.sel_spelling;
  target.instancetype_spelling = source.instancetype_spelling;
  target.object_pointer_type_spelling = source.object_pointer_type_spelling;
  target.object_pointer_type_name = source.object_pointer_type_name;
  target.typecheck_family_symbol = source.typecheck_family_symbol;
  target.has_generic_suffix = source.has_generic_suffix;
  target.generic_suffix_terminated = source.generic_suffix_terminated;
  target.generic_suffix_text = source.generic_suffix_text;
  target.generic_line = source.generic_line;
  target.generic_column = source.generic_column;
  target.lightweight_generic_constraint_profile_is_normalized =
      source.lightweight_generic_constraint_profile_is_normalized;
  target.lightweight_generic_constraint_profile =
      source.lightweight_generic_constraint_profile;
  target.nullability_flow_profile_is_normalized =
      source.nullability_flow_profile_is_normalized;
  target.nullability_flow_profile =
      source.nullability_flow_profile;
  target.protocol_qualified_object_type_profile_is_normalized =
      source.protocol_qualified_object_type_profile_is_normalized;
  target.protocol_qualified_object_type_profile =
      source.protocol_qualified_object_type_profile;
  target.variance_bridge_cast_profile_is_normalized =
      source.variance_bridge_cast_profile_is_normalized;
  target.variance_bridge_cast_profile =
      source.variance_bridge_cast_profile;
  target.generic_metadata_abi_profile_is_normalized =
      source.generic_metadata_abi_profile_is_normalized;
  target.generic_metadata_abi_profile =
      source.generic_metadata_abi_profile;
  target.module_import_graph_profile_is_normalized =
      source.module_import_graph_profile_is_normalized;
  target.module_import_graph_profile =
      source.module_import_graph_profile;
  target.namespace_collision_shadowing_profile_is_normalized =
      source.namespace_collision_shadowing_profile_is_normalized;
  target.namespace_collision_shadowing_profile =
      source.namespace_collision_shadowing_profile;
  target.public_private_api_partition_profile_is_normalized =
      source.public_private_api_partition_profile_is_normalized;
  target.public_private_api_partition_profile =
      source.public_private_api_partition_profile;
  target.incremental_module_cache_invalidation_profile_is_normalized =
      source.incremental_module_cache_invalidation_profile_is_normalized;
  target.incremental_module_cache_invalidation_profile =
      source.incremental_module_cache_invalidation_profile;
  target.cross_module_conformance_profile_is_normalized =
      source.cross_module_conformance_profile_is_normalized;
  target.cross_module_conformance_profile =
      source.cross_module_conformance_profile;
  target.has_pointer_declarator = source.has_pointer_declarator;
  target.pointer_declarator_depth = source.pointer_declarator_depth;
  target.pointer_declarator_tokens = source.pointer_declarator_tokens;
  target.nullability_suffix_tokens = source.nullability_suffix_tokens;
  target.has_ownership_qualifier = source.has_ownership_qualifier;
  target.ownership_qualifier_spelling = source.ownership_qualifier_spelling;
  target.ownership_qualifier_symbol = source.ownership_qualifier_symbol;
  target.ownership_qualifier_tokens = source.ownership_qualifier_tokens;
  target.ownership_insert_retain = source.ownership_insert_retain;
  target.ownership_insert_release = source.ownership_insert_release;
  target.ownership_insert_autorelease = source.ownership_insert_autorelease;
  target.ownership_operation_profile = source.ownership_operation_profile;
  target.ownership_is_weak_reference = source.ownership_is_weak_reference;
  target.ownership_is_unowned_reference = source.ownership_is_unowned_reference;
  target.ownership_is_unowned_safe_reference = source.ownership_is_unowned_safe_reference;
  target.ownership_lifetime_profile = source.ownership_lifetime_profile;
  target.ownership_runtime_hook_profile = source.ownership_runtime_hook_profile;
  target.ownership_arc_diagnostic_candidate = source.ownership_arc_diagnostic_candidate;
  target.ownership_arc_fixit_available = source.ownership_arc_fixit_available;
  target.ownership_arc_diagnostic_profile = source.ownership_arc_diagnostic_profile;
  target.ownership_arc_fixit_hint = source.ownership_arc_fixit_hint;
}

}  // namespace objc3c::parse
