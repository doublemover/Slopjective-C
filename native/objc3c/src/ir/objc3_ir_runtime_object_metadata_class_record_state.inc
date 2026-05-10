struct Objc3IRRuntimeClassMetaclassRecordSymbols {
  std::string descriptor_symbol;
  std::string name_symbol;
  std::string owner_identity_symbol;
  std::string class_object_identity_symbol;
  std::string metaclass_object_identity_symbol;
  std::string instance_method_list_ref_symbol;
  std::string metaclass_method_list_ref_symbol;
  std::string adopted_protocol_refs_symbol;
  std::string class_super_object_identity_symbol = "null";
  std::string metaclass_super_object_identity_symbol = "null";
};

struct Objc3IRRuntimeClassMetaclassMethodListReferences {
  std::string instance_method_list_symbol = "null";
  std::size_t instance_method_list_entry_count = 0;
  std::string metaclass_method_list_symbol = "null";
  std::size_t metaclass_method_list_entry_count = 0;
};

struct Objc3IRRuntimeClassMetaclassEdgeReferences {
  std::string super_bundle_symbol = "null";
};

static std::unordered_map<std::string, std::string>
BuildObjc3IRRuntimeClassMetaclassDescriptorSymbolsByOwnerIdentity(
    const Objc3IRFrontendMetadata &frontend_metadata,
    const Objc3RuntimeMetadataLayoutPolicy &layout_policy,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family) {
  std::unordered_map<std::string, std::string>
      descriptor_symbols_by_owner_identity;
  descriptor_symbols_by_owner_identity.reserve(
      frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic
          .size());
  for (std::size_t i = 0;
       i < frontend_metadata
               .runtime_metadata_class_metaclass_bundles_lexicographic.size();
       ++i) {
    const auto &bundle =
        frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic
            [i];
    descriptor_symbols_by_owner_identity.emplace(
        bundle.owner_identity,
        BuildObjc3IRRuntimeObjectMetadataDescriptorSymbol(layout_policy, family,
                                                          i));
  }
  return descriptor_symbols_by_owner_identity;
}

static Objc3IRRuntimeClassMetaclassRecordSymbols
BuildObjc3IRRuntimeClassMetaclassRecordSymbols(
    const Objc3RuntimeMetadataLayoutPolicy &layout_policy,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const Objc3IRRuntimeMetadataClassMetaclassBundle &bundle,
    std::size_t ordinal) {
  Objc3IRRuntimeClassMetaclassRecordSymbols symbols;
  symbols.descriptor_symbol =
      BuildObjc3IRRuntimeObjectMetadataDescriptorSymbol(layout_policy, family,
                                                        ordinal);
  symbols.name_symbol = BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(
      layout_policy, family, "name", ordinal);
  symbols.owner_identity_symbol =
      BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(
          layout_policy, family, "owner_identity", ordinal);
  symbols.class_object_identity_symbol =
      BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(
          layout_policy, family, "class_object_identity", ordinal);
  symbols.metaclass_object_identity_symbol =
      BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(
          layout_policy, family, "metaclass_object_identity", ordinal);
  symbols.instance_method_list_ref_symbol =
      BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(
          layout_policy, family, "instance_method_list_ref", ordinal);
  symbols.metaclass_method_list_ref_symbol =
      BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(
          layout_policy, "metaclass", "method_list_ref", ordinal);
  symbols.adopted_protocol_refs_symbol =
      BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(
          layout_policy, family, "adopted_protocol_refs", ordinal);

  if (bundle.has_super) {
    symbols.class_super_object_identity_symbol =
        BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(
            layout_policy, family, "super_class_object_identity", ordinal);
    symbols.metaclass_super_object_identity_symbol =
        BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(
            layout_policy, family, "super_metaclass_object_identity", ordinal);
  }
  return symbols;
}
