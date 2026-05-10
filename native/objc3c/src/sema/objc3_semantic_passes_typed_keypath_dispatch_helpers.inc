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
  if (expr.kind != Expr::Kind::Identifier ||
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

  const SemanticTypeInfo local_type =
      ScopeLookupType(scopes, expr.typed_keypath_root_name);
  if (!IsUnknownSemanticType(local_type)) {
    return IsObjCReferenceSemanticType(local_type);
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
