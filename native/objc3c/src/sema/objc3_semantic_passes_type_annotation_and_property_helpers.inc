    return "debugDescription";
  }
  return "";
}

static bool IsValidProtocolIdentifier(const std::string &identifier) {
  if (identifier.empty()) {
    return false;
  }
  const unsigned char first = static_cast<unsigned char>(identifier.front());
  if (!(std::isalpha(first) != 0 || first == '_')) {
    return false;
  }
  for (std::size_t i = 1; i < identifier.size(); ++i) {
    const unsigned char c = static_cast<unsigned char>(identifier[i]);
    if (!(std::isalnum(c) != 0 || c == '_')) {
      return false;
    }
  }
  return true;
}

static ProtocolCompositionParseResult ParseProtocolCompositionSuffixText(const std::string &suffix_text) {
  ProtocolCompositionParseResult result;
  if (suffix_text.empty()) {
    return result;
  }

  result.has_protocol_composition = true;
  if (suffix_text.size() < 2 || suffix_text.front() != '<' || suffix_text.back() != '>') {
    result.malformed_composition = true;
    return result;
  }

  const std::string inner = suffix_text.substr(1, suffix_text.size() - 2);
  if (inner.find('<') != std::string::npos || inner.find('>') != std::string::npos) {
    result.malformed_composition = true;
  }

  std::unordered_set<std::string> seen_names;
  std::size_t start = 0;
  while (start <= inner.size()) {
    const std::size_t comma = inner.find(',', start);
    const std::size_t token_end = (comma == std::string::npos) ? inner.size() : comma;
    const std::string token = TrimAsciiWhitespace(inner.substr(start, token_end - start));
    if (token.empty()) {
      result.empty_composition = true;
    } else if (!IsValidProtocolIdentifier(token)) {
      result.invalid_identifiers.push_back(token);
    } else if (!seen_names.insert(token).second) {
      result.duplicate_identifiers.push_back(token);
    } else {
      result.names_lexicographic.push_back(token);
    }

    if (comma == std::string::npos) {
      break;
    }
    start = comma + 1;
  }

  if (result.names_lexicographic.empty()) {
    result.empty_composition = true;
  }
  std::sort(result.names_lexicographic.begin(), result.names_lexicographic.end());
  return result;
}

static std::vector<std::string> ParseGenericArgumentSuffixTextSourceOrder(
    const std::string &suffix_text) {
  std::vector<std::string> arguments;
  if (suffix_text.size() < 2u || suffix_text.front() != '<' ||
      suffix_text.back() != '>') {
    return arguments;
  }
  const std::string inner = suffix_text.substr(1, suffix_text.size() - 2u);
  int depth = 0;
  std::size_t start = 0;
  for (std::size_t index = 0; index <= inner.size(); ++index) {
    const bool at_end = index == inner.size();
    const char c = at_end ? ',' : inner[index];
    if (!at_end) {
      if (c == '<') {
        ++depth;
      } else if (c == '>' && depth > 0) {
        --depth;
      }
    }
    if ((at_end || c == ',') && depth == 0) {
      const std::string token =
          TrimAsciiWhitespace(inner.substr(start, index - start));
      if (!token.empty()) {
        arguments.push_back(token);
      }
      start = index + 1u;
    }
  }
  return arguments;
}

static std::vector<std::string> BuildGenericArgumentsLexicographic(
    const std::vector<std::string> &arguments_source_order) {
  std::vector<std::string> values = arguments_source_order;
  std::sort(values.begin(), values.end());
  return values;
}

static bool AreEquivalentProtocolCompositions(bool lhs_has_composition,
                                              const std::vector<std::string> &lhs_names,
                                              bool rhs_has_composition,
                                              const std::vector<std::string> &rhs_names) {
  if (lhs_has_composition != rhs_has_composition) {
    return false;
  }
  if (!lhs_has_composition) {
    return true;
  }
  return lhs_names == rhs_names;
}

static bool IsCompoundAssignmentOperator(const std::string &op) {
  return op == "+=" || op == "-=" || op == "*=" || op == "/=" || op == "%=" || op == "&=" || op == "|=" ||
         op == "^=" || op == "<<=" || op == ">>=";
}

