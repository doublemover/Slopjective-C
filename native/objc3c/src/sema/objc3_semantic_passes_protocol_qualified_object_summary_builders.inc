static Objc3ProtocolQualifiedObjectTypeSummary
BuildProtocolQualifiedObjectTypeSummaryFromCanonicalTypes(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3ProtocolQualifiedObjectTypeSummary summary;
  summary.pointer_declarator_sites =
      surface.type_annotation_surface_summary.pointer_declarator_sites;
  ForEachCanonicalCallableTypeInSurface(
      surface,
      [&summary](const Objc3SemanticCanonicalType &type) {
        if (!type.object_type_facts_authoritative ||
            !type.protocol_composition_facts_authoritative) {
          ++summary.contract_violation_sites;
          summary.deterministic = false;
          return;
        }
        if (type.is_objc_object_reference) {
          ++summary.object_pointer_type_sites;
        }
        if (!type.has_protocol_composition) {
          return;
        }
        ++summary.protocol_qualified_object_type_sites;
        ++summary.protocol_composition_sites;
        if (!type.has_invalid_protocol_composition &&
            !type.has_invalid_type_suffix) {
          ++summary.terminated_protocol_composition_sites;
        }
        if (type.is_objc_object_reference &&
            !type.has_invalid_protocol_composition &&
            !type.has_invalid_type_suffix) {
          ++summary.normalized_protocol_composition_sites;
        } else {
          ++summary.contract_violation_sites;
        }
      });
  summary.deterministic =
      summary.deterministic &&
      surface.type_annotation_surface_summary.deterministic &&
      summary.contract_violation_sites == 0u &&
      summary.terminated_protocol_composition_sites <= summary.protocol_composition_sites &&
      summary.normalized_protocol_composition_sites <= summary.protocol_qualified_object_type_sites &&
      summary.contract_violation_sites <= summary.protocol_qualified_object_type_sites;
  return summary;
}

static Objc3ProtocolQualifiedObjectTypeSummary
BuildProtocolQualifiedObjectTypeSummaryFromCanonicalTypes(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3ProtocolQualifiedObjectTypeSummary summary;
  summary.pointer_declarator_sites =
      handoff.type_annotation_surface_summary.pointer_declarator_sites;
  ForEachCanonicalCallableTypeInHandoff(
      handoff,
      [&summary](const Objc3SemanticCanonicalType &type) {
        if (!type.object_type_facts_authoritative ||
            !type.protocol_composition_facts_authoritative) {
          ++summary.contract_violation_sites;
          summary.deterministic = false;
          return;
        }
        if (type.is_objc_object_reference) {
          ++summary.object_pointer_type_sites;
        }
        if (!type.has_protocol_composition) {
          return;
        }
        ++summary.protocol_qualified_object_type_sites;
        ++summary.protocol_composition_sites;
        if (!type.has_invalid_protocol_composition &&
            !type.has_invalid_type_suffix) {
          ++summary.terminated_protocol_composition_sites;
        }
        if (type.is_objc_object_reference &&
            !type.has_invalid_protocol_composition &&
            !type.has_invalid_type_suffix) {
          ++summary.normalized_protocol_composition_sites;
        } else {
          ++summary.contract_violation_sites;
        }
      });
  summary.deterministic =
      summary.deterministic &&
      handoff.type_annotation_surface_summary.deterministic &&
      summary.contract_violation_sites == 0u &&
      summary.terminated_protocol_composition_sites <= summary.protocol_composition_sites &&
      summary.normalized_protocol_composition_sites <= summary.protocol_qualified_object_type_sites &&
      summary.contract_violation_sites <= summary.protocol_qualified_object_type_sites;
  return summary;
}
