static bool SupportsGenericParamTypeSuffix(const FuncParam &param) {
  return objc3c::support::SupportsObjCParamTypeSuffix(param);
}

static bool SupportsNullabilityParamTypeSuffix(const FuncParam &param) {
  return objc3c::support::SupportsObjCParamTypeSuffix(param);
}

static bool SupportsOwnershipQualifierParamTypeSuffix(const FuncParam &param) {
  return objc3c::support::SupportsObjCParamTypeSuffix(param);
}

static bool SupportsPointerParamTypeDeclarator(const FuncParam &param) {
  return objc3c::support::SupportsObjCParamPointerDeclarator(param);
}

static bool SupportsGenericReturnTypeSuffix(const FunctionDecl &fn) {
  return objc3c::support::SupportsObjCReturnTypeSuffix(fn);
}

static bool SupportsGenericReturnTypeSuffix(const Objc3MethodDecl &method) {
  return objc3c::support::SupportsObjCReturnTypeSuffix(method);
}

static bool SupportsNullabilityReturnTypeSuffix(const FunctionDecl &fn) {
  return objc3c::support::SupportsObjCReturnTypeSuffix(fn);
}

static bool SupportsNullabilityReturnTypeSuffix(const Objc3MethodDecl &method) {
  return objc3c::support::SupportsObjCReturnTypeSuffix(method);
}

static bool SupportsOwnershipQualifierReturnTypeSuffix(const FunctionDecl &fn) {
  return objc3c::support::SupportsObjCReturnTypeSuffix(fn);
}

static bool SupportsOwnershipQualifierReturnTypeSuffix(const Objc3MethodDecl &method) {
  return objc3c::support::SupportsObjCReturnTypeSuffix(method);
}

static bool SupportsPointerReturnTypeDeclarator(const FunctionDecl &fn) {
  return objc3c::support::SupportsObjCReturnPointerDeclarator(fn);
}

static bool SupportsPointerReturnTypeDeclarator(const Objc3MethodDecl &method) {
  return objc3c::support::SupportsObjCReturnPointerDeclarator(method);
}

static bool SupportsGenericPropertyTypeSuffix(const Objc3PropertyDecl &property) {
  return objc3c::support::SupportsObjCPropertyTypeSuffix(property);
}

static bool SupportsNullabilityPropertyTypeSuffix(const Objc3PropertyDecl &property) {
  return objc3c::support::SupportsObjCPropertyTypeSuffix(property);
}

static bool SupportsOwnershipQualifierPropertyTypeSuffix(const Objc3PropertyDecl &property) {
  return objc3c::support::SupportsObjCPropertyTypeSuffix(property);
}

static bool SupportsPointerPropertyTypeDeclarator(const Objc3PropertyDecl &property) {
  return objc3c::support::SupportsObjCPropertyPointerDeclarator(property);
}

static bool IsObjCReferenceAnnotationSite(bool id_spelling,
                                          bool class_spelling,
                                          bool instancetype_spelling,
                                          bool object_pointer_type_spelling) {
  return objc3c::support::IsObjCReferenceAnnotationSite(
      id_spelling, class_spelling, instancetype_spelling,
      object_pointer_type_spelling);
}

static bool HasInvalidGenericParamTypeSuffix(const FuncParam &param) {
  return param.has_generic_suffix && !SupportsGenericParamTypeSuffix(param);
}

static bool HasInvalidPointerParamTypeDeclarator(const FuncParam &param) {
  return param.has_pointer_declarator && !SupportsPointerParamTypeDeclarator(param);
}

static bool HasInvalidNullabilityParamTypeSuffix(const FuncParam &param) {
  return !param.nullability_suffix_tokens.empty() && !SupportsNullabilityParamTypeSuffix(param);
}

static bool HasInvalidOwnershipQualifierParamTypeSuffix(const FuncParam &param) {
  return param.has_ownership_qualifier && !SupportsOwnershipQualifierParamTypeSuffix(param);
}

static bool HasInvalidGenericReturnTypeSuffix(const FunctionDecl &fn) {
  return fn.has_return_generic_suffix && !SupportsGenericReturnTypeSuffix(fn);
}