static Objc3SemaAtomicMemoryOrder MapAssignmentOperatorToAtomicMemoryOrder(const std::string &op) {
  if (op == "=" || op == "|=" || op == "^=") {
    return Objc3SemaAtomicMemoryOrder::Release;
  }
  if (op == "&=" || op == "<<=" || op == ">>=") {
    return Objc3SemaAtomicMemoryOrder::Acquire;
  }
  if (op == "+=" || op == "-=" || op == "++" || op == "--") {
    return Objc3SemaAtomicMemoryOrder::AcqRel;
  }
  if (op == "*=" || op == "/=" || op == "%=") {
    return Objc3SemaAtomicMemoryOrder::SeqCst;
  }
  return Objc3SemaAtomicMemoryOrder::Unsupported;
}

static const char *AtomicMemoryOrderName(Objc3SemaAtomicMemoryOrder order) {
  switch (order) {
    case Objc3SemaAtomicMemoryOrder::Relaxed:
      return "relaxed";
    case Objc3SemaAtomicMemoryOrder::Acquire:
      return "acquire";
    case Objc3SemaAtomicMemoryOrder::Release:
      return "release";
    case Objc3SemaAtomicMemoryOrder::AcqRel:
      return "acq_rel";
    case Objc3SemaAtomicMemoryOrder::SeqCst:
      return "seq_cst";
    case Objc3SemaAtomicMemoryOrder::Unsupported:
    default:
      return "unsupported";
  }
}

static void RecordAtomicMemoryOrderMapping(const std::string &op, Objc3AtomicMemoryOrderMappingSummary &summary) {
  const Objc3SemaAtomicMemoryOrder order = MapAssignmentOperatorToAtomicMemoryOrder(op);
  switch (order) {
    case Objc3SemaAtomicMemoryOrder::Relaxed:
      ++summary.relaxed;
      break;
    case Objc3SemaAtomicMemoryOrder::Acquire:
      ++summary.acquire;
      break;
    case Objc3SemaAtomicMemoryOrder::Release:
      ++summary.release;
      break;
    case Objc3SemaAtomicMemoryOrder::AcqRel:
      ++summary.acq_rel;
      break;
    case Objc3SemaAtomicMemoryOrder::SeqCst:
      ++summary.seq_cst;
      break;
    case Objc3SemaAtomicMemoryOrder::Unsupported:
    default:
      ++summary.unsupported;
      summary.deterministic = false;
      break;
  }
}

static std::string FormatAtomicMemoryOrderMappingHint(const std::string &op) {
  const Objc3SemaAtomicMemoryOrder order = MapAssignmentOperatorToAtomicMemoryOrder(op);
  if (order == Objc3SemaAtomicMemoryOrder::Unsupported) {
    return "atomic memory-order mapping unavailable for operator '" + op + "'";
  }
  return "atomic memory-order mapping for operator '" + op + "' uses '" +
         std::string(AtomicMemoryOrderName(order)) + "'";
}

static void RecordVectorTypeLoweringAnnotation(ValueType base_type, unsigned lane_count, bool is_return,
                                               Objc3VectorTypeLoweringSummary &summary) {
  if (is_return) {
    ++summary.return_annotations;
  } else {
    ++summary.param_annotations;
  }

  if (base_type == ValueType::Bool) {
    ++summary.bool_annotations;
  } else if (base_type == ValueType::I32) {
    ++summary.i32_annotations;
  } else {
    ++summary.unsupported_annotations;
    summary.deterministic = false;
  }

  switch (lane_count) {
    case 2u:
      ++summary.lane2_annotations;
      break;
    case 4u:
      ++summary.lane4_annotations;
      break;
    case 8u:
      ++summary.lane8_annotations;
      break;
    case 16u:
      ++summary.lane16_annotations;
      break;
    default:
      ++summary.unsupported_annotations;
      summary.deterministic = false;
      break;
  }
}

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

