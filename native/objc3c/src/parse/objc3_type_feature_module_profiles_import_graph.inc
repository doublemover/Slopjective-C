std::string BuildModuleImportGraphProfile(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  const std::size_t import_edge_candidates =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text) : 0;
  const std::size_t module_segments = CountNamespaceSegments(object_pointer_type_name);
  const bool graph_well_formed =
      !has_generic_suffix ||
      (generic_suffix_terminated && object_pointer_type_spelling && import_edge_candidates > 0);
  const bool namespace_stable = module_segments <= 1 || object_pointer_type_spelling;

  std::ostringstream out;
  out << "module-import-graph:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";module-segments=" << module_segments
      << ";import-edge-candidates=" << import_edge_candidates
      << ";graph-well-formed=" << (graph_well_formed ? "true" : "false")
      << ";namespace-stable=" << (namespace_stable ? "true" : "false");
  return out.str();
}

bool IsModuleImportGraphProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, const std::string &generic_suffix_text) {
  if (!has_generic_suffix) {
    return true;
  }
  const std::size_t import_edge_candidates =
      CountTopLevelGenericArgumentSlots(generic_suffix_text);
  return generic_suffix_terminated &&
         object_pointer_type_spelling &&
         import_edge_candidates > 0;
}
