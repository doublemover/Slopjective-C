namespace {

struct ModuleCacheConformanceEvidence {
  std::size_t import_edge_candidates = 0;
  std::size_t namespace_segments = 0;
  bool cache_key_ready = false;
  bool cache_partitioned = false;
  bool invalidation_on_shape_change = false;
  bool invalidation_ready = false;
  bool cross_module_boundary_engaged = false;
  bool conformance_surface_ready = false;
  bool boundary_shape_stable = false;
  bool pointer_boundary_coupling = false;
  bool deterministic_handoff = false;
};

ModuleCacheConformanceEvidence CollectModuleCacheConformanceEvidence(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name) {
  ModuleCacheConformanceEvidence evidence;
  evidence.import_edge_candidates =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text)
                         : 0;
  evidence.namespace_segments =
      CountNamespaceSegments(object_pointer_type_name);
  evidence.cache_key_ready =
      object_pointer_type_spelling &&
      (!has_generic_suffix ||
       (generic_suffix_terminated && evidence.import_edge_candidates > 0));
  evidence.cache_partitioned = evidence.namespace_segments > 1;
  evidence.invalidation_on_shape_change =
      has_generic_suffix || has_pointer_declarator || evidence.cache_partitioned;
  evidence.invalidation_ready =
      !evidence.invalidation_on_shape_change || evidence.cache_key_ready;
  evidence.cross_module_boundary_engaged =
      evidence.namespace_segments > 1 || has_generic_suffix;
  evidence.conformance_surface_ready = evidence.cache_key_ready;
  evidence.boundary_shape_stable =
      !evidence.cross_module_boundary_engaged ||
      evidence.conformance_surface_ready;
  evidence.pointer_boundary_coupling =
      has_pointer_declarator && evidence.cross_module_boundary_engaged;
  evidence.deterministic_handoff =
      evidence.boundary_shape_stable &&
      (!has_pointer_declarator || object_pointer_type_spelling);
  return evidence;
}

}  // namespace
