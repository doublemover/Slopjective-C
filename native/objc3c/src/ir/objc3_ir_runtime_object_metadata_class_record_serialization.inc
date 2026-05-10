static void EmitObjc3IRRuntimeClassMetaclassIdentityStrings(
    const Objc3IRRuntimeMetadataClassMetaclassBundle &bundle,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const Objc3IRRuntimeClassMetaclassRecordSymbols &symbols,
    std::ostringstream &out) {
  const std::size_t storage_len = bundle.class_name.size() + 1u;
  const std::size_t owner_identity_storage_len =
      bundle.owner_identity.size() + 1u;
  const std::size_t class_object_identity_storage_len =
      bundle.class_owner_identity.size() + 1u;
  const std::size_t metaclass_object_identity_storage_len =
      bundle.metaclass_owner_identity.size() + 1u;

  out << symbols.name_symbol << " = private constant [" << storage_len
      << " x i8] c\"" << EscapeCStringLiteral(bundle.class_name)
      << "\\00\", section \"" << family.emitted_section_name
      << "\", align 1\n";
  out << symbols.owner_identity_symbol << " = private constant ["
      << owner_identity_storage_len << " x i8] c\""
      << EscapeCStringLiteral(bundle.owner_identity)
      << "\\00\", section \"" << family.emitted_section_name
      << "\", align 1\n";
  out << symbols.class_object_identity_symbol << " = private constant ["
      << class_object_identity_storage_len << " x i8] c\""
      << EscapeCStringLiteral(bundle.class_owner_identity)
      << "\\00\", section \"" << family.emitted_section_name
      << "\", align 1\n";
  out << symbols.metaclass_object_identity_symbol << " = private constant ["
      << metaclass_object_identity_storage_len << " x i8] c\""
      << EscapeCStringLiteral(bundle.metaclass_owner_identity)
      << "\\00\", section \"" << family.emitted_section_name
      << "\", align 1\n";
  if (bundle.has_super) {
    out << symbols.class_super_object_identity_symbol
        << " = private constant ["
        << (bundle.super_class_owner_identity.size() + 1u) << " x i8] c\""
        << EscapeCStringLiteral(bundle.super_class_owner_identity)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";
    out << symbols.metaclass_super_object_identity_symbol
        << " = private constant ["
        << (bundle.super_metaclass_owner_identity.size() + 1u)
        << " x i8] c\""
        << EscapeCStringLiteral(bundle.super_metaclass_owner_identity)
        << "\\00\", section \"" << family.emitted_section_name
        << "\", align 1\n";
  }
}

static void EmitObjc3IRRuntimeClassMetaclassDescriptorRecord(
    const Objc3IRRuntimeMetadataClassMetaclassBundle &bundle,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const Objc3IRRuntimeClassMetaclassRecordSymbols &symbols,
    const Objc3IRRuntimeClassMetaclassEdgeReferences &edge_refs,
    std::ostringstream &out) {
  // executable realization-record expansion anchor: class/metaclass records
  // preserve bundle-owner, object-owner, super-object identities, method-list
  // refs, and container intent bits in-line.
  out << symbols.descriptor_symbol
      << " = private global { { ptr, ptr, ptr, ptr, ptr, ptr, ptr, i1, i1 }, { ptr, ptr, ptr, ptr, ptr, ptr, ptr, i1, i1 } } "
         "{ { ptr, ptr, ptr, ptr, ptr, ptr, ptr, i1, i1 } { ptr "
      << symbols.name_symbol << ", ptr " << symbols.owner_identity_symbol
      << ", ptr " << symbols.class_object_identity_symbol << ", ptr "
      << symbols.class_super_object_identity_symbol << ", ptr "
      << edge_refs.super_bundle_symbol << ", ptr "
      << symbols.instance_method_list_ref_symbol << ", ptr "
      << symbols.adopted_protocol_refs_symbol << ", i1 "
      << (bundle.objc_final_declared ? 1 : 0) << ", i1 "
      << (bundle.objc_sealed_declared ? 1 : 0)
      << " }, { ptr, ptr, ptr, ptr, ptr, ptr, ptr, i1, i1 } { ptr "
      << symbols.name_symbol << ", ptr " << symbols.owner_identity_symbol
      << ", ptr " << symbols.metaclass_object_identity_symbol << ", ptr "
      << symbols.metaclass_super_object_identity_symbol << ", ptr "
      << edge_refs.super_bundle_symbol << ", ptr "
      << symbols.metaclass_method_list_ref_symbol << ", ptr "
      << symbols.adopted_protocol_refs_symbol << ", i1 "
      << (bundle.objc_final_declared ? 1 : 0) << ", i1 "
      << (bundle.objc_sealed_declared ? 1 : 0) << " } }, section \""
      << family.emitted_section_name << "\", align 8\n";
}
