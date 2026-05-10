static void EmitObjc3IRRuntimeClassMetaclassAggregate(
    const Objc3RuntimeMetadataLayoutPolicy &layout_policy,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const std::vector<std::string> &descriptor_symbols,
    std::ostringstream &out, std::vector<std::string> &retained_globals) {
  const std::string aggregate_symbol = "@" + family.aggregate_symbol_name;
  out << aggregate_symbol << " = " << layout_policy.aggregate_linkage
      << (descriptor_symbols.empty() ? " constant " : " global ");
  if (descriptor_symbols.empty()) {
    out << "{ i64 } { i64 0 }";
  } else {
    out << "{ i64, [" << descriptor_symbols.size()
        << " x ptr] } { i64 " << descriptor_symbols.size() << ", ["
        << descriptor_symbols.size() << " x ptr] [";
    for (std::size_t i = 0; i < descriptor_symbols.size(); ++i) {
      if (i != 0) {
        out << ", ";
      }
      out << "ptr " << descriptor_symbols[i];
    }
    out << "] }";
  }
  out << ", section \"" << family.emitted_section_name << "\", align 8\n";
  RetainObjc3IRRuntimeObjectMetadataGlobal(retained_globals,
                                           aggregate_symbol);
}
