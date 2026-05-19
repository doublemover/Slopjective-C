static Objc3PropertySynthesisIvarBindingSummary BuildPropertySynthesisIvarBindingSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3PropertySynthesisIvarBindingSummary summary;
  for (const auto &implementation_entry : surface.implementations) {
    const auto interface_it = surface.interfaces.find(implementation_entry.first);
    if (interface_it == surface.interfaces.end()) {
      continue;
    }

    for (const auto &interface_property_entry : interface_it->second.properties) {
      ++summary.property_synthesis_sites;
      ++summary.property_synthesis_default_ivar_bindings;
      ++summary.interface_owned_property_synthesis_sites;
      ++summary.ivar_binding_sites;
      const Objc3PropertyInfo &interface_property = interface_property_entry.second;
      const Objc3PropertyInfo *implementation_property =
          FindPropertyInImplementationInfo(implementation_entry.second,
                                           interface_property_entry.first);
      if (implementation_property != nullptr) {
        ++summary.implementation_property_redeclaration_sites;
      }

      if (!HasResolvedDefaultIvarBinding(interface_property)) {
        ++summary.ivar_binding_missing;
        continue;
      }

      if (implementation_property != nullptr &&
          (!IsCompatiblePropertySignature(interface_property,
                                         *implementation_property) ||
           !HasResolvedDefaultIvarBinding(*implementation_property) ||
           !HasCompatibleResolvedDefaultIvarBinding(interface_property,
                                                   *implementation_property))) {
        ++summary.ivar_binding_conflicts;
      } else {
        ++summary.ivar_binding_resolved;
      }
    }
  }

  summary.deterministic =
      summary.property_synthesis_explicit_ivar_bindings <= summary.property_synthesis_sites &&
      summary.property_synthesis_default_ivar_bindings <= summary.property_synthesis_sites &&
      summary.property_synthesis_explicit_ivar_bindings +
              summary.property_synthesis_default_ivar_bindings ==
          summary.property_synthesis_sites &&
      summary.interface_owned_property_synthesis_sites ==
          summary.property_synthesis_sites &&
      summary.implementation_property_redeclaration_sites <=
          summary.property_synthesis_sites &&
      summary.ivar_binding_sites == summary.property_synthesis_sites &&
      summary.ivar_binding_resolved <= summary.ivar_binding_sites &&
      summary.ivar_binding_missing <= summary.ivar_binding_sites &&
      summary.ivar_binding_conflicts <= summary.ivar_binding_sites &&
      summary.ivar_binding_resolved + summary.ivar_binding_missing + summary.ivar_binding_conflicts ==
          summary.ivar_binding_sites;
  return summary;
}

// export-legality anchor: class implementation property synthesis
// plus ivar-binding preconditions remain the canonical sema summary that later
// runtime metadata export milestones consume without source-shape revalidation.
// binary-boundary anchor: lane-D packages this exact sema-owned
// property/ivar summary into the runtime metadata envelope so later binary
// consumers stay aligned with lowering admission and replay keys.
// semantic-closure gate anchor: lane-E treats this sema-owned
// property/ivar summary as one required section-emission prerequisite beside
// A003 graph closure, C003 replay projection, and D002 packaging.
// corpus-sync anchor: the integrated corpus gate replays this
// property/ivar summary directly from manifest output on the real runner path.
static Objc3PropertySynthesisIvarBindingSummary BuildPropertySynthesisIvarBindingSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3PropertySynthesisIvarBindingSummary summary;
  std::unordered_map<std::string, const Objc3SemanticInterfaceTypeMetadata *> interfaces_by_name;
  interfaces_by_name.reserve(handoff.interfaces_lexicographic.size());
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    interfaces_by_name.emplace(interface_metadata.name, &interface_metadata);
  }

  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    const auto interface_it = interfaces_by_name.find(implementation_metadata.name);
    if (interface_it == interfaces_by_name.end()) {
      continue;
    }

    for (const auto &interface_property : interface_it->second->properties_lexicographic) {
      ++summary.property_synthesis_sites;
      ++summary.property_synthesis_default_ivar_bindings;
      ++summary.interface_owned_property_synthesis_sites;
      ++summary.ivar_binding_sites;
      const Objc3SemanticPropertyTypeMetadata *implementation_property =
          FindPropertyInImplementationMetadata(implementation_metadata,
                                              interface_property.name);
      if (implementation_property != nullptr) {
        ++summary.implementation_property_redeclaration_sites;
      }

      if (!HasResolvedDefaultIvarBinding(interface_property)) {
        ++summary.ivar_binding_missing;
        continue;
      }

      if (implementation_property != nullptr &&
          (!IsCompatiblePropertyTypeMetadataSignature(interface_property,
                                                     *implementation_property) ||
           !HasResolvedDefaultIvarBinding(*implementation_property) ||
           !HasCompatibleResolvedDefaultIvarBinding(interface_property,
                                                   *implementation_property))) {
        ++summary.ivar_binding_conflicts;
      } else {
        ++summary.ivar_binding_resolved;
      }
    }
  }

  summary.deterministic =
      summary.property_synthesis_explicit_ivar_bindings <= summary.property_synthesis_sites &&
      summary.property_synthesis_default_ivar_bindings <= summary.property_synthesis_sites &&
      summary.property_synthesis_explicit_ivar_bindings +
              summary.property_synthesis_default_ivar_bindings ==
          summary.property_synthesis_sites &&
      summary.interface_owned_property_synthesis_sites ==
          summary.property_synthesis_sites &&
      summary.implementation_property_redeclaration_sites <=
          summary.property_synthesis_sites &&
      summary.ivar_binding_sites == summary.property_synthesis_sites &&
      summary.ivar_binding_resolved <= summary.ivar_binding_sites &&
      summary.ivar_binding_missing <= summary.ivar_binding_sites &&
      summary.ivar_binding_conflicts <= summary.ivar_binding_sites &&
      summary.ivar_binding_resolved + summary.ivar_binding_missing + summary.ivar_binding_conflicts ==
          summary.ivar_binding_sites;
  return summary;
}
