      << (graph.protocol_category_declaration_closure_complete ? "true"
                                                               : "false")
      << ",\"protocol_inheritance_identity_closure_complete\":"
      << (graph.protocol_inheritance_identity_closure_complete ? "true"
                                                               : "false")
      << ",\"category_attachment_identity_closure_complete\":"
      << (graph.category_attachment_identity_closure_complete ? "true"
                                                              : "false")
      << ",\"protocol_category_conformance_identity_closure_complete\":"
      << (graph.protocol_category_conformance_identity_closure_complete ? "true"
                                                                        : "false")
      << ",\"interface_node_entries\":[";
  for (std::size_t i = 0; i < graph.interface_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.interface_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"class_name\":\"" << EscapeJsonString(node.class_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"class_owner_identity\":\""
        << EscapeJsonString(node.class_owner_identity)
        << "\",\"metaclass_owner_identity\":\""
        << EscapeJsonString(node.metaclass_owner_identity)
        << "\",\"super_class_owner_identity\":\""
        << EscapeJsonString(node.super_class_owner_identity)
        << "\",\"super_metaclass_owner_identity\":\""
        << EscapeJsonString(node.super_metaclass_owner_identity)
        << "\",\"instance_method_owner_identity\":\""
        << EscapeJsonString(node.instance_method_owner_identity)
        << "\",\"class_method_owner_identity\":\""
        << EscapeJsonString(node.class_method_owner_identity)
        << "\",\"has_super\":" << (node.has_super ? "true" : "false")
        << ",\"declaration_complete\":"
        << (node.declaration_complete ? "true" : "false")
        << ",\"property_count\":" << node.property_count
        << ",\"method_count\":" << node.method_count
        << ",\"class_method_count\":" << node.class_method_count
        << ",\"instance_method_count\":" << node.instance_method_count
        << ",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"implementation_node_entries\":[";
  for (std::size_t i = 0; i < graph.implementation_nodes_lexicographic.size();
       ++i) {
    const auto &node = graph.implementation_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"class_name\":\"" << EscapeJsonString(node.class_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"interface_owner_identity\":\""
        << EscapeJsonString(node.interface_owner_identity)
        << "\",\"class_owner_identity\":\""
        << EscapeJsonString(node.class_owner_identity)
        << "\",\"metaclass_owner_identity\":\""
        << EscapeJsonString(node.metaclass_owner_identity)
        << "\",\"super_class_owner_identity\":\""
        << EscapeJsonString(node.super_class_owner_identity)
        << "\",\"super_metaclass_owner_identity\":\""
        << EscapeJsonString(node.super_metaclass_owner_identity)
        << "\",\"instance_method_owner_identity\":\""
        << EscapeJsonString(node.instance_method_owner_identity)
        << "\",\"class_method_owner_identity\":\""
        << EscapeJsonString(node.class_method_owner_identity)
        << "\",\"has_matching_interface\":"
        << (node.has_matching_interface ? "true" : "false")
        << ",\"has_super\":" << (node.has_super ? "true" : "false")
        << ",\"declaration_complete\":"
        << (node.declaration_complete ? "true" : "false")
        << ",\"property_count\":" << node.property_count
        << ",\"method_count\":" << node.method_count
        << ",\"class_method_count\":" << node.class_method_count
        << ",\"instance_method_count\":" << node.instance_method_count
        << ",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"class_node_entries\":[";
  for (std::size_t i = 0; i < graph.class_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.class_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"class_name\":\"" << EscapeJsonString(node.class_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"interface_owner_identity\":\""
        << EscapeJsonString(node.interface_owner_identity)
        << "\",\"implementation_owner_identity\":\""
        << EscapeJsonString(node.implementation_owner_identity)
        << "\",\"metaclass_owner_identity\":\""
        << EscapeJsonString(node.metaclass_owner_identity)
        << "\",\"super_class_owner_identity\":\""
        << EscapeJsonString(node.super_class_owner_identity)
        << "\",\"super_metaclass_owner_identity\":\""
        << EscapeJsonString(node.super_metaclass_owner_identity)
        << "\",\"adopted_protocol_owner_identities\":[";
    for (std::size_t adopted_index = 0;
         adopted_index <
         node.adopted_protocol_owner_identities_lexicographic.size();
         ++adopted_index) {
      if (adopted_index != 0u) {
        out << ",";
      }
      out << "\""
          << EscapeJsonString(
                 node.adopted_protocol_owner_identities_lexicographic
                     [adopted_index])
          << "\"";
    }
    out << "]"
        << ",\"instance_method_owner_identity\":\""
        << EscapeJsonString(node.instance_method_owner_identity)
        << "\",\"class_method_owner_identity\":\""
        << EscapeJsonString(node.class_method_owner_identity)
        << "\",\"has_interface\":" << (node.has_interface ? "true" : "false")
        << ",\"has_implementation\":"
        << (node.has_implementation ? "true" : "false")
        << ",\"has_super\":" << (node.has_super ? "true" : "false")
        << ",\"realization_identity_complete\":"
        << (node.realization_identity_complete ? "true" : "false")
        << ",\"interface_property_count\":" << node.interface_property_count
        << ",\"implementation_property_count\":"
        << node.implementation_property_count
        << ",\"interface_method_count\":" << node.interface_method_count
        << ",\"implementation_method_count\":"
        << node.implementation_method_count
        << ",\"interface_class_method_count\":"
        << node.interface_class_method_count
        << ",\"implementation_class_method_count\":"
        << node.implementation_class_method_count
        << ",\"interface_instance_method_count\":"
        << node.interface_instance_method_count
        << ",\"implementation_instance_method_count\":"
        << node.implementation_instance_method_count
        << ",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"metaclass_node_entries\":[";
  for (std::size_t i = 0; i < graph.metaclass_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.metaclass_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"class_name\":\"" << EscapeJsonString(node.class_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"class_owner_identity\":\""
        << EscapeJsonString(node.class_owner_identity)
        << "\",\"interface_owner_identity\":\""
        << EscapeJsonString(node.interface_owner_identity)
        << "\",\"implementation_owner_identity\":\""
        << EscapeJsonString(node.implementation_owner_identity)
        << "\",\"super_metaclass_owner_identity\":\""
        << EscapeJsonString(node.super_metaclass_owner_identity)
        << "\",\"derived_from_interface\":"
        << (node.derived_from_interface ? "true" : "false")
        << ",\"has_implementation\":"
        << (node.has_implementation ? "true" : "false")
        << ",\"has_super\":" << (node.has_super ? "true" : "false")
        << ",\"interface_class_method_count\":"
        << node.interface_class_method_count
        << ",\"implementation_class_method_count\":"
        << node.implementation_class_method_count
        << ",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"protocol_node_entries\":[";
  for (std::size_t i = 0; i < graph.protocol_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.protocol_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"protocol_name\":\"" << EscapeJsonString(node.protocol_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"inherited_protocol_owner_identities\":[";
    for (std::size_t inherited_index = 0;
         inherited_index <
         node.inherited_protocol_owner_identities_lexicographic.size();
         ++inherited_index) {
      if (inherited_index != 0u) {
        out << ",";
      }
      out << "\""
          << EscapeJsonString(node.inherited_protocol_owner_identities_lexicographic[inherited_index])
          << "\"";
    }
    out << "],\"property_count\":" << node.property_count
        << ",\"method_count\":" << node.method_count
        << ",\"is_forward_declaration\":"
        << (node.is_forward_declaration ? "true" : "false")
        << ",\"declaration_complete\":"
        << (node.declaration_complete ? "true" : "false")
        << ",\"inherited_protocol_identity_complete\":"
        << (node.inherited_protocol_identity_complete ? "true" : "false")
        << ",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"category_node_entries\":[";
  for (std::size_t i = 0; i < graph.category_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.category_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"class_name\":\"" << EscapeJsonString(node.class_name)
        << "\",\"category_name\":\"" << EscapeJsonString(node.category_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"interface_owner_identity\":\""
        << EscapeJsonString(node.interface_owner_identity)
        << "\",\"implementation_owner_identity\":\""
        << EscapeJsonString(node.implementation_owner_identity)
        << "\",\"class_owner_identity\":\""
        << EscapeJsonString(node.class_owner_identity)
        << "\",\"adopted_protocol_owner_identities\":[";
    for (std::size_t adopted_index = 0;
         adopted_index < node.adopted_protocol_owner_identities_lexicographic.size();
         ++adopted_index) {
      if (adopted_index != 0u) {
        out << ",";
      }
      out << "\""
          << EscapeJsonString(node.adopted_protocol_owner_identities_lexicographic[adopted_index])
          << "\"";
    }
    out << "],\"has_interface\":" << (node.has_interface ? "true" : "false")
        << ",\"has_implementation\":"
        << (node.has_implementation ? "true" : "false")
        << ",\"declaration_complete\":"
        << (node.declaration_complete ? "true" : "false")
        << ",\"attachment_identity_complete\":"
        << (node.attachment_identity_complete ? "true" : "false")
        << ",\"conformance_identity_complete\":"
        << (node.conformance_identity_complete ? "true" : "false")
        << ",\"interface_property_count\":" << node.interface_property_count
        << ",\"implementation_property_count\":"
        << node.implementation_property_count
        << ",\"interface_method_count\":" << node.interface_method_count
        << ",\"implementation_method_count\":"
        << node.implementation_method_count
        << ",\"interface_class_method_count\":"
        << node.interface_class_method_count
        << ",\"implementation_class_method_count\":"
        << node.implementation_class_method_count
        << ",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"property_node_entries\":[";
  for (std::size_t i = 0; i < graph.property_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.property_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"owner_kind\":\"" << EscapeJsonString(node.owner_kind)
        << "\",\"owner_name\":\"" << EscapeJsonString(node.owner_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"declaration_owner_identity\":\""
        << EscapeJsonString(node.declaration_owner_identity)
        << "\",\"export_owner_identity\":\""
        << EscapeJsonString(node.export_owner_identity)
        << "\",\"property_name\":\"" << EscapeJsonString(node.property_name)
        << "\",\"type_name\":\"" << EscapeJsonString(node.type_name)
        << "\",\"has_getter\":" << (node.has_getter ? "true" : "false")
        << ",\"getter_selector\":\"" << EscapeJsonString(node.getter_selector)
        << "\",\"has_setter\":" << (node.has_setter ? "true" : "false")
        << ",\"setter_selector\":\"" << EscapeJsonString(node.setter_selector)
        << "\",\"ivar_binding_symbol\":\""
        << EscapeJsonString(node.ivar_binding_symbol)
        << "\",\"executable_synthesized_binding_kind\":\""
        << EscapeJsonString(node.executable_synthesized_binding_kind)
        << "\",\"executable_synthesized_binding_symbol\":\""
        << EscapeJsonString(node.executable_synthesized_binding_symbol)
        << "\",\"property_attribute_profile\":\""
        << EscapeJsonString(node.property_attribute_profile)
        << "\",\"ownership_lifetime_profile\":\""
        << EscapeJsonString(node.ownership_lifetime_profile)
        << "\",\"ownership_runtime_hook_profile\":\""
        << EscapeJsonString(node.ownership_runtime_hook_profile)
        << "\",\"effective_getter_selector\":\""
        << EscapeJsonString(node.effective_getter_selector)
        << "\",\"effective_setter_available\":"
        << (node.effective_setter_available ? "true" : "false")
        << ",\"effective_setter_selector\":\""
        << EscapeJsonString(node.effective_setter_selector)
        << "\",\"accessor_ownership_profile\":\""
        << EscapeJsonString(node.accessor_ownership_profile)
        << "\",\"synthesizes_executable_accessors\":"
        << (node.synthesizes_executable_accessors ? "true" : "false")
        << ",\"getter_storage_runtime_helper_symbol\":\""
        << EscapeJsonString(node.getter_storage_runtime_helper_symbol)
        << "\",\"setter_storage_runtime_helper_symbol\":\""
        << EscapeJsonString(node.setter_storage_runtime_helper_symbol)
        << "\",\"executable_ivar_layout_symbol\":\""
        << EscapeJsonString(node.executable_ivar_layout_symbol)
        << "\",\"executable_ivar_layout_slot_index\":"
        << node.executable_ivar_layout_slot_index
        << ",\"executable_ivar_layout_size_bytes\":"
        << node.executable_ivar_layout_size_bytes
        << ",\"executable_ivar_layout_alignment_bytes\":"
        << node.executable_ivar_layout_alignment_bytes
        << ",\"executable_ivar_init_order_index\":"
        << node.executable_ivar_init_order_index
        << ",\"executable_ivar_destroy_order_index\":"
        << node.executable_ivar_destroy_order_index
        << ",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"method_node_entries\":[";
  for (std::size_t i = 0; i < graph.method_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.method_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"owner_kind\":\"" << EscapeJsonString(node.owner_kind)
        << "\",\"owner_name\":\"" << EscapeJsonString(node.owner_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"declaration_owner_identity\":\""
        << EscapeJsonString(node.declaration_owner_identity)
        << "\",\"export_owner_identity\":\""
        << EscapeJsonString(node.export_owner_identity)
        << "\",\"selector\":\"" << EscapeJsonString(node.selector)
        << "\",\"is_class_method\":"
        << (node.is_class_method ? "true" : "false")
        << ",\"has_body\":" << (node.has_body ? "true" : "false")
        << ",\"parameter_count\":" << node.parameter_count
        << ",\"return_type_name\":\""
        << EscapeJsonString(node.return_type_name)
        << "\",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"ivar_node_entries\":[";
  for (std::size_t i = 0; i < graph.ivar_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.ivar_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"owner_kind\":\"" << EscapeJsonString(node.owner_kind)
        << "\",\"owner_name\":\"" << EscapeJsonString(node.owner_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"declaration_owner_identity\":\""
        << EscapeJsonString(node.declaration_owner_identity)
        << "\",\"export_owner_identity\":\""
        << EscapeJsonString(node.export_owner_identity)
        << "\",\"property_owner_identity\":\""
        << EscapeJsonString(node.property_owner_identity)
        << "\",\"property_name\":\"" << EscapeJsonString(node.property_name)
        << "\",\"ivar_binding_symbol\":\""
        << EscapeJsonString(node.ivar_binding_symbol)
        << "\",\"executable_synthesized_binding_kind\":\""
        << EscapeJsonString(node.executable_synthesized_binding_kind)
        << "\",\"executable_synthesized_binding_symbol\":\""
        << EscapeJsonString(node.executable_synthesized_binding_symbol)
        << "\",\"executable_ivar_layout_symbol\":\""
        << EscapeJsonString(node.executable_ivar_layout_symbol)
        << "\",\"executable_ivar_layout_slot_index\":"
        << node.executable_ivar_layout_slot_index
        << ",\"executable_ivar_layout_size_bytes\":"
        << node.executable_ivar_layout_size_bytes
        << ",\"executable_ivar_layout_alignment_bytes\":"
        << node.executable_ivar_layout_alignment_bytes
        << ",\"executable_ivar_init_order_index\":"
        << node.executable_ivar_init_order_index
        << ",\"executable_ivar_destroy_order_index\":"
        << node.executable_ivar_destroy_order_index
        << ",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"owner_edges\":[";
  for (std::size_t i = 0; i < graph.owner_edges_lexicographic.size(); ++i) {
    const auto &edge = graph.owner_edges_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"edge_kind\":\"" << EscapeJsonString(edge.edge_kind)
        << "\",\"source_owner_identity\":\""
        << EscapeJsonString(edge.source_owner_identity)
        << "\",\"target_owner_identity\":\""
        << EscapeJsonString(edge.target_owner_identity)
        << "\",\"line\":" << edge.line << ",\"column\":" << edge.column
        << "}";
  }
  out << "],\"lexicographic_owner_edge_ordering\":"
      << (graph.deterministic ? "true" : "false")
      << ",\"source_graph_complete\":"
      << (graph.source_graph_complete ? "true" : "false")
      << ",\"ready_for_semantic_closure\":"
      << (graph.ready_for_semantic_closure ? "true" : "false")
      << ",\"ready_for_lowering\":"
      << (graph.ready_for_lowering ? "true" : "false") << "}";
  return out.str();
}

std::string BuildExecutableMetadataSemanticConsistencyBoundaryJson(
    const Objc3ExecutableMetadataSemanticConsistencyBoundary &boundary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(boundary.contract_id)
      << "\",\"executable_metadata_source_graph_contract_id\":\""
      << EscapeJsonString(boundary.executable_metadata_source_graph_contract_id)
      << "\",\"semantic_boundary_frozen\":"
      << (boundary.semantic_boundary_frozen ? "true" : "false")
      << ",\"lowering_admission_ready\":"
      << (boundary.lowering_admission_ready ? "true" : "false")
      << ",\"fail_closed\":"
      << (boundary.fail_closed ? "true" : "false")
      << ",\"ready\":"
      << (IsReadyObjc3ExecutableMetadataSemanticConsistencyBoundary(boundary)
              ? "true"
              : "false")
      << ",\"source_graph_ready\":"
      << (boundary.source_graph_ready ? "true" : "false")
      << ",\"protocol_category_handoff_deterministic\":"
      << (boundary.protocol_category_handoff_deterministic ? "true" : "false")
      << ",\"class_protocol_category_linking_deterministic\":"
      << (boundary.class_protocol_category_linking_deterministic ? "true"
                                                                 : "false")
      << ",\"selector_normalization_deterministic\":"
      << (boundary.selector_normalization_deterministic ? "true" : "false")
      << ",\"property_attribute_deterministic\":"
      << (boundary.property_attribute_deterministic ? "true" : "false")
      << ",\"symbol_graph_scope_resolution_deterministic\":"
      << (boundary.symbol_graph_scope_resolution_deterministic ? "true"
                                                               : "false")
      << ",\"protocol_inheritance_edges_complete\":"
      << (boundary.protocol_inheritance_edges_complete ? "true" : "false")
      << ",\"category_attachment_edges_complete\":"
      << (boundary.category_attachment_edges_complete ? "true" : "false")
      << ",\"declaration_export_owner_split_complete\":"
      << (boundary.declaration_export_owner_split_complete ? "true" : "false")
      << ",\"property_method_ivar_owner_edges_complete\":"
      << (boundary.property_method_ivar_owner_edges_complete ? "true" : "false")
      << ",\"semantic_conflict_diagnostics_enforcement_pending\":"
      << (boundary.semantic_conflict_diagnostics_enforcement_pending ? "true"
                                                                     : "false")
      << ",\"duplicate_export_owner_enforcement_pending\":"
      << (boundary.duplicate_export_owner_enforcement_pending ? "true"
                                                              : "false")
      << ",\"lowering_admission_pending\":"
      << (boundary.lowering_admission_pending ? "true" : "false")
      << ",\"protocol_node_count\":" << boundary.protocol_node_count
      << ",\"category_node_count\":" << boundary.category_node_count
      << ",\"property_node_count\":" << boundary.property_node_count
      << ",\"method_node_count\":" << boundary.method_node_count
      << ",\"ivar_node_count\":" << boundary.ivar_node_count
      << ",\"owner_edge_count\":" << boundary.owner_edge_count
      << ",\"failure_reason\":\"" << EscapeJsonString(boundary.failure_reason)
      << "\"}";
  return out.str();
}

std::string BuildVersionedConformanceReportLoweringReplayKey(
    const Objc3VersionedConformanceReportLoweringSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";semantic_contract_id=" << summary.semantic_contract_id
      << ";artifact_schema_id=" << summary.artifact_schema_id
      << ";effective_language_profile="
      << summary.effective_language_profile
      << ";canonical_literal_rejection_diagnostics="
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true" : "false")
      << ";runnable_claim_count=" << summary.runnable_feature_claim_count
      << ";source_only_claim_count=" << summary.source_only_feature_claim_count
      << ";unsupported_claim_count="
      << summary.unsupported_feature_claim_count
      << ";live_unsupported_feature_families="
      << summary.live_unsupported_feature_family_count
      << ";live_unsupported_feature_sites="
      << summary.live_unsupported_feature_site_count
      << ";suppressed_macro_claim_ids=";
  for (std::size_t index = 0;
       index < summary.suppressed_macro_claim_ids.size();
       ++index) {
    if (index > 0u) {
      out << ",";
    }
    out << summary.suppressed_macro_claim_ids[index];
  }
  out << ";semantic_boundary_replay_key=" << summary.semantic_boundary_replay_key;
  return out.str();
}

Objc3VersionedConformanceReportLoweringSummary
BuildVersionedConformanceReportLoweringSummary(
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary
        &semantic_summary) {
  Objc3VersionedConformanceReportLoweringSummary summary;
  summary.effective_language_profile =
      semantic_summary.effective_language_profile;
  summary.canonical_literal_rejection_diagnostics_enabled = true;
  summary.runnable_feature_claim_ids = BuildRunnableFeatureClaimIds();
  summary.source_only_feature_claim_ids = BuildSourceOnlyFeatureClaimIds();
  summary.unsupported_feature_claim_ids = BuildUnsupportedFeatureClaimIds();
  summary.runnable_feature_claim_count =
      summary.runnable_feature_claim_ids.size();
  summary.source_only_feature_claim_count =
      summary.source_only_feature_claim_ids.size();
  summary.unsupported_feature_claim_count =
      summary.unsupported_feature_claim_ids.size();
  summary.runnable_feature_claim_inventory_replay_key =
      BuildRunnableFeatureClaimInventoryReplayKey(options, pipeline_result);
  summary.feature_claim_truth_surface_replay_key =
      BuildFeatureClaimStrictnessTruthSurfaceReplayKey(options, pipeline_result);
  if (IsReadyObjc3FrontendCompatibilityStrictnessClaimSemanticsSummary(
          semantic_summary)) {
    summary.suppressed_macro_claim_ids =
        semantic_summary.suppressed_macro_claim_ids;
    summary.live_unsupported_feature_family_count =
        semantic_summary.live_unsupported_feature_family_count;
    summary.live_unsupported_feature_site_count =
        semantic_summary.live_unsupported_feature_site_count;
    summary.live_unsupported_feature_diagnostic_count =
        semantic_summary.live_unsupported_feature_diagnostic_count;
    summary.fail_closed = semantic_summary.fail_closed;
    summary.semantic_surface_published = true;
    summary.runnable_claim_inventory_ready = true;
    summary.feature_claim_truth_surface_ready = true;
    summary.semantic_boundary_ready =
        semantic_summary.semantic_boundary_ready;
    summary.known_unsupported_surface_published =
        semantic_summary.live_unsupported_feature_source_rejection_landed;
    summary.compatibility_selection_truthful =
        semantic_summary.language_profile_semantics_landed;
    summary.strictness_selection_fail_closed =
        semantic_summary.strictness_selection_rejection_semantics_landed;
    summary.strict_concurrency_selection_fail_closed =
        semantic_summary.strictness_selection_rejection_semantics_landed;
    summary.canonical_interface_truthful =
        semantic_summary.canonical_interface_truth_semantics_landed;
    summary.feature_macro_truthful =
        semantic_summary.feature_macro_claim_suppression_semantics_landed &&
        semantic_summary.separate_compilation_macro_truth_semantics_landed;
    summary.ready_for_runtime_conformance_publication =
        semantic_summary.ready_for_lowering_and_runtime;
    summary.semantic_boundary_replay_key =
        semantic_summary.semantic_boundary_replay_key;
    summary.replay_key =
        BuildVersionedConformanceReportLoweringReplayKey(summary);
  }
  if (!IsReadyObjc3VersionedConformanceReportLoweringSummary(summary)) {
    summary.failure_reason =
        "versioned conformance-report lowering summary is incomplete";
  }
  return summary;
}

std::string BuildVersionedConformanceReportLoweringSummaryJson(
    const Objc3VersionedConformanceReportLoweringSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"semantic_contract_id\":\""
      << EscapeJsonString(summary.semantic_contract_id)
      << "\",\"runnable_feature_claim_inventory_contract_id\":\""
      << EscapeJsonString(summary.runnable_feature_claim_inventory_contract_id)
      << "\",\"feature_claim_truth_surface_contract_id\":\""
      << EscapeJsonString(summary.feature_claim_truth_surface_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"artifact_suffix\":\""
      << EscapeJsonString(summary.artifact_suffix)
      << "\",\"artifact_schema_id\":\""
      << EscapeJsonString(summary.artifact_schema_id)
      << "\",\"payload_model\":\""
      << EscapeJsonString(summary.payload_model)
      << "\",\"authority_model\":\""
      << EscapeJsonString(summary.authority_model)
      << "\",\"known_unsupported_model\":\""
      << EscapeJsonString(summary.known_unsupported_model)
      << "\",\"selection_model\":\""
      << EscapeJsonString(summary.selection_model)
      << "\",\"canonical_interface_mode\":\""
      << EscapeJsonString(summary.canonical_interface_mode)
      << "\",\"publication_model\":\""
      << EscapeJsonString(summary.publication_model)
      << "\",\"effective_language_profile\":\""
      << EscapeJsonString(summary.effective_language_profile)
      << "\",\"canonical_literal_rejection_diagnostics_enabled\":"
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true" : "false")
      << ",\"runnable_feature_claim_count\":"
      << summary.runnable_feature_claim_count
      << ",\"source_only_feature_claim_count\":"
      << summary.source_only_feature_claim_count
      << ",\"unsupported_feature_claim_count\":"
      << summary.unsupported_feature_claim_count
      << ",\"live_unsupported_feature_family_count\":"
      << summary.live_unsupported_feature_family_count
      << ",\"live_unsupported_feature_site_count\":"
      << summary.live_unsupported_feature_site_count
      << ",\"live_unsupported_feature_diagnostic_count\":"
      << summary.live_unsupported_feature_diagnostic_count
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"semantic_surface_published\":"
      << (summary.semantic_surface_published ? "true" : "false")
      << ",\"runnable_claim_inventory_ready\":"
      << (summary.runnable_claim_inventory_ready ? "true" : "false")
      << ",\"feature_claim_truth_surface_ready\":"
      << (summary.feature_claim_truth_surface_ready ? "true" : "false")
      << ",\"semantic_boundary_ready\":"
      << (summary.semantic_boundary_ready ? "true" : "false")
      << ",\"known_unsupported_surface_published\":"
      << (summary.known_unsupported_surface_published ? "true" : "false")
      << ",\"compatibility_selection_truthful\":"
      << (summary.compatibility_selection_truthful ? "true" : "false")
      << ",\"strictness_selection_fail_closed\":"
      << (summary.strictness_selection_fail_closed ? "true" : "false")
      << ",\"strict_concurrency_selection_fail_closed\":"
      << (summary.strict_concurrency_selection_fail_closed ? "true" : "false")
      << ",\"canonical_interface_truthful\":"
      << (summary.canonical_interface_truthful ? "true" : "false")
      << ",\"feature_macro_truthful\":"
      << (summary.feature_macro_truthful ? "true" : "false")
      << ",\"ready_for_runtime_conformance_publication\":"
      << (summary.ready_for_runtime_conformance_publication ? "true" : "false")
      << ",\"runnable_feature_claim_ids\":"
      << BuildStringArrayJson(summary.runnable_feature_claim_ids)
      << ",\"source_only_feature_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_feature_claim_ids)
      << ",\"unsupported_feature_claim_ids\":"
      << BuildStringArrayJson(summary.unsupported_feature_claim_ids)
      << ",\"suppressed_macro_claim_ids\":"
      << BuildStringArrayJson(summary.suppressed_macro_claim_ids)
      << ",\"runnable_feature_claim_inventory_replay_key\":\""
      << EscapeJsonString(summary.runnable_feature_claim_inventory_replay_key)
      << "\",\"feature_claim_truth_surface_replay_key\":\""
      << EscapeJsonString(summary.feature_claim_truth_surface_replay_key)
      << "\",\"semantic_boundary_replay_key\":\""
      << EscapeJsonString(summary.semantic_boundary_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"ready\":"
      << (IsReadyObjc3VersionedConformanceReportLoweringSummary(summary)
              ? "true"
              : "false")
      << "}";
  return out.str();
}

std::string BuildRuntimeCapabilityProfilesJson() {
  return "["
         "{\"id\":\"core\",\"status\":\"claimed\"},"
         "{\"id\":\"strict\",\"status\":\"claimed\"},"
         "{\"id\":\"strict-concurrency\",\"status\":\"claimed\"},"
         "{\"id\":\"strict-system\",\"status\":\"claimed\"}"
         "]";
}

std::vector<std::string> BuildClaimedConformanceProfileIds() {
  return {"core", "strict", "strict-concurrency", "strict-system"};
}

std::vector<std::string> BuildNotClaimedConformanceProfileIds() {
  return {};
}

std::string BuildRuntimeCapabilityOptionalFeaturesJson() {
  struct OptionalFeatureEntry {
    const char *id;
    const char *status;
    const char *reason;
    const char *backing_claim_id;
  };
  constexpr OptionalFeatureEntry kEntries[] = {
      {"throws", "not-claimed",
       "runtime-backed throws/error propagation is not part of the runnable native surface yet",
       kObjc3UnsupportedFeatureClaimThrows},
      {"async-await", "not-claimed",
       "async/await lowering and runtime scheduling are not part of the runnable native surface yet",
       kObjc3UnsupportedFeatureClaimAsyncAwait},
      {"actors", "not-claimed",
       "actor isolation and actor runtime support are not part of the runnable native surface yet",
       kObjc3UnsupportedFeatureClaimActors},
      {"blocks", "not-claimed",
       "blocks are still tracked as unsupported in the public conformance claim surface",
       kObjc3UnsupportedFeatureClaimBlocks},
      {"arc", "not-claimed",
       "ARC remains unsupported in the public conformance claim surface until the full runnable ARC contract closes",
       kObjc3UnsupportedFeatureClaimArc},
  };
  constexpr std::size_t kEntryCount = sizeof(kEntries) / sizeof(kEntries[0]);
  std::ostringstream out;
  out << "[";
  for (std::size_t i = 0; i < kEntryCount; ++i) {
    const auto &entry = kEntries[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"id\":\"" << entry.id << "\",\"status\":\""
        << entry.status << "\",\"reason\":\""
        << EscapeJsonString(entry.reason) << "\",\"backing_claim_id\":\""
        << entry.backing_claim_id << "\"}";
  }
  out << "]";
  return out.str();
}

std::string BuildRuntimeCapabilityVersionsJson() {
  std::ostringstream out;
  out << "{\"frontend\":\""
      << kObjc3RuntimeCapabilityToolchainVersion
      << "\",\"runtime\":\"" << kObjc3RuntimeCapabilityToolchainVersion
      << "\",\"stdlib\":\"" << kObjc3RuntimeCapabilityToolchainVersion
      << "\",\"module_format\":\""
      << kObjc3RuntimeCapabilityModuleFormatVersion << "\"}";
  return out.str();
}

std::string BuildToolingMachineReadableConformanceReportContractReplayKey(
    const Objc3ToolingMachineReadableConformanceReportContractSummary
        &summary) {
  std::ostringstream out;
  out << "mode=" << summary.effective_language_profile
      << ";migration=" << (summary.migration_semantics_ready ? "true" : "false")
      << ";lowering=" << (summary.lowering_contract_ready ? "true" : "false")
      << ";runtime-capability="
      << (summary.runtime_capability_surface_published ? "true" : "false")
      << ";ready="
      << (summary.ready_for_runtime_publication ? "true" : "false");
  return out.str();
}

Objc3ToolingMachineReadableConformanceReportContractSummary
BuildToolingMachineReadableConformanceReportContractSummary(
    const Objc3ToolingLegacyCanonicalMigrationSemanticsSummary
        &migration_summary,
    const Objc3VersionedConformanceReportLoweringSummary &lowering_summary) {
  Objc3ToolingMachineReadableConformanceReportContractSummary summary;
  summary.effective_language_profile =
      lowering_summary.effective_language_profile;
  summary.migration_semantics_ready =
      migration_summary.ready_for_lowering_and_runtime;
  summary.lowering_contract_ready =
      IsReadyObjc3VersionedConformanceReportLoweringSummary(lowering_summary);
  summary.runtime_capability_surface_published =
      summary.lowering_contract_ready &&
      lowering_summary.ready_for_runtime_conformance_publication;
  summary.deterministic_handoff =
      summary.migration_semantics_ready && summary.lowering_contract_ready &&
      summary.runtime_capability_surface_published &&
      lowering_summary.artifact_suffix ==
          kObjc3VersionedConformanceReportLoweringArtifactSuffix &&
      lowering_summary.artifact_schema_id ==
          kObjc3VersionedConformanceReportLoweringArtifactSchemaId;
  summary.ready_for_runtime_publication = summary.deterministic_handoff;
  if (!summary.migration_semantics_ready) {
    summary.failure_reason =
        "tooling legacy/canonical migration semantics prerequisite is not ready";
  } else if (!summary.lowering_contract_ready) {
    summary.failure_reason =
        "versioned conformance-report lowering prerequisite is not ready";
  }
  summary.replay_key =
      BuildToolingMachineReadableConformanceReportContractReplayKey(summary);
  return summary;
}

std::string BuildToolingMachineReadableConformanceReportContractSummaryJson(
    const Objc3ToolingMachineReadableConformanceReportContractSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"lowering_contract_id\":\""
      << EscapeJsonString(summary.lowering_contract_id)
      << "\",\"runtime_capability_contract_id\":\""
      << EscapeJsonString(summary.runtime_capability_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"payload_model\":\""
      << EscapeJsonString(summary.payload_model)
      << "\",\"authority_model\":\""
      << EscapeJsonString(summary.authority_model)
      << "\",\"artifact_suffix\":\""
      << EscapeJsonString(summary.artifact_suffix)
      << "\",\"artifact_schema_id\":\""
      << EscapeJsonString(summary.artifact_schema_id)
      << "\",\"runtime_capability_schema_id\":\""
      << EscapeJsonString(summary.runtime_capability_schema_id)
      << "\",\"effective_language_profile\":\""
      << EscapeJsonString(summary.effective_language_profile)
      << "\",\"migration_semantics_ready\":"
      << (summary.migration_semantics_ready ? "true" : "false")
      << ",\"lowering_contract_ready\":"
      << (summary.lowering_contract_ready ? "true" : "false")
      << ",\"runtime_capability_surface_published\":"
      << (summary.runtime_capability_surface_published ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_runtime_publication\":"
      << (summary.ready_for_runtime_publication ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildToolingFeatureAwareConformanceReportEmissionReplayKey(
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary &summary) {
  std::ostringstream out;
  out << "mode=" << summary.effective_language_profile
      << ";canonical-rejection-diagnostics="
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true" : "false")
      << ";families=" << summary.fixit_family_count
      << ";legacy-sites=" << summary.current_run_legacy_literal_sites
      << ";ready="
      << (summary.ready_for_runtime_publication ? "true" : "false");
  return out.str();
}

Objc3ToolingFeatureAwareConformanceReportEmissionSummary
BuildToolingFeatureAwareConformanceReportEmissionSummary(
    const Objc3ToolingFeatureSpecificFixitSynthesisSummary &fixit_summary,
    const Objc3ToolingLegacyCanonicalMigrationSemanticsSummary
        &migration_summary,
    const Objc3ToolingMachineReadableConformanceReportContractSummary
        &report_summary) {
  Objc3ToolingFeatureAwareConformanceReportEmissionSummary summary;
  summary.effective_language_profile =
      migration_summary.effective_language_profile;
  summary.canonical_literal_rejection_diagnostics_enabled =
      migration_summary.canonical_literal_rejection_diagnostics_enabled;
  summary.fixit_family_ids = fixit_summary.fixit_family_ids;
  summary.fixit_family_count = fixit_summary.fixit_family_count;
  summary.current_run_legacy_literal_sites =
      migration_summary.current_run_legacy_literal_sites;
  summary.canonical_mode_rejection_code =
      migration_summary.canonical_mode_rejection_code;
  summary.report_payload_emitted =
      report_summary.ready_for_runtime_publication;
  summary.deterministic_handoff =
      fixit_summary.ready_for_lowering_and_runtime &&
      migration_summary.ready_for_lowering_and_runtime &&
      report_summary.ready_for_runtime_publication &&
      summary.fixit_family_count == summary.fixit_family_ids.size();
  summary.ready_for_runtime_publication = summary.deterministic_handoff;
  if (!report_summary.ready_for_runtime_publication) {
    summary.failure_reason =
        "tooling machine-readable conformance/report contract prerequisite is not ready";
  }
  summary.replay_key =
      BuildToolingFeatureAwareConformanceReportEmissionReplayKey(summary);
  return summary;
}

std::string BuildToolingFeatureAwareConformanceReportEmissionSummaryJson(
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"machine_readable_report_contract_id\":\""
      << EscapeJsonString(summary.machine_readable_report_contract_id)
      << "\",\"fixit_contract_id\":\""
      << EscapeJsonString(summary.fixit_contract_id)
      << "\",\"migration_semantics_contract_id\":\""
      << EscapeJsonString(summary.migration_semantics_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"payload_model\":\""
      << EscapeJsonString(summary.payload_model)
      << "\",\"authority_model\":\""
      << EscapeJsonString(summary.authority_model)
      << "\",\"effective_language_profile\":\""
      << EscapeJsonString(summary.effective_language_profile)
      << "\",\"canonical_literal_rejection_diagnostics_enabled\":"
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true" : "false")
      << ",\"fixit_family_ids\":"
      << BuildStringArrayJson(summary.fixit_family_ids)
      << ",\"fixit_family_count\":" << summary.fixit_family_count
      << ",\"current_run_legacy_literal_sites\":"
      << summary.current_run_legacy_literal_sites
      << ",\"canonical_mode_rejection_code\":\""
      << EscapeJsonString(summary.canonical_mode_rejection_code)
      << "\",\"report_payload_emitted\":"
      << (summary.report_payload_emitted ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_runtime_publication\":"
      << (summary.ready_for_runtime_publication ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildToolingAdvancedFeatureReportingJson(
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary &summary) {
  return BuildToolingFeatureAwareConformanceReportEmissionSummaryJson(summary);
}

std::string BuildToolingCorpusShardingReleaseEvidencePackagingReplayKey(
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary &summary) {
  std::ostringstream out;
  out << "mode=" << summary.effective_language_profile
      << ";canonical-rejection-diagnostics="
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true" : "false")
      << ";profiles=" << summary.targeted_profile_count
      << ";corpus-shards=" << summary.corpus_shard_count
      << ";release-artifacts=" << summary.release_evidence_artifact_count
      << ";ready="
      << (summary.ready_for_release_evidence_packaging ? "true" : "false");
  return out.str();
}

Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary
BuildToolingCorpusShardingReleaseEvidencePackagingSummary(
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary
        &feature_summary) {
  Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary summary;
  summary.effective_language_profile =
      feature_summary.effective_language_profile;
  summary.canonical_literal_rejection_diagnostics_enabled =
      feature_summary.canonical_literal_rejection_diagnostics_enabled;
  summary.feature_report_payload_emitted =
      feature_summary.report_payload_emitted;
  summary.deterministic_handoff =
      IsReadyObjc3ToolingFeatureAwareConformanceReportEmissionSummary(
          feature_summary) &&
      summary.targeted_profile_count == summary.targeted_profile_ids.size() &&
      summary.corpus_shard_count == summary.corpus_shard_ids.size() &&
      summary.corpus_shard_count ==
          summary.corpus_shard_manifest_paths.size() &&
      summary.release_evidence_artifact_count ==
          summary.release_evidence_artifact_ids.size();
  summary.ready_for_release_evidence_packaging =
      summary.deterministic_handoff;
  if (!summary.feature_report_payload_emitted) {
    summary.failure_reason =
        "tooling feature-aware conformance report emission prerequisite is not ready";
  }
  summary.replay_key =
      BuildToolingCorpusShardingReleaseEvidencePackagingReplayKey(summary);
  return summary;
}

std::string BuildToolingCorpusShardingReleaseEvidencePackagingSummaryJson(
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"feature_aware_report_contract_id\":\""
      << EscapeJsonString(summary.feature_aware_report_contract_id)
      << "\",\"machine_readable_report_contract_id\":\""
      << EscapeJsonString(summary.machine_readable_report_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"payload_model\":\""
      << EscapeJsonString(summary.payload_model)
      << "\",\"authority_model\":\""
      << EscapeJsonString(summary.authority_model)
      << "\",\"targeted_profile_ids\":"
      << BuildStringArrayJson(summary.targeted_profile_ids)
      << ",\"targeted_profile_count\":" << summary.targeted_profile_count
      << ",\"corpus_shard_ids\":"
      << BuildStringArrayJson(summary.corpus_shard_ids)
      << ",\"corpus_shard_manifest_paths\":"
      << BuildStringArrayJson(summary.corpus_shard_manifest_paths)
      << ",\"corpus_shard_count\":" << summary.corpus_shard_count
      << ",\"release_evidence_artifact_ids\":"
      << BuildStringArrayJson(summary.release_evidence_artifact_ids)
      << ",\"release_evidence_artifact_count\":"
      << summary.release_evidence_artifact_count
      << ",\"release_evidence_checklist_path\":\""
      << EscapeJsonString(summary.release_evidence_checklist_path)
      << "\",\"release_evidence_schema_path\":\""
      << EscapeJsonString(summary.release_evidence_schema_path)
      << "\",\"report_artifact_suffix\":\""
      << EscapeJsonString(summary.report_artifact_suffix)
      << "\",\"effective_language_profile\":\""
      << EscapeJsonString(summary.effective_language_profile)
      << "\",\"canonical_literal_rejection_diagnostics_enabled\":"
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true" : "false")
      << ",\"feature_report_payload_emitted\":"
      << (summary.feature_report_payload_emitted ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_release_evidence_packaging\":"
      << (summary.ready_for_release_evidence_packaging ? "true" : "false")
      << ",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildToolingAdvancedFeatureReleaseEvidenceJson(
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary &summary) {
  return BuildToolingCorpusShardingReleaseEvidencePackagingSummaryJson(summary);
}

std::string BuildRuntimeCapabilityReportJson(
    const Objc3VersionedConformanceReportLoweringSummary &summary) {
  const std::vector<std::string> claimed_profile_ids =
      BuildClaimedConformanceProfileIds();
  const std::vector<std::string> not_claimed_profile_ids =
      BuildNotClaimedConformanceProfileIds();
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityReportingContractId)
      << "\",\"schema_id\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityReportingSchemaId)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityReportingSurfacePath)
      << "\",\"source_frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"profile_model\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityReportingProfileModel)
      << "\",\"optional_feature_model\":\""
      << EscapeJsonString(
             kObjc3RuntimeCapabilityReportingOptionalFeatureModel)
      << "\",\"version_model\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityReportingVersionModel)
      << "\",\"strictness_mode\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityStrictnessMode)
      << "\",\"concurrency_mode\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityConcurrencyMode)
      << "\",\"claimed_profile_ids\":"
      << BuildStringArrayJson(claimed_profile_ids)
      << ",\"not_claimed_profile_ids\":"
      << BuildStringArrayJson(not_claimed_profile_ids)
      << ",\"profiles\":" << BuildRuntimeCapabilityProfilesJson()
      << ",\"runtime_capability_ids\":"
      << BuildStringArrayJson(summary.runnable_feature_claim_ids)
      << ",\"source_only_feature_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_feature_claim_ids)
      << ",\"unsupported_feature_claim_ids\":"
      << BuildStringArrayJson(summary.unsupported_feature_claim_ids)
      << ",\"optional_features\":"
      << BuildRuntimeCapabilityOptionalFeaturesJson()
      << ",\"versions\":" << BuildRuntimeCapabilityVersionsJson()
      << ",\"public_schema_id\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityPublicSchemaId)
      << "\",\"replay_generated_at\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityGeneratedAtReplayValue)
      << "\",\"ready\":"
      << (IsReadyObjc3VersionedConformanceReportLoweringSummary(summary)
              ? "true"
              : "false")
      << ",\"replay_key\":\""
      << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildPublicConformanceProfilesJson() {
  struct ProfileEntry {
    const char *id;
    const char *status;
  };
  constexpr ProfileEntry kProfiles[] = {
      {"core", "claimed"},
      {"strict", "claimed"},
      {"strict-concurrency", "claimed"},
      {"strict-system", "claimed"},
  };
  constexpr std::size_t kProfileCount = sizeof(kProfiles) / sizeof(kProfiles[0]);
  std::ostringstream out;
  out << "[";
  for (std::size_t i = 0; i < kProfileCount; ++i) {
    const auto &profile = kProfiles[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"id\":\"" << profile.id << "\",\"status\":\""
        << profile.status << "\"}";
  }
  out << "]";
  return out.str();
}

std::string BuildPublicConformanceReportJson(
    const Objc3VersionedConformanceReportLoweringSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"schema_id\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityPublicSchemaId)
      << "\",\"generated_at\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityGeneratedAtReplayValue)
      << "\",\"toolchain\":{\"name\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityToolchainName)
      << "\",\"vendor\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityToolchainVendor)
      << "\",\"version\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityToolchainVersion)
      << "\",\"target_triple\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityTargetTriple)
      << "\"},\"language\":{\"language_family\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityLanguageFamily)
      << "\",\"language_version\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityLanguageVersion)
      << "\",\"spec_revision\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilitySpecRevision)
      << "\"},\"mode\":{\"strictness\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityStrictnessMode)
      << "\",\"concurrency\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityConcurrencyMode)
      << "\",\"compatibility\":\""
      << EscapeJsonString(summary.effective_language_profile)
      << "\",\"canonical_literal_rejection_diagnostics\":"
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true" : "false")
      << "},\"profiles\":"
      << BuildPublicConformanceProfilesJson()
      << ",\"optional_features\":"
      << BuildRuntimeCapabilityOptionalFeaturesJson()
      << ",\"versions\":" << BuildRuntimeCapabilityVersionsJson()
      << ",\"known_deviations\":[]"
      << ",\"source_replay_key\":\""
      << EscapeJsonString(summary.replay_key) << "\"}";
  return out.str();
}

std::string BuildVersionedConformanceReportArtifactJson(
    const Objc3VersionedConformanceReportLoweringSummary &summary,
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary
        &semantic_summary,
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary
        &feature_summary,
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary
        &packaging_summary) {
  std::ostringstream out;
  out << "{\n"
      << "  \"schema_id\": \"" << EscapeJsonString(summary.artifact_schema_id)
      << "\",\n"
      << "  \"contract_id\": \"" << EscapeJsonString(summary.contract_id)
      << "\",\n"
      << "  \"semantic_contract_id\": \""
      << EscapeJsonString(summary.semantic_contract_id) << "\",\n"
      << "  \"runnable_feature_claim_inventory_contract_id\": \""
      << EscapeJsonString(summary.runnable_feature_claim_inventory_contract_id)
      << "\",\n"
      << "  \"feature_claim_truth_surface_contract_id\": \""
      << EscapeJsonString(summary.feature_claim_truth_surface_contract_id)
      << "\",\n"
      << "  \"frontend_surface_path\": \""
      << EscapeJsonString(summary.frontend_surface_path) << "\",\n"
      << "  \"payload_model\": \""
      << EscapeJsonString(summary.payload_model) << "\",\n"
      << "  \"authority_model\": \""
      << EscapeJsonString(summary.authority_model) << "\",\n"
      << "  \"known_unsupported_model\": \""
      << EscapeJsonString(summary.known_unsupported_model) << "\",\n"
      << "  \"selection_model\": \""
      << EscapeJsonString(summary.selection_model) << "\",\n"
      << "  \"canonical_interface_mode\": \""
      << EscapeJsonString(summary.canonical_interface_mode) << "\",\n"
      << "  \"publication_model\": \""
      << EscapeJsonString(summary.publication_model) << "\",\n"
      << "  \"language_mode\": \"" << kObjc3RunnableFeatureClaimModeName
      << "\",\n"
      << "  \"language_version\": "
      << static_cast<unsigned>(options.language_version) << ",\n"
      << "  \"effective_language_profile\": \""
      << EscapeJsonString(summary.effective_language_profile) << "\",\n"
      << "  \"canonical_literal_rejection_diagnostics_enabled\": "
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true" : "false") << ",\n"
      << "  \"runnable_feature_claim_ids\": "
      << BuildStringArrayJson(summary.runnable_feature_claim_ids) << ",\n"
      << "  \"source_only_feature_claim_ids\": "
      << BuildStringArrayJson(summary.source_only_feature_claim_ids) << ",\n"
      << "  \"unsupported_feature_claim_ids\": "
      << BuildStringArrayJson(summary.unsupported_feature_claim_ids) << ",\n"
      << "  \"suppressed_macro_claim_ids\": "
      << BuildStringArrayJson(summary.suppressed_macro_claim_ids) << ",\n"
      << "  \"live_unsupported_feature_family_count\": "
      << summary.live_unsupported_feature_family_count << ",\n"
      << "  \"live_unsupported_feature_site_count\": "
      << summary.live_unsupported_feature_site_count << ",\n"
      << "  \"live_unsupported_feature_diagnostic_count\": "
      << summary.live_unsupported_feature_diagnostic_count << ",\n"
      << "  \"ready\": "
      << (IsReadyObjc3VersionedConformanceReportLoweringSummary(summary)
              ? "true"
              : "false")
      << ",\n"
      << "  \"runnable_feature_claim_inventory\": "
      << BuildRunnableFeatureClaimInventoryJson(options, pipeline_result)
      << ",\n"
      << "  \"feature_claim_truth_surface\": "
      << BuildFeatureClaimStrictnessTruthSurfaceJson(options, pipeline_result)
      << ",\n"
      << "  \"compatibility_strictness_claim_semantics\": "
      << BuildFrontendCompatibilityStrictnessClaimSemanticsSummaryJson(
             semantic_summary)
      << ",\n"
      << "  \"runtime_capability_report\": "
      << BuildRuntimeCapabilityReportJson(summary)
      << ",\n"
      << "  \"public_conformance_report\": "
      << BuildPublicConformanceReportJson(summary)
      << ",\n"
      << "  \"advanced_feature_reporting\": "
      << BuildToolingAdvancedFeatureReportingJson(feature_summary)
      << ",\n"
      << "  \"advanced_feature_release_evidence\": "
      << BuildToolingAdvancedFeatureReleaseEvidenceJson(packaging_summary)
      << ",\n"
      << "  \"runnable_feature_claim_inventory_replay_key\": \""
      << EscapeJsonString(summary.runnable_feature_claim_inventory_replay_key)
      << "\",\n"
      << "  \"feature_claim_truth_surface_replay_key\": \""
      << EscapeJsonString(summary.feature_claim_truth_surface_replay_key)
      << "\",\n"
      << "  \"semantic_boundary_replay_key\": \""
      << EscapeJsonString(summary.semantic_boundary_replay_key) << "\",\n"
      << "  \"replay_key\": \"" << EscapeJsonString(summary.replay_key)
      << "\"\n"
      << "}\n";
  return out.str();
}

std::string BuildExecutableMetadataSemanticValidationSurfaceJson(
    const Objc3ExecutableMetadataSemanticValidationSurface &surface) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(surface.contract_id)
      << "\",\"executable_metadata_semantic_consistency_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_semantic_consistency_contract_id)
      << "\",\"semantic_consistency_ready\":"
      << (surface.semantic_consistency_ready ? "true" : "false")
      << ",\"ready\":"
      << (IsReadyObjc3ExecutableMetadataSemanticValidationSurface(surface)
              ? "true"
              : "false")
      << ",\"method_lookup_override_conflict_handoff_deterministic\":"
      << (surface.method_lookup_override_conflict_handoff_deterministic
              ? "true"
              : "false")
      << ",\"class_protocol_category_linking_deterministic\":"
      << (surface.class_protocol_category_linking_deterministic ? "true"
                                                                : "false")
      << ",\"class_inheritance_edges_complete\":"
      << (surface.class_inheritance_edges_complete ? "true" : "false")
      << ",\"protocol_inheritance_edges_complete\":"
      << (surface.protocol_inheritance_edges_complete ? "true" : "false")
      << ",\"metaclass_edges_complete\":"
      << (surface.metaclass_edges_complete ? "true" : "false")
      << ",\"inheritance_chain_cycle_free\":"
      << (surface.inheritance_chain_cycle_free ? "true" : "false")
      << ",\"superclass_targets_resolved\":"
      << (surface.superclass_targets_resolved ? "true" : "false")
      << ",\"protocol_inheritance_targets_resolved\":"
      << (surface.protocol_inheritance_targets_resolved ? "true" : "false")
      << ",\"metaclass_targets_resolved\":"
      << (surface.metaclass_targets_resolved ? "true" : "false")
      << ",\"metaclass_lineage_aligned\":"
      << (surface.metaclass_lineage_aligned ? "true" : "false")
      << ",\"method_override_edges_complete\":"
      << (surface.method_override_edges_complete ? "true" : "false")
      << ",\"override_lookup_complete\":"
      << (surface.override_lookup_complete ? "true" : "false")
      << ",\"override_conflicts_absent\":"
      << (surface.override_conflicts_absent ? "true" : "false")
      << ",\"protocol_composition_valid\":"
      << (surface.protocol_composition_valid ? "true" : "false")
      << ",\"inheritance_validation_ready\":"
      << (surface.inheritance_validation_ready ? "true" : "false")
      << ",\"override_validation_ready\":"
      << (surface.override_validation_ready ? "true" : "false")
      << ",\"protocol_composition_validation_ready\":"
      << (surface.protocol_composition_validation_ready ? "true" : "false")
      << ",\"metaclass_relationship_validation_ready\":"
      << (surface.metaclass_relationship_validation_ready ? "true" : "false")
      << ",\"semantic_validation_complete\":"
      << (surface.semantic_validation_complete ? "true" : "false")
      << ",\"lowering_admission_ready\":"
      << (surface.lowering_admission_ready ? "true" : "false")
      << ",\"fail_closed\":"
      << (surface.fail_closed ? "true" : "false")
      << ",\"class_inheritance_edge_count\":"
      << surface.class_inheritance_edge_count
      << ",\"protocol_inheritance_edge_count\":"
      << surface.protocol_inheritance_edge_count
      << ",\"metaclass_super_edge_count\":"
      << surface.metaclass_super_edge_count
      << ",\"override_edge_count\":" << surface.override_edge_count
      << ",\"class_method_override_edge_count\":"
      << surface.class_method_override_edge_count
      << ",\"instance_method_override_edge_count\":"
      << surface.instance_method_override_edge_count
      << ",\"override_lookup_sites\":" << surface.override_lookup_sites
      << ",\"override_lookup_hits\":" << surface.override_lookup_hits
      << ",\"override_lookup_misses\":" << surface.override_lookup_misses
      << ",\"override_conflicts\":" << surface.override_conflicts
      << ",\"unresolved_base_interfaces\":"
      << surface.unresolved_base_interfaces
      << ",\"protocol_composition_sites\":"
      << surface.protocol_composition_sites
      << ",\"protocol_composition_symbols\":"
      << surface.protocol_composition_symbols
      << ",\"category_composition_sites\":"
      << surface.category_composition_sites
      << ",\"category_composition_symbols\":"
      << surface.category_composition_symbols
      << ",\"invalid_protocol_composition_sites\":"
      << surface.invalid_protocol_composition_sites
      << ",\"failure_reason\":\""
      << EscapeJsonString(surface.failure_reason) << "\"}";
  return out.str();
}

std::string BuildExecutableMetadataLoweringHandoffSurfaceJson(
    const Objc3ExecutableMetadataLoweringHandoffSurface &surface) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(surface.contract_id)
      << "\",\"executable_metadata_source_graph_contract_id\":\""
      << EscapeJsonString(surface.executable_metadata_source_graph_contract_id)
      << "\",\"executable_metadata_semantic_consistency_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_semantic_consistency_contract_id)
      << "\",\"executable_metadata_semantic_validation_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_semantic_validation_contract_id)
      << "\",\"ready\":"
      << (IsReadyObjc3ExecutableMetadataLoweringHandoffSurface(surface)
              ? "true"
              : "false")
      << ",\"source_graph_ready\":"
      << (surface.source_graph_ready ? "true" : "false")
      << ",\"semantic_consistency_ready\":"
      << (surface.semantic_consistency_ready ? "true" : "false")
      << ",\"semantic_validation_ready\":"
      << (surface.semantic_validation_ready ? "true" : "false")
      << ",\"semantic_type_metadata_handoff_deterministic\":"
      << (surface.semantic_type_metadata_handoff_deterministic ? "true"
                                                               : "false")
      << ",\"protocol_category_handoff_deterministic\":"
      << (surface.protocol_category_handoff_deterministic ? "true" : "false")
      << ",\"class_protocol_category_linking_handoff_deterministic\":"
      << (surface.class_protocol_category_linking_handoff_deterministic ? "true"
                                                                        : "false")
      << ",\"selector_normalization_handoff_deterministic\":"
      << (surface.selector_normalization_handoff_deterministic ? "true"
                                                               : "false")
      << ",\"property_attribute_handoff_deterministic\":"
      << (surface.property_attribute_handoff_deterministic ? "true" : "false")
      << ",\"symbol_graph_scope_resolution_handoff_deterministic\":"
      << (surface.symbol_graph_scope_resolution_handoff_deterministic ? "true"
                                                                      : "false")
      << ",\"property_synthesis_ivar_binding_handoff_deterministic\":"
      << (surface.property_synthesis_ivar_binding_handoff_deterministic ? "true"
                                                                        : "false")
      << ",\"lowering_schema_frozen\":"
      << (surface.lowering_schema_frozen ? "true" : "false")
      << ",\"fail_closed\":" << (surface.fail_closed ? "true" : "false")
      << ",\"ready_for_lowering\":"
      << (surface.ready_for_lowering ? "true" : "false")
      << ",\"interface_node_count\":" << surface.interface_node_count
      << ",\"implementation_node_count\":"
      << surface.implementation_node_count
      << ",\"class_node_count\":" << surface.class_node_count
      << ",\"metaclass_node_count\":" << surface.metaclass_node_count
      << ",\"protocol_node_count\":" << surface.protocol_node_count
      << ",\"category_node_count\":" << surface.category_node_count
      << ",\"property_node_count\":" << surface.property_node_count
      << ",\"method_node_count\":" << surface.method_node_count
      << ",\"ivar_node_count\":" << surface.ivar_node_count
      << ",\"owner_edge_count\":" << surface.owner_edge_count
      << ",\"replay_key\":\"" << EscapeJsonString(surface.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(surface.failure_reason) << "\"}";
  return out.str();
}

std::string BuildExecutableMetadataTypedLoweringHandoffJson(
    const Objc3ExecutableMetadataTypedLoweringHandoff &surface) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(surface.contract_id)
      << "\",\"executable_metadata_lowering_handoff_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_lowering_handoff_contract_id)
      << "\",\"executable_metadata_source_graph_contract_id\":\""
      << EscapeJsonString(surface.executable_metadata_source_graph_contract_id)
      << "\",\"executable_metadata_semantic_consistency_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_semantic_consistency_contract_id)
      << "\",\"executable_metadata_semantic_validation_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_semantic_validation_contract_id)
      << "\",\"manifest_schema_ordering_model\":\""
      << EscapeJsonString(surface.manifest_schema_ordering_model)
      << "\",\"ready\":"
      << (IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(surface) ? "true"
                                                                     : "false")
      << ",\"source_graph_ready\":"
      << (surface.source_graph_ready ? "true" : "false")
      << ",\"semantic_consistency_ready\":"
      << (surface.semantic_consistency_ready ? "true" : "false")
      << ",\"semantic_validation_ready\":"
      << (surface.semantic_validation_ready ? "true" : "false")
      << ",\"lowering_handoff_surface_ready\":"
      << (surface.lowering_handoff_surface_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (surface.deterministic ? "true" : "false")
      << ",\"manifest_schema_frozen\":"
      << (surface.manifest_schema_frozen ? "true" : "false")
      << ",\"fail_closed\":" << (surface.fail_closed ? "true" : "false")
      << ",\"ready_for_lowering\":"
      << (surface.ready_for_lowering ? "true" : "false")
      << ",\"source_graph\":"
      << BuildExecutableMetadataSourceGraphJson(surface.source_graph)
      << ",\"replay_key\":\"" << EscapeJsonString(surface.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(surface.failure_reason) << "\"}";
  return out.str();
