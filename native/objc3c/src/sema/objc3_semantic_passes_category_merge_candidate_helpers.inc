static bool IsRealizedClassOwner(const Objc3SemanticIntegrationSurface &surface,
                                 const std::string &class_name) {
  return surface.interfaces.find(class_name) != surface.interfaces.end() &&
         surface.implementations.find(class_name) !=
             surface.implementations.end();
}

static bool IsDeclaredClassOwner(const Objc3SemanticIntegrationSurface &surface,
                                 const std::string &class_name) {
  return surface.interfaces.find(class_name) != surface.interfaces.end();
}

static void MergeCategoryMethodCandidate(
    const Objc3SemanticIntegrationSurface &surface, const std::string &class_name,
    const Objc3CategoryMergeOrderEntry &entry, const std::string &selector,
    const Objc3MethodInfo &candidate, Objc3CategoryMergeInfo &merge_info,
    std::vector<std::string> &diagnostics) {
  const Objc3MethodInfo *base_interface = ResolveMethodInOwnerInterface(
      surface, class_name, selector, candidate.is_class_method);
  if (base_interface != nullptr &&
      !IsCompatibleMethodSignature(*base_interface, candidate)) {
    diagnostics.push_back(MakeDiag(
        entry.line, entry.column, "O3S219",
        "category merge failure: category '" + entry.container_name +
            "' selector '" +
            FormatMethodSelectorForDiagnostic(selector,
                                             candidate.is_class_method) +
            "' is incompatible with realized class '" + class_name + "'"));
    merge_info.deterministic = false;
    return;
  }

  const Objc3MethodInfo *base_implementation = ResolveMethodInOwnerImplementation(
      surface, class_name, selector, candidate.is_class_method);
  if (base_implementation != nullptr &&
      !IsCompatibleMethodSignature(*base_implementation, candidate)) {
    diagnostics.push_back(MakeDiag(
        entry.line, entry.column, "O3S219",
        "category merge failure: category '" + entry.container_name +
            "' selector '" +
            FormatMethodSelectorForDiagnostic(selector,
                                             candidate.is_class_method) +
            "' is incompatible with realized class '" + class_name + "'"));
    merge_info.deterministic = false;
    return;
  }

  const Objc3MethodInfo *existing = FindMethodInfoWithKind(
      merge_info.merged_methods, selector, candidate.is_class_method);
  if (existing != nullptr && !IsCompatibleMethodSignature(*existing, candidate)) {
    const auto owner_it = merge_info.merged_method_owner_identities.find(selector);
    const std::string previous_owner =
        owner_it == merge_info.merged_method_owner_identities.end()
            ? std::string("another attached category")
            : owner_it->second;
    diagnostics.push_back(MakeDiag(
        entry.line, entry.column, "O3S219",
        "category merge failure: category '" + entry.container_name +
            "' selector '" +
            FormatMethodSelectorForDiagnostic(selector,
                                             candidate.is_class_method) +
            "' conflicts with attached category '" + previous_owner + "'"));
    merge_info.deterministic = false;
    return;
  }

  merge_info.merged_methods[selector] = candidate;
  merge_info.merged_method_owner_identities[selector] = entry.container_name;
}

static void MergeCategoryPropertyCandidate(
    const Objc3SemanticIntegrationSurface &surface, const std::string &class_name,
    const Objc3CategoryMergeOrderEntry &entry,
    const std::string &property_name, const Objc3PropertyInfo &candidate,
    Objc3CategoryMergeInfo &merge_info, std::vector<std::string> &diagnostics) {
  const Objc3PropertyInfo *base_interface =
      ResolvePropertyInOwnerInterface(surface, class_name, property_name);
  if (base_interface != nullptr &&
      !IsCompatiblePropertySignature(*base_interface, candidate)) {
    diagnostics.push_back(MakeDiag(
        entry.line, entry.column, "O3S219",
        "category merge failure: category '" + entry.container_name +
            "' property '" + property_name +
            "' is incompatible with realized class '" + class_name + "'"));
    merge_info.deterministic = false;
    return;
  }

  const Objc3PropertyInfo *base_implementation =
      ResolvePropertyInOwnerImplementation(surface, class_name, property_name);
  if (base_implementation != nullptr &&
      !IsCompatiblePropertySignature(*base_implementation, candidate)) {
    diagnostics.push_back(MakeDiag(
        entry.line, entry.column, "O3S219",
        "category merge failure: category '" + entry.container_name +
            "' property '" + property_name +
            "' is incompatible with realized class '" + class_name + "'"));
    merge_info.deterministic = false;
    return;
  }

  const auto existing_it = merge_info.merged_properties.find(property_name);
  if (existing_it != merge_info.merged_properties.end() &&
      !IsCompatiblePropertySignature(existing_it->second, candidate)) {
    const auto owner_it =
        merge_info.merged_property_owner_identities.find(property_name);
    const std::string previous_owner =
        owner_it == merge_info.merged_property_owner_identities.end()
            ? std::string("another attached category")
            : owner_it->second;
    diagnostics.push_back(MakeDiag(
        entry.line, entry.column, "O3S219",
        "category merge failure: category '" + entry.container_name +
            "' property '" + property_name +
            "' conflicts with attached category '" + previous_owner + "'"));
    merge_info.deterministic = false;
    return;
  }

  merge_info.merged_properties[property_name] = candidate;
  merge_info.merged_property_owner_identities[property_name] =
      entry.container_name;
}
