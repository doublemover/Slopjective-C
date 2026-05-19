namespace {

static void PopulateObjc3SemaCorePublicationFlowCoreReadiness(
    Objc3SemaCoreSemanticParityPublicationReadinessRecord &record,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_semantic_diagnostics,
    bool deterministic_type_metadata_handoff,
    bool deterministic_interface_implementation_handoff,
    bool deterministic_protocol_category_composition_handoff,
    bool deterministic_class_protocol_category_linking_handoff) {
  record.semantic_diagnostics_ready = deterministic_semantic_diagnostics;
  record.type_metadata_handoff_ready = deterministic_type_metadata_handoff;
  record.interface_implementation_ready =
      deterministic_interface_implementation_handoff &&
      surface.interfaces_total == surface.type_metadata_interface_entries &&
      surface.implementations_total ==
          surface.type_metadata_implementation_entries &&
      surface.interface_implementation_summary.resolved_interfaces ==
          surface.type_metadata_interface_entries &&
      surface.interface_implementation_summary.resolved_implementations ==
          surface.type_metadata_implementation_entries;
  record.protocol_category_composition_ready =
      deterministic_protocol_category_composition_handoff &&
      surface.protocol_category_composition_summary.protocol_composition_sites ==
          surface.protocol_composition_sites_total &&
      surface.protocol_category_composition_summary
              .protocol_composition_symbols ==
          surface.protocol_composition_symbols_total &&
      surface.protocol_category_composition_summary.category_composition_sites ==
          surface.category_composition_sites_total &&
      surface.protocol_category_composition_summary
              .category_composition_symbols ==
          surface.category_composition_symbols_total &&
      surface.protocol_category_composition_summary
              .invalid_protocol_composition_sites ==
          surface.invalid_protocol_composition_sites_total &&
      surface.protocol_category_composition_summary
              .invalid_protocol_composition_sites <=
          surface.protocol_category_composition_summary
              .total_composition_sites();
  record.class_protocol_category_linking_ready =
      deterministic_class_protocol_category_linking_handoff &&
      surface.class_protocol_category_linking_summary.declared_interfaces ==
          surface.interface_implementation_summary.declared_interfaces &&
      surface.class_protocol_category_linking_summary.resolved_interfaces ==
          surface.interface_implementation_summary.resolved_interfaces &&
      surface.class_protocol_category_linking_summary.declared_implementations ==
          surface.interface_implementation_summary.declared_implementations &&
      surface.class_protocol_category_linking_summary.resolved_implementations ==
          surface.interface_implementation_summary.resolved_implementations &&
      surface.class_protocol_category_linking_summary.interface_method_symbols ==
          surface.interface_method_symbols_total &&
      surface.class_protocol_category_linking_summary
              .implementation_method_symbols ==
          surface.implementation_method_symbols_total &&
      surface.class_protocol_category_linking_summary
              .linked_implementation_symbols ==
          surface.linked_implementation_symbols_total &&
      surface.class_protocol_category_linking_summary
              .protocol_composition_sites ==
          surface.protocol_composition_sites_total &&
      surface.class_protocol_category_linking_summary
              .protocol_composition_symbols ==
          surface.protocol_composition_symbols_total &&
      surface.class_protocol_category_linking_summary
              .category_composition_sites ==
          surface.category_composition_sites_total &&
      surface.class_protocol_category_linking_summary
              .category_composition_symbols ==
          surface.category_composition_symbols_total &&
      surface.class_protocol_category_linking_summary
              .invalid_protocol_composition_sites ==
          surface.invalid_protocol_composition_sites_total &&
      surface.class_protocol_category_linking_summary
              .invalid_protocol_composition_sites <=
          surface.class_protocol_category_linking_summary
              .total_composition_sites() &&
      surface.class_protocol_category_linking_summary.deterministic;
}

}  // namespace