static bool HasInvalidGenericReturnTypeSuffix(const Objc3MethodDecl &method) {
  return method.has_return_generic_suffix && !SupportsGenericReturnTypeSuffix(method);
}

static bool HasInvalidPointerReturnTypeDeclarator(const FunctionDecl &fn) {
  return fn.has_return_pointer_declarator && !SupportsPointerReturnTypeDeclarator(fn);
}

static bool HasInvalidPointerReturnTypeDeclarator(const Objc3MethodDecl &method) {
  return method.has_return_pointer_declarator && !SupportsPointerReturnTypeDeclarator(method);
}

static bool HasInvalidNullabilityReturnTypeSuffix(const FunctionDecl &fn) {
  return !fn.return_nullability_suffix_tokens.empty() && !SupportsNullabilityReturnTypeSuffix(fn);
}

static bool HasInvalidNullabilityReturnTypeSuffix(const Objc3MethodDecl &method) {
  return !method.return_nullability_suffix_tokens.empty() && !SupportsNullabilityReturnTypeSuffix(method);
}

static bool HasInvalidOwnershipQualifierReturnTypeSuffix(const FunctionDecl &fn) {
  return fn.has_return_ownership_qualifier && !SupportsOwnershipQualifierReturnTypeSuffix(fn);
}

static bool HasInvalidOwnershipQualifierReturnTypeSuffix(const Objc3MethodDecl &method) {
  return method.has_return_ownership_qualifier && !SupportsOwnershipQualifierReturnTypeSuffix(method);
}

static bool HasInvalidGenericPropertyTypeSuffix(const Objc3PropertyDecl &property) {
  return property.has_generic_suffix && !SupportsGenericPropertyTypeSuffix(property);
}

static bool HasInvalidPointerPropertyTypeDeclarator(const Objc3PropertyDecl &property) {
  return property.has_pointer_declarator && !SupportsPointerPropertyTypeDeclarator(property);
}

static bool HasInvalidNullabilityPropertyTypeSuffix(const Objc3PropertyDecl &property) {
  return !property.nullability_suffix_tokens.empty() && !SupportsNullabilityPropertyTypeSuffix(property);
}

static bool HasInvalidOwnershipQualifierPropertyTypeSuffix(const Objc3PropertyDecl &property) {
  return property.has_ownership_qualifier && !SupportsOwnershipQualifierPropertyTypeSuffix(property);
}

static bool HasInvalidPropertyTypeSuffix(const Objc3PropertyDecl &property) {
  return HasInvalidGenericPropertyTypeSuffix(property) ||
         HasInvalidPointerPropertyTypeDeclarator(property) ||
         HasInvalidNullabilityPropertyTypeSuffix(property) ||
         HasInvalidOwnershipQualifierPropertyTypeSuffix(property);
}

static bool IsKnownPropertyAttributeName(const std::string &name) {
  return name == "readonly" || name == "readwrite" || name == "atomic" || name == "nonatomic" || name == "copy" ||
         name == "retain" || name == "strong" || name == "weak" || name == "unowned" ||
         name == "unsafe_unretained" || name == "assign" || name == "nullable" || name == "nonnull" ||
         name == "null_resettable" || name == "class" || name == "direct" || name == "getter" ||
         name == "setter" || name == "behavior";
}

static bool IsValidPropertyGetterSelector(const std::string &selector) {
  return !selector.empty() && selector.find(':') == std::string::npos;
}

static bool IsValidPropertySetterSelector(const std::string &selector) {
  if (selector.empty() || selector.back() != ':') {
    return false;
  }
  return std::count(selector.begin(), selector.end(), ':') == 1u;
}

static bool HasInvalidParamTypeSuffix(const FuncParam &param) {
  return HasInvalidGenericParamTypeSuffix(param) ||
         HasInvalidPointerParamTypeDeclarator(param) ||
         HasInvalidNullabilityParamTypeSuffix(param) ||
         HasInvalidOwnershipQualifierParamTypeSuffix(param);
}
