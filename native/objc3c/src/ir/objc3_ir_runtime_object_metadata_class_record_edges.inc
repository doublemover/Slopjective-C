static Objc3IRRuntimeClassMetaclassEdgeReferences
ResolveObjc3IRRuntimeClassMetaclassEdgeReferences(
    const Objc3IRRuntimeMetadataClassMetaclassBundle &bundle,
    const std::unordered_map<std::string, std::string>
        &descriptor_symbols_by_owner_identity) {
  Objc3IRRuntimeClassMetaclassEdgeReferences refs;
  if (bundle.has_super) {
    const auto super_it = descriptor_symbols_by_owner_identity.find(
        bundle.super_bundle_owner_identity);
    if (super_it != descriptor_symbols_by_owner_identity.end()) {
      refs.super_bundle_symbol = super_it->second;
    }
  }
  return refs;
}

static void EmitObjc3IRRuntimeClassMetaclassAdoptedProtocolRefs(
    const Objc3IRRuntimeObjectMetadataEmissionOptions &options,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const Objc3IRRuntimeMetadataClassMetaclassBundle &bundle,
    const Objc3IRRuntimeClassMetaclassRecordSymbols &symbols,
    std::ostringstream &out) {
  const auto &protocol_descriptor_symbols_by_owner_identity =
      options.protocol_descriptor_symbols_by_owner_identity;

  out << symbols.adopted_protocol_refs_symbol << " = private global ";
  if (bundle.adopted_protocol_owner_identities_lexicographic.empty()) {
    out << "{ i64 } { i64 0 }";
  } else {
    out << "{ i64, ["
        << bundle.adopted_protocol_owner_identities_lexicographic.size()
        << " x ptr] } { i64 "
        << bundle.adopted_protocol_owner_identities_lexicographic.size()
        << ", ["
        << bundle.adopted_protocol_owner_identities_lexicographic.size()
        << " x ptr] [";
    for (std::size_t adopted_index = 0;
         adopted_index <
         bundle.adopted_protocol_owner_identities_lexicographic.size();
         ++adopted_index) {
      if (adopted_index != 0) {
        out << ", ";
      }
      std::string adopted_protocol_symbol = "null";
      const auto adopted_it =
          protocol_descriptor_symbols_by_owner_identity.find(
              bundle.adopted_protocol_owner_identities_lexicographic
                  [adopted_index]);
      if (adopted_it != protocol_descriptor_symbols_by_owner_identity.end()) {
        adopted_protocol_symbol = adopted_it->second;
      }
      out << "ptr " << adopted_protocol_symbol;
    }
    out << "] }";
  }
  out << ", section \"" << family.emitted_section_name << "\", align 8\n";
}
