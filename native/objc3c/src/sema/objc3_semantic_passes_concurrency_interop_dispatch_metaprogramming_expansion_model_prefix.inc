Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary
BuildMetaprogrammingExpansionBehaviorSemanticModelSummary(
    const Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary
        &source_summary,
    const Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary
        &macro_summary,
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &property_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary summary;

  const auto count_diagnostic = [&diagnostics](std::string_view code,
                                               std::string_view needle) {
    return static_cast<std::size_t>(std::count_if(
        diagnostics.begin(), diagnostics.end(),
        [code, needle](const std::string &entry) {
          return entry.find(code) != std::string::npos &&
                 entry.find(needle) != std::string::npos;
        }));
  };

  summary.derive_marker_sites = source_summary.derive_marker_sites;
  summary.macro_marker_sites = source_summary.macro_marker_sites;
  summary.macro_package_sites = macro_summary.macro_package_sites;
  summary.macro_provenance_sites = macro_summary.macro_provenance_sites;
  summary.expansion_visible_macro_sites =
      macro_summary.expansion_visible_macro_sites;
  summary.property_behavior_sites = property_summary.property_behavior_sites;
  summary.interface_property_behavior_sites =
      property_summary.interface_property_behavior_sites;
  summary.implementation_property_behavior_sites =
      property_summary.implementation_property_behavior_sites;
  summary.protocol_property_behavior_sites =
      property_summary.protocol_property_behavior_sites;
  summary.synthesized_binding_visible_sites =
      property_summary.synthesized_binding_visible_sites;
  summary.synthesized_getter_visible_sites =
      property_summary.synthesized_getter_visible_sites;
  summary.synthesized_setter_visible_sites =
      property_summary.synthesized_setter_visible_sites;
  summary.property_behavior_contract_violation_sites =
      count_diagnostic("O3S206", "behavior attribute");

  summary.source_dependency_required = true;
  summary.derive_macro_source_supported =
      source_summary.derive_marker_source_supported &&
      source_summary.macro_marker_source_supported &&
      source_summary.property_behavior_source_supported &&
      source_summary.deterministic_handoff &&
      source_summary.ready_for_semantic_expansion;
  summary.macro_package_provenance_surface_reused =
      macro_summary.macro_package_source_supported &&
      macro_summary.macro_provenance_source_supported &&
      macro_summary.expansion_visible_source_supported &&
      macro_summary.deterministic_handoff &&
      macro_summary.ready_for_semantic_expansion &&
      summary.expansion_visible_macro_sites <= summary.macro_marker_sites &&
      summary.expansion_visible_macro_sites <= summary.macro_package_sites &&
      summary.expansion_visible_macro_sites <= summary.macro_provenance_sites;
  summary.property_behavior_source_supported =
      property_summary.property_behavior_source_supported &&
      property_summary.deterministic_handoff &&
      property_summary.ready_for_semantic_expansion;
  summary.synthesized_visibility_surface_reused =
      property_summary.synthesized_declaration_visibility_supported &&
      summary.synthesized_binding_visible_sites <=
          summary.synthesized_getter_visible_sites &&
      summary.synthesized_setter_visible_sites <=
          summary.synthesized_getter_visible_sites &&
      summary.interface_property_behavior_sites +
              summary.implementation_property_behavior_sites +
              summary.protocol_property_behavior_sites ==
          summary.property_behavior_sites &&
      summary.property_behavior_contract_violation_sites <=
          summary.property_behavior_sites;
  summary.derive_synthesis_deferred = true;
