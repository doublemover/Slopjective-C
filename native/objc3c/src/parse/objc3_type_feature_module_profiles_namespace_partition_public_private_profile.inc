std::string BuildPublicPrivateApiPartitionProfile(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  const ModuleNamespacePartitionEvidence evidence =
      CollectModuleNamespacePartitionEvidence(
          object_pointer_type_spelling, has_generic_suffix,
          generic_suffix_terminated, has_pointer_declarator,
          generic_suffix_text, object_pointer_type_name);

  std::ostringstream out;
  out << "public-private-api-partition:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";namespace-segments=" << evidence.namespace_segments
      << ";import-edge-candidates=" << evidence.import_edge_candidates
      << ";public-api-safe=" << (evidence.public_api_safe ? "true" : "false")
      << ";private-partition-required="
      << (evidence.private_partition_required ? "true" : "false")
      << ";partition-ready=" << (evidence.partition_ready ? "true" : "false")
      << ";pointer-partition-overlap="
      << (evidence.pointer_partition_overlap ? "true" : "false");
  return out.str();
}
