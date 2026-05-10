  const Objc3SymbolGraphScopeResolutionSummary
      symbol_graph_scope_resolution_summary =
          BuildSymbolGraphScopeResolutionSummaryFromTypeMetadataHandoff(handoff);
  if (!handoff.symbol_graph_scope_resolution_summary.deterministic) {
    return "symbol graph/scope resolution summary is not deterministic";
  }
  if (handoff.symbol_graph_scope_resolution_summary.global_symbol_nodes !=
          symbol_graph_scope_resolution_summary.global_symbol_nodes ||
      handoff.symbol_graph_scope_resolution_summary.function_symbol_nodes !=
          symbol_graph_scope_resolution_summary.function_symbol_nodes ||
      handoff.symbol_graph_scope_resolution_summary.interface_symbol_nodes !=
          symbol_graph_scope_resolution_summary.interface_symbol_nodes ||
      handoff.symbol_graph_scope_resolution_summary
              .implementation_symbol_nodes !=
          symbol_graph_scope_resolution_summary
              .implementation_symbol_nodes ||
      handoff.symbol_graph_scope_resolution_summary
              .interface_property_symbol_nodes !=
          symbol_graph_scope_resolution_summary
              .interface_property_symbol_nodes ||
      handoff.symbol_graph_scope_resolution_summary
              .implementation_property_symbol_nodes !=
          symbol_graph_scope_resolution_summary
              .implementation_property_symbol_nodes ||
      handoff.symbol_graph_scope_resolution_summary.interface_method_symbol_nodes !=
          symbol_graph_scope_resolution_summary.interface_method_symbol_nodes ||
      handoff.symbol_graph_scope_resolution_summary
              .implementation_method_symbol_nodes !=
          symbol_graph_scope_resolution_summary
              .implementation_method_symbol_nodes ||
      handoff.symbol_graph_scope_resolution_summary.top_level_scope_symbols !=
          symbol_graph_scope_resolution_summary.top_level_scope_symbols ||
      handoff.symbol_graph_scope_resolution_summary.nested_scope_symbols !=
          symbol_graph_scope_resolution_summary.nested_scope_symbols ||
      handoff.symbol_graph_scope_resolution_summary.scope_frames_total !=
          symbol_graph_scope_resolution_summary.scope_frames_total ||
      handoff.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_sites !=
          symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_sites ||
      handoff.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_hits !=
          symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_hits ||
      handoff.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_misses !=
          symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_misses ||
      handoff.symbol_graph_scope_resolution_summary.method_resolution_sites !=
          symbol_graph_scope_resolution_summary.method_resolution_sites ||
      handoff.symbol_graph_scope_resolution_summary.method_resolution_hits !=
          symbol_graph_scope_resolution_summary.method_resolution_hits ||
      handoff.symbol_graph_scope_resolution_summary.method_resolution_misses !=
          symbol_graph_scope_resolution_summary.method_resolution_misses) {
    return "symbol graph/scope resolution summary drifted from recomputed handoff";
  }

  const Objc3MethodLookupOverrideConflictSummary
      method_lookup_override_conflict_summary =
          BuildMethodLookupOverrideConflictSummaryFromTypeMetadataHandoff(handoff);
  if (!handoff.method_lookup_override_conflict_summary.deterministic) {
    return "method lookup/override summary is not deterministic";
  }
  if (handoff.method_lookup_override_conflict_summary.method_lookup_sites !=
          method_lookup_override_conflict_summary.method_lookup_sites ||
      handoff.method_lookup_override_conflict_summary.method_lookup_hits !=
          method_lookup_override_conflict_summary.method_lookup_hits ||
      handoff.method_lookup_override_conflict_summary.method_lookup_misses !=
          method_lookup_override_conflict_summary.method_lookup_misses ||
      handoff.method_lookup_override_conflict_summary.override_lookup_sites !=
          method_lookup_override_conflict_summary.override_lookup_sites ||
      handoff.method_lookup_override_conflict_summary.override_lookup_hits !=
          method_lookup_override_conflict_summary.override_lookup_hits ||
      handoff.method_lookup_override_conflict_summary.override_lookup_misses !=
          method_lookup_override_conflict_summary.override_lookup_misses ||
      handoff.method_lookup_override_conflict_summary.override_conflicts !=
          method_lookup_override_conflict_summary.override_conflicts ||
      handoff.method_lookup_override_conflict_summary.unresolved_base_interfaces !=
          method_lookup_override_conflict_summary.unresolved_base_interfaces) {
    return "method lookup/override summary drifted from recomputed handoff";
  }

  const Objc3PropertySynthesisIvarBindingSummary
      property_synthesis_ivar_binding_summary =
          BuildPropertySynthesisIvarBindingSummaryFromTypeMetadataHandoff(handoff);
  if (!handoff.property_synthesis_ivar_binding_summary.deterministic) {
    return "property synthesis/ivar binding summary is not deterministic";
  }
  if (handoff.property_synthesis_ivar_binding_summary.property_synthesis_sites !=
          property_synthesis_ivar_binding_summary.property_synthesis_sites ||
      handoff.property_synthesis_ivar_binding_summary
              .property_synthesis_explicit_ivar_bindings !=
          property_synthesis_ivar_binding_summary
              .property_synthesis_explicit_ivar_bindings ||
      handoff.property_synthesis_ivar_binding_summary
              .property_synthesis_default_ivar_bindings !=
          property_synthesis_ivar_binding_summary
              .property_synthesis_default_ivar_bindings ||
      handoff.property_synthesis_ivar_binding_summary
              .interface_owned_property_synthesis_sites !=
          property_synthesis_ivar_binding_summary
              .interface_owned_property_synthesis_sites ||
      handoff.property_synthesis_ivar_binding_summary
              .implementation_property_redeclaration_sites !=
          property_synthesis_ivar_binding_summary
              .implementation_property_redeclaration_sites ||
      handoff.property_synthesis_ivar_binding_summary.ivar_binding_sites !=
          property_synthesis_ivar_binding_summary.ivar_binding_sites ||
      handoff.property_synthesis_ivar_binding_summary.ivar_binding_resolved !=
          property_synthesis_ivar_binding_summary.ivar_binding_resolved ||
      handoff.property_synthesis_ivar_binding_summary.ivar_binding_missing !=
          property_synthesis_ivar_binding_summary.ivar_binding_missing ||
      handoff.property_synthesis_ivar_binding_summary.ivar_binding_conflicts !=
          property_synthesis_ivar_binding_summary.ivar_binding_conflicts) {
    std::ostringstream detail;
    detail << "property synthesis/ivar binding summary drifted from recomputed handoff"
           << " (sites="
           << handoff.property_synthesis_ivar_binding_summary.property_synthesis_sites
           << "/" << property_synthesis_ivar_binding_summary.property_synthesis_sites
           << ", resolved="
           << handoff.property_synthesis_ivar_binding_summary.ivar_binding_resolved
           << "/" << property_synthesis_ivar_binding_summary.ivar_binding_resolved
           << ", missing="
           << handoff.property_synthesis_ivar_binding_summary.ivar_binding_missing
           << "/" << property_synthesis_ivar_binding_summary.ivar_binding_missing
           << ", conflicts="
           << handoff.property_synthesis_ivar_binding_summary.ivar_binding_conflicts
           << "/" << property_synthesis_ivar_binding_summary.ivar_binding_conflicts
           << ")";
    return detail.str();
  }

  const Objc3IdClassSelObjectPointerTypeCheckingSummary
      id_class_sel_object_pointer_type_checking_summary =
          BuildIdClassSelObjectPointerTypeCheckingSummaryFromTypeMetadataHandoff(
              handoff);
  if (!handoff.id_class_sel_object_pointer_type_checking_summary.deterministic) {
    return "id/Class/SEL/object-pointer checking summary is not deterministic";
  }
  if (handoff.id_class_sel_object_pointer_type_checking_summary
              .canonical_reference_form_count !=
          id_class_sel_object_pointer_type_checking_summary
              .canonical_reference_form_count ||
      handoff.id_class_sel_object_pointer_type_checking_summary
              .canonical_message_scalar_form_count !=
          id_class_sel_object_pointer_type_checking_summary
              .canonical_message_scalar_form_count ||
      handoff.id_class_sel_object_pointer_type_checking_summary
              .canonical_bridge_top_form_count !=
          id_class_sel_object_pointer_type_checking_summary
              .canonical_bridge_top_form_count ||
      handoff.id_class_sel_object_pointer_type_checking_summary.param_type_sites !=
          id_class_sel_object_pointer_type_checking_summary.param_type_sites ||
      handoff.id_class_sel_object_pointer_type_checking_summary.return_type_sites !=
          id_class_sel_object_pointer_type_checking_summary.return_type_sites ||
      handoff.id_class_sel_object_pointer_type_checking_summary.property_type_sites !=
          id_class_sel_object_pointer_type_checking_summary.property_type_sites) {
    return "id/Class/SEL/object-pointer checking summary drifted from recomputed handoff";
  }
