static const Objc3PropertyInfo *FindTypedKeyPathPropertyOnOwner(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &property_name) {
  auto merge_it = surface.category_merge_surfaces.find(owner_name);
  if (merge_it != surface.category_merge_surfaces.end()) {
    auto property_it = merge_it->second.merged_properties.find(property_name);
    if (property_it != merge_it->second.merged_properties.end()) {
      return &property_it->second;
    }
  }
  auto interface_it = surface.interfaces.find(owner_name);
  if (interface_it != surface.interfaces.end()) {
    auto property_it = interface_it->second.properties.find(property_name);
    if (property_it != interface_it->second.properties.end()) {
      return &property_it->second;
    }
  }
  auto implementation_it = surface.implementations.find(owner_name);
  if (implementation_it != surface.implementations.end()) {
    auto property_it = implementation_it->second.properties.find(property_name);
    if (property_it != implementation_it->second.properties.end()) {
      return &property_it->second;
    }
  }
  return nullptr;
}

static bool TypedKeyPathOwnerHasDeterministicCategorySurface(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name) {
  auto merge_it = surface.category_merge_surfaces.find(owner_name);
  return merge_it == surface.category_merge_surfaces.end() ||
         merge_it->second.deterministic;
}

static std::string TypedKeyPathPropertyObjectOwnerName(
    const Objc3PropertyInfo &property) {
  if (property.has_invalid_type_suffix || property.has_invalid_generic_suffix ||
      property.has_generic_suffix) {
    return "";
  }
  if (!property.canonical_type.object_pointer_type_name.empty()) {
    return property.canonical_type.object_pointer_type_name;
  }
  return "";
}

static std::string TypedKeyPathPropertyTypeIdentity(
    const Objc3PropertyInfo &property) {
  if (!property.canonical_type.canonical_spelling.empty()) {
    return property.canonical_type.canonical_spelling;
  }
  if (!property.canonical_type.object_pointer_type_name.empty()) {
    return "objc-object:" + property.canonical_type.object_pointer_type_name;
  }
  if (!property.canonical_type.replay_key.empty()) {
    return property.canonical_type.replay_key;
  }
  return "value-type:" + std::to_string(static_cast<int>(property.type));
}

static bool ResolveTypedKeyPathPropertyChain(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &root_owner_name,
    const std::vector<std::string> &components,
    std::string &failure_component,
    std::string &failure_reason,
    std::vector<std::string> *component_owner_identities,
    std::vector<std::string> *component_member_identities,
    std::vector<std::string> *component_type_identities) {
  failure_component.clear();
  failure_reason.clear();
  if (component_owner_identities != nullptr) {
    component_owner_identities->clear();
  }
  if (component_member_identities != nullptr) {
    component_member_identities->clear();
  }
  if (component_type_identities != nullptr) {
    component_type_identities->clear();
  }
  if (root_owner_name.empty() || components.empty()) {
    failure_reason = "typed key-path lowering requires a concrete root owner";
    return false;
  }

  std::string current_owner = root_owner_name;
  for (std::size_t index = 0; index < components.size(); ++index) {
    const std::string &component = components[index];
    failure_component = component;
    if (!TypedKeyPathOwnerHasDeterministicCategorySurface(surface,
                                                         current_owner)) {
      failure_reason =
          "category-merged property metadata for owner '" + current_owner +
          "' is ambiguous";
      return false;
    }
    const Objc3PropertyInfo *property =
        FindTypedKeyPathPropertyOnOwner(surface, current_owner, component);
    if (property == nullptr) {
      failure_reason = "component is not a readable property on owner '" +
                       current_owner + "'";
      return false;
    }
    if (property->has_generic_suffix || property->has_invalid_type_suffix ||
        property->has_invalid_generic_suffix) {
      failure_reason =
          "generic or invalid typed key-path components are unsupported";
      return false;
    }
    if (component_owner_identities != nullptr) {
      component_owner_identities->push_back("class:" + current_owner);
    }
    if (component_member_identities != nullptr) {
      component_member_identities->push_back("class:" + current_owner + "." +
                                             component);
    }
    if (component_type_identities != nullptr) {
      component_type_identities->push_back(
          TypedKeyPathPropertyTypeIdentity(*property));
    }
    if (index + 1u == components.size()) {
      return true;
    }
    const std::string next_owner =
        TypedKeyPathPropertyObjectOwnerName(*property);
    if (next_owner.empty() ||
        surface.interfaces.find(next_owner) == surface.interfaces.end()) {
      failure_reason =
          "nested component requires a concrete object-property owner";
      return false;
    }
    current_owner = next_owner;
  }
  return true;
}