static bool IsGenericInterfaceSpecializationSuffix(
    bool object_pointer_type_spelling,
    const std::string &object_pointer_type_name,
    const std::unordered_set<std::string> *generic_interface_names);

static ProtocolCompositionInfo BuildProtocolCompositionInfoFromParam(
    const FuncParam &param,
    const std::unordered_set<std::string> *generic_interface_names = nullptr) {
  ProtocolCompositionInfo info;
  if (!param.has_generic_suffix) {
    return info;
  }
  if (IsGenericInterfaceSpecializationSuffix(
          param.object_pointer_type_spelling, param.object_pointer_type_name,
          generic_interface_names)) {
    return info;
  }

  const ProtocolCompositionParseResult parsed = ParseProtocolCompositionSuffixText(param.generic_suffix_text);
  info.has_protocol_composition = true;
  info.names_lexicographic = parsed.names_lexicographic;
  info.has_invalid_protocol_composition = !SupportsGenericParamTypeSuffix(param) || !parsed.IsValid();
  return info;
}

static ProtocolCompositionInfo BuildProtocolCompositionInfoFromFunctionReturn(
    const FunctionDecl &fn,
    const std::unordered_set<std::string> *generic_interface_names = nullptr) {
  ProtocolCompositionInfo info;
  if (!fn.has_return_generic_suffix) {
    return info;
  }
  if (IsGenericInterfaceSpecializationSuffix(
          fn.return_object_pointer_type_spelling,
          fn.return_object_pointer_type_name, generic_interface_names)) {
    return info;
  }

  const ProtocolCompositionParseResult parsed = ParseProtocolCompositionSuffixText(fn.return_generic_suffix_text);
  info.has_protocol_composition = true;
  info.names_lexicographic = parsed.names_lexicographic;
  info.has_invalid_protocol_composition = !SupportsGenericReturnTypeSuffix(fn) || !parsed.IsValid();
  return info;
}

static ProtocolCompositionInfo BuildProtocolCompositionInfoFromMethodReturn(
    const Objc3MethodDecl &method,
    const std::unordered_set<std::string> *generic_interface_names = nullptr) {
  ProtocolCompositionInfo info;
  if (!method.has_return_generic_suffix) {
    return info;
  }
  if (IsGenericInterfaceSpecializationSuffix(
          method.return_object_pointer_type_spelling,
          method.return_object_pointer_type_name, generic_interface_names)) {
    return info;
  }

  const ProtocolCompositionParseResult parsed = ParseProtocolCompositionSuffixText(method.return_generic_suffix_text);
  info.has_protocol_composition = true;
  info.names_lexicographic = parsed.names_lexicographic;
  info.has_invalid_protocol_composition = !SupportsGenericReturnTypeSuffix(method) || !parsed.IsValid();
  return info;
}

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

static void ValidateProtocolCompositionSuffix(const std::string &suffix_text,
                                              unsigned line,
                                              unsigned column,
                                              const std::string &context,
                                              bool allow_generic_argument_suffix,
                                              std::vector<std::string> &diagnostics) {
  const ProtocolCompositionParseResult parsed = ParseProtocolCompositionSuffixText(suffix_text);
  if (allow_generic_argument_suffix && !parsed.IsValid() &&
      !ParseGenericArgumentSuffixTextSourceOrder(suffix_text).empty()) {
    return;
  }
  const std::string printable_suffix = suffix_text.empty() ? "<...>" : suffix_text;
  if (parsed.malformed_composition) {
    diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                   "type mismatch: malformed protocol composition suffix '" + printable_suffix +
                                       "' for " + context));
    return;
  }

  if (parsed.empty_composition) {
    diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                   "type mismatch: empty protocol composition suffix '" + printable_suffix +
                                       "' for " + context));
  }

  for (const auto &identifier : parsed.invalid_identifiers) {
    diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                   "type mismatch: invalid protocol identifier '" + identifier +
                                       "' in protocol composition suffix '" + printable_suffix + "' for " + context));
  }
  for (const auto &identifier : parsed.duplicate_identifiers) {
    diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                   "type mismatch: duplicate protocol identifier '" + identifier +
                                       "' in protocol composition suffix '" + printable_suffix + "' for " + context));
  }
}

