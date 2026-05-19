static void EmitObjc3IRRuntimeIvarLayoutTables(
    const Objc3RuntimeMetadataLayoutPolicy &layout_policy,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    std::ostringstream &out, std::vector<std::string> &retained_globals,
    Objc3IRRuntimeIvarDescriptorSectionState &state) {
  std::size_t layout_table_ordinal = 0u;
  for (auto &[owner_identity, owner_descriptor_symbols] :
       state.descriptor_symbols_by_declaration_owner_identity) {
    std::stable_sort(owner_descriptor_symbols.begin(),
                     owner_descriptor_symbols.end(),
                     [](const auto &lhs, const auto &rhs) {
                       return lhs.first < rhs.first ||
                              (lhs.first == rhs.first &&
                               lhs.second < rhs.second);
                     });
    const std::string owner_identity_symbol =
        BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
            layout_policy, family, "layout_owner", layout_table_ordinal);
    const std::string layout_table_symbol =
        BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
            layout_policy, family, "layout_table", layout_table_ordinal);
    out << owner_identity_symbol << " = private constant ["
        << (owner_identity.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(owner_identity) << "\\00\", section \""
        << family.emitted_section_name << "\", align 1\n";
    out << layout_table_symbol << " = private global ";
    if (owner_descriptor_symbols.empty()) {
      out << "{ ptr, i64, i64 } { ptr " << owner_identity_symbol
          << ", i64 0, i64 0 }";
    } else {
      out << "{ ptr, i64, [" << owner_descriptor_symbols.size()
          << " x ptr], i64 } { ptr " << owner_identity_symbol << ", i64 "
          << owner_descriptor_symbols.size() << ", ["
          << owner_descriptor_symbols.size() << " x ptr] [";
      for (std::size_t symbol_index = 0u;
           symbol_index < owner_descriptor_symbols.size(); ++symbol_index) {
        if (symbol_index != 0u) {
          out << ", ";
        }
        out << "ptr " << owner_descriptor_symbols[symbol_index].second;
      }
      out << "], i64 "
          << state.instance_size_by_owner_identity[owner_identity] << " }";
    }
    out << ", section \"" << family.emitted_section_name << "\", align 8\n";
    RetainObjc3IRRuntimeMemberMetadataGlobal(retained_globals,
                                             layout_table_symbol);
    ++layout_table_ordinal;
  }
}

static void EmitObjc3IRRuntimeIvarAggregate(
    const Objc3RuntimeMetadataLayoutPolicy &layout_policy,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    std::ostringstream &out, std::vector<std::string> &retained_globals,
    const Objc3IRRuntimeIvarDescriptorSectionState &state) {
  const std::string aggregate_symbol = "@" + family.aggregate_symbol_name;
  out << aggregate_symbol << " = " << layout_policy.aggregate_linkage
      << (state.descriptor_symbols.empty() ? " constant " : " global ");
  if (state.descriptor_symbols.empty()) {
    out << "{ i64 } { i64 0 }";
  } else {
    out << "{ i64, [" << state.descriptor_symbols.size()
        << " x ptr] } { i64 " << state.descriptor_symbols.size() << ", ["
        << state.descriptor_symbols.size() << " x ptr] [";
    for (std::size_t i = 0; i < state.descriptor_symbols.size(); ++i) {
      if (i != 0) {
        out << ", ";
      }
      out << "ptr " << state.descriptor_symbols[i];
    }
    out << "] }";
  }
  out << ", section \"" << family.emitted_section_name << "\", align 8\n";
  RetainObjc3IRRuntimeMemberMetadataGlobal(retained_globals, aggregate_symbol);
}