static bool ResolveTypedKeyPathClassRootOwner(
    const std::string &root_name,
    const Objc3MessageSendResolutionContext &context,
    std::string &owner_name) {
  if (context.surface == nullptr || root_name.empty()) {
    return false;
  }
  if (context.surface->interfaces.find(root_name) !=
      context.surface->interfaces.end()) {
    owner_name = root_name;
    return true;
  }
  if (context.surface->implementations.find(root_name) !=
      context.surface->implementations.end()) {
    owner_name = root_name;
    return true;
  }
  return false;
}

static bool TryResolveTypedKeyPathValidatedOwner(
    const Expr &expr,
    const std::vector<SemanticScope> &scopes,
    const std::unordered_map<std::string, ValueType> &globals,
    const std::unordered_map<std::string, FunctionInfo> &functions,
    const Objc3MessageSendResolutionContext &context,
    std::string &owner_name,
    bool &is_class_root) {
  owner_name.clear();
  is_class_root = false;
  if (expr.kind != Expr::Kind::KeyPathLiteral ||
      !expr.typed_keypath_literal_enabled) {
    return false;
  }
  if (expr.typed_keypath_root_is_self) {
    if (context.inside_method &&
        !context.current_implementation_name.empty()) {
      owner_name = context.current_implementation_name;
    }
    return true;
  }

  if (const SemanticTypeInfo *local_type =
          ScopeLookupTypeOrNull(scopes, expr.typed_keypath_root_name)) {
    if (!IsObjCReferenceSemanticType(*local_type)) {
      return false;
    }
    if (!local_type->canonical_type.object_pointer_type_name.empty() &&
        ResolveTypedKeyPathClassRootOwner(
            local_type->canonical_type.object_pointer_type_name, context,
            owner_name)) {
      return true;
    }
    return false;
  }

  auto global_it = globals.find(expr.typed_keypath_root_name);
  if (global_it != globals.end()) {
    if (global_it->second == ValueType::ObjCClass &&
        ResolveTypedKeyPathClassRootOwner(expr.typed_keypath_root_name, context,
                                          owner_name)) {
      is_class_root = true;
      return true;
    }
    return IsObjCReferenceSemanticType(
        MakeSemanticTypeFromGlobal(global_it->second));
  }

  if (functions.find(expr.typed_keypath_root_name) != functions.end()) {
    return false;
  }

  if (ResolveTypedKeyPathClassRootOwner(expr.typed_keypath_root_name, context,
                                        owner_name)) {
    is_class_root = true;
    return true;
  }

  return false;
}

static Objc3ResolvedMessageSendMethod ResolveConcreteMessageSendMethod(
    const Expr &expr,
    const Objc3MessageSendResolutionContext &context);
static Objc3ResolvedMessageSendMethod ResolveConcreteOwnerMethod(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &selector,
    bool is_class_method);
static Objc3ResolvedMessageSendPropertyAccessor
ResolveConcreteOwnerPropertyAccessor(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &selector,
    bool is_class_property);
static Expr::DispatchSurfaceKind ResolveNormalizedMessageSendDispatchSurfaceKind(
    const Expr &expr,
    const Objc3MessageSendResolutionContext &context);
