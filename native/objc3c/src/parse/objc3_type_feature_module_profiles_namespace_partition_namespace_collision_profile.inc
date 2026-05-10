std::string BuildNamespaceCollisionShadowingProfile(
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
  out << "namespace-collision-shadowing:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";namespace-segments=" << evidence.namespace_segments
      << ";import-edge-candidates=" << evidence.import_edge_candidates
      << ";namespace-collision-risk="
      << (evidence.namespace_collision_risk ? "true" : "false")
      << ";shadowing-risk=" << (evidence.shadowing_risk ? "true" : "false")
      << ";diagnostics-ready="
      << (evidence.diagnostics_ready ? "true" : "false");
  return out.str();
}
