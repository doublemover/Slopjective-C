  bool FinalizeObjcInterfacePropertyIvarLayoutClosure(
      Objc3ParsedProgram &program, Objc3InterfaceDecl &decl,
      std::unordered_set<std::string> &visiting,
      std::unordered_set<std::string> &completed) {
    if (decl.has_category) {
      for (auto &property : decl.properties) {
        ClearObjcPropertyIvarLayoutClosure(property);
      }
      FinalizeObjcPropertySynthesisIvarBindingPackets(
          decl.properties, decl.property_synthesis_symbols_lexicographic,
          decl.ivar_binding_symbols_lexicographic);
      return true;
    }

    if (completed.count(decl.name) != 0u) {
      return true;
    }
    if (visiting.count(decl.name) != 0u) {
      diagnostics_.push_back(MakeDiag(
          decl.line, decl.column, "O3P150",
          "cyclic Objective-C interface inheritance cannot produce a stable ivar layout"));
      for (auto &property : decl.properties) {
        ClearObjcPropertyIvarLayoutClosure(property);
      }
      completed.insert(decl.name);
      return false;
    }

    visiting.insert(decl.name);
    std::size_t inherited_slot_count = 0u;
    std::size_t inherited_size_bytes = 0u;
    if (!decl.super_name.empty()) {
      Objc3InterfaceDecl *super_decl =
          FindObjcInterfaceLayoutOwner(program, decl.super_name);
      if (super_decl != nullptr) {
        FinalizeObjcInterfacePropertyIvarLayoutClosure(
            program, *super_decl, visiting, completed);
        for (const auto &property : super_decl->properties) {
          if (property.executable_ivar_layout_valid) {
            inherited_slot_count = std::max(
                inherited_slot_count,
                property.executable_ivar_layout_slot_index + 1u);
            inherited_size_bytes = std::max(
                inherited_size_bytes,
                property.executable_ivar_layout_owner_size_bytes);
          }
        }
      }
    }

    std::size_t running_slot = inherited_slot_count;
    std::size_t running_offset = inherited_size_bytes;
    std::size_t max_alignment = 1u;
    bool layout_valid = true;
    for (auto &property : decl.properties) {
      if (property.executable_ivar_layout_symbol.empty()) {
        ClearObjcPropertyIvarLayoutClosure(property);
        continue;
      }
      if (property.executable_ivar_layout_size_bytes == 0u ||
          !IsValidObjcIvarLayoutAlignment(
              property.executable_ivar_layout_alignment_bytes)) {
        diagnostics_.push_back(MakeDiag(
            property.line, property.column, "O3P151",
            "invalid Objective-C property ivar layout size or alignment"));
        ClearObjcPropertyIvarLayoutClosure(property);
        layout_valid = false;
        continue;
      }

      const std::size_t aligned_offset = AlignObjcIvarLayoutOffset(
          running_offset, property.executable_ivar_layout_alignment_bytes);
      if (aligned_offset < running_offset ||
          aligned_offset >
              std::numeric_limits<std::size_t>::max() -
                  property.executable_ivar_layout_size_bytes) {
        diagnostics_.push_back(MakeDiag(
            property.line, property.column, "O3P152",
            "Objective-C property ivar layout offset overflow"));
        ClearObjcPropertyIvarLayoutClosure(property);
        layout_valid = false;
        continue;
      }

      property.executable_ivar_layout_slot_index = running_slot;
      property.executable_ivar_layout_offset_bytes = aligned_offset;
      property.executable_ivar_layout_padding_bytes =
          aligned_offset - running_offset;
      property.executable_ivar_layout_inherited_slot_count =
          inherited_slot_count;
      property.executable_ivar_layout_inherited_size_bytes =
          inherited_size_bytes;
      property.executable_ivar_init_order_index = running_slot;
      property.executable_ivar_layout_valid = true;
      running_offset =
          aligned_offset + property.executable_ivar_layout_size_bytes;
      max_alignment =
          std::max(max_alignment,
                   property.executable_ivar_layout_alignment_bytes);
      ++running_slot;
    }

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
  }
