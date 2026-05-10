static Objc3SemanticCanonicalTypeKind CanonicalTypeKindForValueType(
    ValueType type, bool is_vector, bool object_pointer_type_spelling,
    bool id_spelling, bool class_spelling, bool sel_spelling,
    bool instancetype_spelling) {
  if (is_vector) {
    return Objc3SemanticCanonicalTypeKind::Vector;
  }
  if (object_pointer_type_spelling || type == ValueType::ObjCObjectPtr) {
    return Objc3SemanticCanonicalTypeKind::ObjectPointer;
  }
  if (instancetype_spelling || type == ValueType::ObjCInstancetype) {
    return Objc3SemanticCanonicalTypeKind::Instancetype;
  }
  if (class_spelling || type == ValueType::ObjCClass) {
    return Objc3SemanticCanonicalTypeKind::ClassObject;
  }
  if (sel_spelling || type == ValueType::ObjCSel) {
    return Objc3SemanticCanonicalTypeKind::Selector;
  }
  if (type == ValueType::ObjCProtocol) {
    return Objc3SemanticCanonicalTypeKind::ProtocolObject;
  }
  if (id_spelling || type == ValueType::ObjCId) {
    return Objc3SemanticCanonicalTypeKind::Object;
  }
  if (type == ValueType::Function) {
    return Objc3SemanticCanonicalTypeKind::Function;
  }
  if (type == ValueType::Unknown) {
    return Objc3SemanticCanonicalTypeKind::Unknown;
  }
  return Objc3SemanticCanonicalTypeKind::Scalar;
}

static Objc3SemanticCanonicalNullability CanonicalNullabilityFromTokens(
    const std::vector<Objc3SemaTokenMetadata> &tokens) {
  if (tokens.empty()) {
    return Objc3SemanticCanonicalNullability::Unspecified;
  }
  const std::string &text = tokens.back().text;
  if (text == "?") {
    return Objc3SemanticCanonicalNullability::Nullable;
  }
  if (text == "!") {
    return Objc3SemanticCanonicalNullability::ImplicitlyUnwrapped;
  }
  return Objc3SemanticCanonicalNullability::Unspecified;
}

static Objc3SemanticCanonicalOwnership CanonicalOwnershipFromQualifiers(
    bool has_ownership_qualifier, bool weak_reference, bool unowned_reference,
    bool unsafe_unretained, bool assign, bool copy, bool retain, bool strong) {
  if (weak_reference) {
    return Objc3SemanticCanonicalOwnership::Weak;
  }
  if (unowned_reference) {
    return Objc3SemanticCanonicalOwnership::Unowned;
  }
  if (unsafe_unretained) {
    return Objc3SemanticCanonicalOwnership::UnsafeUnretained;
  }
  if (assign) {
    return Objc3SemanticCanonicalOwnership::Assign;
  }
  if (copy) {
    return Objc3SemanticCanonicalOwnership::Copy;
  }
  if (retain) {
    return Objc3SemanticCanonicalOwnership::Retain;
  }
  if (strong || has_ownership_qualifier) {
    return Objc3SemanticCanonicalOwnership::Strong;
  }
  return Objc3SemanticCanonicalOwnership::Unspecified;
}

static std::string CanonicalTypeReplayKey(
    const Objc3SemanticCanonicalType &type) {
  std::ostringstream out;
  out << "value=" << objc3c::support::ValueTypeName(type.value_type)
      << ";kind=" << static_cast<int>(type.kind)
      << ";nullability=" << static_cast<int>(type.nullability)
      << ";ownership=" << static_cast<int>(type.ownership)
      << ";vector=" << (type.is_vector ? 1 : 0)
      << ";vector-base=" << type.vector_base_spelling
      << ";lanes=" << type.vector_lane_count
      << ";pointer=" << (type.has_pointer_declarator ? 1 : 0)
      << ";pointer-depth=" << type.pointer_declarator_depth
      << ";object-name=" << type.object_pointer_type_name
      << ";protocols="
      << JoinStringVector(type.protocol_composition_lexicographic, "+")
      << ";generics="
      << JoinStringVector(type.generic_arguments_source_order, "+")
      << ";generics-lex="
      << JoinStringVector(type.generic_arguments_lexicographic, "+")
      << ";invalid=" << (type.has_invalid_type_suffix ? 1 : 0);
  return out.str();
}

