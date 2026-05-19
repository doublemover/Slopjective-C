    const std::size_t owner_size =
        AlignObjcIvarLayoutOffset(running_offset, max_alignment);
    for (auto &property : decl.properties) {
      if (!property.executable_ivar_layout_valid) {
        continue;
      }
      property.executable_ivar_layout_owner_size_bytes = owner_size;
      property.executable_ivar_destroy_order_index =
          running_slot > property.executable_ivar_layout_slot_index
              ? (running_slot - 1u) -
                    property.executable_ivar_layout_slot_index
              : 0u;
      property.executable_ivar_layout_replay_key =
          BuildObjcIvarLayoutReplayKey(property, decl);
    }

    FinalizeObjcPropertySynthesisIvarBindingPackets(
        decl.properties, decl.property_synthesis_symbols_lexicographic,
        decl.ivar_binding_symbols_lexicographic);
    visiting.erase(decl.name);
    completed.insert(decl.name);
    return layout_valid;