static bool IsGenericInterfaceSpecializationSuffix(
    bool object_pointer_type_spelling, const std::string &object_pointer_type_name,
    const std::unordered_set<std::string> *generic_interface_names) {
  return object_pointer_type_spelling && generic_interface_names != nullptr &&
         generic_interface_names->find(object_pointer_type_name) !=
             generic_interface_names->end();
}

static void ValidateParameterTypeSuffixes(
    const FunctionDecl &fn, std::vector<std::string> &diagnostics,
    const std::unordered_set<std::string> *generic_interface_names = nullptr) {
  for (const auto &param : fn.params) {
    if (param.has_generic_suffix && !SupportsGenericParamTypeSuffix(param)) {
      std::string suffix = param.generic_suffix_text;
      if (suffix.empty()) {
        suffix = "<...>";
      }
      diagnostics.push_back(MakeDiag(param.generic_line, param.generic_column, "O3S206",
                                     "type mismatch: generic parameter type suffix '" + suffix +
                                         "' is unsupported for non-id/Class/instancetype parameter annotation '" +
                                         param.name + "'"));
    } else if (param.has_generic_suffix) {
      ValidateProtocolCompositionSuffix(param.generic_suffix_text,
                                       param.generic_line,
                                       param.generic_column,
                                       "parameter '" + param.name + "' in function '" + fn.name + "'",
                                       IsGenericInterfaceSpecializationSuffix(
                                           param.object_pointer_type_spelling,
                                           param.object_pointer_type_name,
                                           generic_interface_names),
                                       diagnostics);
    }
    if (!SupportsPointerParamTypeDeclarator(param)) {
      for (const auto &token : param.pointer_declarator_tokens) {
        diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                       "type mismatch: pointer parameter type declarator '" + token.text +
                                           "' is unsupported for non-id/Class/instancetype parameter annotation '" +
                                           param.name + "'"));
      }
    }
    if (!SupportsNullabilityParamTypeSuffix(param)) {
      for (const auto &token : param.nullability_suffix_tokens) {
        diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                       "type mismatch: nullability parameter type suffix '" + token.text +
                                           "' is unsupported for non-id/Class/instancetype parameter annotation '" +
                                           param.name + "'"));
      }
    }
    if (!SupportsOwnershipQualifierParamTypeSuffix(param)) {
      for (const auto &token : param.ownership_qualifier_tokens) {
        diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                       "type mismatch: ownership parameter type qualifier '" + token.text +
                                           "' is unsupported for non-object parameter annotation '" + param.name +
                                           "'"));
      }
    }
  }
}

static void ValidateReturnTypeSuffixes(
    const FunctionDecl &fn, std::vector<std::string> &diagnostics,
    const std::unordered_set<std::string> *generic_interface_names = nullptr) {
  if (fn.has_return_generic_suffix && !SupportsGenericReturnTypeSuffix(fn)) {
    std::string suffix = fn.return_generic_suffix_text;
    if (suffix.empty()) {
      suffix = "<...>";
    }
    diagnostics.push_back(MakeDiag(fn.return_generic_line, fn.return_generic_column, "O3S206",
                                   "type mismatch: unsupported function return type suffix '" + suffix +
                                       "' for non-id/Class/instancetype return annotation in function '" + fn.name +
                                       "'"));
  } else if (fn.has_return_generic_suffix) {
    ValidateProtocolCompositionSuffix(fn.return_generic_suffix_text,
                                     fn.return_generic_line,
                                     fn.return_generic_column,
                                     "return annotation in function '" + fn.name + "'",
                                     IsGenericInterfaceSpecializationSuffix(
                                         fn.return_object_pointer_type_spelling,
                                         fn.return_object_pointer_type_name,
                                         generic_interface_names),
                                     diagnostics);
  }
  if (!SupportsPointerReturnTypeDeclarator(fn)) {
    for (const auto &token : fn.return_pointer_declarator_tokens) {
      diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                     "type mismatch: unsupported function return type declarator '" + token.text +
                                         "' for non-id/Class/instancetype return annotation in function '" + fn.name +
                                         "'"));
    }
  }
  if (!SupportsNullabilityReturnTypeSuffix(fn)) {
    for (const auto &token : fn.return_nullability_suffix_tokens) {
      diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                     "type mismatch: unsupported function return type suffix '" + token.text +
                                         "' for non-id/Class/instancetype return annotation in function '" + fn.name +
                                         "'"));
    }
  }
  if (!SupportsOwnershipQualifierReturnTypeSuffix(fn)) {
    for (const auto &token : fn.return_ownership_qualifier_tokens) {
      diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                     "type mismatch: unsupported function return ownership qualifier '" + token.text +
                                         "' for non-object return annotation in function '" + fn.name + "'"));
    }
  }
}

