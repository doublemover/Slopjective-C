  void AssignObjcExecutablePropertySourceModelFields(
      Objc3PropertyDecl &property, const std::string &storage_owner_symbol,
      std::size_t slot_index, bool supports_storage_layout) {
    property.property_attribute_profile =
        BuildExecutablePropertyAttributeProfile(property);
    property.effective_getter_selector =
        property.has_getter ? TrimAsciiWhitespaceCopy(property.getter_selector)
                            : property.name;
    property.effective_setter_available = !property.is_readonly;
    property.effective_setter_selector =
        property.effective_setter_available
            ? (property.has_setter
                   ? TrimAsciiWhitespaceCopy(property.setter_selector)
                   : BuildDefaultPropertySetterSelector(property.name))
            : std::string{};
    property.executable_synthesized_binding_kind =
        supports_storage_layout ? "implicit-ivar" : "none";
    property.executable_synthesized_binding_symbol =
        supports_storage_layout
            ? storage_owner_symbol + "::" + BuildObjcPropertySynthesisSymbol(property)
            : std::string{};
    if (supports_storage_layout) {
      const Objc3ExecutablePropertyLayoutShape layout_shape =
          ComputeExecutablePropertyLayoutShape(property);
      property.executable_ivar_layout_symbol =
          storage_owner_symbol + "::objc_property_layout:" + property.name;
      property.executable_ivar_layout_slot_index = slot_index;
      property.executable_ivar_layout_size_bytes = layout_shape.size_bytes;
      property.executable_ivar_layout_alignment_bytes =
          layout_shape.alignment_bytes;
      property.executable_ivar_layout_offset_bytes = 0;
      property.executable_ivar_layout_padding_bytes = 0;
      property.executable_ivar_layout_inherited_slot_count = 0;
      property.executable_ivar_layout_inherited_size_bytes = 0;
      property.executable_ivar_layout_owner_size_bytes = 0;
      property.executable_ivar_init_order_index = slot_index;
      property.executable_ivar_destroy_order_index = 0;
      property.executable_ivar_layout_valid = false;
      property.executable_ivar_layout_replay_key.clear();
    } else {
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
    property.accessor_ownership_profile =
        BuildExecutableAccessorOwnershipProfile(property);
  }
