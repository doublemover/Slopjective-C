static void OverlayAuthoritativeDefaultIvarBinding(
    Objc3PropertyInfo &implementation_property,
    const Objc3PropertyInfo &interface_property) {
  implementation_property.ivar_binding_symbol =
      interface_property.ivar_binding_symbol;
  implementation_property.executable_synthesized_binding_kind =
      interface_property.executable_synthesized_binding_kind;
  implementation_property.executable_synthesized_binding_symbol =
      interface_property.executable_synthesized_binding_symbol;
  implementation_property.executable_ivar_layout_symbol =
      interface_property.executable_ivar_layout_symbol;
  implementation_property.executable_ivar_layout_slot_index =
      interface_property.executable_ivar_layout_slot_index;
  implementation_property.executable_ivar_layout_size_bytes =
      interface_property.executable_ivar_layout_size_bytes;
  implementation_property.executable_ivar_layout_alignment_bytes =
      interface_property.executable_ivar_layout_alignment_bytes;
  implementation_property.executable_ivar_layout_offset_bytes =
      interface_property.executable_ivar_layout_offset_bytes;
  implementation_property.executable_ivar_layout_padding_bytes =
      interface_property.executable_ivar_layout_padding_bytes;
  implementation_property.executable_ivar_layout_inherited_slot_count =
      interface_property.executable_ivar_layout_inherited_slot_count;
  implementation_property.executable_ivar_layout_inherited_size_bytes =
      interface_property.executable_ivar_layout_inherited_size_bytes;
  implementation_property.executable_ivar_layout_owner_size_bytes =
      interface_property.executable_ivar_layout_owner_size_bytes;
  implementation_property.executable_ivar_init_order_index =
      interface_property.executable_ivar_init_order_index;
  implementation_property.executable_ivar_destroy_order_index =
      interface_property.executable_ivar_destroy_order_index;
  implementation_property.executable_ivar_layout_valid =
      interface_property.executable_ivar_layout_valid;
  implementation_property.executable_ivar_layout_replay_key =
      interface_property.executable_ivar_layout_replay_key;
}
