static void AccumulatePropertyAttributeSummaryFromPropertyInfo(const Objc3PropertyInfo &property,
                                                               Objc3PropertyAttributeSummary &summary) {
  ++summary.properties_total;
  summary.attribute_entries += property.attribute_entries;
  if (property.is_readonly) {
    ++summary.readonly_modifiers;
  }
  if (property.is_readwrite) {
    ++summary.readwrite_modifiers;
  }
  if (property.is_atomic) {
    ++summary.atomic_modifiers;
  }
  if (property.is_nonatomic) {
    ++summary.nonatomic_modifiers;
  }
  if (property.is_copy) {
    ++summary.copy_modifiers;
  }
  if (property.is_strong || property.is_retain) {
    ++summary.strong_modifiers;
  }
  if (property.is_weak) {
    ++summary.weak_modifiers;
  }
  if (property.is_assign || property.is_unsafe_unretained) {
    ++summary.assign_modifiers;
  }
  if (property.has_getter) {
    ++summary.getter_modifiers;
  }
  if (property.has_setter) {
    ++summary.setter_modifiers;
  }
  summary.invalid_attribute_entries += property.invalid_attribute_entries;
  summary.property_contract_violations += property.property_contract_violations;

  if (property.attribute_entries != property.attribute_names_lexicographic.size() ||
      !std::is_sorted(property.attribute_names_lexicographic.begin(), property.attribute_names_lexicographic.end())) {
    summary.deterministic = false;
  }
  if (property.has_readwrite_conflict != (property.is_readonly && property.is_readwrite)) {
    summary.deterministic = false;
  }
  if (property.has_atomicity_conflict != (property.is_atomic && property.is_nonatomic)) {
    summary.deterministic = false;
  }
  const std::size_t ownership_modifiers =
      (property.is_copy ? 1u : 0u) +
      ((property.is_strong || property.is_retain) ? 1u : 0u) +
      (property.is_weak ? 1u : 0u) +
      (property.is_unowned ? 1u : 0u) +
      ((property.is_assign || property.is_unsafe_unretained) ? 1u : 0u);
  if (property.has_ownership_conflict != (ownership_modifiers > 1u)) {
    summary.deterministic = false;
  }
  if (property.has_weak_unowned_conflict !=
      (property.is_weak && (property.is_unowned || property.is_unsafe_unretained))) {
    summary.deterministic = false;
  }
  if (property.has_setter && property.setter_selector.empty()) {
    summary.deterministic = false;
  }
  if (property.has_getter && property.getter_selector.empty()) {
    summary.deterministic = false;
  }
  const bool expected_invalid_contract =
      property.has_unknown_attribute || property.has_duplicate_attribute || property.has_readwrite_conflict ||
      property.has_atomicity_conflict || property.has_ownership_conflict ||
      property.has_weak_unowned_conflict ||
      property.has_accessor_selector_contract_violation || property.invalid_attribute_entries > 0 ||
      property.property_contract_violations > 0;
  if (property.has_invalid_attribute_contract != expected_invalid_contract) {
    summary.deterministic = false;
  }
}

static Objc3PropertyAttributeSummary BuildPropertyAttributeSummaryFromSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3PropertyAttributeSummary summary;
  for (const auto &interface_entry : surface.interfaces) {
    for (const auto &property_entry : interface_entry.second.properties) {
      AccumulatePropertyAttributeSummaryFromPropertyInfo(property_entry.second, summary);
    }
  }
  for (const auto &implementation_entry : surface.implementations) {
    for (const auto &property_entry : implementation_entry.second.properties) {
      AccumulatePropertyAttributeSummaryFromPropertyInfo(property_entry.second, summary);
    }
  }

  summary.deterministic = summary.deterministic &&
                          summary.invalid_attribute_entries <= summary.attribute_entries &&
                          summary.getter_modifiers <= summary.properties_total &&
                          summary.setter_modifiers <= summary.properties_total;
  return summary;
}