struct MethodSelectorNormalizationContractInfo {
  std::string normalized_selector;
  std::size_t selector_piece_count = 0;
  std::size_t selector_parameter_piece_count = 0;
  bool selector_contract_normalized = false;
  bool selector_had_pieceless_form = false;
  bool selector_has_spelling_mismatch = false;
  bool selector_has_arity_mismatch = false;
  bool selector_has_parameter_linkage_mismatch = false;
  bool selector_has_normalization_flag_mismatch = false;
  bool selector_has_missing_piece_keyword = false;
};

static std::string BuildNormalizedMethodSelectorFromPieces(
    const std::vector<Objc3MethodDecl::SelectorPiece> &pieces) {
  std::string normalized;
  for (const auto &piece : pieces) {
    normalized += piece.keyword;
    if (piece.has_parameter) {
      normalized += ":";
    }
  }
  return normalized;
}

static MethodSelectorNormalizationContractInfo BuildMethodSelectorNormalizationContractInfo(
    const Objc3MethodDecl &method) {
  MethodSelectorNormalizationContractInfo info;
  info.selector_piece_count = method.selector_pieces.size();
  info.selector_had_pieceless_form = method.selector_pieces.empty();

  std::size_t linked_param_index = 0;
  for (const auto &piece : method.selector_pieces) {
    if (piece.keyword.empty()) {
      info.selector_has_missing_piece_keyword = true;
    }
    if (!piece.has_parameter) {
      continue;
    }

    ++info.selector_parameter_piece_count;
    if (linked_param_index >= method.params.size() || piece.parameter_name != method.params[linked_param_index].name) {
      info.selector_has_parameter_linkage_mismatch = true;
    }
    ++linked_param_index;
  }
  info.selector_has_arity_mismatch = info.selector_parameter_piece_count != method.params.size();

  if (method.selector_pieces.empty()) {
    info.normalized_selector = method.selector;
  } else {
    info.normalized_selector = BuildNormalizedMethodSelectorFromPieces(method.selector_pieces);
    info.selector_has_spelling_mismatch = method.selector != info.normalized_selector;
  }

  if (info.normalized_selector.empty()) {
    info.normalized_selector = "<unknown>";
  }

  info.selector_has_normalization_flag_mismatch = !method.selector_is_normalized;
  info.selector_contract_normalized = !info.selector_had_pieceless_form &&
                                      !info.selector_has_spelling_mismatch &&
                                      !info.selector_has_arity_mismatch &&
                                      !info.selector_has_parameter_linkage_mismatch &&
                                      !info.selector_has_normalization_flag_mismatch &&
                                      !info.selector_has_missing_piece_keyword &&
                                      info.normalized_selector != "<unknown>";
  return info;
}

static std::string MethodSelectorName(const Objc3MethodDecl &method) {
  const MethodSelectorNormalizationContractInfo selector_contract =
      BuildMethodSelectorNormalizationContractInfo(method);
  return selector_contract.normalized_selector;
}

