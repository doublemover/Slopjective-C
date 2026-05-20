static bool IsCompatiblePropertyTypeMetadataSignature(const Objc3SemanticPropertyTypeMetadata &lhs,
                                                      const Objc3SemanticPropertyTypeMetadata &rhs) {
  // property-ivar executable semantics anchor:
  // runtime-meaningful property compatibility preserves declaration-level attribute,
  // accessor, and ownership semantics while excluding storage-local layout identities.
  return lhs.type == rhs.type &&
         lhs.is_vector == rhs.is_vector &&
         lhs.vector_base_spelling == rhs.vector_base_spelling &&
         lhs.vector_lane_count == rhs.vector_lane_count &&
         lhs.id_spelling == rhs.id_spelling &&
         lhs.class_spelling == rhs.class_spelling &&
         lhs.instancetype_spelling == rhs.instancetype_spelling &&
         lhs.is_readonly == rhs.is_readonly &&
         lhs.is_readwrite == rhs.is_readwrite &&
         lhs.is_atomic == rhs.is_atomic &&
         lhs.is_nonatomic == rhs.is_nonatomic &&
         lhs.is_copy == rhs.is_copy &&
         lhs.is_retain == rhs.is_retain &&
         lhs.is_strong == rhs.is_strong &&
         lhs.is_weak == rhs.is_weak &&
         lhs.is_unowned == rhs.is_unowned &&
         lhs.is_unsafe_unretained == rhs.is_unsafe_unretained &&
         lhs.is_assign == rhs.is_assign &&
         lhs.is_nullable == rhs.is_nullable &&
         lhs.is_nonnull == rhs.is_nonnull &&
         lhs.is_null_resettable == rhs.is_null_resettable &&
         lhs.is_class == rhs.is_class &&
         lhs.is_direct == rhs.is_direct &&
         lhs.has_getter == rhs.has_getter &&
         lhs.has_setter == rhs.has_setter &&
         lhs.getter_selector == rhs.getter_selector &&
         lhs.setter_selector == rhs.setter_selector &&
         lhs.ownership_lifetime_profile == rhs.ownership_lifetime_profile &&
         lhs.ownership_runtime_hook_profile == rhs.ownership_runtime_hook_profile &&
         lhs.property_attribute_profile == rhs.property_attribute_profile &&
         lhs.property_behavior_declared == rhs.property_behavior_declared &&
         lhs.property_behavior_name == rhs.property_behavior_name &&
         lhs.effective_getter_selector == rhs.effective_getter_selector &&
         lhs.effective_setter_available == rhs.effective_setter_available &&
         lhs.effective_setter_selector == rhs.effective_setter_selector &&
         lhs.accessor_ownership_profile == rhs.accessor_ownership_profile;
}

static bool HasResolvedDefaultIvarBinding(const Objc3PropertyInfo &property) {
  return property.executable_synthesized_binding_kind == "implicit-ivar" &&
         !property.ivar_binding_symbol.empty() &&
         !property.executable_synthesized_binding_symbol.empty() &&
         !property.executable_ivar_layout_symbol.empty() &&
         property.executable_ivar_layout_alignment_bytes > 0u;
}

static bool HasResolvedDefaultIvarBinding(
    const Objc3SemanticPropertyTypeMetadata &property) {
  return property.executable_synthesized_binding_kind == "implicit-ivar" &&
         !property.ivar_binding_symbol.empty() &&
         !property.executable_synthesized_binding_symbol.empty() &&
         !property.executable_ivar_layout_symbol.empty() &&
         property.executable_ivar_layout_alignment_bytes > 0u;
}