static Objc3SemanticCanonicalType BuildCanonicalSemanticType(
    ValueType value_type, bool is_vector, const std::string &vector_base_spelling,
    unsigned vector_lane_count, bool id_spelling, bool class_spelling,
    bool sel_spelling, bool instancetype_spelling,
    bool object_pointer_type_spelling,
    const std::string &object_pointer_type_name, bool has_generic_suffix,
    const std::string &generic_suffix_text, bool has_pointer_declarator,
    unsigned pointer_declarator_depth,
    const std::vector<Objc3SemaTokenMetadata> &nullability_tokens,
    bool has_ownership_qualifier, bool ownership_is_weak_reference,
    bool ownership_is_unowned_reference, bool unsafe_unretained, bool assign,
    bool copy, bool retain, bool strong, bool property_nullable,
    bool property_nonnull, bool property_null_resettable,
    bool has_invalid_type_suffix) {
  Objc3SemanticCanonicalType type;
  type.value_type = value_type;
  type.kind = CanonicalTypeKindForValueType(
      value_type, is_vector, object_pointer_type_spelling, id_spelling,
      class_spelling, sel_spelling, instancetype_spelling);
  type.is_vector = is_vector;
  type.vector_base_spelling = vector_base_spelling;
  type.vector_lane_count = vector_lane_count;
  type.has_pointer_declarator = has_pointer_declarator;
  type.pointer_declarator_depth = pointer_declarator_depth;
  if (object_pointer_type_spelling) {
    type.object_pointer_type_name = object_pointer_type_name;
  }
  type.nullability = CanonicalNullabilityFromTokens(nullability_tokens);
  if (property_null_resettable) {
    type.nullability = Objc3SemanticCanonicalNullability::NullResettable;
  } else if (property_nonnull) {
    type.nullability = Objc3SemanticCanonicalNullability::Nonnull;
  } else if (property_nullable) {
    type.nullability = Objc3SemanticCanonicalNullability::Nullable;
  }
  type.ownership = CanonicalOwnershipFromQualifiers(
      has_ownership_qualifier, ownership_is_weak_reference,
      ownership_is_unowned_reference, unsafe_unretained, assign, copy, retain,
      strong);
  type.has_generic_suffix = has_generic_suffix;
  if (has_generic_suffix) {
    type.generic_arguments_source_order =
        ParseGenericArgumentSuffixTextSourceOrder(generic_suffix_text);
    type.generic_arguments_lexicographic =
        BuildGenericArgumentsLexicographic(type.generic_arguments_source_order);
    const ProtocolCompositionParseResult parsed =
        ParseProtocolCompositionSuffixText(generic_suffix_text);
    if (parsed.IsValid()) {
      type.has_protocol_composition = true;
      type.protocol_composition_lexicographic = parsed.names_lexicographic;
    }
  }
  type.has_invalid_type_suffix = has_invalid_type_suffix;
  type.deterministic = IsSortedUniqueStrings(type.protocol_composition_lexicographic) &&
                       std::is_sorted(type.generic_arguments_lexicographic.begin(),
                                      type.generic_arguments_lexicographic.end());
  type.canonical_spelling =
      object_pointer_type_spelling && !object_pointer_type_name.empty()
          ? object_pointer_type_name
          : objc3c::support::ValueTypeName(value_type);
  if (type.has_generic_suffix) {
    type.canonical_spelling += "<" +
                               JoinStringVector(
                                   type.generic_arguments_source_order, ",") +
                               ">";
  }
  if (type.has_pointer_declarator) {
    type.canonical_spelling += std::string(type.pointer_declarator_depth, '*');
  }
  type.replay_key = CanonicalTypeReplayKey(type);
  return type;
}

