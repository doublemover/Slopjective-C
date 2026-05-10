void EmitObjc3IRRuntimeProtocolBundleSection(
    const Objc3IRRuntimeObjectMetadataEmissionOptions &options,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    std::ostringstream &out, std::vector<std::string> &retained_globals) {
  const Objc3IRFrontendMetadata &frontend_metadata =
      options.frontend_metadata;
  const Objc3RuntimeMetadataLayoutPolicy &layout_policy =
      options.layout_policy;
  const auto &method_list_symbols_by_key = options.method_list_symbols_by_key;
  const auto &method_list_entry_counts_by_key =
      options.method_list_entry_counts_by_key;
  const auto &protocol_descriptor_symbols_by_owner_identity =
      options.protocol_descriptor_symbols_by_owner_identity;

  // protocol/category data emission anchor: protocol records now materialize
  // as real descriptor bundles with inherited protocol-ref lists inside the
  // protocol descriptor section.
  std::vector<std::string> descriptor_symbols;
  descriptor_symbols.reserve(
      frontend_metadata.runtime_metadata_protocol_bundles_lexicographic.size());
  for (std::size_t i = 0;
       i < frontend_metadata.runtime_metadata_protocol_bundles_lexicographic
               .size();
       ++i) {
    const auto &bundle =
        frontend_metadata.runtime_metadata_protocol_bundles_lexicographic[i];
    const std::string descriptor_symbol =
        BuildObjc3IRRuntimeObjectMetadataDescriptorSymbol(layout_policy, family,
                                                          i);
    const std::string name_symbol =
        BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(layout_policy, family,
                                                         "name", i);
    const std::string owner_identity_symbol =
        BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(
            layout_policy, family, "owner_identity", i);
    const std::string inherited_refs_symbol =
        BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(
            layout_policy, family, "inherited_protocol_refs", i);
    const std::string instance_method_list_ref_symbol =
        BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(
            layout_policy, family, "instance_method_list_ref", i);
    const std::string class_method_list_ref_symbol =
        BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(
            layout_policy, family, "class_method_list_ref", i);
    std::string instance_method_list_symbol = "null";
    const auto instance_method_list_it = method_list_symbols_by_key.find(
        BuildObjc3IRRuntimeObjectMetadataMethodListKey(
            family.kind, bundle.owner_identity, "instance"));
    if (instance_method_list_it != method_list_symbols_by_key.end()) {
      instance_method_list_symbol = instance_method_list_it->second;
    }
    std::size_t instance_method_list_entry_count = 0;
    const auto instance_method_count_it =
        method_list_entry_counts_by_key.find(
            BuildObjc3IRRuntimeObjectMetadataMethodListKey(
                family.kind, bundle.owner_identity, "instance"));
    if (instance_method_count_it != method_list_entry_counts_by_key.end()) {
      instance_method_list_entry_count = instance_method_count_it->second;
    }
    std::string class_method_list_symbol = "null";
    const auto class_method_list_it = method_list_symbols_by_key.find(
        BuildObjc3IRRuntimeObjectMetadataMethodListKey(
            family.kind, bundle.owner_identity, "class"));
    if (class_method_list_it != method_list_symbols_by_key.end()) {
      class_method_list_symbol = class_method_list_it->second;
    }
    std::size_t class_method_list_entry_count = 0;
    const auto class_method_count_it =
        method_list_entry_counts_by_key.find(
            BuildObjc3IRRuntimeObjectMetadataMethodListKey(
                family.kind, bundle.owner_identity, "class"));
    if (class_method_count_it != method_list_entry_counts_by_key.end()) {
      class_method_list_entry_count = class_method_count_it->second;
    }
    const std::size_t name_storage_len = bundle.protocol_name.size() + 1u;
    const std::size_t owner_identity_storage_len =
        bundle.owner_identity.size() + 1u;
    descriptor_symbols.push_back(descriptor_symbol);

    out << name_symbol << " = private constant [" << name_storage_len
        << " x i8] c\"" << EscapeCStringLiteral(bundle.protocol_name)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";
    out << owner_identity_symbol << " = private constant ["
        << owner_identity_storage_len << " x i8] c\""
        << EscapeCStringLiteral(bundle.owner_identity) << "\\00\", section \""
        << family.emitted_section_name << "\", align 1\n";
    out << inherited_refs_symbol << " = private global ";
    if (bundle.inherited_protocol_owner_identities_lexicographic.empty()) {
      out << "{ i64 } { i64 0 }";
    } else {
      out << "{ i64, ["
          << bundle.inherited_protocol_owner_identities_lexicographic.size()
          << " x ptr] } { i64 "
          << bundle.inherited_protocol_owner_identities_lexicographic.size()
          << ", ["
          << bundle.inherited_protocol_owner_identities_lexicographic.size()
          << " x ptr] [";
      for (std::size_t inherited_index = 0;
           inherited_index <
           bundle.inherited_protocol_owner_identities_lexicographic.size();
           ++inherited_index) {
        if (inherited_index != 0) {
          out << ", ";
        }
        std::string inherited_protocol_symbol = "null";
        const auto inherited_it =
            protocol_descriptor_symbols_by_owner_identity.find(
                bundle.inherited_protocol_owner_identities_lexicographic
                    [inherited_index]);
        if (inherited_it !=
            protocol_descriptor_symbols_by_owner_identity.end()) {
          inherited_protocol_symbol = inherited_it->second;
        }
        out << "ptr " << inherited_protocol_symbol;
      }
      out << "] }";
    }
    out << ", section \"" << family.emitted_section_name << "\", align 8\n";
    out << instance_method_list_ref_symbol
        << " = private global { i64, ptr, ptr } { i64 "
        << instance_method_list_entry_count << ", ptr "
        << owner_identity_symbol << ", ptr " << instance_method_list_symbol
        << " }, section \"" << family.emitted_section_name << "\", align 8\n";
    out << class_method_list_ref_symbol
        << " = private global { i64, ptr, ptr } { i64 "
        << class_method_list_entry_count << ", ptr " << owner_identity_symbol
        << ", ptr " << class_method_list_symbol << " }, section \""
        << family.emitted_section_name << "\", align 8\n";
    // executable realization-record expansion anchor: protocol records now
    // preserve split instance/class method counts alongside inherited protocol
    // edges so runtime conformance checks do not need to reconstruct that split
    // from sidecar summaries.
    out << descriptor_symbol
        << " = private global { ptr, ptr, ptr, ptr, ptr, i64, i64, i64, i64, i1 } { ptr "
        << name_symbol << ", ptr " << owner_identity_symbol << ", ptr "
        << inherited_refs_symbol << ", ptr " << instance_method_list_ref_symbol
        << ", ptr " << class_method_list_ref_symbol << ", i64 "
        << bundle.property_count << ", i64 " << bundle.method_count
        << ", i64 " << instance_method_list_entry_count << ", i64 "
        << class_method_list_entry_count << ", i1 "
        << (bundle.is_forward_declaration ? 1 : 0) << " }, section \""
        << family.emitted_section_name << "\", align 8\n";
    RetainObjc3IRRuntimeObjectMetadataGlobal(retained_globals,
                                             descriptor_symbol);
  }

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
  RetainObjc3IRRuntimeObjectMetadataGlobal(retained_globals, aggregate_symbol);
}