static void ValidateMethodSelectorNormalizationContract(
    const Objc3MethodDecl &method,
    const std::string &owner_name,
    const std::string &owner_kind,
    const MethodSelectorNormalizationContractInfo &selector_contract,
    std::vector<std::string> &diagnostics) {
  const std::string selector = selector_contract.normalized_selector.empty()
                                   ? std::string("<unknown>")
                                   : selector_contract.normalized_selector;
  if (selector_contract.selector_had_pieceless_form) {
    diagnostics.push_back(MakeDiag(method.line,
                                   method.column,
                                   "O3S206",
                                   "type mismatch: selector normalization requires selector pieces for selector '" +
                                       selector + "' in " + owner_kind + " '" + owner_name + "'"));
  }
  if (selector_contract.selector_has_spelling_mismatch) {
    const std::string raw_selector = method.selector.empty() ? "<unknown>" : method.selector;
    diagnostics.push_back(MakeDiag(method.line,
                                   method.column,
                                   "O3S206",
                                   "type mismatch: selector normalization mismatch in " + owner_kind + " '" +
                                       owner_name + "' for selector '" + raw_selector + "' (expected '" + selector +
                                       "')"));
  }
  if (selector_contract.selector_has_normalization_flag_mismatch) {
    diagnostics.push_back(MakeDiag(method.line,
                                   method.column,
                                   "O3S206",
                                   "type mismatch: selector normalization flag mismatch for selector '" + selector +
                                       "' in " + owner_kind + " '" + owner_name + "'"));
  }
  if (selector_contract.selector_has_missing_piece_keyword) {
    for (const auto &piece : method.selector_pieces) {
      if (!piece.keyword.empty()) {
        continue;
      }
      diagnostics.push_back(MakeDiag(piece.line,
                                     piece.column,
                                     "O3S206",
                                     "type mismatch: selector piece keyword must be non-empty for selector '" +
                                         selector + "' in " + owner_kind + " '" + owner_name + "'"));
    }
  }
  if (selector_contract.selector_has_arity_mismatch) {
    diagnostics.push_back(MakeDiag(method.line,
                                   method.column,
                                   "O3S206",
                                   "type mismatch: selector arity mismatch for selector '" + selector + "' in " +
                                       owner_kind + " '" + owner_name + "' (selector parameters=" +
                                       std::to_string(selector_contract.selector_parameter_piece_count) +
                                       ", declaration parameters=" + std::to_string(method.params.size()) + ")"));
  }
  if (selector_contract.selector_has_parameter_linkage_mismatch) {
    std::size_t linked_param_index = 0;
    for (const auto &piece : method.selector_pieces) {
      if (!piece.has_parameter) {
        continue;
      }
      const bool missing_decl_param = linked_param_index >= method.params.size();
      const std::string expected_param = piece.parameter_name.empty() ? "<unnamed>" : piece.parameter_name;
      const std::string actual_param =
          missing_decl_param
              ? "<missing>"
              : (method.params[linked_param_index].name.empty() ? std::string("<unnamed>")
                                                                : method.params[linked_param_index].name);
      if (missing_decl_param || expected_param != actual_param) {
        diagnostics.push_back(MakeDiag(piece.line,
                                       piece.column,
                                       "O3S206",
                                       "type mismatch: selector parameter linkage mismatch for selector '" +
                                           selector + "' in " + owner_kind + " '" + owner_name + "' piece '" +
                                           piece.keyword + ":' (piece parameter='" + expected_param +
                                           "', declaration parameter='" + actual_param + "')"));
      }
      ++linked_param_index;
    }

    while (linked_param_index < method.params.size()) {
      const FuncParam &param = method.params[linked_param_index];
      const std::string param_name = param.name.empty() ? "<unnamed>" : param.name;
      diagnostics.push_back(MakeDiag(param.line,
                                     param.column,
                                     "O3S206",
                                     "type mismatch: selector parameter linkage mismatch for selector '" + selector +
                                         "' in " + owner_kind + " '" + owner_name +
                                         "' (declaration parameter '" + param_name +
                                         "' has no selector piece linkage)"));
      ++linked_param_index;
    }
  }
}

