  record.symbol_graph_scope_resolution_ready =
      deterministic_symbol_graph_scope_resolution_handoff &&
      surface.symbol_graph_scope_resolution_summary.global_symbol_nodes ==
          surface.symbol_graph_global_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary.function_symbol_nodes ==
          surface.symbol_graph_function_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary.interface_symbol_nodes ==
          surface.symbol_graph_interface_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary.implementation_symbol_nodes ==
          surface.symbol_graph_implementation_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary
              .interface_property_symbol_nodes ==
          surface.symbol_graph_interface_property_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_property_symbol_nodes ==
          surface.symbol_graph_implementation_property_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary.interface_method_symbol_nodes ==
          surface.symbol_graph_interface_method_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_method_symbol_nodes ==
          surface.symbol_graph_implementation_method_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary.top_level_scope_symbols ==
          surface.symbol_graph_top_level_scope_symbols_total &&
      surface.symbol_graph_scope_resolution_summary.nested_scope_symbols ==
          surface.symbol_graph_nested_scope_symbols_total &&
      surface.symbol_graph_scope_resolution_summary.scope_frames_total ==
          surface.symbol_graph_scope_frames_total &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_sites ==
          surface
              .symbol_graph_implementation_interface_resolution_sites_total &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_hits ==
          surface
              .symbol_graph_implementation_interface_resolution_hits_total &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_misses ==
          surface
              .symbol_graph_implementation_interface_resolution_misses_total &&
      surface.symbol_graph_scope_resolution_summary.method_resolution_sites ==
          surface.symbol_graph_method_resolution_sites_total &&
      surface.symbol_graph_scope_resolution_summary.method_resolution_hits ==
          surface.symbol_graph_method_resolution_hits_total &&
      surface.symbol_graph_scope_resolution_summary.method_resolution_misses ==
          surface.symbol_graph_method_resolution_misses_total &&
      surface.symbol_graph_scope_resolution_summary.symbol_nodes_total() ==
          surface.symbol_graph_scope_resolution_summary
              .top_level_scope_symbols +
              surface.symbol_graph_scope_resolution_summary
                  .nested_scope_symbols &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_hits <=
          surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_sites &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_hits +
              surface.symbol_graph_scope_resolution_summary
                  .implementation_interface_resolution_misses ==
          surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_sites &&
      surface.symbol_graph_scope_resolution_summary.method_resolution_hits <=
          surface.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      surface.symbol_graph_scope_resolution_summary.method_resolution_hits +
              surface.symbol_graph_scope_resolution_summary
                  .method_resolution_misses ==
          surface.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      surface.symbol_graph_scope_resolution_summary.resolution_hits_total() <=
          surface.symbol_graph_scope_resolution_summary
              .resolution_sites_total() &&
      surface.symbol_graph_scope_resolution_summary.resolution_hits_total() +
              surface.symbol_graph_scope_resolution_summary
                  .resolution_misses_total() ==
          surface.symbol_graph_scope_resolution_summary
              .resolution_sites_total() &&
      surface.symbol_graph_scope_resolution_summary.deterministic;
  record.method_lookup_override_conflict_ready =
      deterministic_method_lookup_override_conflict_handoff &&
      surface.method_lookup_override_conflict_summary.method_lookup_sites ==
          surface.method_lookup_override_conflict_lookup_sites_total &&
      surface.method_lookup_override_conflict_summary.method_lookup_hits ==
          surface.method_lookup_override_conflict_lookup_hits_total &&
      surface.method_lookup_override_conflict_summary.method_lookup_misses ==
          surface.method_lookup_override_conflict_lookup_misses_total &&
      surface.method_lookup_override_conflict_summary.override_lookup_sites ==
          surface.method_lookup_override_conflict_override_sites_total &&
      surface.method_lookup_override_conflict_summary.override_lookup_hits ==
          surface.method_lookup_override_conflict_override_hits_total &&
      surface.method_lookup_override_conflict_summary.override_lookup_misses ==
          surface.method_lookup_override_conflict_override_misses_total &&
      surface.method_lookup_override_conflict_summary.override_conflicts ==
          surface.method_lookup_override_conflict_override_conflicts_total &&
      surface.method_lookup_override_conflict_summary
              .unresolved_base_interfaces ==
          surface
              .method_lookup_override_conflict_unresolved_base_interfaces_total &&
      surface.method_lookup_override_conflict_summary.method_lookup_hits <=
          surface.method_lookup_override_conflict_summary.method_lookup_sites &&
      surface.method_lookup_override_conflict_summary.method_lookup_hits +
              surface.method_lookup_override_conflict_summary
                  .method_lookup_misses ==
          surface.method_lookup_override_conflict_summary.method_lookup_sites &&
      surface.method_lookup_override_conflict_summary.override_lookup_hits <=
          surface.method_lookup_override_conflict_summary.override_lookup_sites &&
      surface.method_lookup_override_conflict_summary.override_lookup_hits +
              surface.method_lookup_override_conflict_summary
                  .override_lookup_misses ==
          surface.method_lookup_override_conflict_summary.override_lookup_sites &&
      surface.method_lookup_override_conflict_summary.override_conflicts <=
          surface.method_lookup_override_conflict_summary.override_lookup_hits &&
      surface.method_lookup_override_conflict_summary.deterministic;
  record.property_synthesis_ivar_binding_ready =
      deterministic_property_synthesis_ivar_binding_handoff &&
      surface.property_synthesis_ivar_binding_summary.property_synthesis_sites ==
          surface
              .property_synthesis_ivar_binding_property_synthesis_sites_total &&
      surface.property_synthesis_ivar_binding_summary
              .property_synthesis_explicit_ivar_bindings ==
          surface
              .property_synthesis_ivar_binding_explicit_ivar_bindings_total &&
      surface.property_synthesis_ivar_binding_summary
              .property_synthesis_default_ivar_bindings ==
          surface
              .property_synthesis_ivar_binding_default_ivar_bindings_total &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
          surface.property_synthesis_ivar_binding_ivar_binding_sites_total &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_resolved ==
          surface.property_synthesis_ivar_binding_ivar_binding_resolved_total &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_missing ==
          surface.property_synthesis_ivar_binding_ivar_binding_missing_total &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_conflicts ==
          surface.property_synthesis_ivar_binding_ivar_binding_conflicts_total &&
      surface.property_synthesis_ivar_binding_summary
              .property_synthesis_explicit_ivar_bindings +
              surface.property_synthesis_ivar_binding_summary
                  .property_synthesis_default_ivar_bindings ==
          surface.property_synthesis_ivar_binding_summary
              .property_synthesis_sites &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
          surface.property_synthesis_ivar_binding_summary
              .property_synthesis_sites &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_resolved +
              surface.property_synthesis_ivar_binding_summary
                  .ivar_binding_missing +
              surface.property_synthesis_ivar_binding_summary
                  .ivar_binding_conflicts ==
          surface.property_synthesis_ivar_binding_summary
              .ivar_binding_sites &&
      surface.property_synthesis_ivar_binding_summary.deterministic;
  record.id_class_sel_object_pointer_type_checking_ready =
      deterministic_id_class_sel_object_pointer_type_checking_handoff &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_type_sites ==
          surface.id_class_sel_object_pointer_param_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_id_spelling_sites ==
          surface.id_class_sel_object_pointer_param_id_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_class_spelling_sites ==
          surface.id_class_sel_object_pointer_param_class_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_sel_spelling_sites ==
          surface.id_class_sel_object_pointer_param_sel_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_instancetype_spelling_sites ==
          surface
              .id_class_sel_object_pointer_param_instancetype_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_object_pointer_type_sites ==
          surface
              .id_class_sel_object_pointer_param_object_pointer_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_type_sites ==
          surface.id_class_sel_object_pointer_return_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_id_spelling_sites ==
          surface.id_class_sel_object_pointer_return_id_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_class_spelling_sites ==
          surface
              .id_class_sel_object_pointer_return_class_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_sel_spelling_sites ==
          surface.id_class_sel_object_pointer_return_sel_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_instancetype_spelling_sites ==
          surface
              .id_class_sel_object_pointer_return_instancetype_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_object_pointer_type_sites ==
          surface
              .id_class_sel_object_pointer_return_object_pointer_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_type_sites ==
          surface.id_class_sel_object_pointer_property_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_id_spelling_sites ==
          surface.id_class_sel_object_pointer_property_id_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_class_spelling_sites ==
          surface
              .id_class_sel_object_pointer_property_class_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_sel_spelling_sites ==
          surface.id_class_sel_object_pointer_property_sel_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_instancetype_spelling_sites ==
          surface
              .id_class_sel_object_pointer_property_instancetype_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_object_pointer_type_sites ==
          surface
              .id_class_sel_object_pointer_property_object_pointer_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_id_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .param_class_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .param_sel_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .param_instancetype_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .param_object_pointer_type_sites <=
          surface.id_class_sel_object_pointer_type_checking_summary
              .param_type_sites &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_id_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .return_class_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .return_sel_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .return_instancetype_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .return_object_pointer_type_sites <=
          surface.id_class_sel_object_pointer_type_checking_summary
              .return_type_sites &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_id_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .property_class_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .property_sel_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .property_instancetype_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .property_object_pointer_type_sites <=
          surface.id_class_sel_object_pointer_type_checking_summary
              .property_type_sites &&
      surface.id_class_sel_object_pointer_type_checking_summary.deterministic;
