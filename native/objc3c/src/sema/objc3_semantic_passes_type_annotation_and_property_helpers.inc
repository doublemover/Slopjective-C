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

#include "sema/objc3_semantic_passes_canonical_type_helpers.inc"

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

#include "sema/objc3_semantic_passes_method_property_type_validation.inc"

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
