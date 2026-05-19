static SemanticTypeInfo MakeSemanticTypeFromParam(const FuncParam &param) {
  if (param.vector_spelling) {
    return MakeVectorSemanticType(param.type, param.vector_base_spelling, param.vector_lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(param.type);
  info.canonical_type = BuildCanonicalSemanticTypeFromParam(param);
  if (param.object_pointer_type_spelling) {
    info.object_pointer_type_name = param.object_pointer_type_name;
  }
  if (IsObjCReferenceAliasValueType(param.type)) {
    if (param.ownership_is_weak_reference) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (param.ownership_is_unowned_reference) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix = !param.nullability_suffix_tokens.empty();
  }
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromFunctionReturn(const FunctionDecl &fn) {
  if (fn.return_vector_spelling) {
    return MakeVectorSemanticType(fn.return_type, fn.return_vector_base_spelling, fn.return_vector_lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(fn.return_type);
  info.canonical_type = BuildCanonicalSemanticTypeFromFunctionReturn(fn);
  if (fn.return_object_pointer_type_spelling) {
    info.object_pointer_type_name = fn.return_object_pointer_type_name;
  }
  if (IsObjCReferenceAliasValueType(fn.return_type)) {
    if (fn.return_ownership_is_weak_reference) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (fn.return_ownership_is_unowned_reference) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix = !fn.return_nullability_suffix_tokens.empty();
  }
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromMethodReturn(const Objc3MethodDecl &method) {
  if (method.return_vector_spelling) {
    return MakeVectorSemanticType(method.return_type, method.return_vector_base_spelling, method.return_vector_lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(method.return_type);
  info.canonical_type = BuildCanonicalSemanticTypeFromMethodReturn(method);
  if (method.return_object_pointer_type_spelling) {
    info.object_pointer_type_name = method.return_object_pointer_type_name;
  }
  if (IsObjCReferenceAliasValueType(method.return_type)) {
    if (method.return_ownership_is_weak_reference) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (method.return_ownership_is_unowned_reference) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix = !method.return_nullability_suffix_tokens.empty();
  }
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromFunctionInfoParam(const FunctionInfo &fn, std::size_t index) {
  if (index >= fn.param_types.size()) {
    return MakeScalarSemanticType(ValueType::Unknown);
  }

  if (index < fn.param_is_vector.size() && fn.param_is_vector[index]) {
    const std::string base_spelling =
        index < fn.param_vector_base_spelling.size() ? fn.param_vector_base_spelling[index] : "";
    const unsigned lane_count = index < fn.param_vector_lane_count.size() ? fn.param_vector_lane_count[index] : 1u;
    return MakeVectorSemanticType(fn.param_types[index], base_spelling, lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(fn.param_types[index]);
  if (index < fn.param_canonical_types.size()) {
    info.canonical_type = fn.param_canonical_types[index];
  }
  if (IsObjCReferenceAliasValueType(fn.param_types[index])) {
    if (index < fn.param_ownership_is_weak_reference.size() &&
        fn.param_ownership_is_weak_reference[index]) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (index < fn.param_ownership_is_unowned_reference.size() &&
               fn.param_ownership_is_unowned_reference[index]) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix =
        index < fn.param_has_nullability_suffix.size() &&
        fn.param_has_nullability_suffix[index];
  }
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromFunctionInfoReturn(const FunctionInfo &fn) {
  if (fn.return_is_vector) {
    return MakeVectorSemanticType(fn.return_type, fn.return_vector_base_spelling, fn.return_vector_lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(fn.return_type);
  info.canonical_type = fn.return_canonical_type;
  if (IsObjCReferenceAliasValueType(fn.return_type)) {
    if (fn.return_ownership_is_weak_reference) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (fn.return_ownership_is_unowned_reference) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix = fn.return_has_nullability_suffix;
  }
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromMethodInfoParam(const Objc3MethodInfo &method,
                                                            std::size_t index) {
  if (index >= method.param_types.size()) {
    return MakeScalarSemanticType(ValueType::Unknown);
  }
  if (index < method.param_is_vector.size() && method.param_is_vector[index]) {
    const std::string base_spelling =
        index < method.param_vector_base_spelling.size()
            ? method.param_vector_base_spelling[index]
            : "";
    const unsigned lane_count =
        index < method.param_vector_lane_count.size()
            ? method.param_vector_lane_count[index]
            : 1u;
    return MakeVectorSemanticType(method.param_types[index], base_spelling,
                                  lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(method.param_types[index]);
  if (index < method.param_canonical_types.size()) {
    info.canonical_type = method.param_canonical_types[index];
  }
  if (IsObjCReferenceAliasValueType(method.param_types[index])) {
    if (index < method.param_ownership_is_weak_reference.size() &&
        method.param_ownership_is_weak_reference[index]) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (index < method.param_ownership_is_unowned_reference.size() &&
               method.param_ownership_is_unowned_reference[index]) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix =
        index < method.param_has_nullability_suffix.size() &&
        method.param_has_nullability_suffix[index];
  }
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromMethodInfoReturn(
    const Objc3MethodInfo &method) {
  if (method.return_is_vector) {
    return MakeVectorSemanticType(method.return_type,
                                  method.return_vector_base_spelling,
                                  method.return_vector_lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(method.return_type);
  info.canonical_type = method.return_canonical_type;
  if (IsObjCReferenceAliasValueType(method.return_type)) {
    if (method.return_ownership_is_weak_reference) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (method.return_ownership_is_unowned_reference) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix = method.return_has_nullability_suffix;
  }
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromPropertyInfo(
    const Objc3PropertyInfo &property) {
  if (property.is_vector) {
    return MakeVectorSemanticType(property.type, property.vector_base_spelling,
                                  property.vector_lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(property.type);
  info.canonical_type = property.canonical_type;
  if (IsObjCReferenceAliasValueType(property.type)) {
    if (property.ownership_is_weak_reference) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (property.ownership_is_unowned_reference) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix = property.has_nullability_suffix;
  }
  return info;
}

static bool AreObjCReferenceTypesAssignmentCompatible(
    const SemanticTypeInfo &target, const SemanticTypeInfo &value,
    const Objc3SemanticIntegrationSurface *surface = nullptr);
static std::optional<bool> AreGenericSpecializationsVarianceAssignmentCompatible(
    const SemanticTypeInfo &target, const SemanticTypeInfo &value,
    const Objc3SemanticIntegrationSurface *surface);

static bool IsUnsafeNullableToNonnullFlow(const SemanticTypeInfo &target,
                                          const SemanticTypeInfo &value,
                                          const Objc3SemanticIntegrationSurface *surface = nullptr) {
  return IsNonnullDestinationObjCReferenceSemanticType(target) &&
         IsNullableObjCReferenceSemanticType(value) &&
         AreObjCReferenceTypesAssignmentCompatible(target, value, surface);
}

static bool AreObjCReferenceTypesAssignmentCompatible(const SemanticTypeInfo &target,
                                                      const SemanticTypeInfo &value,
                                                      const Objc3SemanticIntegrationSurface *surface) {
  if (!IsCanonicalObjc3TypeFormScaffoldReady()) {
    return false;
  }
  if (!IsObjCReferenceSemanticType(target) || !IsObjCReferenceSemanticType(value)) {
    return false;
  }
  if (const std::optional<bool> variance_compatibility =
          AreGenericSpecializationsVarianceAssignmentCompatible(target, value,
                                                               surface)) {
    return *variance_compatibility;
  }
  if (target.type == value.type) {
    return true;
  }
  // Canonical ObjC bridge-top forms are assignment-compatible by design.
  if (IsObjc3CanonicalBridgeTopReferenceTypeForm(target.type) ||
      IsObjc3CanonicalBridgeTopReferenceTypeForm(value.type)) {
    return true;
  }
  return false;
}
