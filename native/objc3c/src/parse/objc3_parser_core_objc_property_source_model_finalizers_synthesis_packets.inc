  void FinalizeObjcPropertySynthesisIvarBindingPackets(
      std::vector<Objc3PropertyDecl> &properties,
      std::vector<std::string> &property_synthesis_symbols_lexicographic,
      std::vector<std::string> &ivar_binding_symbols_lexicographic) {
    const std::size_t property_count = properties.size();
    for (auto &property : properties) {
      if (property.executable_ivar_layout_valid) {
        continue;
      }
      if (!property.executable_ivar_layout_symbol.empty()) {
        property.executable_ivar_init_order_index =
            property.executable_ivar_layout_slot_index;
        property.executable_ivar_destroy_order_index =
            property_count > property.executable_ivar_layout_slot_index
                ? (property_count - 1u) -
                      property.executable_ivar_layout_slot_index
                : 0u;
      } else {
        property.executable_ivar_init_order_index = 0;
        property.executable_ivar_destroy_order_index = 0;
      }
    }
    property_synthesis_symbols_lexicographic =
        BuildObjcPropertySynthesisSymbolsLexicographic(properties);
    ivar_binding_symbols_lexicographic =
        BuildObjcIvarBindingSymbolsLexicographic(properties);
  }