static void ValidateMethodParameterTypeSuffixes(const Objc3MethodDecl &method,
                                                const std::string &owner_name,
                                                const std::string &owner_kind,
                                                std::vector<std::string> &diagnostics,
                                                const std::unordered_set<std::string> *generic_interface_names = nullptr) {
  const std::string selector = MethodSelectorName(method);
  for (const auto &param : method.params) {
    if (param.has_generic_suffix && !SupportsGenericParamTypeSuffix(param)) {
      std::string suffix = param.generic_suffix_text;
      if (suffix.empty()) {
        suffix = "<...>";
      }
      diagnostics.push_back(MakeDiag(param.generic_line, param.generic_column, "O3S206",
                                     "type mismatch: generic parameter type suffix '" + suffix +
                                         "' is unsupported for selector '" + selector + "' parameter '" + param.name +
                                         "' in " + owner_kind + " '" + owner_name + "'"));
    } else if (param.has_generic_suffix) {
      ValidateProtocolCompositionSuffix(
          param.generic_suffix_text,
          param.generic_line,
          param.generic_column,
          "selector '" + selector + "' parameter '" + param.name + "' in " + owner_kind + " '" + owner_name + "'",
          IsGenericInterfaceSpecializationSuffix(
              param.object_pointer_type_spelling,
              param.object_pointer_type_name, generic_interface_names),
          diagnostics);
    }
    if (!SupportsPointerParamTypeDeclarator(param)) {
      for (const auto &token : param.pointer_declarator_tokens) {
        diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                       "type mismatch: pointer parameter type declarator '" + token.text +
                                           "' is unsupported for selector '" + selector + "' parameter '" +
                                           param.name + "' in " + owner_kind + " '" + owner_name + "'"));
      }
    }
    if (!SupportsNullabilityParamTypeSuffix(param)) {
      for (const auto &token : param.nullability_suffix_tokens) {
        diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                       "type mismatch: nullability parameter type suffix '" + token.text +
                                           "' is unsupported for selector '" + selector + "' parameter '" +
                                           param.name + "' in " + owner_kind + " '" + owner_name + "'"));
      }
    }
    if (!SupportsOwnershipQualifierParamTypeSuffix(param)) {
      for (const auto &token : param.ownership_qualifier_tokens) {
        diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                       "type mismatch: ownership parameter type qualifier '" + token.text +
                                           "' is unsupported for selector '" + selector + "' parameter '" +
                                           param.name + "' in " + owner_kind + " '" + owner_name + "'"));
      }
    }
  }
}

static void ValidateMethodReturnTypeSuffixes(const Objc3MethodDecl &method,
                                             const std::string &owner_name,
                                             const std::string &owner_kind,
                                             std::vector<std::string> &diagnostics,
                                             const std::unordered_set<std::string> *generic_interface_names = nullptr) {
  const std::string selector = MethodSelectorName(method);
  if (method.has_return_generic_suffix && !SupportsGenericReturnTypeSuffix(method)) {
    std::string suffix = method.return_generic_suffix_text;
    if (suffix.empty()) {
      suffix = "<...>";
    }
    diagnostics.push_back(MakeDiag(method.return_generic_line, method.return_generic_column, "O3S206",
                                   "type mismatch: unsupported method return type suffix '" + suffix +
                                       "' for selector '" + selector + "' in " + owner_kind + " '" + owner_name +
                                       "'"));
  } else if (method.has_return_generic_suffix) {
    ValidateProtocolCompositionSuffix(method.return_generic_suffix_text,
                                     method.return_generic_line,
                                     method.return_generic_column,
                                     "selector '" + selector + "' in " + owner_kind + " '" + owner_name +
                                         "' return annotation",
                                     IsGenericInterfaceSpecializationSuffix(
                                         method.return_object_pointer_type_spelling,
                                         method.return_object_pointer_type_name,
                                         generic_interface_names),
                                     diagnostics);
  }
  if (!SupportsPointerReturnTypeDeclarator(method)) {
    for (const auto &token : method.return_pointer_declarator_tokens) {
      diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                     "type mismatch: unsupported method return type declarator '" + token.text +
                                         "' for selector '" + selector + "' in " + owner_kind + " '" + owner_name +
                                         "'"));
    }
  }
  if (!SupportsNullabilityReturnTypeSuffix(method)) {
    for (const auto &token : method.return_nullability_suffix_tokens) {
      diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                     "type mismatch: unsupported method return type suffix '" + token.text +
                                         "' for selector '" + selector + "' in " + owner_kind + " '" + owner_name +
                                         "'"));
    }
  }
  if (!SupportsOwnershipQualifierReturnTypeSuffix(method)) {
    for (const auto &token : method.return_ownership_qualifier_tokens) {
      diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                     "type mismatch: unsupported method return ownership qualifier '" + token.text +
                                         "' for selector '" + selector + "' in " + owner_kind + " '" + owner_name +
                                         "'"));
    }
  }
}

