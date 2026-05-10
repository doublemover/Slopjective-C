static Objc3ProtocolQualifiedObjectTypeSummary
BuildProtocolQualifiedObjectTypeSummaryFromTypeAnnotationSurfaceSummary(
    const Objc3TypeAnnotationSurfaceSummary &type_annotation_summary) {
  Objc3ProtocolQualifiedObjectTypeSummary summary;
  summary.protocol_qualified_object_type_sites = type_annotation_summary.generic_suffix_sites;
  summary.protocol_composition_sites = type_annotation_summary.generic_suffix_sites;
  summary.object_pointer_type_sites = type_annotation_summary.object_pointer_type_sites;
  summary.terminated_protocol_composition_sites =
      type_annotation_summary.generic_suffix_sites - type_annotation_summary.invalid_generic_suffix_sites;
  summary.pointer_declarator_sites = type_annotation_summary.pointer_declarator_sites;
  summary.normalized_protocol_composition_sites =
      std::min(summary.terminated_protocol_composition_sites, summary.object_pointer_type_sites);
  summary.contract_violation_sites =
      type_annotation_summary.invalid_generic_suffix_sites +
      (summary.protocol_composition_sites > summary.object_pointer_type_sites
           ? summary.protocol_composition_sites - summary.object_pointer_type_sites
           : 0u);
  summary.deterministic =
      type_annotation_summary.deterministic &&
      summary.contract_violation_sites == 0u &&
      summary.terminated_protocol_composition_sites <= summary.protocol_composition_sites &&
      summary.normalized_protocol_composition_sites <= summary.protocol_qualified_object_type_sites &&
      summary.contract_violation_sites <= summary.protocol_qualified_object_type_sites;
  return summary;
}
