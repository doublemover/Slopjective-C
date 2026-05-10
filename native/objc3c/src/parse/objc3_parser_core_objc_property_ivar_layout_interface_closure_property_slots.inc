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
