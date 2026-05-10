bool IsCrossModuleConformanceProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  const ModuleCacheConformanceEvidence evidence =
      CollectModuleCacheConformanceEvidence(
          object_pointer_type_spelling, has_generic_suffix,
          generic_suffix_terminated, has_pointer_declarator,
          generic_suffix_text, object_pointer_type_name);
  if (has_pointer_declarator && !object_pointer_type_spelling) {
    return false;
  }
  if (!evidence.cross_module_boundary_engaged) {
    return true;
  }
  if (!object_pointer_type_spelling) {
    return false;
  }
  if (!has_generic_suffix) {
    return true;
  }
  return generic_suffix_terminated && evidence.import_edge_candidates > 0;
}
