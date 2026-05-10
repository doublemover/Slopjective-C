Objc3TypeSystemTypeSemanticModelSummary BuildTypeSystemTypeSemanticModelSummary(
    const Objc3Program &ast, const Objc3SemanticIntegrationSurface &surface,
    std::size_t max_message_send_args) {
  Objc3TypeSystemTypeSemanticModelSummary summary;
  AccumulateCanonicalTypeSemanticModelSummaryFromSurface(surface, summary);
  summary.object_pointer_semantic_sites =
      surface.type_annotation_surface_summary.object_pointer_type_sites;
  summary.protocol_composition_semantic_sites =
      surface.protocol_qualified_object_type_summary.protocol_composition_sites;
  summary.generic_suffix_semantic_sites =
      surface.type_annotation_surface_summary.generic_suffix_sites;
  summary.generic_erasure_semantic_sites =
      surface.generic_metadata_abi_summary.generic_metadata_abi_sites;
  summary.nullability_suffix_semantic_sites =
      surface.type_annotation_surface_summary.nullability_suffix_sites;
  summary.nullability_semantic_sites =
      surface.nullability_flow_warning_precision_summary.nullability_flow_sites;
  summary.invalid_generic_suffix_semantic_sites =
      surface.type_annotation_surface_summary.invalid_generic_suffix_sites;
  summary.invalid_nullability_suffix_semantic_sites =
      surface.type_annotation_surface_summary.invalid_nullability_suffix_sites;
  summary.invalid_protocol_composition_semantic_sites =
      surface.protocol_qualified_object_type_summary.contract_violation_sites;
  std::unordered_map<std::string, ValueType> body_globals = surface.globals;
  for (const auto &interface_decl : ast.interfaces) {
    body_globals.try_emplace(interface_decl.name, ValueType::ObjCClass);
  }
  for (const auto &implementation_decl : ast.implementations) {
    body_globals.try_emplace(implementation_decl.name, ValueType::ObjCClass);
  }
  for (const auto &function : ast.functions) {
    std::vector<SemanticScope> scopes;
    scopes.push_back({});
    for (const auto &param : function.params) {
      scopes.back().emplace(param.name, MakeSemanticTypeFromParam(param));
    }
    Objc3MessageSendResolutionContext context;
    context.surface = &surface;
    CollectTypeSystemTypeSemanticStmtSitesFromList(function.body, scopes, body_globals,
                                              surface.functions,
                                              max_message_send_args, context,
                                              summary);
  }
  for (const auto &implementation : ast.implementations) {
    std::string super_name;
    auto interface_it = surface.interfaces.find(implementation.name);
    if (interface_it != surface.interfaces.end()) {
      super_name = interface_it->second.super_name;
    }
    for (const auto &method : implementation.methods) {
      std::vector<SemanticScope> scopes;
      scopes.push_back({});
      scopes.back().emplace("self",
                            MakeScalarSemanticType(ValueType::ObjCInstancetype));
      if (!super_name.empty()) {
        scopes.back().emplace("super",
                              MakeScalarSemanticType(ValueType::ObjCInstancetype));
      }
      for (const auto &param : method.params) {
        scopes.back().emplace(param.name, MakeSemanticTypeFromParam(param));
      }
      Objc3MessageSendResolutionContext context;
      context.inside_method = true;
      context.surface = &surface;
      context.current_implementation_name = implementation.name;
      context.current_super_name = super_name;
      CollectTypeSystemTypeSemanticStmtSitesFromList(method.body, scopes, body_globals,
                                                surface.functions,
                                                max_message_send_args, context,
                                                summary);
    }
  }
  summary.deterministic =
      summary.optional_binding_contract_violation_sites <=
          summary.optional_binding_clause_sites &&
      summary.optional_send_contract_violation_sites <= summary.optional_send_sites &&
      summary.optional_flow_contract_violation_sites <=
          (summary.optional_flow_refinement_sites +
           summary.guard_binding_exit_enforcement_sites) &&
      summary.typed_keypath_root_legality_violation_sites <=
          summary.typed_keypath_literal_sites &&
      summary.typed_keypath_member_path_contract_violation_sites <=
          summary.typed_keypath_literal_sites &&
      summary.typed_keypath_contract_violation_sites <=
          summary.typed_keypath_literal_sites &&
      summary.typed_keypath_self_root_sites <= summary.typed_keypath_literal_sites &&
      summary.typed_keypath_class_root_sites <= summary.typed_keypath_literal_sites &&
      summary.protocol_composition_semantic_sites <=
          summary.object_pointer_semantic_sites &&
      summary.generic_suffix_semantic_sites <=
          summary.object_pointer_semantic_sites &&
      summary.nullability_suffix_semantic_sites <=
          summary.object_pointer_semantic_sites &&
      summary.invalid_generic_suffix_semantic_sites <=
          summary.generic_suffix_semantic_sites &&
      summary.invalid_nullability_suffix_semantic_sites <=
          summary.nullability_suffix_semantic_sites &&
      summary.invalid_protocol_composition_semantic_sites <=
          summary.protocol_composition_semantic_sites &&
      summary.canonical_invalid_type_entries <= summary.canonical_type_entries &&
      summary.canonical_object_type_entries <= summary.canonical_type_entries &&
      summary.canonical_protocol_qualified_entries <=
          summary.canonical_type_entries &&
      summary.canonical_nullable_entries +
              summary.canonical_nonnull_entries +
              summary.canonical_implicitly_unwrapped_entries +
              summary.canonical_null_resettable_entries +
              summary.canonical_unspecified_nullability_entries ==
          summary.canonical_type_entries &&
      summary.optional_propagation_sites == summary.nil_coalescing_sites &&
      summary.guard_binding_exit_enforcement_sites <=
          summary.guard_binding_sites;
  summary.ready_for_lowering_and_runtime = summary.deterministic;
  std::ostringstream out;
  out << summary.contract_id << ";bindings=" << summary.optional_binding_sites
      << ";clauses=" << summary.optional_binding_clause_sites
      << ";guards=" << summary.guard_binding_sites
      << ";optional-sends=" << summary.optional_send_sites
      << ";coalescing=" << summary.nil_coalescing_sites
      << ";propagation=" << summary.optional_propagation_sites
      << ";refinement=" << summary.optional_flow_refinement_sites
      << ";guard-exit=" << summary.guard_binding_exit_enforcement_sites
      << ";keypaths=" << summary.typed_keypath_literal_sites
      << ";keypath-self=" << summary.typed_keypath_self_root_sites
      << ";keypath-class=" << summary.typed_keypath_class_root_sites
      << ";object-pointers=" << summary.object_pointer_semantic_sites
      << ";protocol-compositions="
      << summary.protocol_composition_semantic_sites
      << ";generic-suffixes=" << summary.generic_suffix_semantic_sites
      << ";generic-erasure=" << summary.generic_erasure_semantic_sites
      << ";nullability-suffixes="
      << summary.nullability_suffix_semantic_sites
      << ";nullability=" << summary.nullability_semantic_sites
      << ";canonical-types=" << summary.canonical_type_entries
      << ";canonical-object-types="
      << summary.canonical_object_type_entries
      << ";canonical-protocol-qualified="
      << summary.canonical_protocol_qualified_entries
      << ";canonical-generics="
      << summary.canonical_generic_argument_entries
      << ";canonical-nullable=" << summary.canonical_nullable_entries
      << ";canonical-nonnull=" << summary.canonical_nonnull_entries
      << ";canonical-iuo="
      << summary.canonical_implicitly_unwrapped_entries
      << ";canonical-null-resettable="
      << summary.canonical_null_resettable_entries
      << ";canonical-unspecified-nullability="
      << summary.canonical_unspecified_nullability_entries
      << ";canonical-invalid=" << summary.canonical_invalid_type_entries
      << ";invalid-generic-suffixes="
      << summary.invalid_generic_suffix_semantic_sites
      << ";invalid-nullability-suffixes="
      << summary.invalid_nullability_suffix_semantic_sites
      << ";invalid-protocol-compositions="
      << summary.invalid_protocol_composition_semantic_sites
      << ";binding-violations=" << summary.optional_binding_contract_violation_sites
      << ";optional-send-violations=" << summary.optional_send_contract_violation_sites
      << ";flow-violations=" << summary.optional_flow_contract_violation_sites
      << ";keypath-root-violations="
      << summary.typed_keypath_root_legality_violation_sites
      << ";keypath-member-violations="
      << summary.typed_keypath_member_path_contract_violation_sites
      << ";keypath-violations=" << summary.typed_keypath_contract_violation_sites;
  summary.replay_key = out.str();
  return summary;
}
