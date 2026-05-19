static Objc3IRRuntimeClassMetaclassMethodListReferences
ResolveObjc3IRRuntimeClassMetaclassMethodListReferences(
    const Objc3IRRuntimeObjectMetadataEmissionOptions &options,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const Objc3IRRuntimeMetadataClassMetaclassBundle &bundle) {
  Objc3IRRuntimeClassMetaclassMethodListReferences refs;
  const auto &method_list_symbols_by_key = options.method_list_symbols_by_key;
  const auto &method_list_entry_counts_by_key =
      options.method_list_entry_counts_by_key;

  const auto instance_method_list_it = method_list_symbols_by_key.find(
      BuildObjc3IRRuntimeObjectMetadataMethodListKey(
          family.kind, bundle.owner_identity, "instance"));
  if (instance_method_list_it != method_list_symbols_by_key.end()) {
    refs.instance_method_list_symbol = instance_method_list_it->second;
  }
  const auto instance_method_count_it = method_list_entry_counts_by_key.find(
      BuildObjc3IRRuntimeObjectMetadataMethodListKey(
          family.kind, bundle.owner_identity, "instance"));
  if (instance_method_count_it != method_list_entry_counts_by_key.end()) {
    refs.instance_method_list_entry_count =
        instance_method_count_it->second;
  }

  const auto metaclass_method_list_it = method_list_symbols_by_key.find(
      BuildObjc3IRRuntimeObjectMetadataMethodListKey(
          family.kind, bundle.owner_identity, "class"));
  if (metaclass_method_list_it != method_list_symbols_by_key.end()) {
    refs.metaclass_method_list_symbol = metaclass_method_list_it->second;
  }
  const auto metaclass_method_count_it = method_list_entry_counts_by_key.find(
      BuildObjc3IRRuntimeObjectMetadataMethodListKey(
          family.kind, bundle.owner_identity, "class"));
  if (metaclass_method_count_it != method_list_entry_counts_by_key.end()) {
    refs.metaclass_method_list_entry_count =
        metaclass_method_count_it->second;
  }
  return refs;
}

static void EmitObjc3IRRuntimeClassMetaclassMethodListReferences(
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const Objc3IRRuntimeClassMetaclassRecordSymbols &symbols,
    const Objc3IRRuntimeClassMetaclassMethodListReferences &refs,
    std::ostringstream &out) {
  out << symbols.instance_method_list_ref_symbol
      << " = private global { i64, ptr, ptr } { i64 "
      << refs.instance_method_list_entry_count << ", ptr "
      << symbols.owner_identity_symbol << ", ptr "
      << refs.instance_method_list_symbol << " }, section \""
      << family.emitted_section_name << "\", align 8\n";
  out << symbols.metaclass_method_list_ref_symbol
      << " = private global { i64, ptr, ptr } { i64 "
      << refs.metaclass_method_list_entry_count << ", ptr "
      << symbols.owner_identity_symbol << ", ptr "
      << refs.metaclass_method_list_symbol << " }, section \""
      << family.emitted_section_name << "\", align 8\n";
}
