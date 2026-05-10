  std::string BuildObjcIvarLayoutReplayKey(
      const Objc3PropertyDecl &property, const Objc3InterfaceDecl &owner) const {
    std::ostringstream out;
    out << "owner=" << owner.semantic_link_symbol
        << ";property=" << property.name
        << ";slot=" << property.executable_ivar_layout_slot_index
        << ";offset=" << property.executable_ivar_layout_offset_bytes
        << ";size=" << property.executable_ivar_layout_size_bytes
        << ";alignment=" << property.executable_ivar_layout_alignment_bytes
        << ";padding=" << property.executable_ivar_layout_padding_bytes
        << ";inherited_slots="
        << property.executable_ivar_layout_inherited_slot_count
        << ";inherited_size="
        << property.executable_ivar_layout_inherited_size_bytes
        << ";owner_size=" << property.executable_ivar_layout_owner_size_bytes;
    return out.str();
  }
