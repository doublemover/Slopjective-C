bool IsPublicPrivateApiPartitionProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  const ModuleNamespacePartitionEvidence evidence =
      CollectModuleNamespacePartitionEvidence(
          object_pointer_type_spelling, has_generic_suffix,
          generic_suffix_terminated, false, generic_suffix_text,
          object_pointer_type_name);
  if (!evidence.private_partition_required) {
    return true;
  }
  if (evidence.import_edge_candidates == 0) {
    return object_pointer_type_spelling;
  }
  return generic_suffix_terminated && object_pointer_type_spelling;
}
