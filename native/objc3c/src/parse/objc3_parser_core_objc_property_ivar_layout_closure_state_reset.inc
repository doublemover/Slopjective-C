  void ClearObjcPropertyIvarLayoutClosure(Objc3PropertyDecl &property) {
    property.executable_ivar_layout_symbol.clear();
    property.executable_ivar_layout_slot_index = 0;
    property.executable_ivar_layout_size_bytes = 0;
    property.executable_ivar_layout_alignment_bytes = 0;
    property.executable_ivar_layout_offset_bytes = 0;
    property.executable_ivar_layout_padding_bytes = 0;
    property.executable_ivar_layout_inherited_slot_count = 0;
    property.executable_ivar_layout_inherited_size_bytes = 0;
    property.executable_ivar_layout_owner_size_bytes = 0;
    property.executable_ivar_init_order_index = 0;
    property.executable_ivar_destroy_order_index = 0;
    property.executable_ivar_layout_valid = false;
    property.executable_ivar_layout_replay_key.clear();
  }
