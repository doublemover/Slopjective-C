  void CountDraftSyntaxProperties(
      const std::vector<Objc3PropertyDecl> &properties,
      Objc3DraftSyntaxSurfaceSummary &summary) const {
    for (const auto &property : properties) {
      summary.property_attribute_sites += property.attributes.size();
      if (property.property_behavior_declared) {
        ++summary.property_behavior_sites;
      }
      if (property.has_getter) {
        ++summary.property_accessor_selector_sites;
      }
      if (property.has_setter) {
        ++summary.property_accessor_selector_sites;
      }
      if (!property.property_synthesis_symbol.empty()) {
        ++summary.property_synthesis_metadata_sites;
      }
      if (!property.ivar_binding_symbol.empty()) {
        ++summary.property_synthesis_metadata_sites;
      }
      if (!property.property_attribute_profile.empty()) {
        ++summary.property_reflection_input_sites;
      }
      if (!property.effective_getter_selector.empty()) {
        ++summary.property_reflection_input_sites;
      }
      if (property.effective_setter_available &&
          !property.effective_setter_selector.empty()) {
        ++summary.property_reflection_input_sites;
      }
      if (property.property_behavior_declared) {
        ++summary.property_reflection_input_sites;
      }
      summary.property_ownership_nullability_sites +=
          (property.is_copy ? 1u : 0u) + (property.is_retain ? 1u : 0u) +
          (property.is_strong ? 1u : 0u) + (property.is_weak ? 1u : 0u) +
          (property.is_unowned ? 1u : 0u) +
          (property.is_unsafe_unretained ? 1u : 0u) +
          (property.is_assign ? 1u : 0u) + (property.is_nullable ? 1u : 0u) +
          (property.is_nonnull ? 1u : 0u) +
          (property.is_null_resettable ? 1u : 0u);
    }
  }
