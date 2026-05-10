  struct Objc3ExecutablePropertyLayoutShape {
    std::size_t size_bytes = 0;
    std::size_t alignment_bytes = 1;
  };

  std::string TrimAsciiWhitespaceCopy(const std::string &text) {
    std::size_t start = 0;
    while (start < text.size() &&
           std::isspace(static_cast<unsigned char>(text[start])) != 0) {
      ++start;
    }
    std::size_t end = text.size();
    while (end > start &&
           std::isspace(static_cast<unsigned char>(text[end - 1])) != 0) {
      --end;
    }
    return text.substr(start, end - start);
  }

  std::string BuildDefaultPropertySetterSelector(
      const std::string &property_name) {
    if (property_name.empty()) {
      return {};
    }
    std::string selector = "set";
    selector.push_back(static_cast<char>(
        std::toupper(static_cast<unsigned char>(property_name.front()))));
    selector.append(property_name.substr(1));
    selector.push_back(':');
    return selector;
  }

  Objc3ExecutablePropertyLayoutShape ComputeExecutablePropertyLayoutShape(
      const Objc3PropertyDecl &property) {
    const auto scalar_layout = [&](std::size_t size_bytes,
                                   std::size_t alignment_bytes) {
      return Objc3ExecutablePropertyLayoutShape{size_bytes, alignment_bytes};
    };
    if (property.vector_spelling) {
      const std::size_t lane_width =
          property.type == ValueType::Bool ? 1u : 4u;
      const std::size_t size_bytes =
          lane_width * static_cast<std::size_t>(property.vector_lane_count);
      return scalar_layout(size_bytes, std::min<std::size_t>(size_bytes, 16u));
    }
    switch (property.type) {
    case ValueType::Bool:
      return scalar_layout(1u, 1u);
    case ValueType::I32:
      return scalar_layout(4u, 4u);
    case ValueType::Void:
      return scalar_layout(0u, 1u);
    case ValueType::ObjCId:
    case ValueType::ObjCClass:
    case ValueType::ObjCSel:
    case ValueType::ObjCProtocol:
    case ValueType::ObjCInstancetype:
    case ValueType::ObjCObjectPtr:
    case ValueType::Function:
    case ValueType::Unknown:
    default:
      return scalar_layout(8u, 8u);
    }
  }

  std::size_t AlignObjcIvarLayoutOffset(std::size_t offset,
                                        std::size_t alignment) const {
    const std::size_t effective_alignment =
        std::max<std::size_t>(alignment, 1u);
    const std::size_t remainder = offset % effective_alignment;
    return remainder == 0u ? offset
                           : offset + (effective_alignment - remainder);
  }

  bool IsValidObjcIvarLayoutAlignment(std::size_t alignment) const {
    return alignment != 0u && (alignment & (alignment - 1u)) == 0u;
  }

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