static bool HasCompatibleResolvedDefaultIvarBinding(
    const Objc3PropertyInfo &interface_property,
    const Objc3PropertyInfo &implementation_property) {
  return interface_property.ivar_binding_symbol ==
             implementation_property.ivar_binding_symbol &&
         interface_property.executable_synthesized_binding_kind ==
             implementation_property.executable_synthesized_binding_kind &&
         interface_property.executable_synthesized_binding_symbol ==
             implementation_property.executable_synthesized_binding_symbol &&
         interface_property.executable_ivar_layout_symbol ==
             implementation_property.executable_ivar_layout_symbol &&
         interface_property.executable_ivar_layout_slot_index ==
             implementation_property.executable_ivar_layout_slot_index &&
         interface_property.executable_ivar_layout_size_bytes ==
             implementation_property.executable_ivar_layout_size_bytes &&
         interface_property.executable_ivar_layout_alignment_bytes ==
             implementation_property.executable_ivar_layout_alignment_bytes &&
         interface_property.executable_ivar_layout_offset_bytes ==
             implementation_property.executable_ivar_layout_offset_bytes &&
         interface_property.executable_ivar_layout_padding_bytes ==
             implementation_property.executable_ivar_layout_padding_bytes &&
         interface_property.executable_ivar_layout_inherited_slot_count ==
             implementation_property.executable_ivar_layout_inherited_slot_count &&
         interface_property.executable_ivar_layout_inherited_size_bytes ==
             implementation_property.executable_ivar_layout_inherited_size_bytes &&
         interface_property.executable_ivar_layout_owner_size_bytes ==
             implementation_property.executable_ivar_layout_owner_size_bytes &&
         interface_property.executable_ivar_init_order_index ==
             implementation_property.executable_ivar_init_order_index &&
         interface_property.executable_ivar_destroy_order_index ==
             implementation_property.executable_ivar_destroy_order_index &&
         interface_property.executable_ivar_layout_valid ==
             implementation_property.executable_ivar_layout_valid &&
         interface_property.executable_ivar_layout_replay_key ==
             implementation_property.executable_ivar_layout_replay_key;
}

static bool HasCompatibleResolvedDefaultIvarBinding(
    const Objc3SemanticPropertyTypeMetadata &interface_property,
    const Objc3SemanticPropertyTypeMetadata &implementation_property) {
  return interface_property.ivar_binding_symbol ==
             implementation_property.ivar_binding_symbol &&
         interface_property.executable_synthesized_binding_kind ==
             implementation_property.executable_synthesized_binding_kind &&
         interface_property.executable_synthesized_binding_symbol ==
             implementation_property.executable_synthesized_binding_symbol &&
         interface_property.executable_ivar_layout_symbol ==
             implementation_property.executable_ivar_layout_symbol &&
         interface_property.executable_ivar_layout_slot_index ==
             implementation_property.executable_ivar_layout_slot_index &&
         interface_property.executable_ivar_layout_size_bytes ==
             implementation_property.executable_ivar_layout_size_bytes &&
         interface_property.executable_ivar_layout_alignment_bytes ==
             implementation_property.executable_ivar_layout_alignment_bytes &&
         interface_property.executable_ivar_layout_offset_bytes ==
             implementation_property.executable_ivar_layout_offset_bytes &&
         interface_property.executable_ivar_layout_padding_bytes ==
             implementation_property.executable_ivar_layout_padding_bytes &&
         interface_property.executable_ivar_layout_inherited_slot_count ==
             implementation_property.executable_ivar_layout_inherited_slot_count &&
         interface_property.executable_ivar_layout_inherited_size_bytes ==
             implementation_property.executable_ivar_layout_inherited_size_bytes &&
         interface_property.executable_ivar_layout_owner_size_bytes ==
             implementation_property.executable_ivar_layout_owner_size_bytes &&
         interface_property.executable_ivar_init_order_index ==
             implementation_property.executable_ivar_init_order_index &&
         interface_property.executable_ivar_destroy_order_index ==
             implementation_property.executable_ivar_destroy_order_index &&
         interface_property.executable_ivar_layout_valid ==
             implementation_property.executable_ivar_layout_valid &&
         interface_property.executable_ivar_layout_replay_key ==
             implementation_property.executable_ivar_layout_replay_key;
}
