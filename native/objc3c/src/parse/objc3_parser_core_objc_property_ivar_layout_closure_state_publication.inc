  void CopyObjcIvarLayoutClosure(Objc3PropertyDecl &target,
                                 const Objc3PropertyDecl &source) {
    target.executable_ivar_layout_symbol =
        source.executable_ivar_layout_symbol;
    target.executable_ivar_layout_slot_index =
        source.executable_ivar_layout_slot_index;
    target.executable_ivar_layout_size_bytes =
        source.executable_ivar_layout_size_bytes;
    target.executable_ivar_layout_alignment_bytes =
        source.executable_ivar_layout_alignment_bytes;
    target.executable_ivar_layout_offset_bytes =
        source.executable_ivar_layout_offset_bytes;
    target.executable_ivar_layout_padding_bytes =
        source.executable_ivar_layout_padding_bytes;
    target.executable_ivar_layout_inherited_slot_count =
        source.executable_ivar_layout_inherited_slot_count;
    target.executable_ivar_layout_inherited_size_bytes =
        source.executable_ivar_layout_inherited_size_bytes;
    target.executable_ivar_layout_owner_size_bytes =
        source.executable_ivar_layout_owner_size_bytes;
    target.executable_ivar_init_order_index =
        source.executable_ivar_init_order_index;
    target.executable_ivar_destroy_order_index =
        source.executable_ivar_destroy_order_index;
    target.executable_ivar_layout_valid =
        source.executable_ivar_layout_valid;
    target.executable_ivar_layout_replay_key =
        source.executable_ivar_layout_replay_key;
  }
