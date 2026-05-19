static void EmitObjc3IRRuntimeMemberMetadataMethodListOwnerConstants(
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const Objc3IRRuntimeMetadataMethodListBundle &bundle,
    const std::string &owner_identity_symbol,
    const std::string &export_owner_identity_symbol,
    std::ostringstream &out) {
  out << owner_identity_symbol << " = private constant ["
      << (bundle.declaration_owner_identity.size() + 1u) << " x i8] c\""
      << EscapeCStringLiteral(bundle.declaration_owner_identity)
      << "\\00\", section \"" << family.emitted_section_name
      << "\", align 1\n";
  out << export_owner_identity_symbol << " = private constant ["
      << (bundle.export_owner_identity.size() + 1u) << " x i8] c\""
      << EscapeCStringLiteral(bundle.export_owner_identity)
      << "\\00\", section \"" << family.emitted_section_name
      << "\", align 1\n";
}

static bool BuildObjc3IRRuntimeMemberMetadataMethodEntryInitializers(
    const Objc3RuntimeMetadataLayoutPolicy &layout_policy,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const Objc3IRRuntimeMetadataMethodListBundle &bundle,
    std::size_t bundle_index,
    const std::unordered_map<std::string, std::string>
        &implementation_method_symbols_by_owner_identity,
    std::ostringstream &out, std::vector<std::string> &entry_initializers,
    std::string &error) {
  entry_initializers.reserve(bundle.entries_lexicographic.size());
  for (std::size_t entry_index = 0;
       entry_index < bundle.entries_lexicographic.size(); ++entry_index) {
    const auto &entry = bundle.entries_lexicographic[entry_index];
    const std::string bundle_ordinal =
        FormatObjc3IRRuntimeMetadataDescriptorOrdinal(bundle_index);
    const std::string entry_ordinal =
        FormatObjc3IRRuntimeMetadataDescriptorOrdinal(entry_index);
    const std::string selector_symbol =
        "@" + layout_policy.descriptor_symbol_prefix + family.kind + "_" +
        bundle.list_kind + "_method_selector_" + bundle_ordinal + "_" +
        entry_ordinal;
    const std::string entry_owner_identity_symbol =
        "@" + layout_policy.descriptor_symbol_prefix + family.kind + "_" +
        bundle.list_kind + "_method_owner_identity_" + bundle_ordinal + "_" +
        entry_ordinal;
    const std::string return_type_symbol =
        "@" + layout_policy.descriptor_symbol_prefix + family.kind + "_" +
        bundle.list_kind + "_method_return_type_" + bundle_ordinal + "_" +
        entry_ordinal;
    std::string implementation_symbol = "null";
    if (entry.has_body &&
        (bundle.owner_kind == "class-implementation" ||
         bundle.owner_kind == "category-implementation")) {
      // executable method-body binding anchor: implementation-owned method
      // entries must resolve to one emitted LLVM body symbol.
      const auto implementation_it =
          implementation_method_symbols_by_owner_identity.find(
              entry.owner_identity);
      if (implementation_it ==
          implementation_method_symbols_by_owner_identity.end()) {
        error = "missing executable method-body binding for owner identity '" +
                entry.owner_identity + "' in emitted " + bundle.owner_kind +
                " " + bundle.list_kind + " method list";
        return false;
      }
      implementation_symbol = implementation_it->second;
    }

    out << selector_symbol << " = private constant ["
        << (entry.selector.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(entry.selector) << "\\00\", section \""
        << family.emitted_section_name << "\", align 1\n";
    out << entry_owner_identity_symbol << " = private constant ["
        << (entry.owner_identity.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(entry.owner_identity) << "\\00\", section \""
        << family.emitted_section_name << "\", align 1\n";
    out << return_type_symbol << " = private constant ["
        << (entry.return_type_name.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(entry.return_type_name)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";
    std::ostringstream entry_initializer;
    // dispatch-control lowering anchor: method entries preserve
    // effective-direct and objc_final intent bits alongside implementation
    // bindings.
    entry_initializer << "{ ptr, ptr, ptr, i64, ptr, i64, i1, i1 } { ptr "
                      << selector_symbol << ", ptr "
                      << entry_owner_identity_symbol << ", ptr "
                      << return_type_symbol << ", i64 "
                      << entry.parameter_count << ", ptr "
                      << implementation_symbol << ", i64 "
                      << (entry.has_body ? 1 : 0) << ", i1 "
                      << (entry.effective_direct_dispatch ? 1 : 0) << ", i1 "
                      << (entry.objc_final_declared ? 1 : 0) << " }";
    entry_initializers.push_back(entry_initializer.str());
  }
  return true;
}

static void EmitObjc3IRRuntimeMemberMetadataMethodListGlobal(
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const std::string &list_symbol,
    const std::string &owner_identity_symbol,
    const std::string &export_owner_identity_symbol,
    const std::vector<std::string> &entry_initializers,
    std::ostringstream &out, std::vector<std::string> &retained_globals) {
  out << list_symbol << " = private global ";
  if (entry_initializers.empty()) {
    out << "{ i64, ptr, ptr } { i64 0, ptr " << owner_identity_symbol
        << ", ptr " << export_owner_identity_symbol << " }";
  } else {
    out << "{ i64, ptr, ptr, [" << entry_initializers.size()
        << " x { ptr, ptr, ptr, i64, ptr, i64, i1, i1 }] } { i64 "
        << entry_initializers.size() << ", ptr " << owner_identity_symbol
        << ", ptr " << export_owner_identity_symbol << ", ["
        << entry_initializers.size()
        << " x { ptr, ptr, ptr, i64, ptr, i64, i1, i1 }] [";
    for (std::size_t entry_index = 0;
         entry_index < entry_initializers.size(); ++entry_index) {
      if (entry_index != 0) {
        out << ", ";
      }
      out << entry_initializers[entry_index];
    }
    out << "] }";
  }
  out << ", section \"" << family.emitted_section_name << "\", align 8\n";
  RetainObjc3IRRuntimeMemberMetadataGlobal(retained_globals, list_symbol);
}

static bool EmitObjc3IRRuntimeMemberMetadataMethodListBundle(
    const Objc3IRRuntimeMemberMetadataEmissionOptions &options,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    std::size_t bundle_index, std::ostringstream &out,
    std::vector<std::string> &retained_globals, std::string &error) {
  const Objc3IRFrontendMetadata &frontend_metadata =
      options.frontend_metadata;
  const Objc3RuntimeMetadataLayoutPolicy &layout_policy =
      options.layout_policy;
  const auto &bundle =
      frontend_metadata.runtime_metadata_method_list_bundles_lexicographic
          [bundle_index];
  if (bundle.owner_family_kind != family.kind) {
    return true;
  }

  const auto method_list_it = options.method_list_symbols_by_key.find(
      BuildObjc3IRRuntimeMemberMetadataMethodListKey(
          bundle.owner_family_kind, bundle.declaration_owner_identity,
          bundle.list_kind));
  if (method_list_it == options.method_list_symbols_by_key.end()) {
    return true;
  }

  const std::string list_symbol = method_list_it->second;
  const std::string owner_identity_symbol =
      BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
          layout_policy, family, bundle.list_kind + "_methods_owner_identity",
          bundle_index);
  const std::string export_owner_identity_symbol =
      BuildObjc3IRRuntimeMemberMetadataAuxiliarySymbol(
          layout_policy, family,
          bundle.list_kind + "_methods_export_owner_identity", bundle_index);

  EmitObjc3IRRuntimeMemberMetadataMethodListOwnerConstants(
      family, bundle, owner_identity_symbol, export_owner_identity_symbol, out);

  std::vector<std::string> entry_initializers;
  if (!BuildObjc3IRRuntimeMemberMetadataMethodEntryInitializers(
          layout_policy, family, bundle, bundle_index,
          options.implementation_method_symbols_by_owner_identity, out,
          entry_initializers, error)) {
    return false;
  }
  EmitObjc3IRRuntimeMemberMetadataMethodListGlobal(
      family, list_symbol, owner_identity_symbol, export_owner_identity_symbol,
      entry_initializers, out, retained_globals);
  return true;
}
