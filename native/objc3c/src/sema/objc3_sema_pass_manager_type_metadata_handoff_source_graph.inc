  result.symbol_graph_scope_resolution_summary = result.integration_surface.symbol_graph_scope_resolution_summary;
  result.deterministic_symbol_graph_scope_resolution_handoff =
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.deterministic &&
      result.integration_surface.symbol_graph_scope_resolution_summary.deterministic &&
      IsEquivalentSymbolGraphScopeResolutionSummary(result.integration_surface.symbol_graph_scope_resolution_summary,
                                                    result.type_metadata_handoff.symbol_graph_scope_resolution_summary) &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.symbol_nodes_total() ==
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.top_level_scope_symbols +
              result.type_metadata_handoff.symbol_graph_scope_resolution_summary.nested_scope_symbols &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits <=
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits +
              result.type_metadata_handoff.symbol_graph_scope_resolution_summary
                  .implementation_interface_resolution_misses ==
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.method_resolution_hits <=
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.method_resolution_hits +
              result.type_metadata_handoff.symbol_graph_scope_resolution_summary.method_resolution_misses ==
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.resolution_hits_total() <=
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.resolution_sites_total() &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.resolution_hits_total() +
              result.type_metadata_handoff.symbol_graph_scope_resolution_summary.resolution_misses_total() ==
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.resolution_sites_total();
  result.method_lookup_override_conflict_summary =
      result.integration_surface.method_lookup_override_conflict_summary;
  result.deterministic_method_lookup_override_conflict_handoff =
      result.type_metadata_handoff.method_lookup_override_conflict_summary.deterministic &&
      result.integration_surface.method_lookup_override_conflict_summary.deterministic &&
      IsEquivalentMethodLookupOverrideConflictSummary(
          result.integration_surface.method_lookup_override_conflict_summary,
          result.type_metadata_handoff.method_lookup_override_conflict_summary) &&
      result.type_metadata_handoff.method_lookup_override_conflict_summary.method_lookup_hits <=
          result.type_metadata_handoff.method_lookup_override_conflict_summary.method_lookup_sites &&
      result.type_metadata_handoff.method_lookup_override_conflict_summary.method_lookup_hits +
              result.type_metadata_handoff.method_lookup_override_conflict_summary.method_lookup_misses ==
          result.type_metadata_handoff.method_lookup_override_conflict_summary.method_lookup_sites &&
      result.type_metadata_handoff.method_lookup_override_conflict_summary.override_lookup_hits <=
          result.type_metadata_handoff.method_lookup_override_conflict_summary.override_lookup_sites &&
      result.type_metadata_handoff.method_lookup_override_conflict_summary.override_lookup_hits +
              result.type_metadata_handoff.method_lookup_override_conflict_summary.override_lookup_misses ==
          result.type_metadata_handoff.method_lookup_override_conflict_summary.override_lookup_sites &&
      result.type_metadata_handoff.method_lookup_override_conflict_summary.override_conflicts <=
          result.type_metadata_handoff.method_lookup_override_conflict_summary.override_lookup_hits;
  result.property_synthesis_ivar_binding_summary =
      result.integration_surface.property_synthesis_ivar_binding_summary;
  result.deterministic_property_synthesis_ivar_binding_handoff =
      result.type_metadata_handoff.property_synthesis_ivar_binding_summary.deterministic &&
      result.integration_surface.property_synthesis_ivar_binding_summary.deterministic &&
      IsEquivalentPropertySynthesisIvarBindingSummary(
          result.integration_surface.property_synthesis_ivar_binding_summary,
          result.type_metadata_handoff.property_synthesis_ivar_binding_summary) &&
      result.type_metadata_handoff.property_synthesis_ivar_binding_summary
              .property_synthesis_explicit_ivar_bindings +
              result.type_metadata_handoff.property_synthesis_ivar_binding_summary
                  .property_synthesis_default_ivar_bindings ==
          result.type_metadata_handoff.property_synthesis_ivar_binding_summary.property_synthesis_sites &&
      result.type_metadata_handoff.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
          result.type_metadata_handoff.property_synthesis_ivar_binding_summary.property_synthesis_sites &&
      result.type_metadata_handoff.property_synthesis_ivar_binding_summary.ivar_binding_resolved +
              result.type_metadata_handoff.property_synthesis_ivar_binding_summary.ivar_binding_missing +
              result.type_metadata_handoff.property_synthesis_ivar_binding_summary.ivar_binding_conflicts ==
          result.type_metadata_handoff.property_synthesis_ivar_binding_summary.ivar_binding_sites;
  result.id_class_sel_object_pointer_type_checking_summary =
      result.integration_surface.id_class_sel_object_pointer_type_checking_summary;
  result.deterministic_id_class_sel_object_pointer_type_checking_handoff =
      result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.deterministic &&
      result.integration_surface.id_class_sel_object_pointer_type_checking_summary.deterministic &&
      IsEquivalentIdClassSelObjectPointerTypeCheckingSummary(
          result.integration_surface.id_class_sel_object_pointer_type_checking_summary,
          result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary) &&
      result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.param_id_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.param_class_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.param_sel_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary
                  .param_instancetype_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary
                  .param_object_pointer_type_sites <=
          result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.param_type_sites &&
      result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.return_id_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.return_class_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.return_sel_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary
                  .return_instancetype_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary
                  .return_object_pointer_type_sites <=
          result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.return_type_sites &&
      result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.property_id_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary
                  .property_class_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.property_sel_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary
                  .property_instancetype_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary
                  .property_object_pointer_type_sites <=
          result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.property_type_sites;
