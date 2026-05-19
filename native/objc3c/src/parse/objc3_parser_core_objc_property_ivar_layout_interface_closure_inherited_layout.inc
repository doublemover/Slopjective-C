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
