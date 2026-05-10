namespace {

struct ModuleNamespacePartitionEvidence {
  std::size_t import_edge_candidates = 0;
  std::size_t namespace_segments = 0;
  bool namespace_collision_risk = false;
  bool shadowing_risk = false;
  bool diagnostics_ready = false;
  bool private_partition_required = false;
  bool public_api_safe = false;
  bool partition_ready = false;
  bool pointer_partition_overlap = false;
};

ModuleNamespacePartitionEvidence CollectModuleNamespacePartitionEvidence(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  ModuleNamespacePartitionEvidence evidence;
  evidence.import_edge_candidates =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text)
                         : 0;
  evidence.namespace_segments =
      CountNamespaceSegments(object_pointer_type_name);
  evidence.namespace_collision_risk =
      evidence.namespace_segments > 1 && evidence.import_edge_candidates > 0;
  evidence.shadowing_risk =
      has_pointer_declarator && evidence.namespace_segments > 1;
  evidence.diagnostics_ready =
      !evidence.namespace_collision_risk ||
      (generic_suffix_terminated && object_pointer_type_spelling);
  evidence.private_partition_required = evidence.namespace_segments > 1;
  evidence.public_api_safe = !evidence.private_partition_required;
  evidence.partition_ready =
      !evidence.private_partition_required ||
      (generic_suffix_terminated && object_pointer_type_spelling);
  evidence.pointer_partition_overlap =
      has_pointer_declarator && evidence.private_partition_required;
  return evidence;
}

}  // namespace
