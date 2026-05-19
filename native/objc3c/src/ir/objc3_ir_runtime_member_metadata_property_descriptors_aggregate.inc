static void EmitObjc3IRRuntimePropertyDescriptorAggregate(
    const Objc3RuntimeMetadataLayoutPolicy &layout_policy,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const Objc3IRRuntimePropertyDescriptorSectionState &state,
    std::ostringstream &out, std::vector<std::string> &retained_globals) {
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
