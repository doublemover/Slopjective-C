                                is_class_method);
}

static const Objc3PropertyInfo *ResolvePropertyInOwnerInterface(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &property_name) {
  const auto interface_it = surface.interfaces.find(owner_name);
  if (interface_it == surface.interfaces.end()) {
    return nullptr;
  }
  const auto property_it = interface_it->second.properties.find(property_name);
  if (property_it == interface_it->second.properties.end()) {
    return nullptr;
  }
  return &property_it->second;
}

static const Objc3PropertyInfo *ResolvePropertyInOwnerImplementation(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &property_name) {
  const auto implementation_it = surface.implementations.find(owner_name);
  if (implementation_it == surface.implementations.end()) {
    return nullptr;
  }
  const auto property_it =
      implementation_it->second.properties.find(property_name);
  if (property_it == implementation_it->second.properties.end()) {
    return nullptr;
  }
  return &property_it->second;
}

static const Objc3PropertyInfo *ResolvePropertyInCategoryInterface(
    const std::unordered_map<std::string, Objc3InterfaceInfo>
        &category_interfaces,
    const std::string &owner_identity,
    const std::string &property_name) {
  const auto interface_it = category_interfaces.find(owner_identity);
  if (interface_it == category_interfaces.end()) {
    return nullptr;
  }
  const auto property_it = interface_it->second.properties.find(property_name);
  if (property_it == interface_it->second.properties.end()) {
    return nullptr;
  }
  return &property_it->second;
}

static const Objc3PropertyInfo *ResolvePropertyInCategoryImplementation(
    const std::unordered_map<std::string, Objc3ImplementationInfo>
        &category_implementations,
    const std::string &owner_identity,
    const std::string &property_name) {
  const auto implementation_it = category_implementations.find(owner_identity);
  if (implementation_it == category_implementations.end()) {
    return nullptr;
  }
  const auto property_it =
      implementation_it->second.properties.find(property_name);
  if (property_it == implementation_it->second.properties.end()) {
    return nullptr;
  }
  return &property_it->second;
}

static const Objc3PropertyInfo *ResolvePropertyInMergedCategorySurface(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &property_name) {
  const auto merge_it = surface.category_merge_surfaces.find(owner_name);
  if (merge_it == surface.category_merge_surfaces.end()) {
    return nullptr;
  }
  const auto property_it =
      merge_it->second.merged_properties.find(property_name);
  if (property_it == merge_it->second.merged_properties.end()) {
    return nullptr;
  }
  return &property_it->second;
}

static const Objc3PropertyInfo *FindSurfacePropertyInSuperChain(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &interface_name,
    const std::string &property_name,
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
    const auto property_it = super_it->second.properties.find(property_name);
    if (property_it != super_it->second.properties.end()) {
      return &property_it->second;
    }
    next_super = super_it->second.super_name;
  }
  return nullptr;
}

struct Objc3InheritedMethodLookup {
  const Objc3MethodInfo *method = nullptr;
  std::string owner_name;
  bool missing_base = false;
  bool cycle_detected = false;
};

static Objc3InheritedMethodLookup FindSurfaceMethodInSuperChainDetailed(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &interface_name,
    const std::string &selector,
    bool match_method_kind,
    bool is_class_method) {
  Objc3InheritedMethodLookup lookup;
  const auto interface_it = surface.interfaces.find(interface_name);
  if (interface_it == surface.interfaces.end()) {
    lookup.missing_base = true;
    return lookup;
  }

  std::string next_super = interface_it->second.super_name;
  std::unordered_set<std::string> visited;
  while (!next_super.empty()) {
    if (!visited.insert(next_super).second) {
      lookup.cycle_detected = true;
      return lookup;
    }
    const auto super_it = surface.interfaces.find(next_super);
    if (super_it == surface.interfaces.end()) {
      lookup.missing_base = true;
      lookup.owner_name = next_super;
      return lookup;
    }
    const auto method_it = super_it->second.methods.find(selector);
    if (method_it != super_it->second.methods.end() &&
        (!match_method_kind ||
         method_it->second.is_class_method == is_class_method)) {
      lookup.method = &method_it->second;
      lookup.owner_name = next_super;
      return lookup;
    }
    next_super = super_it->second.super_name;
  }
  return lookup;
}

struct Objc3InheritedPropertyLookup {
  const Objc3PropertyInfo *property = nullptr;
  std::string owner_name;
  bool missing_base = false;
  bool cycle_detected = false;
};

static Objc3InheritedPropertyLookup FindSurfacePropertyInSuperChainDetailed(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &interface_name,
    const std::string &property_name) {
  Objc3InheritedPropertyLookup lookup;
  const auto interface_it = surface.interfaces.find(interface_name);
  if (interface_it == surface.interfaces.end()) {
    lookup.missing_base = true;
    return lookup;
  }

  std::string next_super = interface_it->second.super_name;
  std::unordered_set<std::string> visited;
  while (!next_super.empty()) {
    if (!visited.insert(next_super).second) {
      lookup.cycle_detected = true;
      return lookup;
    }
    const auto super_it = surface.interfaces.find(next_super);
    if (super_it == surface.interfaces.end()) {
      lookup.missing_base = true;
      lookup.owner_name = next_super;
      return lookup;
    }
    const auto property_it = super_it->second.properties.find(property_name);
    if (property_it != super_it->second.properties.end()) {
      lookup.property = &property_it->second;
      lookup.owner_name = next_super;
      return lookup;
    }
    next_super = super_it->second.super_name;
  }
  return lookup;
}

struct Objc3SuperclassRealizationClosure {
  bool missing_super_interface = false;
  bool missing_super_implementation = false;
  bool cycle_detected = false;
  std::string failing_super_name;
};

static Objc3SuperclassRealizationClosure ResolveSuperclassRealizationClosure(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &interface_name) {
  Objc3SuperclassRealizationClosure closure;
  const auto interface_it = surface.interfaces.find(interface_name);
  if (interface_it == surface.interfaces.end()) {
    return closure;
  }

  std::string next_super = interface_it->second.super_name;
  std::unordered_set<std::string> visited;
  while (!next_super.empty()) {
    if (!visited.insert(next_super).second) {
      closure.cycle_detected = true;
      closure.failing_super_name = next_super;
      return closure;
    }
    const auto super_interface_it = surface.interfaces.find(next_super);
    if (super_interface_it == surface.interfaces.end()) {
      closure.missing_super_interface = true;
      closure.failing_super_name = next_super;
      return closure;
    }
    if (surface.implementations.find(next_super) == surface.implementations.end()) {
      closure.missing_super_implementation = true;
      closure.failing_super_name = next_super;
      return closure;
    }
    next_super = super_interface_it->second.super_name;
  }
  return closure;
}
