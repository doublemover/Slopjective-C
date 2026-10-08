#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "ast/objc3_ast_core.h"
#include "ast/objc3_ast_method_decl_nodes.h"
#include "token/objc3_token_contract.h"

struct Objc3PropertyAttributeDecl {
  std::string name;
  std::string value;
  bool has_value = false;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3PropertyDecl {
  std::string name;
  ValueType type = ValueType::Unknown;
  bool vector_spelling = false;
  std::string vector_base_spelling;
  unsigned vector_lane_count = 1;
  bool id_spelling = false;
  bool class_spelling = false;
  bool sel_spelling = false;
  bool instancetype_spelling = false;
  bool object_pointer_type_spelling = false;
  std::string object_pointer_type_name;
  Objc3ValueOptionalTypeDescriptor value_optional;
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
  std::vector<Objc3PropertyAttributeDecl> attributes;
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
  bool has_weak_unowned_conflict = false;
  bool ownership_is_weak_reference = false;
  bool ownership_is_unowned_reference = false;
  bool ownership_is_unowned_safe_reference = false;
  std::string ownership_lifetime_profile;
  std::string ownership_runtime_hook_profile;
  bool ownership_arc_diagnostic_candidate = false;
  bool ownership_arc_fixit_available = false;
  std::string ownership_arc_diagnostic_profile;
  std::string ownership_arc_fixit_hint;
  bool has_getter = false;
  bool has_setter = false;
  std::string getter_selector;
  std::string setter_selector;
  Objc3ProtocolRequirementKind protocol_requirement_kind =
      Objc3ProtocolRequirementKind::NotApplicable;
  std::string scope_owner_symbol;
  std::string scope_path_symbol;
  std::string property_synthesis_symbol;
  std::string ivar_binding_symbol;
  // synthesized accessor/property lowering anchor: lane-C consumes
  // the effective accessor selectors plus synthesized binding identity below
  // to materialize executable getter/setter bodies without reopening property
  // parsing or sema ownership/layout derivation.
  // runtime property/layout consumption freeze anchor: lane-D must
  // consume the same emitted binding and layout identities below rather than
  // rederiving property storage or allocator state from source.
  // instance-allocation-layout-runtime anchor: the same emitted
  // binding and layout identities must also be sufficient for true
  // per-instance allocation and slot storage without rederiving layout from
  // source.
  // property-metadata-reflection anchor: private runtime reflection
  // helpers must likewise surface property/accessor/layout facts from these
  // same emitted identities rather than rediscovering them from source.
  // property-ivar-execution gate anchor: lane-E freezes the
  // executable claim over these same binding, accessor, and layout identities
  // before broader runnable sample expansion is allowed.
  // runnable property-ivar execution-matrix anchor: these same
  // emitted identities must now survive one live integrated storage,
  // synthesized-accessor, and reflection proof.
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
  unsigned line = 1;
  unsigned column = 1;
};