static void ValidatePropertyTypeSuffixes(const Objc3PropertyDecl &property,
                                         const std::string &owner_name,
                                         const std::string &owner_kind,
                                         std::vector<std::string> &diagnostics,
                                         const std::unordered_set<std::string> *generic_interface_names = nullptr) {
  if (property.has_generic_suffix && !SupportsGenericPropertyTypeSuffix(property)) {
    std::string suffix = property.generic_suffix_text;
    if (suffix.empty()) {
      suffix = "<...>";
    }
    diagnostics.push_back(MakeDiag(property.generic_line,
                                   property.generic_column,
                                   "O3S206",
                                   "type mismatch: generic property type suffix '" + suffix +
                                       "' is unsupported for property '" + property.name + "' in " + owner_kind +
                                       " '" + owner_name + "'"));
  } else if (property.has_generic_suffix) {
    ValidateProtocolCompositionSuffix(property.generic_suffix_text,
                                     property.generic_line,
                                     property.generic_column,
                                     "property '" + property.name + "' in " + owner_kind + " '" + owner_name +
                                         "' type annotation",
                                     IsGenericInterfaceSpecializationSuffix(
                                         property.object_pointer_type_spelling,
                                         property.object_pointer_type_name,
                                         generic_interface_names),
                                     diagnostics);
  }
  if (!SupportsPointerPropertyTypeDeclarator(property)) {
    for (const auto &token : property.pointer_declarator_tokens) {
      diagnostics.push_back(MakeDiag(token.line,
                                     token.column,
                                     "O3S206",
                                     "type mismatch: unsupported property type declarator '" + token.text +
                                         "' for property '" + property.name + "' in " + owner_kind + " '" +
                                         owner_name + "'"));
    }
  }
  if (!SupportsNullabilityPropertyTypeSuffix(property)) {
    for (const auto &token : property.nullability_suffix_tokens) {
      diagnostics.push_back(MakeDiag(token.line,
                                     token.column,
                                     "O3S206",
                                     "type mismatch: unsupported property type suffix '" + token.text +
                                         "' for property '" + property.name + "' in " + owner_kind + " '" +
                                         owner_name + "'"));
    }
  }
  if (!SupportsOwnershipQualifierPropertyTypeSuffix(property)) {
    for (const auto &token : property.ownership_qualifier_tokens) {
      diagnostics.push_back(MakeDiag(token.line,
                                     token.column,
                                     "O3S206",
                                     "type mismatch: unsupported property ownership qualifier '" + token.text +
                                         "' for property '" + property.name + "' in " + owner_kind + " '" +
                                         owner_name + "'"));
    }
  }
}

static Objc3PropertyInfo BuildPropertyInfo(const Objc3PropertyDecl &property,
                                           const std::string &owner_name,
                                           const std::string &owner_kind,
                                           bool arc_mode_enabled,
                                           std::vector<std::string> &diagnostics) {
  const auto property_has_attribute_named =
      [&property](std::string_view name) {
        for (const auto &attribute : property.attributes) {
          if (attribute.name == name) {
            return true;
          }
        }
        return false;
      };
  const auto property_is_object_like = [](const Objc3PropertyInfo &info) {
    return info.id_spelling || info.instancetype_spelling ||
           info.object_pointer_type_spelling || info.type == ValueType::ObjCId ||
           info.type == ValueType::ObjCClass ||
           info.type == ValueType::ObjCInstancetype ||
