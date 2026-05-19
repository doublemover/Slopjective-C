static const Objc3MethodInfo *FindMethodInfoWithKind(
    const std::unordered_map<std::string, Objc3MethodInfo> &methods,
    const std::string &selector,
    bool is_class_method) {
  const auto method_it = methods.find(selector);
  if (method_it == methods.end() || method_it->second.is_class_method != is_class_method) {
    return nullptr;
  }
  return &method_it->second;
}

static const Objc3MethodInfo *FindSurfaceMethodInSuperChain(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &interface_name,
    const std::string &selector,
    bool is_class_method,
    bool &missing_base,
    bool &cycle_detected) {
  missing_base = false;
  cycle_detected = false;
  const auto interface_it = surface.interfaces.find(interface_name);
  if (interface_it == surface.interfaces.end()) {
    missing_base = true;
    return nullptr;
  }

  std::string next_super = interface_it->second.super_name;
  std::unordered_set<std::string> visited;
  while (!next_super.empty()) {
    if (!visited.insert(next_super).second) {
      cycle_detected = true;
      return nullptr;
    }
    const auto super_it = surface.interfaces.find(next_super);
    if (super_it == surface.interfaces.end()) {
      missing_base = true;
      return nullptr;
    }
    const Objc3MethodInfo *method =
        FindMethodInfoWithKind(super_it->second.methods, selector, is_class_method);
    if (method != nullptr) {
      return method;
    }
    next_super = super_it->second.super_name;
  }
  return nullptr;
}

static const Objc3MethodInfo *ResolveMethodInOwnerInterface(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &selector,
    bool is_class_method) {
  const auto interface_it = surface.interfaces.find(owner_name);
  if (interface_it == surface.interfaces.end()) {
    return nullptr;
  }
  return FindMethodInfoWithKind(interface_it->second.methods, selector,
                                is_class_method);
}

static const Objc3MethodInfo *ResolveMethodInOwnerImplementation(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &selector,
    bool is_class_method) {
  const auto implementation_it = surface.implementations.find(owner_name);
  if (implementation_it == surface.implementations.end()) {
    return nullptr;
  }
  return FindMethodInfoWithKind(implementation_it->second.methods, selector,
                                is_class_method);
}

static const Objc3MethodInfo *ResolveMethodInCategoryInterface(
    const std::unordered_map<std::string, Objc3InterfaceInfo> &category_interfaces,
    const std::string &owner_identity,
    const std::string &selector,
    bool is_class_method) {
  const auto interface_it = category_interfaces.find(owner_identity);
  if (interface_it == category_interfaces.end()) {
    return nullptr;
  }
  return FindMethodInfoWithKind(interface_it->second.methods, selector,
                                is_class_method);
}

static const Objc3MethodInfo *ResolveMethodInCategoryImplementation(
    const std::unordered_map<std::string, Objc3ImplementationInfo>
        &category_implementations,
    const std::string &owner_identity,
    const std::string &selector,
    bool is_class_method) {
  const auto implementation_it = category_implementations.find(owner_identity);
  if (implementation_it == category_implementations.end()) {
    return nullptr;
  }
  return FindMethodInfoWithKind(implementation_it->second.methods, selector,
                                is_class_method);
}

static const Objc3MethodInfo *ResolveMethodInMergedCategorySurface(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &selector,
    bool is_class_method) {
  const auto merge_it = surface.category_merge_surfaces.find(owner_name);
  if (merge_it == surface.category_merge_surfaces.end()) {
    return nullptr;
  }
  return FindMethodInfoWithKind(merge_it->second.merged_methods, selector,
                                is_class_method);
}