static Objc3SemanticCanonicalType BuildCanonicalSemanticTypeFromParam(
    const FuncParam &param) {
  return BuildCanonicalSemanticType(
      param.type, param.vector_spelling, param.vector_base_spelling,
      param.vector_lane_count, param.id_spelling, param.class_spelling,
      param.sel_spelling, param.instancetype_spelling,
      param.object_pointer_type_spelling, param.object_pointer_type_name,
      param.has_generic_suffix, param.generic_suffix_text,
      param.has_pointer_declarator,
      param.pointer_declarator_depth, param.nullability_suffix_tokens,
      param.has_ownership_qualifier, param.ownership_is_weak_reference,
      param.ownership_is_unowned_reference, false, false, false, false, false,
      false, false, false, HasInvalidParamTypeSuffix(param));
}

static Objc3SemanticCanonicalType BuildCanonicalSemanticTypeFromFunctionReturn(
    const FunctionDecl &fn) {
  return BuildCanonicalSemanticType(
      fn.return_type, fn.return_vector_spelling, fn.return_vector_base_spelling,
      fn.return_vector_lane_count, fn.return_id_spelling, fn.return_class_spelling,
      fn.return_sel_spelling, fn.return_instancetype_spelling,
      fn.return_object_pointer_type_spelling, fn.return_object_pointer_type_name,
      fn.has_return_generic_suffix, fn.return_generic_suffix_text,
      fn.has_return_pointer_declarator,
      fn.return_pointer_declarator_depth, fn.return_nullability_suffix_tokens,
      fn.has_return_ownership_qualifier, fn.return_ownership_is_weak_reference,
      fn.return_ownership_is_unowned_reference, false, false, false, false,
      false, false, false, false,
      HasInvalidGenericReturnTypeSuffix(fn) ||
          HasInvalidPointerReturnTypeDeclarator(fn) ||
          HasInvalidNullabilityReturnTypeSuffix(fn) ||
          HasInvalidOwnershipQualifierReturnTypeSuffix(fn));
}

static Objc3SemanticCanonicalType BuildCanonicalSemanticTypeFromMethodReturn(
    const Objc3MethodDecl &method) {
  return BuildCanonicalSemanticType(
      method.return_type, method.return_vector_spelling,
      method.return_vector_base_spelling, method.return_vector_lane_count,
      method.return_id_spelling, method.return_class_spelling,
      method.return_sel_spelling, method.return_instancetype_spelling,
      method.return_object_pointer_type_spelling,
      method.return_object_pointer_type_name, method.has_return_generic_suffix,
      method.return_generic_suffix_text,
      method.has_return_pointer_declarator,
      method.return_pointer_declarator_depth,
      method.return_nullability_suffix_tokens,
      method.has_return_ownership_qualifier,
      method.return_ownership_is_weak_reference,
      method.return_ownership_is_unowned_reference, false, false, false, false,
      false, false, false, false,
      HasInvalidGenericReturnTypeSuffix(method) ||
          HasInvalidPointerReturnTypeDeclarator(method) ||
          HasInvalidNullabilityReturnTypeSuffix(method) ||
          HasInvalidOwnershipQualifierReturnTypeSuffix(method));
}

static Objc3SemanticCanonicalType BuildCanonicalSemanticTypeFromProperty(
    const Objc3PropertyDecl &property) {
  return BuildCanonicalSemanticType(
      property.type, property.vector_spelling, property.vector_base_spelling,
      property.vector_lane_count, property.id_spelling, property.class_spelling,
      property.sel_spelling, property.instancetype_spelling,
      property.object_pointer_type_spelling, property.object_pointer_type_name,
      property.has_generic_suffix, property.generic_suffix_text,
      property.has_pointer_declarator,
      property.pointer_declarator_depth, property.nullability_suffix_tokens,
      property.has_ownership_qualifier, property.ownership_is_weak_reference,
      property.ownership_is_unowned_reference, property.is_unsafe_unretained,
      property.is_assign, property.is_copy, property.is_retain,
      property.is_strong, property.is_nullable, property.is_nonnull,
      property.is_null_resettable, HasInvalidPropertyTypeSuffix(property));
}
