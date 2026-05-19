static const Objc3MethodInfo *ResolveDeclaredConformanceMethod(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &selector,
    bool is_class_method) {
  const Objc3MethodInfo *method =
      ResolveMethodInOwnerInterface(surface, owner_name, selector, is_class_method);
  if (method != nullptr) {
    return method;
  }
  method =
      ResolveMethodInOwnerImplementation(surface, owner_name, selector, is_class_method);
  if (method != nullptr) {
    return method;
  }
  method = ResolveMethodInMergedCategorySurface(surface, owner_name, selector,
                                                is_class_method);
  if (method != nullptr) {
    return method;
  }
  bool missing_base = false;
  bool cycle_detected = false;
  return FindSurfaceMethodInSuperChain(surface, owner_name, selector,
                                       is_class_method, missing_base,
                                       cycle_detected);
}

static const Objc3MethodInfo *ResolveCategoryConformanceMethod(
    const Objc3SemanticIntegrationSurface &surface,
    const std::unordered_map<std::string, Objc3InterfaceInfo> &category_interfaces,
    const std::unordered_map<std::string, Objc3ImplementationInfo>
        &category_implementations,
    const std::string &class_name,
    const std::string &category_name,
    const std::string &selector,
    bool is_class_method) {
  const std::string category_owner_identity =
      BuildCategoryOwnerIdentity(class_name, category_name);
  const Objc3MethodInfo *method = ResolveMethodInCategoryInterface(
      category_interfaces, category_owner_identity, selector, is_class_method);
  if (method != nullptr) {
    return method;
  }
  method = ResolveMethodInCategoryImplementation(category_implementations,
                                                 category_owner_identity,
                                                 selector, is_class_method);
  if (method != nullptr) {
    return method;
  }
  method = ResolveMethodInOwnerInterface(surface, class_name, selector,
                                         is_class_method);
  if (method != nullptr) {
    return method;
  }
  method = ResolveMethodInOwnerImplementation(surface, class_name, selector,
                                              is_class_method);
  if (method != nullptr) {
    return method;
  }
  method = ResolveMethodInMergedCategorySurface(surface, class_name, selector,
                                                is_class_method);
  if (method != nullptr) {
    return method;
  }
  bool missing_base = false;
  bool cycle_detected = false;
  return FindSurfaceMethodInSuperChain(surface, class_name, selector,
                                       is_class_method, missing_base,
                                       cycle_detected);
}

static const Objc3PropertyInfo *ResolveDeclaredConformanceProperty(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &property_name) {
  const Objc3PropertyInfo *property =
      ResolvePropertyInOwnerInterface(surface, owner_name, property_name);
  if (property != nullptr) {
    return property;
  }
  property =
      ResolvePropertyInOwnerImplementation(surface, owner_name, property_name);
  if (property != nullptr) {
    return property;
  }
  property =
      ResolvePropertyInMergedCategorySurface(surface, owner_name, property_name);
  if (property != nullptr) {
    return property;
  }
  bool missing_base = false;
  bool cycle_detected = false;
  return FindSurfacePropertyInSuperChain(surface, owner_name, property_name,
                                         missing_base, cycle_detected);
}

static Objc3ResolvedMessageSendPropertyAccessor
ResolveConcreteOwnerPropertyAccessor(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &selector,
    bool is_class_property) {
  Objc3ResolvedMessageSendPropertyAccessor resolution;
  auto consider = [&](const Objc3PropertyInfo *property) {
    if (property == nullptr || property->is_class != is_class_property) {
      return false;
    }
    if (property->effective_getter_selector == selector) {
      resolution.property = property;
      resolution.is_setter = false;
      return true;
    }
    if (property->effective_setter_available &&
        property->effective_setter_selector == selector) {
      resolution.property = property;
      resolution.is_setter = true;
      return true;
    }
    return false;
  };

  const auto interface_it = surface.interfaces.find(owner_name);
  if (interface_it != surface.interfaces.end()) {
    for (const auto &entry : interface_it->second.properties) {
      if (consider(&entry.second)) {
        return resolution;
      }
    }
  }
  const auto implementation_it = surface.implementations.find(owner_name);
  if (implementation_it != surface.implementations.end()) {
    for (const auto &entry : implementation_it->second.properties) {
      if (consider(&entry.second)) {
        return resolution;
      }
    }
  }
  const auto merge_it = surface.category_merge_surfaces.find(owner_name);
  if (merge_it != surface.category_merge_surfaces.end()) {
    for (const auto &entry : merge_it->second.merged_properties) {
      if (consider(&entry.second)) {
        return resolution;
      }
    }
  }
  const auto owner_it = surface.interfaces.find(owner_name);
  if (owner_it == surface.interfaces.end()) {
    return resolution;
  }
  std::string next_super = owner_it->second.super_name;
  std::unordered_set<std::string> visited;
  while (!next_super.empty() && visited.insert(next_super).second) {
    const auto super_it = surface.interfaces.find(next_super);
    if (super_it == surface.interfaces.end()) {
      return resolution;
    }
    for (const auto &entry : super_it->second.properties) {
      if (consider(&entry.second)) {
        return resolution;
      }
    }
    next_super = super_it->second.super_name;
  }
  return resolution;
}

static const Objc3PropertyInfo *ResolveCategoryConformanceProperty(
    const Objc3SemanticIntegrationSurface &surface,
    const std::unordered_map<std::string, Objc3InterfaceInfo> &category_interfaces,
    const std::unordered_map<std::string, Objc3ImplementationInfo>
        &category_implementations,
    const std::string &class_name,
    const std::string &category_name,
    const std::string &property_name) {
  const std::string category_owner_identity =
      BuildCategoryOwnerIdentity(class_name, category_name);
  const Objc3PropertyInfo *property = ResolvePropertyInCategoryInterface(
      category_interfaces, category_owner_identity, property_name);
  if (property != nullptr) {
    return property;
  }
  property = ResolvePropertyInCategoryImplementation(category_implementations,
                                                     category_owner_identity,
                                                     property_name);
  if (property != nullptr) {
    return property;
  }
  property = ResolvePropertyInOwnerInterface(surface, class_name, property_name);
  if (property != nullptr) {
    return property;
  }
  property =
      ResolvePropertyInOwnerImplementation(surface, class_name, property_name);
  if (property != nullptr) {
    return property;
  }
  property = ResolvePropertyInMergedCategorySurface(surface, class_name,
                                                    property_name);
  if (property != nullptr) {
    return property;
  }
  bool missing_base = false;
  bool cycle_detected = false;
  return FindSurfacePropertyInSuperChain(surface, class_name, property_name,
                                         missing_base, cycle_detected);
}
