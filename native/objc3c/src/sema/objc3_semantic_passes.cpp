#include "sema/objc3_semantic_passes.h"
#include "sema/objc3_static_analysis.h"

#include <algorithm>
#include <cctype>
#include <limits>
#include <map>
#include <optional>
#include <sstream>
#include <unordered_set>
#include <vector>

static std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message) {
  std::ostringstream out;
  out << "error:" << line << ":" << column << ": " << message << " [" << code << "]";
  return out.str();
}

static const char *TypeName(ValueType type) {
  switch (type) {
    case ValueType::I32:
      return "i32";
    case ValueType::Bool:
      return "bool";
    case ValueType::Void:
      return "void";
    case ValueType::Function:
      return "function";
    default:
      return "unknown";
  }
}

struct SemanticTypeInfo {
  ValueType type = ValueType::Unknown;
  bool is_vector = false;
  std::string vector_base_spelling;
  unsigned vector_lane_count = 1;
};

using SemanticScope = std::unordered_map<std::string, SemanticTypeInfo>;

static SemanticTypeInfo MakeScalarSemanticType(ValueType type) {
  SemanticTypeInfo info;
  info.type = type;
  return info;
}

static SemanticTypeInfo MakeVectorSemanticType(ValueType base_type, const std::string &base_spelling,
                                               unsigned lane_count) {
  SemanticTypeInfo info;
  info.type = base_type;
  info.is_vector = true;
  info.vector_base_spelling = base_spelling;
  info.vector_lane_count = lane_count;
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromParam(const FuncParam &param) {
  if (param.vector_spelling) {
    return MakeVectorSemanticType(param.type, param.vector_base_spelling, param.vector_lane_count);
  }
  return MakeScalarSemanticType(param.type);
}

static SemanticTypeInfo MakeSemanticTypeFromFunctionReturn(const FunctionDecl &fn) {
  if (fn.return_vector_spelling) {
    return MakeVectorSemanticType(fn.return_type, fn.return_vector_base_spelling, fn.return_vector_lane_count);
  }
  return MakeScalarSemanticType(fn.return_type);
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
  return MakeScalarSemanticType(fn.param_types[index]);
}

static SemanticTypeInfo MakeSemanticTypeFromFunctionInfoReturn(const FunctionInfo &fn) {
  if (fn.return_is_vector) {
    return MakeVectorSemanticType(fn.return_type, fn.return_vector_base_spelling, fn.return_vector_lane_count);
  }
  return MakeScalarSemanticType(fn.return_type);
}

static SemanticTypeInfo MakeSemanticTypeFromGlobal(ValueType type) {
  return MakeScalarSemanticType(type);
}

static bool IsUnknownSemanticType(const SemanticTypeInfo &info) {
  return !info.is_vector && info.type == ValueType::Unknown;
}

static bool IsScalarSemanticType(const SemanticTypeInfo &info) {
  return !info.is_vector;
}

static bool IsScalarBoolCompatibleType(const SemanticTypeInfo &info) {
  return !info.is_vector && (info.type == ValueType::Bool || info.type == ValueType::I32);
}

static bool IsMessageI32CompatibleType(const SemanticTypeInfo &info) {
  return !info.is_vector && (info.type == ValueType::I32 || info.type == ValueType::Bool);
}

static bool IsSameSemanticType(const SemanticTypeInfo &lhs, const SemanticTypeInfo &rhs) {
  if (lhs.is_vector != rhs.is_vector) {
    return false;
  }
  if (lhs.type != rhs.type) {
    return false;
  }
  if (!lhs.is_vector) {
    return true;
  }
  return lhs.vector_lane_count == rhs.vector_lane_count && lhs.vector_base_spelling == rhs.vector_base_spelling;
}

static std::string SemanticTypeName(const SemanticTypeInfo &info) {
  if (!info.is_vector) {
    return TypeName(info.type);
  }
  const std::string base = info.vector_base_spelling.empty() ? std::string(TypeName(info.type)) : info.vector_base_spelling;
  return base + "x" + std::to_string(info.vector_lane_count);
}

struct ProtocolCompositionParseResult {
  bool has_protocol_composition = false;
  bool malformed_composition = false;
  bool empty_composition = false;
  std::vector<std::string> names_lexicographic;
  std::vector<std::string> invalid_identifiers;
  std::vector<std::string> duplicate_identifiers;

  bool IsValid() const {
    return !malformed_composition && !empty_composition && invalid_identifiers.empty() && duplicate_identifiers.empty();
  }
};

struct ProtocolCompositionInfo {
  bool has_protocol_composition = false;
  std::vector<std::string> names_lexicographic;
  bool has_invalid_protocol_composition = false;
};

static std::string TrimAsciiWhitespace(const std::string &text) {
  std::size_t start = 0;
  while (start < text.size() && std::isspace(static_cast<unsigned char>(text[start])) != 0) {
    ++start;
  }
  if (start == text.size()) {
    return "";
  }

  std::size_t end = text.size();
  while (end > start && std::isspace(static_cast<unsigned char>(text[end - 1])) != 0) {
    --end;
  }
  return text.substr(start, end - start);
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

static bool IsSortedUniqueStrings(const std::vector<std::string> &values) {
  if (!std::is_sorted(values.begin(), values.end())) {
    return false;
  }
  return std::adjacent_find(values.begin(), values.end()) == values.end();
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

static bool EvalConstExpr(const Expr *expr, int &value,
                          const std::unordered_map<std::string, int> *resolved_globals = nullptr) {
  if (expr == nullptr) {
    return false;
  }
  if (expr->kind == Expr::Kind::Number) {
    value = expr->number;
    return true;
  }
  if (expr->kind == Expr::Kind::NilLiteral) {
    value = 0;
    return true;
  }
  if (expr->kind == Expr::Kind::BoolLiteral) {
    value = expr->bool_value ? 1 : 0;
    return true;
  }
  if (expr->kind == Expr::Kind::Identifier) {
    if (resolved_globals == nullptr) {
      return false;
    }
    auto it = resolved_globals->find(expr->ident);
    if (it == resolved_globals->end()) {
      return false;
    }
    value = it->second;
    return true;
  }
  if (expr->kind == Expr::Kind::Conditional) {
    if (expr->left == nullptr || expr->right == nullptr || expr->third == nullptr) {
      return false;
    }
    int cond_value = 0;
    if (!EvalConstExpr(expr->left.get(), cond_value, resolved_globals)) {
      return false;
    }
    if (cond_value != 0) {
      return EvalConstExpr(expr->right.get(), value, resolved_globals);
    }
    return EvalConstExpr(expr->third.get(), value, resolved_globals);
  }
  if (expr->kind != Expr::Kind::Binary || expr->left == nullptr || expr->right == nullptr) {
    return false;
  }
  int lhs = 0;
  int rhs = 0;
  if (!EvalConstExpr(expr->left.get(), lhs, resolved_globals) ||
      !EvalConstExpr(expr->right.get(), rhs, resolved_globals)) {
    return false;
  }
  if (expr->op == "+") {
    value = lhs + rhs;
    return true;
  }
  if (expr->op == "-") {
    value = lhs - rhs;
    return true;
  }
  if (expr->op == "*") {
    value = lhs * rhs;
    return true;
  }
  if (expr->op == "/") {
    if (rhs == 0) {
      return false;
    }
    value = lhs / rhs;
    return true;
  }
  if (expr->op == "%") {
    if (rhs == 0) {
      return false;
    }
    value = lhs % rhs;
    return true;
  }
  if (expr->op == "&") {
    value = lhs & rhs;
    return true;
  }
  if (expr->op == "|") {
    value = lhs | rhs;
    return true;
  }
  if (expr->op == "^") {
    value = lhs ^ rhs;
    return true;
  }
  if (expr->op == "<<" || expr->op == ">>") {
    if (rhs < 0 || rhs > 31) {
      return false;
    }
    value = expr->op == "<<" ? (lhs << rhs) : (lhs >> rhs);
    return true;
  }
  if (expr->op == "==") {
    value = lhs == rhs ? 1 : 0;
    return true;
  }
  if (expr->op == "!=") {
    value = lhs != rhs ? 1 : 0;
    return true;
  }
  if (expr->op == "<") {
    value = lhs < rhs ? 1 : 0;
    return true;
  }
  if (expr->op == "<=") {
    value = lhs <= rhs ? 1 : 0;
    return true;
  }
  if (expr->op == ">") {
    value = lhs > rhs ? 1 : 0;
    return true;
  }
  if (expr->op == ">=") {
    value = lhs >= rhs ? 1 : 0;
    return true;
  }
  if (expr->op == "&&") {
    value = (lhs != 0 && rhs != 0) ? 1 : 0;
    return true;
  }
  if (expr->op == "||") {
    value = (lhs != 0 || rhs != 0) ? 1 : 0;
    return true;
  }
  return false;
}

bool ResolveGlobalInitializerValues(const std::vector<Objc3ParsedGlobalDecl> &globals, std::vector<int> &values) {
  values.clear();
  values.reserve(globals.size());
  std::unordered_map<std::string, int> resolved_globals;
  for (const auto &global : globals) {
    int value = 0;
    if (!EvalConstExpr(global.value.get(), value, &resolved_globals)) {
      return false;
    }
    values.push_back(value);
    resolved_globals[global.name] = value;
  }
  return true;
}

static SemanticTypeInfo ScopeLookupType(const std::vector<SemanticScope> &scopes, const std::string &name) {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    auto found = it->find(name);
    if (found != it->end()) {
      return found->second;
    }
  }
  return MakeScalarSemanticType(ValueType::Unknown);
}

static bool SupportsGenericParamTypeSuffix(const FuncParam &param) {
  return param.id_spelling || param.class_spelling || param.instancetype_spelling;
}

static bool SupportsNullabilityParamTypeSuffix(const FuncParam &param) {
  return param.id_spelling || param.class_spelling || param.instancetype_spelling;
}

static bool SupportsOwnershipQualifierParamTypeSuffix(const FuncParam &param) {
  return param.id_spelling || param.class_spelling || param.instancetype_spelling ||
         param.object_pointer_type_spelling;
}

static bool SupportsPointerParamTypeDeclarator(const FuncParam &param) {
  return param.id_spelling || param.class_spelling || param.instancetype_spelling;
}

static bool SupportsGenericReturnTypeSuffix(const FunctionDecl &fn) {
  return fn.return_id_spelling || fn.return_class_spelling || fn.return_instancetype_spelling;
}

static bool SupportsGenericReturnTypeSuffix(const Objc3MethodDecl &method) {
  return method.return_id_spelling || method.return_class_spelling || method.return_instancetype_spelling;
}

static bool SupportsNullabilityReturnTypeSuffix(const FunctionDecl &fn) {
  return fn.return_id_spelling || fn.return_class_spelling || fn.return_instancetype_spelling;
}

static bool SupportsNullabilityReturnTypeSuffix(const Objc3MethodDecl &method) {
  return method.return_id_spelling || method.return_class_spelling || method.return_instancetype_spelling;
}

static bool SupportsOwnershipQualifierReturnTypeSuffix(const FunctionDecl &fn) {
  return fn.return_id_spelling || fn.return_class_spelling || fn.return_instancetype_spelling ||
         fn.return_object_pointer_type_spelling;
}

static bool SupportsOwnershipQualifierReturnTypeSuffix(const Objc3MethodDecl &method) {
  return method.return_id_spelling || method.return_class_spelling || method.return_instancetype_spelling ||
         method.return_object_pointer_type_spelling;
}

static bool SupportsPointerReturnTypeDeclarator(const FunctionDecl &fn) {
  return fn.return_id_spelling || fn.return_class_spelling || fn.return_instancetype_spelling;
}

static bool SupportsPointerReturnTypeDeclarator(const Objc3MethodDecl &method) {
  return method.return_id_spelling || method.return_class_spelling || method.return_instancetype_spelling;
}

static bool SupportsGenericPropertyTypeSuffix(const Objc3PropertyDecl &property) {
  return property.id_spelling || property.class_spelling || property.instancetype_spelling;
}

static bool SupportsNullabilityPropertyTypeSuffix(const Objc3PropertyDecl &property) {
  return property.id_spelling || property.class_spelling || property.instancetype_spelling;
}

static bool SupportsOwnershipQualifierPropertyTypeSuffix(const Objc3PropertyDecl &property) {
  return property.id_spelling || property.class_spelling || property.instancetype_spelling ||
         property.object_pointer_type_spelling;
}

static bool SupportsPointerPropertyTypeDeclarator(const Objc3PropertyDecl &property) {
  return property.id_spelling || property.class_spelling || property.instancetype_spelling;
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
         name == "strong" || name == "weak" || name == "assign" || name == "getter" || name == "setter";
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

static ProtocolCompositionInfo BuildProtocolCompositionInfoFromParam(const FuncParam &param) {
  ProtocolCompositionInfo info;
  if (!param.has_generic_suffix) {
    return info;
  }

  const ProtocolCompositionParseResult parsed = ParseProtocolCompositionSuffixText(param.generic_suffix_text);
  info.has_protocol_composition = true;
  info.names_lexicographic = parsed.names_lexicographic;
  info.has_invalid_protocol_composition = !SupportsGenericParamTypeSuffix(param) || !parsed.IsValid();
  return info;
}

static ProtocolCompositionInfo BuildProtocolCompositionInfoFromFunctionReturn(const FunctionDecl &fn) {
  ProtocolCompositionInfo info;
  if (!fn.has_return_generic_suffix) {
    return info;
  }

  const ProtocolCompositionParseResult parsed = ParseProtocolCompositionSuffixText(fn.return_generic_suffix_text);
  info.has_protocol_composition = true;
  info.names_lexicographic = parsed.names_lexicographic;
  info.has_invalid_protocol_composition = !SupportsGenericReturnTypeSuffix(fn) || !parsed.IsValid();
  return info;
}

static ProtocolCompositionInfo BuildProtocolCompositionInfoFromMethodReturn(const Objc3MethodDecl &method) {
  ProtocolCompositionInfo info;
  if (!method.has_return_generic_suffix) {
    return info;
  }

  const ProtocolCompositionParseResult parsed = ParseProtocolCompositionSuffixText(method.return_generic_suffix_text);
  info.has_protocol_composition = true;
  info.names_lexicographic = parsed.names_lexicographic;
  info.has_invalid_protocol_composition = !SupportsGenericReturnTypeSuffix(method) || !parsed.IsValid();
  return info;
}

static void ValidateProtocolCompositionSuffix(const std::string &suffix_text,
                                              unsigned line,
                                              unsigned column,
                                              const std::string &context,
                                              std::vector<std::string> &diagnostics) {
  const ProtocolCompositionParseResult parsed = ParseProtocolCompositionSuffixText(suffix_text);
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

static void ValidateParameterTypeSuffixes(const FunctionDecl &fn, std::vector<std::string> &diagnostics) {
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

static void ValidateReturnTypeSuffixes(const FunctionDecl &fn, std::vector<std::string> &diagnostics) {
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
                                                std::vector<std::string> &diagnostics) {
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
                                             std::vector<std::string> &diagnostics) {
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
                                         std::vector<std::string> &diagnostics) {
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
                                           std::vector<std::string> &diagnostics) {
  Objc3PropertyInfo info;
  info.type = property.type;
  info.is_vector = property.vector_spelling;
  info.vector_base_spelling = property.vector_base_spelling;
  info.vector_lane_count = property.vector_lane_count;
  info.id_spelling = property.id_spelling;
  info.class_spelling = property.class_spelling;
  info.instancetype_spelling = property.instancetype_spelling;
  info.object_pointer_type_spelling = property.object_pointer_type_spelling;
  info.has_generic_suffix = property.has_generic_suffix;
  info.has_pointer_declarator = property.has_pointer_declarator;
  info.has_nullability_suffix = !property.nullability_suffix_tokens.empty();
  info.has_ownership_qualifier = property.has_ownership_qualifier;
  info.ownership_insert_retain = property.ownership_insert_retain;
  info.ownership_insert_release = property.ownership_insert_release;
  info.ownership_insert_autorelease = property.ownership_insert_autorelease;
  info.ownership_is_weak_reference = property.ownership_is_weak_reference;
  info.ownership_is_unowned_reference = property.ownership_is_unowned_reference;
  info.ownership_is_unowned_safe_reference = property.ownership_is_unowned_safe_reference;
  info.ownership_arc_diagnostic_candidate = property.ownership_arc_diagnostic_candidate;
  info.ownership_arc_fixit_available = property.ownership_arc_fixit_available;
  info.ownership_arc_diagnostic_profile = property.ownership_arc_diagnostic_profile;
  info.ownership_arc_fixit_hint = property.ownership_arc_fixit_hint;
  info.has_invalid_generic_suffix = HasInvalidGenericPropertyTypeSuffix(property);
  info.has_invalid_pointer_declarator = HasInvalidPointerPropertyTypeDeclarator(property);
  info.has_invalid_nullability_suffix = HasInvalidNullabilityPropertyTypeSuffix(property);
  info.has_invalid_ownership_qualifier = HasInvalidOwnershipQualifierPropertyTypeSuffix(property);
  info.has_invalid_type_suffix = HasInvalidPropertyTypeSuffix(property);
  info.attribute_entries = property.attributes.size();
  info.is_readonly = property.is_readonly;
  info.is_readwrite = property.is_readwrite;
  info.is_atomic = property.is_atomic;
  info.is_nonatomic = property.is_nonatomic;
  info.is_copy = property.is_copy;
  info.is_strong = property.is_strong;
  info.is_weak = property.is_weak;
  info.is_unowned = property.is_unowned;
  info.is_assign = property.is_assign;
  info.has_getter = property.has_getter;
  info.has_setter = property.has_setter;
  info.getter_selector = TrimAsciiWhitespace(property.getter_selector);
  info.setter_selector = TrimAsciiWhitespace(property.setter_selector);

  std::unordered_map<std::string, std::size_t> attribute_name_counts;
  for (const auto &attribute : property.attributes) {
    info.attribute_names_lexicographic.push_back(attribute.name);
    const std::size_t count = ++attribute_name_counts[attribute.name];
    bool invalid_attribute = false;
    const auto emit_invalid_attribute = [&](const std::string &message) {
      diagnostics.push_back(MakeDiag(attribute.line, attribute.column, "O3S206", message));
      invalid_attribute = true;
    };

    if (!IsKnownPropertyAttributeName(attribute.name)) {
      info.has_unknown_attribute = true;
      emit_invalid_attribute("type mismatch: unknown @property attribute '" + attribute.name + "' for property '" +
                             property.name + "' in " + owner_kind + " '" + owner_name + "'");
    }
    if (count > 1u) {
      info.has_duplicate_attribute = true;
      emit_invalid_attribute("type mismatch: duplicate @property attribute '" + attribute.name +
                             "' for property '" + property.name + "' in " + owner_kind + " '" + owner_name + "'");
    }
    if ((attribute.name != "getter" && attribute.name != "setter") && attribute.has_value) {
      emit_invalid_attribute("type mismatch: @property attribute '" + attribute.name +
                             "' must not specify a value for property '" + property.name + "' in " + owner_kind +
                             " '" + owner_name + "'");
    }
    if ((attribute.name == "getter" || attribute.name == "setter") &&
        (!attribute.has_value || TrimAsciiWhitespace(attribute.value).empty())) {
      emit_invalid_attribute("type mismatch: @property accessor attribute '" + attribute.name +
                             "' requires a selector value for property '" + property.name + "' in " + owner_kind +
                             " '" + owner_name + "'");
    }

    if (invalid_attribute) {
      ++info.invalid_attribute_entries;
    }
  }
  std::sort(info.attribute_names_lexicographic.begin(), info.attribute_names_lexicographic.end());

  const auto emit_property_contract_violation = [&](unsigned line, unsigned column, const std::string &message) {
    diagnostics.push_back(MakeDiag(line, column, "O3S206", message));
    ++info.property_contract_violations;
  };

  if (info.has_getter) {
    if (info.getter_selector.empty() || !IsValidPropertyGetterSelector(info.getter_selector)) {
      info.has_accessor_selector_contract_violation = true;
      emit_property_contract_violation(
          property.line,
          property.column,
          "type mismatch: invalid @property getter selector '" +
              (info.getter_selector.empty() ? std::string("<empty>") : info.getter_selector) + "' for property '" +
              property.name + "' in " + owner_kind + " '" + owner_name + "'");
    }
  }
  if (info.has_setter) {
    if (info.setter_selector.empty() || !IsValidPropertySetterSelector(info.setter_selector)) {
      info.has_accessor_selector_contract_violation = true;
      emit_property_contract_violation(
          property.line,
          property.column,
          "type mismatch: invalid @property setter selector '" +
              (info.setter_selector.empty() ? std::string("<empty>") : info.setter_selector) + "' for property '" +
              property.name + "' in " + owner_kind + " '" + owner_name + "'");
    }
  }
  if (info.is_readonly && info.is_readwrite) {
    info.has_readwrite_conflict = true;
    emit_property_contract_violation(
        property.line,
        property.column,
        "type mismatch: @property modifiers 'readonly' and 'readwrite' conflict for property '" + property.name +
            "' in " + owner_kind + " '" + owner_name + "'");
  }
  if (info.is_atomic && info.is_nonatomic) {
    info.has_atomicity_conflict = true;
    emit_property_contract_violation(
        property.line,
        property.column,
        "type mismatch: @property modifiers 'atomic' and 'nonatomic' conflict for property '" + property.name +
            "' in " + owner_kind + " '" + owner_name + "'");
  }
  const std::size_t ownership_modifiers = (info.is_copy ? 1u : 0u) + (info.is_strong ? 1u : 0u) +
                                          (info.is_weak ? 1u : 0u) + (info.is_unowned ? 1u : 0u) +
                                          (info.is_assign ? 1u : 0u);
  if (ownership_modifiers > 1u) {
    info.has_ownership_conflict = true;
    emit_property_contract_violation(
        property.line,
        property.column,
        "type mismatch: @property ownership modifiers conflict for property '" + property.name + "' in " + owner_kind +
            " '" + owner_name + "'");
  }
  info.has_weak_unowned_conflict = property.has_weak_unowned_conflict || (info.is_weak && info.is_unowned);
  if (info.has_weak_unowned_conflict) {
    emit_property_contract_violation(
        property.line,
        property.column,
        "type mismatch: @property ownership modifiers 'weak' and 'unowned' conflict for property '" + property.name +
            "' in " + owner_kind + " '" + owner_name + "'");
  }
  if (info.is_readonly && info.has_setter) {
    info.has_accessor_selector_contract_violation = true;
    emit_property_contract_violation(
        property.line,
        property.column,
        "type mismatch: readonly property '" + property.name + "' in " + owner_kind + " '" + owner_name +
            "' must not declare a setter modifier");
  }

  info.has_invalid_attribute_contract =
      info.has_unknown_attribute || info.has_duplicate_attribute || info.has_readwrite_conflict ||
      info.has_atomicity_conflict || info.has_ownership_conflict || info.has_accessor_selector_contract_violation ||
      info.invalid_attribute_entries > 0 || info.property_contract_violations > 0;
  return info;
}

static bool IsCompatiblePropertySignature(const Objc3PropertyInfo &lhs, const Objc3PropertyInfo &rhs) {
  return lhs.type == rhs.type &&
         lhs.is_vector == rhs.is_vector &&
         lhs.vector_base_spelling == rhs.vector_base_spelling &&
         lhs.vector_lane_count == rhs.vector_lane_count &&
         lhs.id_spelling == rhs.id_spelling &&
         lhs.class_spelling == rhs.class_spelling &&
         lhs.instancetype_spelling == rhs.instancetype_spelling &&
         lhs.is_readonly == rhs.is_readonly &&
         lhs.is_readwrite == rhs.is_readwrite &&
         lhs.is_atomic == rhs.is_atomic &&
         lhs.is_nonatomic == rhs.is_nonatomic &&
         lhs.is_copy == rhs.is_copy &&
         lhs.is_strong == rhs.is_strong &&
         lhs.is_weak == rhs.is_weak &&
         lhs.is_unowned == rhs.is_unowned &&
         lhs.is_assign == rhs.is_assign &&
         lhs.has_getter == rhs.has_getter &&
         lhs.has_setter == rhs.has_setter &&
         lhs.getter_selector == rhs.getter_selector &&
         lhs.setter_selector == rhs.setter_selector;
}

static Objc3MethodInfo BuildMethodInfo(const Objc3MethodDecl &method,
                                       const MethodSelectorNormalizationContractInfo &selector_contract) {
  Objc3MethodInfo info;
  info.selector_normalized = selector_contract.normalized_selector;
  info.selector_piece_count = selector_contract.selector_piece_count;
  info.selector_parameter_piece_count = selector_contract.selector_parameter_piece_count;
  info.selector_contract_normalized = selector_contract.selector_contract_normalized;
  info.selector_had_pieceless_form = selector_contract.selector_had_pieceless_form;
  info.selector_has_spelling_mismatch = selector_contract.selector_has_spelling_mismatch;
  info.selector_has_arity_mismatch = selector_contract.selector_has_arity_mismatch;
  info.selector_has_parameter_linkage_mismatch = selector_contract.selector_has_parameter_linkage_mismatch;
  info.selector_has_normalization_flag_mismatch = selector_contract.selector_has_normalization_flag_mismatch;
  info.selector_has_missing_piece_keyword = selector_contract.selector_has_missing_piece_keyword;
  info.arity = method.params.size();
  info.param_types.reserve(method.params.size());
  info.param_is_vector.reserve(method.params.size());
  info.param_vector_base_spelling.reserve(method.params.size());
  info.param_vector_lane_count.reserve(method.params.size());
  info.param_has_generic_suffix.reserve(method.params.size());
  info.param_has_pointer_declarator.reserve(method.params.size());
  info.param_has_nullability_suffix.reserve(method.params.size());
  info.param_has_ownership_qualifier.reserve(method.params.size());
  info.param_object_pointer_type_spelling.reserve(method.params.size());
  info.param_has_invalid_generic_suffix.reserve(method.params.size());
  info.param_has_invalid_pointer_declarator.reserve(method.params.size());
  info.param_has_invalid_nullability_suffix.reserve(method.params.size());
  info.param_has_invalid_ownership_qualifier.reserve(method.params.size());
  info.param_has_invalid_type_suffix.reserve(method.params.size());
  info.param_ownership_insert_retain.reserve(method.params.size());
  info.param_ownership_insert_release.reserve(method.params.size());
  info.param_ownership_insert_autorelease.reserve(method.params.size());
  info.param_ownership_is_weak_reference.reserve(method.params.size());
  info.param_ownership_is_unowned_reference.reserve(method.params.size());
  info.param_ownership_is_unowned_safe_reference.reserve(method.params.size());
  info.param_ownership_arc_diagnostic_candidate.reserve(method.params.size());
  info.param_ownership_arc_fixit_available.reserve(method.params.size());
  info.param_ownership_arc_diagnostic_profile.reserve(method.params.size());
  info.param_ownership_arc_fixit_hint.reserve(method.params.size());
  info.param_has_protocol_composition.reserve(method.params.size());
  info.param_protocol_composition_lexicographic.reserve(method.params.size());
  info.param_has_invalid_protocol_composition.reserve(method.params.size());
  for (const auto &param : method.params) {
    const ProtocolCompositionInfo protocol_composition = BuildProtocolCompositionInfoFromParam(param);
    info.param_types.push_back(param.type);
    info.param_is_vector.push_back(param.vector_spelling);
    info.param_vector_base_spelling.push_back(param.vector_base_spelling);
    info.param_vector_lane_count.push_back(param.vector_lane_count);
    info.param_has_generic_suffix.push_back(param.has_generic_suffix);
    info.param_has_pointer_declarator.push_back(param.has_pointer_declarator);
    info.param_has_nullability_suffix.push_back(!param.nullability_suffix_tokens.empty());
    info.param_has_ownership_qualifier.push_back(param.has_ownership_qualifier);
    info.param_object_pointer_type_spelling.push_back(param.object_pointer_type_spelling);
    info.param_has_invalid_generic_suffix.push_back(HasInvalidGenericParamTypeSuffix(param));
    info.param_has_invalid_pointer_declarator.push_back(HasInvalidPointerParamTypeDeclarator(param));
    info.param_has_invalid_nullability_suffix.push_back(HasInvalidNullabilityParamTypeSuffix(param));
    info.param_has_invalid_ownership_qualifier.push_back(HasInvalidOwnershipQualifierParamTypeSuffix(param));
    info.param_has_invalid_type_suffix.push_back(HasInvalidParamTypeSuffix(param));
    info.param_ownership_insert_retain.push_back(param.ownership_insert_retain);
    info.param_ownership_insert_release.push_back(param.ownership_insert_release);
    info.param_ownership_insert_autorelease.push_back(param.ownership_insert_autorelease);
    info.param_ownership_is_weak_reference.push_back(param.ownership_is_weak_reference);
    info.param_ownership_is_unowned_reference.push_back(param.ownership_is_unowned_reference);
    info.param_ownership_is_unowned_safe_reference.push_back(param.ownership_is_unowned_safe_reference);
    info.param_ownership_arc_diagnostic_candidate.push_back(param.ownership_arc_diagnostic_candidate);
    info.param_ownership_arc_fixit_available.push_back(param.ownership_arc_fixit_available);
    info.param_ownership_arc_diagnostic_profile.push_back(param.ownership_arc_diagnostic_profile);
    info.param_ownership_arc_fixit_hint.push_back(param.ownership_arc_fixit_hint);
    info.param_has_protocol_composition.push_back(protocol_composition.has_protocol_composition);
    info.param_protocol_composition_lexicographic.push_back(protocol_composition.names_lexicographic);
    info.param_has_invalid_protocol_composition.push_back(protocol_composition.has_invalid_protocol_composition);
  }
  const ProtocolCompositionInfo return_protocol_composition = BuildProtocolCompositionInfoFromMethodReturn(method);
  info.return_has_generic_suffix = method.has_return_generic_suffix;
  info.return_has_pointer_declarator = method.has_return_pointer_declarator;
  info.return_has_nullability_suffix = !method.return_nullability_suffix_tokens.empty();
  info.return_has_ownership_qualifier = method.has_return_ownership_qualifier;
  info.return_object_pointer_type_spelling = method.return_object_pointer_type_spelling;
  info.return_has_invalid_generic_suffix = HasInvalidGenericReturnTypeSuffix(method);
  info.return_has_invalid_pointer_declarator = HasInvalidPointerReturnTypeDeclarator(method);
  info.return_has_invalid_nullability_suffix = HasInvalidNullabilityReturnTypeSuffix(method);
  info.return_has_invalid_ownership_qualifier = HasInvalidOwnershipQualifierReturnTypeSuffix(method);
  info.return_has_invalid_type_suffix = info.return_has_invalid_generic_suffix ||
                                        info.return_has_invalid_pointer_declarator ||
                                        info.return_has_invalid_nullability_suffix ||
                                        info.return_has_invalid_ownership_qualifier;
  info.return_ownership_insert_retain = method.return_ownership_insert_retain;
  info.return_ownership_insert_release = method.return_ownership_insert_release;
  info.return_ownership_insert_autorelease = method.return_ownership_insert_autorelease;
  info.return_ownership_is_weak_reference = method.return_ownership_is_weak_reference;
  info.return_ownership_is_unowned_reference = method.return_ownership_is_unowned_reference;
  info.return_ownership_is_unowned_safe_reference = method.return_ownership_is_unowned_safe_reference;
  info.return_ownership_arc_diagnostic_candidate = method.return_ownership_arc_diagnostic_candidate;
  info.return_ownership_arc_fixit_available = method.return_ownership_arc_fixit_available;
  info.return_ownership_arc_diagnostic_profile = method.return_ownership_arc_diagnostic_profile;
  info.return_ownership_arc_fixit_hint = method.return_ownership_arc_fixit_hint;
  info.return_type = method.return_type;
  info.return_is_vector = method.return_vector_spelling;
  info.return_vector_base_spelling = method.return_vector_base_spelling;
  info.return_vector_lane_count = method.return_vector_lane_count;
  info.return_has_protocol_composition = return_protocol_composition.has_protocol_composition;
  info.return_protocol_composition_lexicographic = return_protocol_composition.names_lexicographic;
  info.return_has_invalid_protocol_composition = return_protocol_composition.has_invalid_protocol_composition;
  info.is_class_method = method.is_class_method;
  info.has_definition = method.has_body;
  return info;
}

static bool IsCompatibleMethodSignature(const Objc3MethodInfo &lhs, const Objc3MethodInfo &rhs) {
  if (lhs.arity != rhs.arity || lhs.return_type != rhs.return_type || lhs.return_is_vector != rhs.return_is_vector ||
      lhs.is_class_method != rhs.is_class_method ||
      lhs.return_has_ownership_qualifier != rhs.return_has_ownership_qualifier ||
      lhs.return_ownership_insert_retain != rhs.return_ownership_insert_retain ||
      lhs.return_ownership_insert_release != rhs.return_ownership_insert_release ||
      lhs.return_ownership_insert_autorelease != rhs.return_ownership_insert_autorelease ||
      lhs.return_ownership_is_weak_reference != rhs.return_ownership_is_weak_reference ||
      lhs.return_ownership_is_unowned_reference != rhs.return_ownership_is_unowned_reference ||
      lhs.return_ownership_is_unowned_safe_reference != rhs.return_ownership_is_unowned_safe_reference ||
      lhs.return_ownership_arc_diagnostic_candidate != rhs.return_ownership_arc_diagnostic_candidate ||
      lhs.return_ownership_arc_fixit_available != rhs.return_ownership_arc_fixit_available ||
      lhs.return_ownership_arc_diagnostic_profile != rhs.return_ownership_arc_diagnostic_profile ||
      lhs.return_ownership_arc_fixit_hint != rhs.return_ownership_arc_fixit_hint) {
    return false;
  }
  if (lhs.return_is_vector &&
      (lhs.return_vector_base_spelling != rhs.return_vector_base_spelling ||
       lhs.return_vector_lane_count != rhs.return_vector_lane_count)) {
    return false;
  }
  if (lhs.param_has_ownership_qualifier.size() != lhs.arity ||
      rhs.param_has_ownership_qualifier.size() != rhs.arity) {
    return false;
  }
  if (lhs.param_ownership_insert_retain.size() != lhs.arity ||
      rhs.param_ownership_insert_retain.size() != rhs.arity ||
      lhs.param_ownership_insert_release.size() != lhs.arity ||
      rhs.param_ownership_insert_release.size() != rhs.arity ||
      lhs.param_ownership_insert_autorelease.size() != lhs.arity ||
      rhs.param_ownership_insert_autorelease.size() != rhs.arity ||
      lhs.param_ownership_is_weak_reference.size() != lhs.arity ||
      rhs.param_ownership_is_weak_reference.size() != rhs.arity ||
      lhs.param_ownership_is_unowned_reference.size() != lhs.arity ||
      rhs.param_ownership_is_unowned_reference.size() != rhs.arity ||
      lhs.param_ownership_is_unowned_safe_reference.size() != lhs.arity ||
      rhs.param_ownership_is_unowned_safe_reference.size() != rhs.arity ||
      lhs.param_ownership_arc_diagnostic_candidate.size() != lhs.arity ||
      rhs.param_ownership_arc_diagnostic_candidate.size() != rhs.arity ||
      lhs.param_ownership_arc_fixit_available.size() != lhs.arity ||
      rhs.param_ownership_arc_fixit_available.size() != rhs.arity ||
      lhs.param_ownership_arc_diagnostic_profile.size() != lhs.arity ||
      rhs.param_ownership_arc_diagnostic_profile.size() != rhs.arity ||
      lhs.param_ownership_arc_fixit_hint.size() != lhs.arity ||
      rhs.param_ownership_arc_fixit_hint.size() != rhs.arity) {
    return false;
  }
  if (!AreEquivalentProtocolCompositions(lhs.return_has_protocol_composition,
                                         lhs.return_protocol_composition_lexicographic,
                                         rhs.return_has_protocol_composition,
                                         rhs.return_protocol_composition_lexicographic)) {
    return false;
  }
  for (std::size_t i = 0; i < lhs.arity; ++i) {
    if (i >= lhs.param_types.size() || i >= lhs.param_is_vector.size() || i >= lhs.param_vector_base_spelling.size() ||
        i >= lhs.param_vector_lane_count.size() || i >= lhs.param_has_protocol_composition.size() ||
        i >= lhs.param_protocol_composition_lexicographic.size() || i >= rhs.param_types.size() ||
        i >= rhs.param_is_vector.size() || i >= rhs.param_vector_base_spelling.size() ||
        i >= rhs.param_vector_lane_count.size() || i >= rhs.param_has_protocol_composition.size() ||
        i >= rhs.param_protocol_composition_lexicographic.size()) {
      return false;
    }
    if (i >= lhs.param_has_ownership_qualifier.size() || i >= rhs.param_has_ownership_qualifier.size()) {
      return false;
    }
    if (lhs.param_types[i] != rhs.param_types[i] || lhs.param_is_vector[i] != rhs.param_is_vector[i]) {
      return false;
    }
    if (lhs.param_has_ownership_qualifier[i] != rhs.param_has_ownership_qualifier[i]) {
      return false;
    }
    if (lhs.param_ownership_insert_retain[i] != rhs.param_ownership_insert_retain[i] ||
        lhs.param_ownership_insert_release[i] != rhs.param_ownership_insert_release[i] ||
        lhs.param_ownership_insert_autorelease[i] != rhs.param_ownership_insert_autorelease[i] ||
        lhs.param_ownership_is_weak_reference[i] != rhs.param_ownership_is_weak_reference[i] ||
        lhs.param_ownership_is_unowned_reference[i] != rhs.param_ownership_is_unowned_reference[i] ||
        lhs.param_ownership_is_unowned_safe_reference[i] != rhs.param_ownership_is_unowned_safe_reference[i] ||
        lhs.param_ownership_arc_diagnostic_candidate[i] != rhs.param_ownership_arc_diagnostic_candidate[i] ||
        lhs.param_ownership_arc_fixit_available[i] != rhs.param_ownership_arc_fixit_available[i] ||
        lhs.param_ownership_arc_diagnostic_profile[i] != rhs.param_ownership_arc_diagnostic_profile[i] ||
        lhs.param_ownership_arc_fixit_hint[i] != rhs.param_ownership_arc_fixit_hint[i]) {
      return false;
    }
    if (lhs.param_is_vector[i] &&
        (lhs.param_vector_base_spelling[i] != rhs.param_vector_base_spelling[i] ||
         lhs.param_vector_lane_count[i] != rhs.param_vector_lane_count[i])) {
      return false;
    }
    if (!AreEquivalentProtocolCompositions(lhs.param_has_protocol_composition[i],
                                           lhs.param_protocol_composition_lexicographic[i],
                                           rhs.param_has_protocol_composition[i],
                                           rhs.param_protocol_composition_lexicographic[i])) {
      return false;
    }
  }
  return true;
}

static SemanticTypeInfo ValidateMessageSendExpr(const Expr *expr,
                                                const std::vector<SemanticScope> &scopes,
                                                const std::unordered_map<std::string, ValueType> &globals,
                                                const std::unordered_map<std::string, FunctionInfo> &functions,
                                                std::vector<std::string> &diagnostics,
                                                std::size_t max_message_send_args);

static SemanticTypeInfo ValidateExpr(const Expr *expr, const std::vector<SemanticScope> &scopes,
                                     const std::unordered_map<std::string, ValueType> &globals,
                                     const std::unordered_map<std::string, FunctionInfo> &functions,
                                     std::vector<std::string> &diagnostics,
                                     std::size_t max_message_send_args) {
  if (expr == nullptr) {
    return MakeScalarSemanticType(ValueType::Unknown);
  }
  switch (expr->kind) {
    case Expr::Kind::Number:
      return MakeScalarSemanticType(ValueType::I32);
    case Expr::Kind::BoolLiteral:
      return MakeScalarSemanticType(ValueType::Bool);
    case Expr::Kind::NilLiteral:
      return MakeScalarSemanticType(ValueType::I32);
    case Expr::Kind::Identifier: {
      const SemanticTypeInfo local_type = ScopeLookupType(scopes, expr->ident);
      if (!IsUnknownSemanticType(local_type)) {
        return local_type;
      }
      auto global_it = globals.find(expr->ident);
      if (global_it != globals.end()) {
        return MakeSemanticTypeFromGlobal(global_it->second);
      }
      if (functions.find(expr->ident) != functions.end()) {
        diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                       "type mismatch: function '" + expr->ident +
                                           "' cannot be used as a value"));
        return MakeScalarSemanticType(ValueType::Function);
      }
      diagnostics.push_back(
          MakeDiag(expr->line, expr->column, "O3S202", "undefined identifier '" + expr->ident + "'"));
      return MakeScalarSemanticType(ValueType::Unknown);
    }
    case Expr::Kind::Binary: {
      const SemanticTypeInfo lhs =
          ValidateExpr(expr->left.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      const SemanticTypeInfo rhs =
          ValidateExpr(expr->right.get(), scopes, globals, functions, diagnostics, max_message_send_args);

      if (expr->op == "+" || expr->op == "-" || expr->op == "*" || expr->op == "/" || expr->op == "%") {
        if (!IsUnknownSemanticType(lhs) && (lhs.is_vector || lhs.type != ValueType::I32)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for arithmetic lhs, got '" +
                                             SemanticTypeName(lhs) + "'"));
        }
        if (!IsUnknownSemanticType(rhs) && (rhs.is_vector || rhs.type != ValueType::I32)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for arithmetic rhs, got '" +
                                             SemanticTypeName(rhs) + "'"));
        }
        return MakeScalarSemanticType(ValueType::I32);
      }

      if (expr->op == "&" || expr->op == "|" || expr->op == "^" || expr->op == "<<" || expr->op == ">>") {
        if (!IsUnknownSemanticType(lhs) && (lhs.is_vector || lhs.type != ValueType::I32)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for bitwise lhs, got '" +
                                             SemanticTypeName(lhs) + "'"));
        }
        if (!IsUnknownSemanticType(rhs) && (rhs.is_vector || rhs.type != ValueType::I32)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for bitwise rhs, got '" +
                                             SemanticTypeName(rhs) + "'"));
        }
        return MakeScalarSemanticType(ValueType::I32);
      }

      if (expr->op == "==" || expr->op == "!=") {
        if (lhs.is_vector || rhs.is_vector) {
          if (!IsUnknownSemanticType(lhs) && !IsUnknownSemanticType(rhs) && !IsSameSemanticType(lhs, rhs)) {
            diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                           "type mismatch: equality compares '" + SemanticTypeName(lhs) +
                                               "' with '" + SemanticTypeName(rhs) + "'"));
          }
          return MakeScalarSemanticType(ValueType::Bool);
        }

        const bool bool_to_i32_literal =
            (lhs.type == ValueType::Bool && rhs.type == ValueType::I32 && IsBoolLikeI32Literal(expr->right.get())) ||
            (rhs.type == ValueType::Bool && lhs.type == ValueType::I32 && IsBoolLikeI32Literal(expr->left.get()));
        if (!IsUnknownSemanticType(lhs) && !IsUnknownSemanticType(rhs) && lhs.type != rhs.type && !bool_to_i32_literal) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: equality compares '" + SemanticTypeName(lhs) +
                                             "' with '" + SemanticTypeName(rhs) + "'"));
        }
        return MakeScalarSemanticType(ValueType::Bool);
      }

      if (expr->op == "<" || expr->op == "<=" || expr->op == ">" || expr->op == ">=") {
        if (!IsUnknownSemanticType(lhs) && (lhs.is_vector || lhs.type != ValueType::I32)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for relational lhs, got '" +
                                             SemanticTypeName(lhs) + "'"));
        }
        if (!IsUnknownSemanticType(rhs) && (rhs.is_vector || rhs.type != ValueType::I32)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for relational rhs, got '" +
                                             SemanticTypeName(rhs) + "'"));
        }
        return MakeScalarSemanticType(ValueType::Bool);
      }

      if (expr->op == "&&" || expr->op == "||") {
        if (!IsUnknownSemanticType(lhs) && (lhs.is_vector || (lhs.type != ValueType::Bool && lhs.type != ValueType::I32))) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected bool for logical lhs, got '" +
                                             SemanticTypeName(lhs) + "'"));
        }
        if (!IsUnknownSemanticType(rhs) && (rhs.is_vector || (rhs.type != ValueType::Bool && rhs.type != ValueType::I32))) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected bool for logical rhs, got '" +
                                             SemanticTypeName(rhs) + "'"));
        }
        return MakeScalarSemanticType(ValueType::Bool);
      }

      return MakeScalarSemanticType(ValueType::Unknown);
    }
    case Expr::Kind::Conditional: {
      if (expr->left == nullptr || expr->right == nullptr || expr->third == nullptr) {
        return MakeScalarSemanticType(ValueType::Unknown);
      }

      const SemanticTypeInfo condition_type =
          ValidateExpr(expr->left.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      if (!IsUnknownSemanticType(condition_type) && !IsScalarBoolCompatibleType(condition_type)) {
        diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                       "type mismatch: conditional condition must be bool-compatible"));
      }

      const SemanticTypeInfo then_type =
          ValidateExpr(expr->right.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      const SemanticTypeInfo else_type =
          ValidateExpr(expr->third.get(), scopes, globals, functions, diagnostics, max_message_send_args);

      if (IsUnknownSemanticType(then_type)) {
        return else_type;
      }
      if (IsUnknownSemanticType(else_type)) {
        return then_type;
      }
      const bool then_scalar = IsScalarSemanticType(then_type) &&
                               (then_type.type == ValueType::I32 || then_type.type == ValueType::Bool);
      const bool else_scalar = IsScalarSemanticType(else_type) &&
                               (else_type.type == ValueType::I32 || else_type.type == ValueType::Bool);
      if (then_scalar && else_scalar) {
        if (then_type.type == else_type.type) {
          return then_type;
        }
        return MakeScalarSemanticType(ValueType::I32);
      }
      if (!IsSameSemanticType(then_type, else_type)) {
        diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                       "type mismatch: conditional branches must be type-compatible"));
      }
      return IsSameSemanticType(then_type, else_type) ? then_type : MakeScalarSemanticType(ValueType::Unknown);
    }
    case Expr::Kind::BlockLiteral: {
      const bool parameter_count_match = expr->block_parameter_names_lexicographic.size() == expr->block_parameter_count;
      const bool capture_count_match = expr->block_capture_names_lexicographic.size() == expr->block_capture_count;
      const bool parameters_deterministic =
          parameter_count_match && IsSortedUniqueStrings(expr->block_parameter_names_lexicographic);
      const bool captures_deterministic =
          capture_count_match && IsSortedUniqueStrings(expr->block_capture_names_lexicographic);

      if (!parameters_deterministic || !captures_deterministic) {
        diagnostics.push_back(MakeDiag(expr->line,
                                       expr->column,
                                       "O3S206",
                                       "type mismatch: block literal capture metadata must be deterministic"));
      }
      if (!expr->block_capture_set_deterministic) {
        diagnostics.push_back(MakeDiag(expr->line,
                                       expr->column,
                                       "O3S206",
                                       "type mismatch: block literal capture-set normalization failed"));
      }
      if (!expr->block_literal_is_normalized) {
        diagnostics.push_back(
            MakeDiag(expr->line, expr->column, "O3S206", "type mismatch: block literal semantic surface is not normalized"));
      }
      if (expr->block_capture_count > 0u && expr->block_capture_profile.empty()) {
        diagnostics.push_back(
            MakeDiag(expr->line, expr->column, "O3S206", "type mismatch: block literal capture profile is missing"));
      }
      return MakeScalarSemanticType(ValueType::Function);
    }
    case Expr::Kind::Call: {
      auto fn_it = functions.find(expr->ident);
      if (fn_it == functions.end()) {
        diagnostics.push_back(
            MakeDiag(expr->line, expr->column, "O3S203", "unknown function '" + expr->ident + "'"));
      } else if (fn_it->second.arity != expr->args.size()) {
        diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S204",
                                       "arity mismatch for function '" + expr->ident + "'"));
      }

      for (std::size_t i = 0; i < expr->args.size(); ++i) {
        const SemanticTypeInfo arg_type =
            ValidateExpr(expr->args[i].get(), scopes, globals, functions, diagnostics, max_message_send_args);
        if (fn_it != functions.end() && i < fn_it->second.param_types.size()) {
          if (i < fn_it->second.param_has_invalid_type_suffix.size() &&
              fn_it->second.param_has_invalid_type_suffix[i]) {
            continue;
          }
          const SemanticTypeInfo expected = MakeSemanticTypeFromFunctionInfoParam(fn_it->second, i);
          const bool bool_coercion =
              !expected.is_vector && expected.type == ValueType::Bool && !arg_type.is_vector && arg_type.type == ValueType::I32;
          if (!IsUnknownSemanticType(arg_type) && !IsUnknownSemanticType(expected) &&
              !IsSameSemanticType(arg_type, expected) &&
              !bool_coercion) {
            diagnostics.push_back(MakeDiag(expr->args[i]->line, expr->args[i]->column, "O3S206",
                                           "type mismatch: expected '" + SemanticTypeName(expected) +
                                               "' argument for parameter " + std::to_string(i) + " of '" +
                                               expr->ident + "', got '" + SemanticTypeName(arg_type) + "'"));
          }
        }
      }
      if (fn_it != functions.end()) {
        return MakeSemanticTypeFromFunctionInfoReturn(fn_it->second);
      }
      return MakeScalarSemanticType(ValueType::Unknown);
    }
    case Expr::Kind::MessageSend: {
      return ValidateMessageSendExpr(expr, scopes, globals, functions, diagnostics, max_message_send_args);
    }
  }
  return MakeScalarSemanticType(ValueType::Unknown);
}

static SemanticTypeInfo ValidateMessageSendExpr(const Expr *expr,
                                                const std::vector<SemanticScope> &scopes,
                                                const std::unordered_map<std::string, ValueType> &globals,
                                                const std::unordered_map<std::string, FunctionInfo> &functions,
                                                std::vector<std::string> &diagnostics,
                                                std::size_t max_message_send_args) {
  const SemanticTypeInfo receiver_type =
      ValidateExpr(expr->receiver.get(), scopes, globals, functions, diagnostics, max_message_send_args);
  const std::string selector = expr->selector.empty() ? "<unknown>" : expr->selector;
  if (!IsUnknownSemanticType(receiver_type) && !IsMessageI32CompatibleType(receiver_type)) {
    const unsigned diag_line = expr->receiver != nullptr ? expr->receiver->line : expr->line;
    const unsigned diag_column = expr->receiver != nullptr ? expr->receiver->column : expr->column;
    diagnostics.push_back(MakeDiag(diag_line, diag_column, "O3S207",
                                   "type mismatch: message receiver for selector '" + selector +
                                       "' must be i32-compatible, got '" +
                                       SemanticTypeName(receiver_type) + "'"));
  }

  if (expr->args.size() > max_message_send_args) {
    diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S208",
                                   "arity mismatch: message '" + selector + "' has " +
                                       std::to_string(expr->args.size()) +
                                       " argument(s); native frontend supports at most " +
                                       std::to_string(max_message_send_args)));
  }

  for (std::size_t i = 0; i < expr->args.size(); ++i) {
    const auto &arg = expr->args[i];
    const SemanticTypeInfo arg_type =
        ValidateExpr(arg.get(), scopes, globals, functions, diagnostics, max_message_send_args);
    if (!IsUnknownSemanticType(arg_type) && !IsMessageI32CompatibleType(arg_type)) {
      diagnostics.push_back(MakeDiag(arg->line, arg->column, "O3S209",
                                     "type mismatch: message argument " + std::to_string(i) +
                                         " for selector '" + selector +
                                         "' must be i32-compatible, got '" +
                                         SemanticTypeName(arg_type) + "'"));
    }
  }
  return MakeScalarSemanticType(ValueType::I32);
}

static void ValidateStatements(const std::vector<std::unique_ptr<Stmt>> &statements,
                               std::vector<SemanticScope> &scopes,
                               const std::unordered_map<std::string, ValueType> &globals,
                               const std::unordered_map<std::string, FunctionInfo> &functions,
                               const SemanticTypeInfo &expected_return_type, const std::string &function_name,
                               std::vector<std::string> &diagnostics, int loop_depth, int switch_depth,
                               std::size_t max_message_send_args);

static void ValidateAssignmentCompatibility(const std::string &target_name, const std::string &op,
                                           const Expr *value_expr, unsigned line, unsigned column,
                                           bool found_target,
                                           const SemanticTypeInfo &target_type,
                                           const SemanticTypeInfo &value_type,
                                           std::vector<std::string> &diagnostics) {
  if (op == "=") {
    const bool target_known_scalar = IsScalarSemanticType(target_type) &&
                                     (target_type.type == ValueType::I32 || target_type.type == ValueType::Bool);
    const bool value_known_scalar = IsScalarSemanticType(value_type) &&
                                    (value_type.type == ValueType::I32 || value_type.type == ValueType::Bool);
    const bool assign_matches =
        IsSameSemanticType(target_type, value_type) ||
        (target_known_scalar && value_known_scalar && target_type.type == ValueType::I32 &&
         value_type.type == ValueType::Bool) ||
        (target_known_scalar && value_known_scalar && target_type.type == ValueType::Bool &&
         value_type.type == ValueType::I32 && IsBoolLikeI32Literal(value_expr));
    if (found_target && target_known_scalar && !IsUnknownSemanticType(value_type) && !value_known_scalar) {
      diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                     "type mismatch: assignment to '" + target_name + "' expects '" +
                                         SemanticTypeName(target_type) + "', got '" +
                                         SemanticTypeName(value_type) + "'; " +
                                         FormatAtomicMemoryOrderMappingHint(op)));
      return;
    }
    if (found_target && target_known_scalar && value_known_scalar && !assign_matches) {
      diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                     "type mismatch: assignment to '" + target_name + "' expects '" +
                                         SemanticTypeName(target_type) + "', got '" +
                                         SemanticTypeName(value_type) + "'; " +
                                         FormatAtomicMemoryOrderMappingHint(op)));
      return;
    }

    if (found_target && target_type.is_vector && !IsUnknownSemanticType(value_type) && !assign_matches) {
      diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                     "type mismatch: assignment to '" + target_name + "' expects '" +
                                         SemanticTypeName(target_type) + "', got '" +
                                         SemanticTypeName(value_type) + "'; " +
                                         FormatAtomicMemoryOrderMappingHint(op)));
    }
    return;
  }

  if (!IsCompoundAssignmentOperator(op)) {
    if (op == "++" || op == "--") {
      if (found_target && !IsUnknownSemanticType(target_type) &&
          (target_type.is_vector || target_type.type != ValueType::I32)) {
        diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                       "type mismatch: update operator '" + op + "' target '" + target_name +
                                           "' must be 'i32', got '" + SemanticTypeName(target_type) + "'; " +
                                           FormatAtomicMemoryOrderMappingHint(op)));
      }
      return;
    }
    diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                   "type mismatch: unsupported assignment operator '" + op + "'; " +
                                       FormatAtomicMemoryOrderMappingHint(op)));
    return;
  }
  if (!found_target) {
    return;
  }
  if (!IsUnknownSemanticType(target_type) && (target_type.is_vector || target_type.type != ValueType::I32)) {
    diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                   "type mismatch: compound assignment '" + op + "' target '" + target_name +
                                       "' must be 'i32', got '" + SemanticTypeName(target_type) + "'; " +
                                       FormatAtomicMemoryOrderMappingHint(op)));
  }
  if (target_type.type == ValueType::I32 && !target_type.is_vector &&
      !IsUnknownSemanticType(value_type) && (value_type.is_vector || value_type.type != ValueType::I32)) {
    diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                   "type mismatch: compound assignment '" + op + "' value for '" + target_name +
                                       "' must be 'i32', got '" + SemanticTypeName(value_type) + "'; " +
                                       FormatAtomicMemoryOrderMappingHint(op)));
  }
}

static void CollectAtomicMemoryOrderMappingsInStatements(const std::vector<std::unique_ptr<Stmt>> &statements,
                                                         Objc3AtomicMemoryOrderMappingSummary &summary);

static void CollectAtomicMemoryOrderMappingsInForClause(const ForClause &clause,
                                                        Objc3AtomicMemoryOrderMappingSummary &summary) {
  if (clause.kind != ForClause::Kind::Assign) {
    return;
  }
  RecordAtomicMemoryOrderMapping(clause.op, summary);
}

static void CollectAtomicMemoryOrderMappingsInStatement(const Stmt *stmt,
                                                        Objc3AtomicMemoryOrderMappingSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        RecordAtomicMemoryOrderMapping(stmt->assign_stmt->op, summary);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt != nullptr) {
        CollectAtomicMemoryOrderMappingsInStatements(stmt->if_stmt->then_body, summary);
        CollectAtomicMemoryOrderMappingsInStatements(stmt->if_stmt->else_body, summary);
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt != nullptr) {
        CollectAtomicMemoryOrderMappingsInStatements(stmt->do_while_stmt->body, summary);
      }
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt != nullptr) {
        CollectAtomicMemoryOrderMappingsInForClause(stmt->for_stmt->init, summary);
        CollectAtomicMemoryOrderMappingsInForClause(stmt->for_stmt->step, summary);
        CollectAtomicMemoryOrderMappingsInStatements(stmt->for_stmt->body, summary);
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt != nullptr) {
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          CollectAtomicMemoryOrderMappingsInStatements(case_stmt.body, summary);
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt != nullptr) {
        CollectAtomicMemoryOrderMappingsInStatements(stmt->while_stmt->body, summary);
      }
      return;
    case Stmt::Kind::Block:
      if (stmt->block_stmt != nullptr) {
        CollectAtomicMemoryOrderMappingsInStatements(stmt->block_stmt->body, summary);
      }
      return;
    case Stmt::Kind::Let:
    case Stmt::Kind::Return:
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
    case Stmt::Kind::Expr:
      return;
  }
}

static void CollectAtomicMemoryOrderMappingsInStatements(const std::vector<std::unique_ptr<Stmt>> &statements,
                                                         Objc3AtomicMemoryOrderMappingSummary &summary) {
  for (const auto &stmt : statements) {
    CollectAtomicMemoryOrderMappingsInStatement(stmt.get(), summary);
  }
}

static void ValidateStatement(const Stmt *stmt, std::vector<SemanticScope> &scopes,
                              const std::unordered_map<std::string, ValueType> &globals,
                              const std::unordered_map<std::string, FunctionInfo> &functions,
                              const SemanticTypeInfo &expected_return_type, const std::string &function_name,
                              std::vector<std::string> &diagnostics, int loop_depth, int switch_depth,
                              std::size_t max_message_send_args) {
  if (stmt == nullptr) {
    return;
  }
  const auto resolve_assignment_target_type = [&](const std::string &target_name, SemanticTypeInfo &target_type) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
      auto found = it->find(target_name);
      if (found != it->end()) {
        target_type = found->second;
        return true;
      }
    }
    auto global_it = globals.find(target_name);
    if (global_it != globals.end()) {
      target_type = MakeSemanticTypeFromGlobal(global_it->second);
      return true;
    }
    return false;
  };
  const auto validate_for_clause = [&](const ForClause &clause) {
    switch (clause.kind) {
      case ForClause::Kind::None:
        return;
      case ForClause::Kind::Expr:
        (void)ValidateExpr(clause.value.get(), scopes, globals, functions, diagnostics, max_message_send_args);
        return;
      case ForClause::Kind::Let: {
        if (scopes.empty()) {
          return;
        }
        const SemanticTypeInfo value_type =
            ValidateExpr(clause.value.get(), scopes, globals, functions, diagnostics, max_message_send_args);
        if (scopes.back().find(clause.name) != scopes.back().end()) {
          diagnostics.push_back(MakeDiag(clause.line, clause.column, "O3S201",
                                         "duplicate declaration '" + clause.name + "'"));
        } else {
          scopes.back().emplace(clause.name, value_type);
        }
        return;
      }
      case ForClause::Kind::Assign: {
        if (scopes.empty()) {
          return;
        }
        SemanticTypeInfo target_type = MakeScalarSemanticType(ValueType::Unknown);
        const bool found_target = resolve_assignment_target_type(clause.name, target_type);
        if (!found_target) {
          diagnostics.push_back(MakeDiag(clause.line, clause.column, "O3S214",
                                         "invalid assignment target '" + clause.name +
                                             "': target must be a mutable symbol"));
        }
        const SemanticTypeInfo value_type =
            ValidateExpr(clause.value.get(), scopes, globals, functions, diagnostics, max_message_send_args);
        ValidateAssignmentCompatibility(clause.name, clause.op, clause.value.get(), clause.line, clause.column,
                                        found_target, target_type, value_type, diagnostics);
        return;
      }
    }
  };

  switch (stmt->kind) {
    case Stmt::Kind::Let: {
      const LetStmt *let = stmt->let_stmt.get();
      if (let == nullptr || scopes.empty()) {
        return;
      }
      const SemanticTypeInfo value_type =
          ValidateExpr(let->value.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      if (scopes.back().find(let->name) != scopes.back().end()) {
        diagnostics.push_back(MakeDiag(let->line, let->column, "O3S201",
                                       "duplicate declaration '" + let->name + "'"));
      } else {
        scopes.back().emplace(let->name, value_type);
      }
      return;
    }
    case Stmt::Kind::Assign: {
      const AssignStmt *assign = stmt->assign_stmt.get();
      if (assign == nullptr || scopes.empty()) {
        return;
      }
      SemanticTypeInfo target_type = MakeScalarSemanticType(ValueType::Unknown);
      const bool found_target = resolve_assignment_target_type(assign->name, target_type);
      if (!found_target) {
        diagnostics.push_back(MakeDiag(assign->line, assign->column, "O3S214",
                                       "invalid assignment target '" + assign->name +
                                           "': target must be a mutable symbol"));
      }
      const SemanticTypeInfo value_type =
          ValidateExpr(assign->value.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      ValidateAssignmentCompatibility(assign->name, assign->op, assign->value.get(), assign->line, assign->column,
                                      found_target, target_type, value_type, diagnostics);
      return;
    }
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        const ReturnStmt *ret = stmt->return_stmt.get();
        if (ret->value == nullptr) {
          if (!(IsScalarSemanticType(expected_return_type) && expected_return_type.type == ValueType::Void)) {
            diagnostics.push_back(MakeDiag(ret->line, ret->column, "O3S211",
                                           "type mismatch: function '" + function_name + "' must return '" +
                                               SemanticTypeName(expected_return_type) + "'"));
          }
          return;
        }

        if (IsScalarSemanticType(expected_return_type) && expected_return_type.type == ValueType::Void) {
          diagnostics.push_back(MakeDiag(ret->line, ret->column, "O3S211",
                                         "type mismatch: void function '" + function_name +
                                             "' must use 'return;'"));
          (void)ValidateExpr(ret->value.get(), scopes, globals, functions, diagnostics, max_message_send_args);
          return;
        }

        const SemanticTypeInfo return_type =
            ValidateExpr(ret->value.get(), scopes, globals, functions, diagnostics, max_message_send_args);
        const bool return_matches = IsSameSemanticType(return_type, expected_return_type) ||
            (IsScalarSemanticType(expected_return_type) && IsScalarSemanticType(return_type) &&
             expected_return_type.type == ValueType::I32 && return_type.type == ValueType::Bool) ||
            (IsScalarSemanticType(expected_return_type) && IsScalarSemanticType(return_type) &&
             expected_return_type.type == ValueType::Bool && return_type.type == ValueType::I32 &&
             IsBoolLikeI32Literal(ret->value.get()));
        if (!return_matches && !IsUnknownSemanticType(return_type) &&
            !(IsScalarSemanticType(return_type) && return_type.type == ValueType::Function)) {
          diagnostics.push_back(MakeDiag(ret->line, ret->column, "O3S211",
                                         "type mismatch: return expression in function '" + function_name +
                                             "' must be '" + SemanticTypeName(expected_return_type) +
                                             "', got '" + SemanticTypeName(return_type) + "'"));
        }
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        (void)ValidateExpr(stmt->expr_stmt->value.get(), scopes, globals, functions, diagnostics,
                           max_message_send_args);
      }
      return;
    case Stmt::Kind::If: {
      const IfStmt *if_stmt = stmt->if_stmt.get();
      if (if_stmt == nullptr) {
        return;
      }
      const SemanticTypeInfo condition_type =
          ValidateExpr(if_stmt->condition.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      if (!IsUnknownSemanticType(condition_type) && !IsScalarBoolCompatibleType(condition_type)) {
        diagnostics.push_back(MakeDiag(if_stmt->line, if_stmt->column, "O3S206",
                                       "type mismatch: if condition must be bool-compatible"));
      }
      scopes.push_back({});
      ValidateStatements(if_stmt->then_body, scopes, globals, functions, expected_return_type, function_name,
                         diagnostics, loop_depth, switch_depth, max_message_send_args);
      scopes.pop_back();
      scopes.push_back({});
      ValidateStatements(if_stmt->else_body, scopes, globals, functions, expected_return_type, function_name,
                         diagnostics, loop_depth, switch_depth, max_message_send_args);
      scopes.pop_back();
      return;
    }
    case Stmt::Kind::DoWhile: {
      const DoWhileStmt *do_while_stmt = stmt->do_while_stmt.get();
      if (do_while_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      ValidateStatements(do_while_stmt->body, scopes, globals, functions, expected_return_type, function_name,
                         diagnostics, loop_depth + 1, switch_depth, max_message_send_args);
      scopes.pop_back();

      const SemanticTypeInfo condition_type =
          ValidateExpr(do_while_stmt->condition.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      if (!IsUnknownSemanticType(condition_type) && !IsScalarBoolCompatibleType(condition_type)) {
        diagnostics.push_back(MakeDiag(do_while_stmt->line, do_while_stmt->column, "O3S206",
                                       "type mismatch: do-while condition must be bool-compatible"));
      }
      return;
    }
    case Stmt::Kind::For: {
      const ForStmt *for_stmt = stmt->for_stmt.get();
      if (for_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      validate_for_clause(for_stmt->init);
      if (for_stmt->condition != nullptr) {
        const SemanticTypeInfo condition_type =
            ValidateExpr(for_stmt->condition.get(), scopes, globals, functions, diagnostics, max_message_send_args);
        if (!IsUnknownSemanticType(condition_type) && !IsScalarBoolCompatibleType(condition_type)) {
          diagnostics.push_back(MakeDiag(for_stmt->line, for_stmt->column, "O3S206",
                                         "type mismatch: for condition must be bool-compatible"));
        }
      }
      validate_for_clause(for_stmt->step);
      scopes.push_back({});
      ValidateStatements(for_stmt->body, scopes, globals, functions, expected_return_type, function_name, diagnostics,
                         loop_depth + 1, switch_depth, max_message_send_args);
      scopes.pop_back();
      scopes.pop_back();
      return;
    }
    case Stmt::Kind::Switch: {
      const SwitchStmt *switch_stmt = stmt->switch_stmt.get();
      if (switch_stmt == nullptr) {
        return;
      }
      const SemanticTypeInfo condition_type =
          ValidateExpr(switch_stmt->condition.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      if (!IsUnknownSemanticType(condition_type) && !IsScalarBoolCompatibleType(condition_type)) {
        diagnostics.push_back(MakeDiag(switch_stmt->line, switch_stmt->column, "O3S206",
                                       "type mismatch: switch condition must be i32-compatible"));
      }

      std::unordered_set<int> seen_case_values;
      bool seen_default = false;
      for (const auto &case_stmt : switch_stmt->cases) {
        if (case_stmt.is_default) {
          if (seen_default) {
            diagnostics.push_back(MakeDiag(case_stmt.line, case_stmt.column, "O3S206",
                                           "type mismatch: duplicate default label in switch"));
          }
          seen_default = true;
        } else {
          if (!seen_case_values.insert(case_stmt.value).second) {
            diagnostics.push_back(MakeDiag(case_stmt.value_line, case_stmt.value_column, "O3S206",
                                           "type mismatch: duplicate case label '" +
                                               std::to_string(case_stmt.value) + "' in switch"));
          }
        }
        scopes.push_back({});
        ValidateStatements(case_stmt.body, scopes, globals, functions, expected_return_type, function_name,
                           diagnostics, loop_depth, switch_depth + 1, max_message_send_args);
        scopes.pop_back();
      }
      return;
    }
    case Stmt::Kind::While: {
      const WhileStmt *while_stmt = stmt->while_stmt.get();
      if (while_stmt == nullptr) {
        return;
      }
      const SemanticTypeInfo condition_type =
          ValidateExpr(while_stmt->condition.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      if (!IsUnknownSemanticType(condition_type) && !IsScalarBoolCompatibleType(condition_type)) {
        diagnostics.push_back(MakeDiag(while_stmt->line, while_stmt->column, "O3S206",
                                       "type mismatch: while condition must be bool-compatible"));
      }
      scopes.push_back({});
      ValidateStatements(while_stmt->body, scopes, globals, functions, expected_return_type, function_name,
                         diagnostics, loop_depth + 1, switch_depth, max_message_send_args);
      scopes.pop_back();
      return;
    }
    case Stmt::Kind::Block: {
      const BlockStmt *block_stmt = stmt->block_stmt.get();
      if (block_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      ValidateStatements(block_stmt->body, scopes, globals, functions, expected_return_type, function_name,
                         diagnostics, loop_depth, switch_depth, max_message_send_args);
      scopes.pop_back();
      return;
    }
    case Stmt::Kind::Break:
      if (loop_depth <= 0 && switch_depth <= 0) {
        diagnostics.push_back(MakeDiag(stmt->line, stmt->column, "O3S212", "loop-control misuse: 'break' outside loop"));
      }
      return;
    case Stmt::Kind::Continue:
      if (loop_depth <= 0) {
        diagnostics.push_back(
            MakeDiag(stmt->line, stmt->column, "O3S213", "loop-control misuse: 'continue' outside loop"));
      }
      return;
    case Stmt::Kind::Empty:
      return;
  }
}

static void ValidateStatements(const std::vector<std::unique_ptr<Stmt>> &statements,
                               std::vector<SemanticScope> &scopes,
                               const std::unordered_map<std::string, ValueType> &globals,
                               const std::unordered_map<std::string, FunctionInfo> &functions,
                               const SemanticTypeInfo &expected_return_type, const std::string &function_name,
                               std::vector<std::string> &diagnostics, int loop_depth, int switch_depth,
                               std::size_t max_message_send_args) {
  for (const auto &stmt : statements) {
    ValidateStatement(stmt.get(), scopes, globals, functions, expected_return_type, function_name, diagnostics,
                      loop_depth, switch_depth, max_message_send_args);
  }
}

static void CollectAssignedIdentifiers(const std::vector<std::unique_ptr<Stmt>> &statements,
                                       std::unordered_set<std::string> &assigned);

static void CollectAssignedIdentifiersFromStmt(const Stmt *stmt, std::unordered_set<std::string> &assigned) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        assigned.insert(stmt->assign_stmt->name);
      }
      return;
    case Stmt::Kind::Block:
      if (stmt->block_stmt != nullptr) {
        CollectAssignedIdentifiers(stmt->block_stmt->body, assigned);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt != nullptr) {
        CollectAssignedIdentifiers(stmt->if_stmt->then_body, assigned);
        CollectAssignedIdentifiers(stmt->if_stmt->else_body, assigned);
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt != nullptr) {
        CollectAssignedIdentifiers(stmt->do_while_stmt->body, assigned);
      }
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt != nullptr) {
        if (stmt->for_stmt->init.kind == ForClause::Kind::Assign) {
          assigned.insert(stmt->for_stmt->init.name);
        }
        if (stmt->for_stmt->step.kind == ForClause::Kind::Assign) {
          assigned.insert(stmt->for_stmt->step.name);
        }
        CollectAssignedIdentifiers(stmt->for_stmt->body, assigned);
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt != nullptr) {
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          CollectAssignedIdentifiers(case_stmt.body, assigned);
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt != nullptr) {
        CollectAssignedIdentifiers(stmt->while_stmt->body, assigned);
      }
      return;
    default:
      return;
  }
}

static void CollectAssignedIdentifiers(const std::vector<std::unique_ptr<Stmt>> &statements,
                                       std::unordered_set<std::string> &assigned) {
  for (const auto &stmt : statements) {
    CollectAssignedIdentifiersFromStmt(stmt.get(), assigned);
  }
}

static void CollectNonTopLevelLetNames(const std::vector<std::unique_ptr<Stmt>> &statements, bool is_top_level,
                                       std::unordered_set<std::string> &names);

static void CollectNonTopLevelLetNamesFromStmt(const Stmt *stmt, bool is_top_level,
                                               std::unordered_set<std::string> &names) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (!is_top_level && stmt->let_stmt != nullptr) {
        names.insert(stmt->let_stmt->name);
      }
      return;
    case Stmt::Kind::Block:
      if (stmt->block_stmt != nullptr) {
        CollectNonTopLevelLetNames(stmt->block_stmt->body, false, names);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt != nullptr) {
        CollectNonTopLevelLetNames(stmt->if_stmt->then_body, false, names);
        CollectNonTopLevelLetNames(stmt->if_stmt->else_body, false, names);
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt != nullptr) {
        CollectNonTopLevelLetNames(stmt->do_while_stmt->body, false, names);
      }
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt != nullptr) {
        if (stmt->for_stmt->init.kind == ForClause::Kind::Let) {
          names.insert(stmt->for_stmt->init.name);
        }
        CollectNonTopLevelLetNames(stmt->for_stmt->body, false, names);
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt != nullptr) {
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          CollectNonTopLevelLetNames(case_stmt.body, false, names);
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt != nullptr) {
        CollectNonTopLevelLetNames(stmt->while_stmt->body, false, names);
      }
      return;
    default:
      return;
  }
}

static void CollectNonTopLevelLetNames(const std::vector<std::unique_ptr<Stmt>> &statements, bool is_top_level,
                                       std::unordered_set<std::string> &names) {
  for (const auto &stmt : statements) {
    CollectNonTopLevelLetNamesFromStmt(stmt.get(), is_top_level, names);
  }
}

static void CollectSwitchConditionIdentifierNames(const std::vector<std::unique_ptr<Stmt>> &statements,
                                                  std::unordered_set<std::string> &names);

static void CollectSwitchConditionIdentifierNamesFromStmt(const Stmt *stmt, std::unordered_set<std::string> &names) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt != nullptr) {
        const Expr *condition = stmt->switch_stmt->condition.get();
        if (condition != nullptr && condition->kind == Expr::Kind::Identifier && !condition->ident.empty()) {
          names.insert(condition->ident);
        }
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          CollectSwitchConditionIdentifierNames(case_stmt.body, names);
        }
      }
      return;
    case Stmt::Kind::Block:
      if (stmt->block_stmt != nullptr) {
        CollectSwitchConditionIdentifierNames(stmt->block_stmt->body, names);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt != nullptr) {
        CollectSwitchConditionIdentifierNames(stmt->if_stmt->then_body, names);
        CollectSwitchConditionIdentifierNames(stmt->if_stmt->else_body, names);
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt != nullptr) {
        CollectSwitchConditionIdentifierNames(stmt->do_while_stmt->body, names);
      }
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt != nullptr) {
        CollectSwitchConditionIdentifierNames(stmt->for_stmt->body, names);
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt != nullptr) {
        CollectSwitchConditionIdentifierNames(stmt->while_stmt->body, names);
      }
      return;
    default:
      return;
  }
}

static void CollectSwitchConditionIdentifierNames(const std::vector<std::unique_ptr<Stmt>> &statements,
                                                  std::unordered_set<std::string> &names) {
  for (const auto &stmt : statements) {
    CollectSwitchConditionIdentifierNamesFromStmt(stmt.get(), names);
  }
}

static StaticScalarBindings CollectFunctionStaticScalarBindings(const FunctionDecl &fn,
                                                               const StaticScalarBindings *global_bindings = nullptr) {
  std::unordered_set<std::string> assigned;
  CollectAssignedIdentifiers(fn.body, assigned);

  std::unordered_set<std::string> non_top_level_lets;
  CollectNonTopLevelLetNames(fn.body, true, non_top_level_lets);

  std::unordered_set<std::string> switch_condition_identifiers;
  CollectSwitchConditionIdentifierNames(fn.body, switch_condition_identifiers);

  StaticScalarBindings bindings;
  for (const auto &stmt : fn.body) {
    if (stmt == nullptr || stmt->kind != Stmt::Kind::Let || stmt->let_stmt == nullptr || stmt->let_stmt->value == nullptr) {
      continue;
    }
    const std::string &name = stmt->let_stmt->name;
    if (assigned.find(name) != assigned.end() || non_top_level_lets.find(name) != non_top_level_lets.end() ||
        switch_condition_identifiers.find(name) != switch_condition_identifiers.end()) {
      continue;
    }
    int value = 0;
    if (TryEvalStaticScalarValue(stmt->let_stmt->value.get(), value, &bindings)) {
      bindings[name] = value;
    }
  }

  if (global_bindings != nullptr) {
    for (const auto &[name, value] : *global_bindings) {
      if (bindings.find(name) != bindings.end()) {
        continue;
      }
      if (assigned.find(name) != assigned.end() || non_top_level_lets.find(name) != non_top_level_lets.end() ||
          switch_condition_identifiers.find(name) != switch_condition_identifiers.end()) {
        continue;
      }
      bindings[name] = value;
    }
  }
  return bindings;
}

Objc3AtomicMemoryOrderMappingSummary BuildAtomicMemoryOrderMappingSummary(const Objc3ParsedProgram &program) {
  Objc3AtomicMemoryOrderMappingSummary summary;
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  for (const auto &fn : ast.functions) {
    CollectAtomicMemoryOrderMappingsInStatements(fn.body, summary);
  }
  return summary;
}

Objc3VectorTypeLoweringSummary BuildVectorTypeLoweringSummary(const Objc3SemanticIntegrationSurface &surface) {
  Objc3VectorTypeLoweringSummary summary;
  for (const auto &entry : surface.functions) {
    const FunctionInfo &fn = entry.second;
    if (fn.param_types.size() != fn.arity ||
        fn.param_is_vector.size() != fn.arity ||
        fn.param_vector_base_spelling.size() != fn.arity ||
        fn.param_vector_lane_count.size() != fn.arity ||
        fn.param_has_invalid_type_suffix.size() != fn.arity) {
      summary.deterministic = false;
      continue;
    }

    if (fn.return_is_vector) {
      RecordVectorTypeLoweringAnnotation(fn.return_type, fn.return_vector_lane_count, true, summary);
    }

    for (std::size_t i = 0; i < fn.arity; ++i) {
      if (!fn.param_is_vector[i]) {
        continue;
      }
      RecordVectorTypeLoweringAnnotation(fn.param_types[i], fn.param_vector_lane_count[i], false, summary);
    }
  }
  return summary;
}

static void AccumulateProtocolCompositionSite(bool has_protocol_composition,
                                              const std::vector<std::string> &composition_names_lexicographic,
                                              bool has_invalid_protocol_composition,
                                              bool is_category_context,
                                              Objc3ProtocolCategoryCompositionSummary &summary) {
  if (!has_protocol_composition) {
    if (has_invalid_protocol_composition) {
      summary.deterministic = false;
    }
    return;
  }

  ++summary.protocol_composition_sites;
  summary.protocol_composition_symbols += composition_names_lexicographic.size();
  if (is_category_context) {
    ++summary.category_composition_sites;
    summary.category_composition_symbols += composition_names_lexicographic.size();
  }
  if (has_invalid_protocol_composition) {
    ++summary.invalid_protocol_composition_sites;
  }
  if (!IsSortedUniqueStrings(composition_names_lexicographic)) {
    summary.deterministic = false;
  }
}

static void AccumulateProtocolCategoryCompositionFromFunctionInfo(const FunctionInfo &fn,
                                                                  Objc3ProtocolCategoryCompositionSummary &summary) {
  if (fn.param_types.size() != fn.arity ||
      fn.param_has_protocol_composition.size() != fn.arity ||
      fn.param_protocol_composition_lexicographic.size() != fn.arity ||
      fn.param_has_invalid_protocol_composition.size() != fn.arity) {
    summary.deterministic = false;
    return;
  }

  for (std::size_t i = 0; i < fn.arity; ++i) {
    AccumulateProtocolCompositionSite(fn.param_has_protocol_composition[i],
                                      fn.param_protocol_composition_lexicographic[i],
                                      fn.param_has_invalid_protocol_composition[i],
                                      false,
                                      summary);
  }
  AccumulateProtocolCompositionSite(fn.return_has_protocol_composition,
                                    fn.return_protocol_composition_lexicographic,
                                    fn.return_has_invalid_protocol_composition,
                                    false,
                                    summary);
}

static void AccumulateProtocolCategoryCompositionFromMethodInfo(const Objc3MethodInfo &method,
                                                                Objc3ProtocolCategoryCompositionSummary &summary) {
  if (method.param_types.size() != method.arity ||
      method.param_has_protocol_composition.size() != method.arity ||
      method.param_protocol_composition_lexicographic.size() != method.arity ||
      method.param_has_invalid_protocol_composition.size() != method.arity) {
    summary.deterministic = false;
    return;
  }

  for (std::size_t i = 0; i < method.arity; ++i) {
    AccumulateProtocolCompositionSite(method.param_has_protocol_composition[i],
                                      method.param_protocol_composition_lexicographic[i],
                                      method.param_has_invalid_protocol_composition[i],
                                      true,
                                      summary);
  }
  AccumulateProtocolCompositionSite(method.return_has_protocol_composition,
                                    method.return_protocol_composition_lexicographic,
                                    method.return_has_invalid_protocol_composition,
                                    true,
                                    summary);
}

static Objc3ProtocolCategoryCompositionSummary BuildProtocolCategoryCompositionSummaryFromSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3ProtocolCategoryCompositionSummary summary;
  for (const auto &entry : surface.functions) {
    AccumulateProtocolCategoryCompositionFromFunctionInfo(entry.second, summary);
  }
  for (const auto &entry : surface.interfaces) {
    for (const auto &method_entry : entry.second.methods) {
      AccumulateProtocolCategoryCompositionFromMethodInfo(method_entry.second, summary);
    }
  }
  for (const auto &entry : surface.implementations) {
    for (const auto &method_entry : entry.second.methods) {
      AccumulateProtocolCategoryCompositionFromMethodInfo(method_entry.second, summary);
    }
  }

  summary.deterministic = summary.deterministic &&
                          summary.invalid_protocol_composition_sites <= summary.total_composition_sites() &&
                          summary.category_composition_sites <= summary.protocol_composition_sites &&
                          summary.category_composition_symbols <= summary.protocol_composition_symbols;
  return summary;
}

static Objc3ClassProtocolCategoryLinkingSummary BuildClassProtocolCategoryLinkingSummary(
    const Objc3InterfaceImplementationSummary &interface_implementation_summary,
    const Objc3ProtocolCategoryCompositionSummary &protocol_category_composition_summary) {
  Objc3ClassProtocolCategoryLinkingSummary summary;
  summary.declared_interfaces = interface_implementation_summary.declared_interfaces;
  summary.resolved_interfaces = interface_implementation_summary.resolved_interfaces;
  summary.declared_implementations = interface_implementation_summary.declared_implementations;
  summary.resolved_implementations = interface_implementation_summary.resolved_implementations;
  summary.interface_method_symbols = interface_implementation_summary.interface_method_symbols;
  summary.implementation_method_symbols = interface_implementation_summary.implementation_method_symbols;
  summary.linked_implementation_symbols = interface_implementation_summary.linked_implementation_symbols;
  summary.protocol_composition_sites = protocol_category_composition_summary.protocol_composition_sites;
  summary.protocol_composition_symbols = protocol_category_composition_summary.protocol_composition_symbols;
  summary.category_composition_sites = protocol_category_composition_summary.category_composition_sites;
  summary.category_composition_symbols = protocol_category_composition_summary.category_composition_symbols;
  summary.invalid_protocol_composition_sites = protocol_category_composition_summary.invalid_protocol_composition_sites;
  summary.deterministic = interface_implementation_summary.deterministic &&
                          protocol_category_composition_summary.deterministic &&
                          summary.resolved_interfaces <= summary.declared_interfaces &&
                          summary.resolved_implementations <= summary.declared_implementations &&
                          summary.linked_implementation_symbols <= summary.implementation_method_symbols &&
                          summary.linked_implementation_symbols <= summary.interface_method_symbols &&
                          summary.invalid_protocol_composition_sites <= summary.total_composition_sites() &&
                          summary.category_composition_sites <= summary.protocol_composition_sites &&
                          summary.category_composition_symbols <= summary.protocol_composition_symbols;
  return summary;
}

static void AccumulateSelectorNormalizationFromMethodInfo(const Objc3MethodInfo &method,
                                                          Objc3SelectorNormalizationSummary &summary) {
  ++summary.methods_total;
  if (method.selector_contract_normalized) {
    ++summary.normalized_methods;
  }
  summary.selector_piece_entries += method.selector_piece_count;
  summary.selector_parameter_piece_entries += method.selector_parameter_piece_count;
  if (method.selector_had_pieceless_form) {
    ++summary.selector_pieceless_methods;
  }
  if (method.selector_has_spelling_mismatch) {
    ++summary.selector_spelling_mismatches;
  }
  if (method.selector_has_arity_mismatch) {
    ++summary.selector_arity_mismatches;
  }
  if (method.selector_has_parameter_linkage_mismatch) {
    ++summary.selector_parameter_linkage_mismatches;
  }
  if (method.selector_has_normalization_flag_mismatch) {
    ++summary.selector_normalization_flag_mismatches;
  }
  if (method.selector_has_missing_piece_keyword) {
    ++summary.selector_missing_keyword_pieces;
  }
  if (method.selector_parameter_piece_count > method.selector_piece_count ||
      method.selector_normalized.empty()) {
    summary.deterministic = false;
  }
}

static Objc3SelectorNormalizationSummary BuildSelectorNormalizationSummaryFromSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3SelectorNormalizationSummary summary;
  for (const auto &entry : surface.interfaces) {
    for (const auto &method_entry : entry.second.methods) {
      AccumulateSelectorNormalizationFromMethodInfo(method_entry.second, summary);
    }
  }
  for (const auto &entry : surface.implementations) {
    for (const auto &method_entry : entry.second.methods) {
      AccumulateSelectorNormalizationFromMethodInfo(method_entry.second, summary);
    }
  }

  summary.deterministic = summary.deterministic &&
                          summary.normalized_methods <= summary.methods_total &&
                          summary.selector_parameter_piece_entries <= summary.selector_piece_entries &&
                          summary.contract_violations() <= summary.methods_total;
  return summary;
}

static void AccumulatePropertyAttributeSummaryFromPropertyInfo(const Objc3PropertyInfo &property,
                                                               Objc3PropertyAttributeSummary &summary) {
  ++summary.properties_total;
  summary.attribute_entries += property.attribute_entries;
  if (property.is_readonly) {
    ++summary.readonly_modifiers;
  }
  if (property.is_readwrite) {
    ++summary.readwrite_modifiers;
  }
  if (property.is_atomic) {
    ++summary.atomic_modifiers;
  }
  if (property.is_nonatomic) {
    ++summary.nonatomic_modifiers;
  }
  if (property.is_copy) {
    ++summary.copy_modifiers;
  }
  if (property.is_strong) {
    ++summary.strong_modifiers;
  }
  if (property.is_weak) {
    ++summary.weak_modifiers;
  }
  if (property.is_assign) {
    ++summary.assign_modifiers;
  }
  if (property.has_getter) {
    ++summary.getter_modifiers;
  }
  if (property.has_setter) {
    ++summary.setter_modifiers;
  }
  summary.invalid_attribute_entries += property.invalid_attribute_entries;
  summary.property_contract_violations += property.property_contract_violations;

  if (property.attribute_entries != property.attribute_names_lexicographic.size() ||
      !std::is_sorted(property.attribute_names_lexicographic.begin(), property.attribute_names_lexicographic.end())) {
    summary.deterministic = false;
  }
  if (property.has_readwrite_conflict != (property.is_readonly && property.is_readwrite)) {
    summary.deterministic = false;
  }
  if (property.has_atomicity_conflict != (property.is_atomic && property.is_nonatomic)) {
    summary.deterministic = false;
  }
  const std::size_t ownership_modifiers = (property.is_copy ? 1u : 0u) + (property.is_strong ? 1u : 0u) +
                                          (property.is_weak ? 1u : 0u) + (property.is_assign ? 1u : 0u);
  if (property.has_ownership_conflict != (ownership_modifiers > 1u)) {
    summary.deterministic = false;
  }
  if (property.has_setter && property.setter_selector.empty()) {
    summary.deterministic = false;
  }
  if (property.has_getter && property.getter_selector.empty()) {
    summary.deterministic = false;
  }
  const bool expected_invalid_contract =
      property.has_unknown_attribute || property.has_duplicate_attribute || property.has_readwrite_conflict ||
      property.has_atomicity_conflict || property.has_ownership_conflict ||
      property.has_accessor_selector_contract_violation || property.invalid_attribute_entries > 0 ||
      property.property_contract_violations > 0;
  if (property.has_invalid_attribute_contract != expected_invalid_contract) {
    summary.deterministic = false;
  }
}

static Objc3PropertyAttributeSummary BuildPropertyAttributeSummaryFromSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3PropertyAttributeSummary summary;
  for (const auto &interface_entry : surface.interfaces) {
    for (const auto &property_entry : interface_entry.second.properties) {
      AccumulatePropertyAttributeSummaryFromPropertyInfo(property_entry.second, summary);
    }
  }
  for (const auto &implementation_entry : surface.implementations) {
    for (const auto &property_entry : implementation_entry.second.properties) {
      AccumulatePropertyAttributeSummaryFromPropertyInfo(property_entry.second, summary);
    }
  }

  summary.deterministic = summary.deterministic &&
                          summary.invalid_attribute_entries <= summary.attribute_entries &&
                          summary.getter_modifiers <= summary.properties_total &&
                          summary.setter_modifiers <= summary.properties_total;
  return summary;
}

static void AccumulateTypeAnnotationSummaryFromFunctionInfo(const FunctionInfo &function_info,
                                                            Objc3TypeAnnotationSurfaceSummary &summary) {
  const std::size_t arity = function_info.arity;
  if (function_info.param_has_generic_suffix.size() != arity ||
      function_info.param_has_pointer_declarator.size() != arity ||
      function_info.param_has_nullability_suffix.size() != arity ||
      function_info.param_has_ownership_qualifier.size() != arity ||
      function_info.param_object_pointer_type_spelling.size() != arity ||
      function_info.param_has_invalid_generic_suffix.size() != arity ||
      function_info.param_has_invalid_pointer_declarator.size() != arity ||
      function_info.param_has_invalid_nullability_suffix.size() != arity ||
      function_info.param_has_invalid_ownership_qualifier.size() != arity ||
      function_info.param_has_invalid_type_suffix.size() != arity) {
    summary.deterministic = false;
    return;
  }

  for (std::size_t i = 0; i < arity; ++i) {
    if (function_info.param_has_generic_suffix[i]) {
      ++summary.generic_suffix_sites;
    }
    if (function_info.param_has_pointer_declarator[i]) {
      ++summary.pointer_declarator_sites;
    }
    if (function_info.param_has_nullability_suffix[i]) {
      ++summary.nullability_suffix_sites;
    }
    if (function_info.param_has_ownership_qualifier[i]) {
      ++summary.ownership_qualifier_sites;
    }
    if (function_info.param_object_pointer_type_spelling[i]) {
      ++summary.object_pointer_type_sites;
    }
    if (function_info.param_has_invalid_generic_suffix[i]) {
      ++summary.invalid_generic_suffix_sites;
    }
    if (function_info.param_has_invalid_pointer_declarator[i]) {
      ++summary.invalid_pointer_declarator_sites;
    }
    if (function_info.param_has_invalid_nullability_suffix[i]) {
      ++summary.invalid_nullability_suffix_sites;
    }
    if (function_info.param_has_invalid_ownership_qualifier[i]) {
      ++summary.invalid_ownership_qualifier_sites;
    }
    const bool expected_invalid =
        function_info.param_has_invalid_generic_suffix[i] ||
        function_info.param_has_invalid_pointer_declarator[i] ||
        function_info.param_has_invalid_nullability_suffix[i] ||
        function_info.param_has_invalid_ownership_qualifier[i];
    if (function_info.param_has_invalid_type_suffix[i] != expected_invalid) {
      summary.deterministic = false;
    }
  }

  if (function_info.return_has_generic_suffix) {
    ++summary.generic_suffix_sites;
  }
  if (function_info.return_has_pointer_declarator) {
    ++summary.pointer_declarator_sites;
  }
  if (function_info.return_has_nullability_suffix) {
    ++summary.nullability_suffix_sites;
  }
  if (function_info.return_has_ownership_qualifier) {
    ++summary.ownership_qualifier_sites;
  }
  if (function_info.return_object_pointer_type_spelling) {
    ++summary.object_pointer_type_sites;
  }
  if (function_info.return_has_invalid_generic_suffix) {
    ++summary.invalid_generic_suffix_sites;
  }
  if (function_info.return_has_invalid_pointer_declarator) {
    ++summary.invalid_pointer_declarator_sites;
  }
  if (function_info.return_has_invalid_nullability_suffix) {
    ++summary.invalid_nullability_suffix_sites;
  }
  if (function_info.return_has_invalid_ownership_qualifier) {
    ++summary.invalid_ownership_qualifier_sites;
  }

  const bool expected_return_invalid = function_info.return_has_invalid_generic_suffix ||
                                       function_info.return_has_invalid_pointer_declarator ||
                                       function_info.return_has_invalid_nullability_suffix ||
                                       function_info.return_has_invalid_ownership_qualifier;
  if (function_info.return_has_invalid_type_suffix != expected_return_invalid) {
    summary.deterministic = false;
  }
}

static void AccumulateTypeAnnotationSummaryFromMethodInfo(const Objc3MethodInfo &method_info,
                                                          Objc3TypeAnnotationSurfaceSummary &summary) {
  const std::size_t arity = method_info.arity;
  if (method_info.param_has_generic_suffix.size() != arity ||
      method_info.param_has_pointer_declarator.size() != arity ||
      method_info.param_has_nullability_suffix.size() != arity ||
      method_info.param_has_ownership_qualifier.size() != arity ||
      method_info.param_object_pointer_type_spelling.size() != arity ||
      method_info.param_has_invalid_generic_suffix.size() != arity ||
      method_info.param_has_invalid_pointer_declarator.size() != arity ||
      method_info.param_has_invalid_nullability_suffix.size() != arity ||
      method_info.param_has_invalid_ownership_qualifier.size() != arity ||
      method_info.param_has_invalid_type_suffix.size() != arity) {
    summary.deterministic = false;
    return;
  }

  for (std::size_t i = 0; i < arity; ++i) {
    if (method_info.param_has_generic_suffix[i]) {
      ++summary.generic_suffix_sites;
    }
    if (method_info.param_has_pointer_declarator[i]) {
      ++summary.pointer_declarator_sites;
    }
    if (method_info.param_has_nullability_suffix[i]) {
      ++summary.nullability_suffix_sites;
    }
    if (method_info.param_has_ownership_qualifier[i]) {
      ++summary.ownership_qualifier_sites;
    }
    if (method_info.param_object_pointer_type_spelling[i]) {
      ++summary.object_pointer_type_sites;
    }
    if (method_info.param_has_invalid_generic_suffix[i]) {
      ++summary.invalid_generic_suffix_sites;
    }
    if (method_info.param_has_invalid_pointer_declarator[i]) {
      ++summary.invalid_pointer_declarator_sites;
    }
    if (method_info.param_has_invalid_nullability_suffix[i]) {
      ++summary.invalid_nullability_suffix_sites;
    }
    if (method_info.param_has_invalid_ownership_qualifier[i]) {
      ++summary.invalid_ownership_qualifier_sites;
    }
    const bool expected_invalid = method_info.param_has_invalid_generic_suffix[i] ||
                                  method_info.param_has_invalid_pointer_declarator[i] ||
                                  method_info.param_has_invalid_nullability_suffix[i] ||
                                  method_info.param_has_invalid_ownership_qualifier[i];
    if (method_info.param_has_invalid_type_suffix[i] != expected_invalid) {
      summary.deterministic = false;
    }
  }

  if (method_info.return_has_generic_suffix) {
    ++summary.generic_suffix_sites;
  }
  if (method_info.return_has_pointer_declarator) {
    ++summary.pointer_declarator_sites;
  }
  if (method_info.return_has_nullability_suffix) {
    ++summary.nullability_suffix_sites;
  }
  if (method_info.return_has_ownership_qualifier) {
    ++summary.ownership_qualifier_sites;
  }
  if (method_info.return_object_pointer_type_spelling) {
    ++summary.object_pointer_type_sites;
  }
  if (method_info.return_has_invalid_generic_suffix) {
    ++summary.invalid_generic_suffix_sites;
  }
  if (method_info.return_has_invalid_pointer_declarator) {
    ++summary.invalid_pointer_declarator_sites;
  }
  if (method_info.return_has_invalid_nullability_suffix) {
    ++summary.invalid_nullability_suffix_sites;
  }
  if (method_info.return_has_invalid_ownership_qualifier) {
    ++summary.invalid_ownership_qualifier_sites;
  }
  const bool expected_return_invalid = method_info.return_has_invalid_generic_suffix ||
                                       method_info.return_has_invalid_pointer_declarator ||
                                       method_info.return_has_invalid_nullability_suffix ||
                                       method_info.return_has_invalid_ownership_qualifier;
  if (method_info.return_has_invalid_type_suffix != expected_return_invalid) {
    summary.deterministic = false;
  }
}

static void AccumulateTypeAnnotationSummaryFromPropertyInfo(const Objc3PropertyInfo &property_info,
                                                            Objc3TypeAnnotationSurfaceSummary &summary) {
  if (property_info.has_generic_suffix) {
    ++summary.generic_suffix_sites;
  }
  if (property_info.has_pointer_declarator) {
    ++summary.pointer_declarator_sites;
  }
  if (property_info.has_nullability_suffix) {
    ++summary.nullability_suffix_sites;
  }
  if (property_info.has_ownership_qualifier) {
    ++summary.ownership_qualifier_sites;
  }
  if (property_info.object_pointer_type_spelling) {
    ++summary.object_pointer_type_sites;
  }
  if (property_info.has_invalid_generic_suffix) {
    ++summary.invalid_generic_suffix_sites;
  }
  if (property_info.has_invalid_pointer_declarator) {
    ++summary.invalid_pointer_declarator_sites;
  }
  if (property_info.has_invalid_nullability_suffix) {
    ++summary.invalid_nullability_suffix_sites;
  }
  if (property_info.has_invalid_ownership_qualifier) {
    ++summary.invalid_ownership_qualifier_sites;
  }
  const bool expected_invalid = property_info.has_invalid_generic_suffix ||
                                property_info.has_invalid_pointer_declarator ||
                                property_info.has_invalid_nullability_suffix ||
                                property_info.has_invalid_ownership_qualifier;
  if (property_info.has_invalid_type_suffix != expected_invalid) {
    summary.deterministic = false;
  }
}

static Objc3TypeAnnotationSurfaceSummary BuildTypeAnnotationSurfaceSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3TypeAnnotationSurfaceSummary summary;
  for (const auto &function_entry : surface.functions) {
    AccumulateTypeAnnotationSummaryFromFunctionInfo(function_entry.second, summary);
  }
  for (const auto &interface_entry : surface.interfaces) {
    for (const auto &method_entry : interface_entry.second.methods) {
      AccumulateTypeAnnotationSummaryFromMethodInfo(method_entry.second, summary);
    }
    for (const auto &property_entry : interface_entry.second.properties) {
      AccumulateTypeAnnotationSummaryFromPropertyInfo(property_entry.second, summary);
    }
  }
  for (const auto &implementation_entry : surface.implementations) {
    for (const auto &method_entry : implementation_entry.second.methods) {
      AccumulateTypeAnnotationSummaryFromMethodInfo(method_entry.second, summary);
    }
    for (const auto &property_entry : implementation_entry.second.properties) {
      AccumulateTypeAnnotationSummaryFromPropertyInfo(property_entry.second, summary);
    }
  }

  summary.deterministic = summary.deterministic &&
                          summary.invalid_generic_suffix_sites <= summary.generic_suffix_sites &&
                          summary.invalid_pointer_declarator_sites <= summary.pointer_declarator_sites &&
                          summary.invalid_nullability_suffix_sites <= summary.nullability_suffix_sites &&
                          summary.invalid_ownership_qualifier_sites <= summary.ownership_qualifier_sites &&
                          summary.invalid_type_annotation_sites() <= summary.total_type_annotation_sites();
  return summary;
}

static Objc3SymbolGraphScopeResolutionSummary BuildSymbolGraphScopeResolutionSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3SymbolGraphScopeResolutionSummary summary;
  summary.global_symbol_nodes = surface.globals.size();
  summary.function_symbol_nodes = surface.functions.size();
  summary.interface_symbol_nodes = surface.interfaces.size();
  summary.implementation_symbol_nodes = surface.implementations.size();
  summary.top_level_scope_symbols = summary.global_symbol_nodes + summary.function_symbol_nodes +
                                    summary.interface_symbol_nodes + summary.implementation_symbol_nodes;
  summary.scope_frames_total =
      static_cast<std::size_t>(1u) + summary.function_symbol_nodes + summary.interface_symbol_nodes +
      summary.implementation_symbol_nodes;

  for (const auto &entry : surface.interfaces) {
    summary.interface_property_symbol_nodes += entry.second.properties.size();
    summary.interface_method_symbol_nodes += entry.second.methods.size();
  }
  for (const auto &entry : surface.implementations) {
    summary.implementation_property_symbol_nodes += entry.second.properties.size();
    summary.implementation_method_symbol_nodes += entry.second.methods.size();
    if (entry.second.has_matching_interface) {
      ++summary.implementation_interface_resolution_hits;
    }
  }

  summary.nested_scope_symbols = summary.interface_property_symbol_nodes + summary.implementation_property_symbol_nodes +
                                 summary.interface_method_symbol_nodes + summary.implementation_method_symbol_nodes;
  summary.implementation_interface_resolution_sites = summary.implementation_symbol_nodes;
  if (summary.implementation_interface_resolution_hits > summary.implementation_interface_resolution_sites) {
    summary.deterministic = false;
    summary.implementation_interface_resolution_misses = 0;
  } else {
    summary.implementation_interface_resolution_misses =
        summary.implementation_interface_resolution_sites - summary.implementation_interface_resolution_hits;
  }

  summary.method_resolution_sites = summary.implementation_method_symbol_nodes;
  summary.method_resolution_hits = surface.interface_implementation_summary.linked_implementation_symbols;
  if (summary.method_resolution_hits > summary.method_resolution_sites) {
    summary.deterministic = false;
    summary.method_resolution_misses = 0;
  } else {
    summary.method_resolution_misses = summary.method_resolution_sites - summary.method_resolution_hits;
  }

  summary.deterministic = summary.deterministic &&
                          summary.interface_method_symbol_nodes ==
                              surface.interface_implementation_summary.interface_method_symbols &&
                          summary.implementation_method_symbol_nodes ==
                              surface.interface_implementation_summary.implementation_method_symbols &&
                          summary.symbol_nodes_total() == summary.top_level_scope_symbols + summary.nested_scope_symbols &&
                          summary.implementation_interface_resolution_hits <=
                              summary.implementation_interface_resolution_sites &&
                          summary.implementation_interface_resolution_hits +
                                  summary.implementation_interface_resolution_misses ==
                              summary.implementation_interface_resolution_sites &&
                          summary.method_resolution_hits <= summary.method_resolution_sites &&
                          summary.method_resolution_hits + summary.method_resolution_misses ==
                              summary.method_resolution_sites &&
                          summary.resolution_hits_total() <= summary.resolution_sites_total() &&
                          summary.resolution_hits_total() + summary.resolution_misses_total() ==
                              summary.resolution_sites_total();
  return summary;
}

static Objc3SymbolGraphScopeResolutionSummary BuildSymbolGraphScopeResolutionSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3SymbolGraphScopeResolutionSummary summary;
  summary.global_symbol_nodes = handoff.global_names_lexicographic.size();
  summary.function_symbol_nodes = handoff.functions_lexicographic.size();
  summary.interface_symbol_nodes = handoff.interfaces_lexicographic.size();
  summary.implementation_symbol_nodes = handoff.implementations_lexicographic.size();
  summary.top_level_scope_symbols = summary.global_symbol_nodes + summary.function_symbol_nodes +
                                    summary.interface_symbol_nodes + summary.implementation_symbol_nodes;
  summary.scope_frames_total =
      static_cast<std::size_t>(1u) + summary.function_symbol_nodes + summary.interface_symbol_nodes +
      summary.implementation_symbol_nodes;

  for (const auto &metadata : handoff.interfaces_lexicographic) {
    summary.interface_property_symbol_nodes += metadata.properties_lexicographic.size();
    summary.interface_method_symbol_nodes += metadata.methods_lexicographic.size();
  }
  for (const auto &metadata : handoff.implementations_lexicographic) {
    summary.implementation_property_symbol_nodes += metadata.properties_lexicographic.size();
    summary.implementation_method_symbol_nodes += metadata.methods_lexicographic.size();
    if (metadata.has_matching_interface) {
      ++summary.implementation_interface_resolution_hits;
    }
  }

  summary.nested_scope_symbols = summary.interface_property_symbol_nodes + summary.implementation_property_symbol_nodes +
                                 summary.interface_method_symbol_nodes + summary.implementation_method_symbol_nodes;
  summary.implementation_interface_resolution_sites = summary.implementation_symbol_nodes;
  if (summary.implementation_interface_resolution_hits > summary.implementation_interface_resolution_sites) {
    summary.deterministic = false;
    summary.implementation_interface_resolution_misses = 0;
  } else {
    summary.implementation_interface_resolution_misses =
        summary.implementation_interface_resolution_sites - summary.implementation_interface_resolution_hits;
  }

  summary.method_resolution_sites = summary.implementation_method_symbol_nodes;
  summary.method_resolution_hits = handoff.interface_implementation_summary.linked_implementation_symbols;
  if (summary.method_resolution_hits > summary.method_resolution_sites) {
    summary.deterministic = false;
    summary.method_resolution_misses = 0;
  } else {
    summary.method_resolution_misses = summary.method_resolution_sites - summary.method_resolution_hits;
  }

  summary.deterministic = summary.deterministic &&
                          summary.interface_method_symbol_nodes ==
                              handoff.interface_implementation_summary.interface_method_symbols &&
                          summary.implementation_method_symbol_nodes ==
                              handoff.interface_implementation_summary.implementation_method_symbols &&
                          summary.symbol_nodes_total() == summary.top_level_scope_symbols + summary.nested_scope_symbols &&
                          summary.implementation_interface_resolution_hits <=
                              summary.implementation_interface_resolution_sites &&
                          summary.implementation_interface_resolution_hits +
                                  summary.implementation_interface_resolution_misses ==
                              summary.implementation_interface_resolution_sites &&
                          summary.method_resolution_hits <= summary.method_resolution_sites &&
                          summary.method_resolution_hits + summary.method_resolution_misses ==
                              summary.method_resolution_sites &&
                          summary.resolution_hits_total() <= summary.resolution_sites_total() &&
                          summary.resolution_hits_total() + summary.resolution_misses_total() ==
                              summary.resolution_sites_total();
  return summary;
}

static const Objc3MethodInfo *FindSurfaceMethodInSuperChain(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &interface_name,
    const std::string &selector,
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
    const auto method_it = super_it->second.methods.find(selector);
    if (method_it != super_it->second.methods.end()) {
      return &method_it->second;
    }
    next_super = super_it->second.super_name;
  }
  return nullptr;
}

static Objc3MethodLookupOverrideConflictSummary BuildMethodLookupOverrideConflictSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3MethodLookupOverrideConflictSummary summary;
  std::unordered_set<std::string> unresolved_units;

  for (const auto &implementation_entry : surface.implementations) {
    const std::string &implementation_name = implementation_entry.first;
    const Objc3ImplementationInfo &implementation_info = implementation_entry.second;
    summary.method_lookup_sites += implementation_info.methods.size();

    const auto interface_it = surface.interfaces.find(implementation_name);
    if (interface_it == surface.interfaces.end()) {
      unresolved_units.insert("impl:" + implementation_name);
      summary.method_lookup_misses += implementation_info.methods.size();
      continue;
    }

    for (const auto &method_entry : implementation_info.methods) {
      const auto interface_method_it = interface_it->second.methods.find(method_entry.first);
      if (interface_method_it == interface_it->second.methods.end()) {
        ++summary.method_lookup_misses;
      } else {
        ++summary.method_lookup_hits;
      }
    }
  }

  for (const auto &interface_entry : surface.interfaces) {
    const std::string &interface_name = interface_entry.first;
    const Objc3InterfaceInfo &interface_info = interface_entry.second;
    if (interface_info.super_name.empty()) {
      continue;
    }
    for (const auto &method_entry : interface_info.methods) {
      ++summary.override_lookup_sites;
      bool missing_base = false;
      bool cycle_detected = false;
      const Objc3MethodInfo *base_method =
          FindSurfaceMethodInSuperChain(surface, interface_name, method_entry.first, missing_base, cycle_detected);
      if (cycle_detected) {
        summary.deterministic = false;
      }
      if (missing_base) {
        unresolved_units.insert("iface:" + interface_name);
        ++summary.override_lookup_misses;
        continue;
      }
      if (base_method == nullptr) {
        ++summary.override_lookup_misses;
        continue;
      }
      ++summary.override_lookup_hits;
      if (!IsCompatibleMethodSignature(*base_method, method_entry.second)) {
        ++summary.override_conflicts;
      }
    }
  }

  summary.unresolved_base_interfaces = unresolved_units.size();
  summary.deterministic =
      summary.deterministic &&
      summary.method_lookup_hits <= summary.method_lookup_sites &&
      summary.method_lookup_hits + summary.method_lookup_misses == summary.method_lookup_sites &&
      summary.override_lookup_hits <= summary.override_lookup_sites &&
      summary.override_lookup_hits + summary.override_lookup_misses == summary.override_lookup_sites &&
      summary.override_conflicts <= summary.override_lookup_hits;
  return summary;
}

static bool IsCompatibleMethodTypeMetadataSignature(const Objc3SemanticMethodTypeMetadata &lhs,
                                                    const Objc3SemanticMethodTypeMetadata &rhs) {
  if (lhs.arity != rhs.arity || lhs.return_type != rhs.return_type ||
      lhs.return_is_vector != rhs.return_is_vector ||
      lhs.is_class_method != rhs.is_class_method) {
    return false;
  }
  if (lhs.return_is_vector &&
      (lhs.return_vector_base_spelling != rhs.return_vector_base_spelling ||
       lhs.return_vector_lane_count != rhs.return_vector_lane_count)) {
    return false;
  }
  if (lhs.param_types.size() != rhs.param_types.size() ||
      lhs.param_is_vector.size() != rhs.param_is_vector.size() ||
      lhs.param_vector_base_spelling.size() != rhs.param_vector_base_spelling.size() ||
      lhs.param_vector_lane_count.size() != rhs.param_vector_lane_count.size()) {
    return false;
  }
  for (std::size_t i = 0; i < lhs.param_types.size(); ++i) {
    if (lhs.param_types[i] != rhs.param_types[i] ||
        lhs.param_is_vector[i] != rhs.param_is_vector[i]) {
      return false;
    }
    if (lhs.param_is_vector[i] &&
        (lhs.param_vector_base_spelling[i] != rhs.param_vector_base_spelling[i] ||
         lhs.param_vector_lane_count[i] != rhs.param_vector_lane_count[i])) {
      return false;
    }
  }
  return true;
}

static const Objc3SemanticMethodTypeMetadata *FindMethodInInterfaceMetadata(
    const Objc3SemanticInterfaceTypeMetadata &metadata,
    const std::string &selector) {
  for (const auto &method_metadata : metadata.methods_lexicographic) {
    if (method_metadata.selector == selector) {
      return &method_metadata;
    }
  }
  return nullptr;
}

static const Objc3SemanticMethodTypeMetadata *FindHandoffMethodInSuperChain(
    const std::unordered_map<std::string, const Objc3SemanticInterfaceTypeMetadata *> &interfaces_by_name,
    const Objc3SemanticInterfaceTypeMetadata &metadata,
    const std::string &selector,
    bool &missing_base,
    bool &cycle_detected) {
  missing_base = false;
  cycle_detected = false;
  std::string next_super = metadata.super_name;
  std::unordered_set<std::string> visited;
  while (!next_super.empty()) {
    if (!visited.insert(next_super).second) {
      cycle_detected = true;
      return nullptr;
    }
    const auto super_it = interfaces_by_name.find(next_super);
    if (super_it == interfaces_by_name.end()) {
      missing_base = true;
      return nullptr;
    }
    const Objc3SemanticMethodTypeMetadata *method =
        FindMethodInInterfaceMetadata(*super_it->second, selector);
    if (method != nullptr) {
      return method;
    }
    next_super = super_it->second->super_name;
  }
  return nullptr;
}

static Objc3MethodLookupOverrideConflictSummary BuildMethodLookupOverrideConflictSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3MethodLookupOverrideConflictSummary summary;
  std::unordered_map<std::string, const Objc3SemanticInterfaceTypeMetadata *> interfaces_by_name;
  interfaces_by_name.reserve(handoff.interfaces_lexicographic.size());
  for (const auto &metadata : handoff.interfaces_lexicographic) {
    interfaces_by_name.emplace(metadata.name, &metadata);
  }
  std::unordered_set<std::string> unresolved_units;

  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    summary.method_lookup_sites += implementation_metadata.methods_lexicographic.size();
    const auto interface_it = interfaces_by_name.find(implementation_metadata.name);
    if (interface_it == interfaces_by_name.end()) {
      unresolved_units.insert("impl:" + implementation_metadata.name);
      summary.method_lookup_misses += implementation_metadata.methods_lexicographic.size();
      continue;
    }
    for (const auto &method_metadata : implementation_metadata.methods_lexicographic) {
      const Objc3SemanticMethodTypeMetadata *interface_method =
          FindMethodInInterfaceMetadata(*interface_it->second, method_metadata.selector);
      if (interface_method == nullptr) {
        ++summary.method_lookup_misses;
      } else {
        ++summary.method_lookup_hits;
      }
    }
  }

  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    if (interface_metadata.super_name.empty()) {
      continue;
    }
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      ++summary.override_lookup_sites;
      bool missing_base = false;
      bool cycle_detected = false;
      const Objc3SemanticMethodTypeMetadata *base_method =
          FindHandoffMethodInSuperChain(interfaces_by_name,
                                        interface_metadata,
                                        method_metadata.selector,
                                        missing_base,
                                        cycle_detected);
      if (cycle_detected) {
        summary.deterministic = false;
      }
      if (missing_base) {
        unresolved_units.insert("iface:" + interface_metadata.name);
        ++summary.override_lookup_misses;
        continue;
      }
      if (base_method == nullptr) {
        ++summary.override_lookup_misses;
        continue;
      }
      ++summary.override_lookup_hits;
      if (!IsCompatibleMethodTypeMetadataSignature(*base_method, method_metadata)) {
        ++summary.override_conflicts;
      }
    }
  }

  summary.unresolved_base_interfaces = unresolved_units.size();
  summary.deterministic =
      summary.deterministic &&
      summary.method_lookup_hits <= summary.method_lookup_sites &&
      summary.method_lookup_hits + summary.method_lookup_misses == summary.method_lookup_sites &&
      summary.override_lookup_hits <= summary.override_lookup_sites &&
      summary.override_lookup_hits + summary.override_lookup_misses == summary.override_lookup_sites &&
      summary.override_conflicts <= summary.override_lookup_hits;
  return summary;
}

static const Objc3SemanticPropertyTypeMetadata *FindPropertyInInterfaceMetadata(
    const Objc3SemanticInterfaceTypeMetadata &metadata,
    const std::string &property_name) {
  for (const auto &property_metadata : metadata.properties_lexicographic) {
    if (property_metadata.name == property_name) {
      return &property_metadata;
    }
  }
  return nullptr;
}

static bool IsCompatiblePropertyTypeMetadataSignature(const Objc3SemanticPropertyTypeMetadata &lhs,
                                                      const Objc3SemanticPropertyTypeMetadata &rhs) {
  return lhs.type == rhs.type &&
         lhs.is_vector == rhs.is_vector &&
         lhs.vector_base_spelling == rhs.vector_base_spelling &&
         lhs.vector_lane_count == rhs.vector_lane_count &&
         lhs.id_spelling == rhs.id_spelling &&
         lhs.class_spelling == rhs.class_spelling &&
         lhs.instancetype_spelling == rhs.instancetype_spelling &&
         lhs.is_readonly == rhs.is_readonly &&
         lhs.is_readwrite == rhs.is_readwrite &&
         lhs.is_atomic == rhs.is_atomic &&
         lhs.is_nonatomic == rhs.is_nonatomic &&
         lhs.is_copy == rhs.is_copy &&
         lhs.is_strong == rhs.is_strong &&
         lhs.is_weak == rhs.is_weak &&
         lhs.is_assign == rhs.is_assign &&
         lhs.has_getter == rhs.has_getter &&
         lhs.has_setter == rhs.has_setter &&
         lhs.getter_selector == rhs.getter_selector &&
         lhs.setter_selector == rhs.setter_selector;
}

static Objc3PropertySynthesisIvarBindingSummary BuildPropertySynthesisIvarBindingSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3PropertySynthesisIvarBindingSummary summary;
  for (const auto &implementation_entry : surface.implementations) {
    const auto interface_it = surface.interfaces.find(implementation_entry.first);
    const bool has_interface = interface_it != surface.interfaces.end();
    for (const auto &property_entry : implementation_entry.second.properties) {
      ++summary.property_synthesis_sites;
      ++summary.property_synthesis_default_ivar_bindings;
      ++summary.ivar_binding_sites;
      if (!has_interface) {
        ++summary.ivar_binding_missing;
        continue;
      }

      const auto interface_property_it = interface_it->second.properties.find(property_entry.first);
      if (interface_property_it == interface_it->second.properties.end()) {
        ++summary.ivar_binding_missing;
        continue;
      }

      if (!IsCompatiblePropertySignature(interface_property_it->second, property_entry.second)) {
        ++summary.ivar_binding_conflicts;
      } else {
        ++summary.ivar_binding_resolved;
      }
    }
  }

  summary.deterministic =
      summary.property_synthesis_explicit_ivar_bindings <= summary.property_synthesis_sites &&
      summary.property_synthesis_default_ivar_bindings <= summary.property_synthesis_sites &&
      summary.property_synthesis_explicit_ivar_bindings +
              summary.property_synthesis_default_ivar_bindings ==
          summary.property_synthesis_sites &&
      summary.ivar_binding_sites == summary.property_synthesis_sites &&
      summary.ivar_binding_resolved <= summary.ivar_binding_sites &&
      summary.ivar_binding_missing <= summary.ivar_binding_sites &&
      summary.ivar_binding_conflicts <= summary.ivar_binding_sites &&
      summary.ivar_binding_resolved + summary.ivar_binding_missing + summary.ivar_binding_conflicts ==
          summary.ivar_binding_sites;
  return summary;
}

static Objc3PropertySynthesisIvarBindingSummary BuildPropertySynthesisIvarBindingSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3PropertySynthesisIvarBindingSummary summary;
  std::unordered_map<std::string, const Objc3SemanticInterfaceTypeMetadata *> interfaces_by_name;
  interfaces_by_name.reserve(handoff.interfaces_lexicographic.size());
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    interfaces_by_name.emplace(interface_metadata.name, &interface_metadata);
  }

  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    const auto interface_it = interfaces_by_name.find(implementation_metadata.name);
    const bool has_interface = interface_it != interfaces_by_name.end();
    for (const auto &property_metadata : implementation_metadata.properties_lexicographic) {
      ++summary.property_synthesis_sites;
      ++summary.property_synthesis_default_ivar_bindings;
      ++summary.ivar_binding_sites;
      if (!has_interface) {
        ++summary.ivar_binding_missing;
        continue;
      }

      const Objc3SemanticPropertyTypeMetadata *interface_property =
          FindPropertyInInterfaceMetadata(*interface_it->second, property_metadata.name);
      if (interface_property == nullptr) {
        ++summary.ivar_binding_missing;
        continue;
      }

      if (!IsCompatiblePropertyTypeMetadataSignature(*interface_property, property_metadata)) {
        ++summary.ivar_binding_conflicts;
      } else {
        ++summary.ivar_binding_resolved;
      }
    }
  }

  summary.deterministic =
      summary.property_synthesis_explicit_ivar_bindings <= summary.property_synthesis_sites &&
      summary.property_synthesis_default_ivar_bindings <= summary.property_synthesis_sites &&
      summary.property_synthesis_explicit_ivar_bindings +
              summary.property_synthesis_default_ivar_bindings ==
          summary.property_synthesis_sites &&
      summary.ivar_binding_sites == summary.property_synthesis_sites &&
      summary.ivar_binding_resolved <= summary.ivar_binding_sites &&
      summary.ivar_binding_missing <= summary.ivar_binding_sites &&
      summary.ivar_binding_conflicts <= summary.ivar_binding_sites &&
      summary.ivar_binding_resolved + summary.ivar_binding_missing + summary.ivar_binding_conflicts ==
          summary.ivar_binding_sites;
  return summary;
}

static std::size_t CountSelectorKeywordPieces(const std::string &selector_symbol) {
  return static_cast<std::size_t>(std::count(selector_symbol.begin(), selector_symbol.end(), ':'));
}

static Expr::MessageSendForm ResolveMessageSendForm(const Expr &expr) {
  if (expr.message_send_form == Expr::MessageSendForm::Unary ||
      expr.message_send_form == Expr::MessageSendForm::Keyword) {
    return expr.message_send_form;
  }
  return expr.args.empty() ? Expr::MessageSendForm::Unary : Expr::MessageSendForm::Keyword;
}

static std::string ClassifyMethodFamilyFromSelector(const std::string &selector) {
  if (selector.rfind("mutableCopy", 0) == 0) {
    return "mutableCopy";
  }
  if (selector.rfind("copy", 0) == 0) {
    return "copy";
  }
  if (selector.rfind("init", 0) == 0) {
    return "init";
  }
  if (selector.rfind("new", 0) == 0) {
    return "new";
  }
  return "none";
}

static Objc3BlockLiteralCaptureSiteMetadata BuildBlockLiteralCaptureSiteMetadata(const Expr &expr) {
  Objc3BlockLiteralCaptureSiteMetadata metadata;
  metadata.parameter_count = expr.block_parameter_count;
  metadata.capture_count = expr.block_capture_count;
  metadata.body_statement_count = expr.block_body_statement_count;
  metadata.capture_set_deterministic = expr.block_capture_set_deterministic;
  metadata.literal_is_normalized = expr.block_literal_is_normalized;
  metadata.capture_profile = expr.block_capture_profile;
  metadata.line = expr.line;
  metadata.column = expr.column;
  metadata.has_count_mismatch =
      expr.block_parameter_names_lexicographic.size() != metadata.parameter_count ||
      expr.block_capture_names_lexicographic.size() != metadata.capture_count ||
      !IsSortedUniqueStrings(expr.block_parameter_names_lexicographic) ||
      !IsSortedUniqueStrings(expr.block_capture_names_lexicographic);
  return metadata;
}

static void CollectBlockLiteralCaptureSiteMetadataFromExpr(const Expr *expr,
                                                           std::vector<Objc3BlockLiteralCaptureSiteMetadata> &sites) {
  if (expr == nullptr) {
    return;
  }

  if (expr->kind == Expr::Kind::BlockLiteral) {
    sites.push_back(BuildBlockLiteralCaptureSiteMetadata(*expr));
  }

  CollectBlockLiteralCaptureSiteMetadataFromExpr(expr->receiver.get(), sites);
  CollectBlockLiteralCaptureSiteMetadataFromExpr(expr->left.get(), sites);
  CollectBlockLiteralCaptureSiteMetadataFromExpr(expr->right.get(), sites);
  CollectBlockLiteralCaptureSiteMetadataFromExpr(expr->third.get(), sites);
  for (const auto &arg : expr->args) {
    CollectBlockLiteralCaptureSiteMetadataFromExpr(arg.get(), sites);
  }
}

static void CollectBlockLiteralCaptureSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3BlockLiteralCaptureSiteMetadata> &sites);

static void CollectBlockLiteralCaptureSiteMetadataFromForClause(
    const ForClause &clause,
    std::vector<Objc3BlockLiteralCaptureSiteMetadata> &sites) {
  CollectBlockLiteralCaptureSiteMetadataFromExpr(clause.value.get(), sites);
}

static void CollectBlockLiteralCaptureSiteMetadataFromStatement(
    const Stmt *stmt, std::vector<Objc3BlockLiteralCaptureSiteMetadata> &sites) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
    case Stmt::Kind::Let:
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->let_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Assign:
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->assign_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Return:
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->return_stmt->value.get(), sites);
      return;
    case Stmt::Kind::If:
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->if_stmt->condition.get(), sites);
      CollectBlockLiteralCaptureSiteMetadataFromStatements(stmt->if_stmt->then_body, sites);
      CollectBlockLiteralCaptureSiteMetadataFromStatements(stmt->if_stmt->else_body, sites);
      return;
    case Stmt::Kind::DoWhile:
      CollectBlockLiteralCaptureSiteMetadataFromStatements(stmt->do_while_stmt->body, sites);
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->do_while_stmt->condition.get(), sites);
      return;
    case Stmt::Kind::For:
      CollectBlockLiteralCaptureSiteMetadataFromForClause(stmt->for_stmt->init, sites);
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->for_stmt->condition.get(), sites);
      CollectBlockLiteralCaptureSiteMetadataFromForClause(stmt->for_stmt->step, sites);
      CollectBlockLiteralCaptureSiteMetadataFromStatements(stmt->for_stmt->body, sites);
      return;
    case Stmt::Kind::Switch:
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->switch_stmt->condition.get(), sites);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        CollectBlockLiteralCaptureSiteMetadataFromStatements(switch_case.body, sites);
      }
      return;
    case Stmt::Kind::While:
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->while_stmt->condition.get(), sites);
      CollectBlockLiteralCaptureSiteMetadataFromStatements(stmt->while_stmt->body, sites);
      return;
    case Stmt::Kind::Block:
      CollectBlockLiteralCaptureSiteMetadataFromStatements(stmt->block_stmt->body, sites);
      return;
    case Stmt::Kind::Expr:
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->expr_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectBlockLiteralCaptureSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3BlockLiteralCaptureSiteMetadata> &sites) {
  for (const auto &statement : statements) {
    CollectBlockLiteralCaptureSiteMetadataFromStatement(statement.get(), sites);
  }
}

static bool IsBlockLiteralCaptureSiteMetadataLess(const Objc3BlockLiteralCaptureSiteMetadata &lhs,
                                                  const Objc3BlockLiteralCaptureSiteMetadata &rhs) {
  if (lhs.capture_profile != rhs.capture_profile) {
    return lhs.capture_profile < rhs.capture_profile;
  }
  if (lhs.parameter_count != rhs.parameter_count) {
    return lhs.parameter_count < rhs.parameter_count;
  }
  if (lhs.capture_count != rhs.capture_count) {
    return lhs.capture_count < rhs.capture_count;
  }
  if (lhs.body_statement_count != rhs.body_statement_count) {
    return lhs.body_statement_count < rhs.body_statement_count;
  }
  if (lhs.capture_set_deterministic != rhs.capture_set_deterministic) {
    return lhs.capture_set_deterministic < rhs.capture_set_deterministic;
  }
  if (lhs.literal_is_normalized != rhs.literal_is_normalized) {
    return lhs.literal_is_normalized < rhs.literal_is_normalized;
  }
  if (lhs.has_count_mismatch != rhs.has_count_mismatch) {
    return lhs.has_count_mismatch < rhs.has_count_mismatch;
  }
  if (lhs.line != rhs.line) {
    return lhs.line < rhs.line;
  }
  return lhs.column < rhs.column;
}

static std::vector<Objc3BlockLiteralCaptureSiteMetadata>
BuildBlockLiteralCaptureSiteMetadataLexicographic(const Objc3Program &ast) {
  std::vector<Objc3BlockLiteralCaptureSiteMetadata> sites;
  for (const auto &global : ast.globals) {
    CollectBlockLiteralCaptureSiteMetadataFromExpr(global.value.get(), sites);
  }
  for (const auto &fn : ast.functions) {
    CollectBlockLiteralCaptureSiteMetadataFromStatements(fn.body, sites);
  }
  std::sort(sites.begin(), sites.end(), IsBlockLiteralCaptureSiteMetadataLess);
  return sites;
}

static Objc3BlockLiteralCaptureSemanticsSummary BuildBlockLiteralCaptureSemanticsSummaryFromSites(
    const std::vector<Objc3BlockLiteralCaptureSiteMetadata> &sites) {
  Objc3BlockLiteralCaptureSemanticsSummary summary;
  summary.block_literal_sites = sites.size();
  for (const auto &site : sites) {
    summary.block_parameter_entries += site.parameter_count;
    summary.block_capture_entries += site.capture_count;
    summary.block_body_statement_entries += site.body_statement_count;
    if (site.capture_count == 0u) {
      ++summary.block_empty_capture_sites;
    }
    if (!site.capture_set_deterministic) {
      ++summary.block_nondeterministic_capture_sites;
    }
    if (!site.literal_is_normalized) {
      ++summary.block_non_normalized_sites;
    }
    const bool profile_missing = site.capture_count > 0u && site.capture_profile.empty();
    const bool site_contract_violation = site.has_count_mismatch || !site.capture_set_deterministic ||
                                         !site.literal_is_normalized || profile_missing;
    if (site_contract_violation) {
      ++summary.contract_violation_sites;
    }
  }
  summary.deterministic =
      summary.contract_violation_sites == 0u &&
      summary.block_empty_capture_sites <= summary.block_literal_sites &&
      summary.block_nondeterministic_capture_sites <= summary.block_literal_sites &&
      summary.block_non_normalized_sites <= summary.block_literal_sites &&
      summary.contract_violation_sites <= summary.block_literal_sites;
  return summary;
}

static Objc3BlockLiteralCaptureSemanticsSummary BuildBlockLiteralCaptureSemanticsSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  return BuildBlockLiteralCaptureSemanticsSummaryFromSites(surface.block_literal_capture_sites_lexicographic);
}

static Objc3BlockLiteralCaptureSemanticsSummary BuildBlockLiteralCaptureSemanticsSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  return BuildBlockLiteralCaptureSemanticsSummaryFromSites(handoff.block_literal_capture_sites_lexicographic);
}

static Objc3BlockAbiInvokeTrampolineSiteMetadata BuildBlockAbiInvokeTrampolineSiteMetadata(const Expr &expr) {
  Objc3BlockAbiInvokeTrampolineSiteMetadata metadata;
  metadata.invoke_argument_slots = expr.block_abi_invoke_argument_slots;
  metadata.capture_word_count = expr.block_abi_capture_word_count;
  metadata.parameter_count = expr.block_parameter_count;
  metadata.capture_count = expr.block_capture_count;
  metadata.body_statement_count = expr.block_body_statement_count;
  metadata.has_invoke_trampoline = expr.block_abi_has_invoke_trampoline;
  metadata.layout_is_normalized = expr.block_abi_layout_is_normalized;
  metadata.layout_profile = expr.block_abi_layout_profile;
  metadata.descriptor_symbol = expr.block_abi_descriptor_symbol;
  metadata.invoke_trampoline_symbol = expr.block_invoke_trampoline_symbol;
  metadata.line = expr.line;
  metadata.column = expr.column;
  metadata.has_count_mismatch =
      metadata.invoke_argument_slots != metadata.parameter_count ||
      metadata.capture_word_count != metadata.capture_count ||
      !IsSortedUniqueStrings(expr.block_parameter_names_lexicographic) ||
      !IsSortedUniqueStrings(expr.block_capture_names_lexicographic);
  return metadata;
}

static void CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(
    const Expr *expr,
    std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> &sites) {
  if (expr == nullptr) {
    return;
  }

  if (expr->kind == Expr::Kind::BlockLiteral) {
    sites.push_back(BuildBlockAbiInvokeTrampolineSiteMetadata(*expr));
  }

  CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(expr->receiver.get(), sites);
  CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(expr->left.get(), sites);
  CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(expr->right.get(), sites);
  CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(expr->third.get(), sites);
  for (const auto &arg : expr->args) {
    CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(arg.get(), sites);
  }
}

static void CollectBlockAbiInvokeTrampolineSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> &sites);

static void CollectBlockAbiInvokeTrampolineSiteMetadataFromForClause(
    const ForClause &clause,
    std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> &sites) {
  CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(clause.value.get(), sites);
}

static void CollectBlockAbiInvokeTrampolineSiteMetadataFromStatement(
    const Stmt *stmt,
    std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> &sites) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
    case Stmt::Kind::Let:
      CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(stmt->let_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Assign:
      CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(stmt->assign_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Return:
      CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(stmt->return_stmt->value.get(), sites);
      return;
    case Stmt::Kind::If:
      CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(stmt->if_stmt->condition.get(), sites);
      CollectBlockAbiInvokeTrampolineSiteMetadataFromStatements(stmt->if_stmt->then_body, sites);
      CollectBlockAbiInvokeTrampolineSiteMetadataFromStatements(stmt->if_stmt->else_body, sites);
      return;
    case Stmt::Kind::DoWhile:
      CollectBlockAbiInvokeTrampolineSiteMetadataFromStatements(stmt->do_while_stmt->body, sites);
      CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(stmt->do_while_stmt->condition.get(), sites);
      return;
    case Stmt::Kind::For:
      CollectBlockAbiInvokeTrampolineSiteMetadataFromForClause(stmt->for_stmt->init, sites);
      CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(stmt->for_stmt->condition.get(), sites);
      CollectBlockAbiInvokeTrampolineSiteMetadataFromForClause(stmt->for_stmt->step, sites);
      CollectBlockAbiInvokeTrampolineSiteMetadataFromStatements(stmt->for_stmt->body, sites);
      return;
    case Stmt::Kind::Switch:
      CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(stmt->switch_stmt->condition.get(), sites);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        CollectBlockAbiInvokeTrampolineSiteMetadataFromStatements(switch_case.body, sites);
      }
      return;
    case Stmt::Kind::While:
      CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(stmt->while_stmt->condition.get(), sites);
      CollectBlockAbiInvokeTrampolineSiteMetadataFromStatements(stmt->while_stmt->body, sites);
      return;
    case Stmt::Kind::Block:
      CollectBlockAbiInvokeTrampolineSiteMetadataFromStatements(stmt->block_stmt->body, sites);
      return;
    case Stmt::Kind::Expr:
      CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(stmt->expr_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectBlockAbiInvokeTrampolineSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> &sites) {
  for (const auto &statement : statements) {
    CollectBlockAbiInvokeTrampolineSiteMetadataFromStatement(statement.get(), sites);
  }
}

static bool IsBlockAbiInvokeTrampolineSiteMetadataLess(
    const Objc3BlockAbiInvokeTrampolineSiteMetadata &lhs,
    const Objc3BlockAbiInvokeTrampolineSiteMetadata &rhs) {
  if (lhs.layout_profile != rhs.layout_profile) {
    return lhs.layout_profile < rhs.layout_profile;
  }
  if (lhs.descriptor_symbol != rhs.descriptor_symbol) {
    return lhs.descriptor_symbol < rhs.descriptor_symbol;
  }
  if (lhs.invoke_trampoline_symbol != rhs.invoke_trampoline_symbol) {
    return lhs.invoke_trampoline_symbol < rhs.invoke_trampoline_symbol;
  }
  if (lhs.invoke_argument_slots != rhs.invoke_argument_slots) {
    return lhs.invoke_argument_slots < rhs.invoke_argument_slots;
  }
  if (lhs.capture_word_count != rhs.capture_word_count) {
    return lhs.capture_word_count < rhs.capture_word_count;
  }
  if (lhs.parameter_count != rhs.parameter_count) {
    return lhs.parameter_count < rhs.parameter_count;
  }
  if (lhs.capture_count != rhs.capture_count) {
    return lhs.capture_count < rhs.capture_count;
  }
  if (lhs.body_statement_count != rhs.body_statement_count) {
    return lhs.body_statement_count < rhs.body_statement_count;
  }
  if (lhs.has_invoke_trampoline != rhs.has_invoke_trampoline) {
    return lhs.has_invoke_trampoline < rhs.has_invoke_trampoline;
  }
  if (lhs.layout_is_normalized != rhs.layout_is_normalized) {
    return lhs.layout_is_normalized < rhs.layout_is_normalized;
  }
  if (lhs.has_count_mismatch != rhs.has_count_mismatch) {
    return lhs.has_count_mismatch < rhs.has_count_mismatch;
  }
  if (lhs.line != rhs.line) {
    return lhs.line < rhs.line;
  }
  return lhs.column < rhs.column;
}

static std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata>
BuildBlockAbiInvokeTrampolineSiteMetadataLexicographic(const Objc3Program &ast) {
  std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> sites;
  for (const auto &global : ast.globals) {
    CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(global.value.get(), sites);
  }
  for (const auto &fn : ast.functions) {
    CollectBlockAbiInvokeTrampolineSiteMetadataFromStatements(fn.body, sites);
  }
  std::sort(sites.begin(), sites.end(), IsBlockAbiInvokeTrampolineSiteMetadataLess);
  return sites;
}

static Objc3BlockAbiInvokeTrampolineSemanticsSummary BuildBlockAbiInvokeTrampolineSemanticsSummaryFromSites(
    const std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> &sites) {
  Objc3BlockAbiInvokeTrampolineSemanticsSummary summary;
  summary.block_literal_sites = sites.size();
  for (const auto &site : sites) {
    summary.invoke_argument_slots_total += site.invoke_argument_slots;
    summary.capture_word_count_total += site.capture_word_count;
    summary.parameter_entries_total += site.parameter_count;
    summary.capture_entries_total += site.capture_count;
    summary.body_statement_entries_total += site.body_statement_count;
    if (!site.descriptor_symbol.empty()) {
      ++summary.descriptor_symbolized_sites;
    }
    if (!site.invoke_trampoline_symbol.empty()) {
      ++summary.invoke_trampoline_symbolized_sites;
    }
    if (!site.has_invoke_trampoline) {
      ++summary.missing_invoke_trampoline_sites;
    }
    if (!site.layout_is_normalized) {
      ++summary.non_normalized_layout_sites;
    }
    const bool layout_profile_missing = site.layout_profile.empty();
    const bool descriptor_symbol_missing = site.descriptor_symbol.empty();
    const bool invoke_trampoline_symbol_missing =
        site.has_invoke_trampoline && site.invoke_trampoline_symbol.empty();
    const bool invoke_trampoline_symbol_mismatch =
        !site.has_invoke_trampoline && !site.invoke_trampoline_symbol.empty();
    const bool invoke_argument_slot_mismatch = site.invoke_argument_slots != site.parameter_count;
    const bool capture_word_count_mismatch = site.capture_word_count != site.capture_count;
    const bool site_contract_violation =
        site.has_count_mismatch || !site.has_invoke_trampoline || !site.layout_is_normalized ||
        layout_profile_missing || descriptor_symbol_missing || invoke_trampoline_symbol_missing ||
        invoke_trampoline_symbol_mismatch || invoke_argument_slot_mismatch ||
        capture_word_count_mismatch;
    if (site_contract_violation) {
      ++summary.contract_violation_sites;
    }
  }
  summary.deterministic =
      summary.contract_violation_sites == 0u &&
      summary.descriptor_symbolized_sites <= summary.block_literal_sites &&
      summary.invoke_trampoline_symbolized_sites <= summary.block_literal_sites &&
      summary.missing_invoke_trampoline_sites <= summary.block_literal_sites &&
      summary.non_normalized_layout_sites <= summary.block_literal_sites &&
      summary.invoke_trampoline_symbolized_sites + summary.missing_invoke_trampoline_sites ==
          summary.block_literal_sites &&
      summary.invoke_argument_slots_total == summary.parameter_entries_total &&
      summary.capture_word_count_total == summary.capture_entries_total;
  return summary;
}

static Objc3BlockAbiInvokeTrampolineSemanticsSummary BuildBlockAbiInvokeTrampolineSemanticsSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  return BuildBlockAbiInvokeTrampolineSemanticsSummaryFromSites(
      surface.block_abi_invoke_trampoline_sites_lexicographic);
}

static Objc3BlockAbiInvokeTrampolineSemanticsSummary BuildBlockAbiInvokeTrampolineSemanticsSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  return BuildBlockAbiInvokeTrampolineSemanticsSummaryFromSites(
      handoff.block_abi_invoke_trampoline_sites_lexicographic);
}

static Objc3BlockStorageEscapeSiteMetadata BuildBlockStorageEscapeSiteMetadata(const Expr &expr) {
  Objc3BlockStorageEscapeSiteMetadata metadata;
  metadata.mutable_capture_count = expr.block_storage_mutable_capture_count;
  metadata.byref_slot_count = expr.block_storage_byref_slot_count;
  metadata.parameter_count = expr.block_parameter_count;
  metadata.capture_count = expr.block_capture_count;
  metadata.body_statement_count = expr.block_body_statement_count;
  metadata.requires_byref_cells = expr.block_storage_requires_byref_cells;
  metadata.escape_analysis_enabled = expr.block_storage_escape_analysis_enabled;
  metadata.escape_to_heap = expr.block_storage_escape_to_heap;
  metadata.escape_profile_is_normalized = expr.block_storage_escape_profile_is_normalized;
  metadata.escape_profile = expr.block_storage_escape_profile;
  metadata.byref_layout_symbol = expr.block_storage_byref_layout_symbol;
  metadata.line = expr.line;
  metadata.column = expr.column;
  metadata.has_count_mismatch =
      metadata.mutable_capture_count != metadata.capture_count ||
      metadata.byref_slot_count != metadata.capture_count ||
      !IsSortedUniqueStrings(expr.block_parameter_names_lexicographic) ||
      !IsSortedUniqueStrings(expr.block_capture_names_lexicographic);
  return metadata;
}

static void CollectBlockStorageEscapeSiteMetadataFromExpr(
    const Expr *expr,
    std::vector<Objc3BlockStorageEscapeSiteMetadata> &sites) {
  if (expr == nullptr) {
    return;
  }

  if (expr->kind == Expr::Kind::BlockLiteral) {
    sites.push_back(BuildBlockStorageEscapeSiteMetadata(*expr));
  }

  CollectBlockStorageEscapeSiteMetadataFromExpr(expr->receiver.get(), sites);
  CollectBlockStorageEscapeSiteMetadataFromExpr(expr->left.get(), sites);
  CollectBlockStorageEscapeSiteMetadataFromExpr(expr->right.get(), sites);
  CollectBlockStorageEscapeSiteMetadataFromExpr(expr->third.get(), sites);
  for (const auto &arg : expr->args) {
    CollectBlockStorageEscapeSiteMetadataFromExpr(arg.get(), sites);
  }
}

static void CollectBlockStorageEscapeSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3BlockStorageEscapeSiteMetadata> &sites);

static void CollectBlockStorageEscapeSiteMetadataFromForClause(
    const ForClause &clause,
    std::vector<Objc3BlockStorageEscapeSiteMetadata> &sites) {
  CollectBlockStorageEscapeSiteMetadataFromExpr(clause.value.get(), sites);
}

static void CollectBlockStorageEscapeSiteMetadataFromStatement(
    const Stmt *stmt,
    std::vector<Objc3BlockStorageEscapeSiteMetadata> &sites) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
    case Stmt::Kind::Let:
      CollectBlockStorageEscapeSiteMetadataFromExpr(stmt->let_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Assign:
      CollectBlockStorageEscapeSiteMetadataFromExpr(stmt->assign_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Return:
      CollectBlockStorageEscapeSiteMetadataFromExpr(stmt->return_stmt->value.get(), sites);
      return;
    case Stmt::Kind::If:
      CollectBlockStorageEscapeSiteMetadataFromExpr(stmt->if_stmt->condition.get(), sites);
      CollectBlockStorageEscapeSiteMetadataFromStatements(stmt->if_stmt->then_body, sites);
      CollectBlockStorageEscapeSiteMetadataFromStatements(stmt->if_stmt->else_body, sites);
      return;
    case Stmt::Kind::DoWhile:
      CollectBlockStorageEscapeSiteMetadataFromStatements(stmt->do_while_stmt->body, sites);
      CollectBlockStorageEscapeSiteMetadataFromExpr(stmt->do_while_stmt->condition.get(), sites);
      return;
    case Stmt::Kind::For:
      CollectBlockStorageEscapeSiteMetadataFromForClause(stmt->for_stmt->init, sites);
      CollectBlockStorageEscapeSiteMetadataFromExpr(stmt->for_stmt->condition.get(), sites);
      CollectBlockStorageEscapeSiteMetadataFromForClause(stmt->for_stmt->step, sites);
      CollectBlockStorageEscapeSiteMetadataFromStatements(stmt->for_stmt->body, sites);
      return;
    case Stmt::Kind::Switch:
      CollectBlockStorageEscapeSiteMetadataFromExpr(stmt->switch_stmt->condition.get(), sites);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        CollectBlockStorageEscapeSiteMetadataFromStatements(switch_case.body, sites);
      }
      return;
    case Stmt::Kind::While:
      CollectBlockStorageEscapeSiteMetadataFromExpr(stmt->while_stmt->condition.get(), sites);
      CollectBlockStorageEscapeSiteMetadataFromStatements(stmt->while_stmt->body, sites);
      return;
    case Stmt::Kind::Block:
      CollectBlockStorageEscapeSiteMetadataFromStatements(stmt->block_stmt->body, sites);
      return;
    case Stmt::Kind::Expr:
      CollectBlockStorageEscapeSiteMetadataFromExpr(stmt->expr_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectBlockStorageEscapeSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3BlockStorageEscapeSiteMetadata> &sites) {
  for (const auto &statement : statements) {
    CollectBlockStorageEscapeSiteMetadataFromStatement(statement.get(), sites);
  }
}

static bool IsBlockStorageEscapeSiteMetadataLess(
    const Objc3BlockStorageEscapeSiteMetadata &lhs,
    const Objc3BlockStorageEscapeSiteMetadata &rhs) {
  if (lhs.escape_profile != rhs.escape_profile) {
    return lhs.escape_profile < rhs.escape_profile;
  }
  if (lhs.byref_layout_symbol != rhs.byref_layout_symbol) {
    return lhs.byref_layout_symbol < rhs.byref_layout_symbol;
  }
  if (lhs.mutable_capture_count != rhs.mutable_capture_count) {
    return lhs.mutable_capture_count < rhs.mutable_capture_count;
  }
  if (lhs.byref_slot_count != rhs.byref_slot_count) {
    return lhs.byref_slot_count < rhs.byref_slot_count;
  }
  if (lhs.parameter_count != rhs.parameter_count) {
    return lhs.parameter_count < rhs.parameter_count;
  }
  if (lhs.capture_count != rhs.capture_count) {
    return lhs.capture_count < rhs.capture_count;
  }
  if (lhs.body_statement_count != rhs.body_statement_count) {
    return lhs.body_statement_count < rhs.body_statement_count;
  }
  if (lhs.requires_byref_cells != rhs.requires_byref_cells) {
    return lhs.requires_byref_cells < rhs.requires_byref_cells;
  }
  if (lhs.escape_analysis_enabled != rhs.escape_analysis_enabled) {
    return lhs.escape_analysis_enabled < rhs.escape_analysis_enabled;
  }
  if (lhs.escape_to_heap != rhs.escape_to_heap) {
    return lhs.escape_to_heap < rhs.escape_to_heap;
  }
  if (lhs.escape_profile_is_normalized != rhs.escape_profile_is_normalized) {
    return lhs.escape_profile_is_normalized < rhs.escape_profile_is_normalized;
  }
  if (lhs.has_count_mismatch != rhs.has_count_mismatch) {
    return lhs.has_count_mismatch < rhs.has_count_mismatch;
  }
  if (lhs.line != rhs.line) {
    return lhs.line < rhs.line;
  }
  return lhs.column < rhs.column;
}

static std::vector<Objc3BlockStorageEscapeSiteMetadata>
BuildBlockStorageEscapeSiteMetadataLexicographic(const Objc3Program &ast) {
  std::vector<Objc3BlockStorageEscapeSiteMetadata> sites;
  for (const auto &global : ast.globals) {
    CollectBlockStorageEscapeSiteMetadataFromExpr(global.value.get(), sites);
  }
  for (const auto &fn : ast.functions) {
    CollectBlockStorageEscapeSiteMetadataFromStatements(fn.body, sites);
  }
  std::sort(sites.begin(), sites.end(), IsBlockStorageEscapeSiteMetadataLess);
  return sites;
}

static Objc3BlockStorageEscapeSemanticsSummary BuildBlockStorageEscapeSemanticsSummaryFromSites(
    const std::vector<Objc3BlockStorageEscapeSiteMetadata> &sites) {
  Objc3BlockStorageEscapeSemanticsSummary summary;
  summary.block_literal_sites = sites.size();
  for (const auto &site : sites) {
    summary.mutable_capture_count_total += site.mutable_capture_count;
    summary.byref_slot_count_total += site.byref_slot_count;
    summary.parameter_entries_total += site.parameter_count;
    summary.capture_entries_total += site.capture_count;
    summary.body_statement_entries_total += site.body_statement_count;
    if (site.requires_byref_cells) {
      ++summary.requires_byref_cells_sites;
    }
    if (site.escape_analysis_enabled) {
      ++summary.escape_analysis_enabled_sites;
    }
    if (site.escape_to_heap) {
      ++summary.escape_to_heap_sites;
    }
    if (site.escape_profile_is_normalized) {
      ++summary.escape_profile_normalized_sites;
    }
    if (!site.byref_layout_symbol.empty()) {
      ++summary.byref_layout_symbolized_sites;
    }
    const bool escape_profile_missing = site.escape_analysis_enabled && site.escape_profile.empty();
    const bool byref_layout_symbol_missing = site.requires_byref_cells && site.byref_layout_symbol.empty();
    const bool byref_requirement_mismatch = site.requires_byref_cells != (site.byref_slot_count > 0u);
    const bool escape_heap_mismatch = site.escape_to_heap && !site.requires_byref_cells;
    const bool count_mismatch = site.mutable_capture_count != site.capture_count ||
                                site.byref_slot_count != site.capture_count;
    const bool site_contract_violation =
        site.has_count_mismatch || count_mismatch || !site.escape_analysis_enabled ||
        !site.escape_profile_is_normalized || escape_profile_missing ||
        byref_layout_symbol_missing || byref_requirement_mismatch || escape_heap_mismatch;
    if (site_contract_violation) {
      ++summary.contract_violation_sites;
    }
  }
  summary.deterministic =
      summary.contract_violation_sites == 0u &&
      summary.requires_byref_cells_sites <= summary.block_literal_sites &&
      summary.escape_analysis_enabled_sites <= summary.block_literal_sites &&
      summary.escape_to_heap_sites <= summary.block_literal_sites &&
      summary.escape_profile_normalized_sites <= summary.block_literal_sites &&
      summary.byref_layout_symbolized_sites <= summary.block_literal_sites &&
      summary.contract_violation_sites <= summary.block_literal_sites &&
      summary.mutable_capture_count_total == summary.capture_entries_total &&
      summary.byref_slot_count_total == summary.capture_entries_total &&
      summary.escape_analysis_enabled_sites == summary.block_literal_sites &&
      summary.requires_byref_cells_sites == summary.escape_to_heap_sites;
  return summary;
}

static Objc3BlockStorageEscapeSemanticsSummary BuildBlockStorageEscapeSemanticsSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  return BuildBlockStorageEscapeSemanticsSummaryFromSites(
      surface.block_storage_escape_sites_lexicographic);
}

static Objc3BlockStorageEscapeSemanticsSummary BuildBlockStorageEscapeSemanticsSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  return BuildBlockStorageEscapeSemanticsSummaryFromSites(
      handoff.block_storage_escape_sites_lexicographic);
}

static Objc3MessageSendSelectorLoweringSiteMetadata BuildMessageSendSelectorLoweringSiteMetadata(const Expr &expr) {
  Objc3MessageSendSelectorLoweringSiteMetadata metadata;
  metadata.selector = expr.selector;
  metadata.selector_lowering_symbol =
      expr.selector_lowering_symbol.empty() ? expr.selector : expr.selector_lowering_symbol;
  metadata.argument_count = expr.args.size();
  const Expr::MessageSendForm resolved_form = ResolveMessageSendForm(expr);
  metadata.unary_form = resolved_form == Expr::MessageSendForm::Unary;
  metadata.keyword_form = resolved_form == Expr::MessageSendForm::Keyword;
  metadata.line = expr.line;
  metadata.column = expr.column;

  if (!expr.selector_lowering_pieces.empty()) {
    metadata.selector_piece_count = expr.selector_lowering_pieces.size();
    for (const Expr::MessageSendSelectorPiece &piece : expr.selector_lowering_pieces) {
      if (piece.has_argument) {
        ++metadata.selector_argument_piece_count;
      }
    }
  } else if (metadata.keyword_form) {
    const std::string selector_symbol =
        metadata.selector_lowering_symbol.empty() ? metadata.selector : metadata.selector_lowering_symbol;
    metadata.selector_piece_count = CountSelectorKeywordPieces(selector_symbol);
    metadata.selector_argument_piece_count = metadata.selector_piece_count;
  }

  metadata.selector_lowering_is_normalized =
      expr.selector_lowering_is_normalized ||
      (!metadata.selector.empty() && !metadata.selector_lowering_symbol.empty() &&
       metadata.selector == metadata.selector_lowering_symbol);
  metadata.receiver_is_nil_literal = expr.receiver != nullptr && expr.receiver->kind == Expr::Kind::NilLiteral;
  metadata.nil_receiver_semantics_enabled =
      expr.nil_receiver_semantics_is_normalized ? expr.nil_receiver_semantics_enabled : metadata.receiver_is_nil_literal;
  metadata.nil_receiver_foldable =
      expr.nil_receiver_semantics_is_normalized ? expr.nil_receiver_foldable : metadata.nil_receiver_semantics_enabled;
  metadata.nil_receiver_requires_runtime_dispatch =
      expr.nil_receiver_semantics_is_normalized ? expr.nil_receiver_requires_runtime_dispatch
                                                : !metadata.nil_receiver_foldable;
  metadata.nil_receiver_semantics_is_normalized =
      expr.nil_receiver_semantics_is_normalized ||
      (metadata.nil_receiver_semantics_enabled == metadata.receiver_is_nil_literal &&
       metadata.nil_receiver_semantics_enabled == metadata.nil_receiver_foldable &&
       metadata.nil_receiver_requires_runtime_dispatch == !metadata.nil_receiver_foldable);
  metadata.runtime_shim_host_link_required =
      expr.runtime_shim_host_link_is_normalized ? expr.runtime_shim_host_link_required
                                                : metadata.nil_receiver_requires_runtime_dispatch;
  metadata.runtime_shim_host_link_elided =
      expr.runtime_shim_host_link_is_normalized ? expr.runtime_shim_host_link_elided
                                                : !metadata.runtime_shim_host_link_required;
  metadata.runtime_shim_host_link_runtime_dispatch_arg_slots = expr.dispatch_abi_runtime_arg_slots;
  metadata.runtime_shim_host_link_declaration_parameter_count =
      expr.runtime_shim_host_link_is_normalized
          ? static_cast<std::size_t>(expr.runtime_shim_host_link_declaration_parameter_count)
          : metadata.runtime_shim_host_link_runtime_dispatch_arg_slots + 2u;
  metadata.runtime_dispatch_bridge_symbol = expr.runtime_dispatch_bridge_symbol.empty()
                                                ? kObjc3RuntimeShimHostLinkDefaultDispatchSymbol
                                                : expr.runtime_dispatch_bridge_symbol;
  metadata.runtime_shim_host_link_symbol = expr.runtime_shim_host_link_symbol;
  metadata.runtime_shim_host_link_is_normalized =
      expr.runtime_shim_host_link_is_normalized ||
      (metadata.runtime_shim_host_link_required == metadata.nil_receiver_requires_runtime_dispatch &&
       metadata.runtime_shim_host_link_elided == !metadata.runtime_shim_host_link_required &&
       metadata.runtime_shim_host_link_declaration_parameter_count ==
           metadata.runtime_shim_host_link_runtime_dispatch_arg_slots + 2u &&
       !metadata.runtime_dispatch_bridge_symbol.empty());
  metadata.receiver_is_super_identifier =
      expr.receiver != nullptr && expr.receiver->kind == Expr::Kind::Identifier && expr.receiver->ident == "super";
  metadata.super_dispatch_enabled =
      expr.super_dispatch_semantics_is_normalized ? expr.super_dispatch_enabled : metadata.receiver_is_super_identifier;
  metadata.super_dispatch_requires_class_context =
      expr.super_dispatch_semantics_is_normalized ? expr.super_dispatch_requires_class_context
                                                  : metadata.super_dispatch_enabled;
  metadata.super_dispatch_semantics_is_normalized =
      expr.super_dispatch_semantics_is_normalized ||
      (metadata.super_dispatch_enabled == metadata.receiver_is_super_identifier &&
       metadata.super_dispatch_requires_class_context == metadata.super_dispatch_enabled);
  metadata.method_family_name = expr.method_family_semantics_is_normalized && !expr.method_family_name.empty()
                                    ? expr.method_family_name
                                    : ClassifyMethodFamilyFromSelector(metadata.selector);
  metadata.method_family_returns_retained_result =
      expr.method_family_semantics_is_normalized
          ? expr.method_family_returns_retained_result
          : (metadata.method_family_name == "init" || metadata.method_family_name == "copy" ||
             metadata.method_family_name == "mutableCopy" || metadata.method_family_name == "new");
  metadata.method_family_returns_related_result =
      expr.method_family_semantics_is_normalized ? expr.method_family_returns_related_result
                                                 : metadata.method_family_name == "init";
  metadata.method_family_semantics_is_normalized =
      expr.method_family_semantics_is_normalized ||
      ((metadata.method_family_name == "init" || metadata.method_family_name == "copy" ||
        metadata.method_family_name == "mutableCopy" || metadata.method_family_name == "new" ||
        metadata.method_family_name == "none") &&
       (!metadata.method_family_returns_related_result || metadata.method_family_name == "init"));
  return metadata;
}

static void CollectMessageSendSelectorLoweringSiteMetadataFromExpr(
    const Expr *expr, std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  if (expr == nullptr) {
    return;
  }

  if (expr->kind == Expr::Kind::MessageSend) {
    sites.push_back(BuildMessageSendSelectorLoweringSiteMetadata(*expr));
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->receiver.get(), sites);
    for (const auto &arg : expr->args) {
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(arg.get(), sites);
    }
    return;
  }

  if (expr->kind == Expr::Kind::Binary) {
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->left.get(), sites);
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->right.get(), sites);
    return;
  }

  if (expr->kind == Expr::Kind::Conditional) {
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->left.get(), sites);
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->right.get(), sites);
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->third.get(), sites);
    return;
  }

  if (expr->kind == Expr::Kind::Call) {
    for (const auto &arg : expr->args) {
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(arg.get(), sites);
    }
  }
}

static void CollectMessageSendSelectorLoweringSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites);

static void CollectMessageSendSelectorLoweringSiteMetadataFromForClause(
    const ForClause &clause,
    std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  CollectMessageSendSelectorLoweringSiteMetadataFromExpr(clause.value.get(), sites);
}

static void CollectMessageSendSelectorLoweringSiteMetadataFromStatement(
    const Stmt *stmt, std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
    case Stmt::Kind::Let:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->let_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Assign:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->assign_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Return:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->return_stmt->value.get(), sites);
      return;
    case Stmt::Kind::If:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->if_stmt->condition.get(), sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->if_stmt->then_body, sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->if_stmt->else_body, sites);
      return;
    case Stmt::Kind::DoWhile:
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->do_while_stmt->body, sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->do_while_stmt->condition.get(), sites);
      return;
    case Stmt::Kind::For:
      CollectMessageSendSelectorLoweringSiteMetadataFromForClause(stmt->for_stmt->init, sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->for_stmt->condition.get(), sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromForClause(stmt->for_stmt->step, sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->for_stmt->body, sites);
      return;
    case Stmt::Kind::Switch:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->switch_stmt->condition.get(), sites);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        CollectMessageSendSelectorLoweringSiteMetadataFromStatements(switch_case.body, sites);
      }
      return;
    case Stmt::Kind::While:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->while_stmt->condition.get(), sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->while_stmt->body, sites);
      return;
    case Stmt::Kind::Block:
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->block_stmt->body, sites);
      return;
    case Stmt::Kind::Expr:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->expr_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectMessageSendSelectorLoweringSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  for (const auto &statement : statements) {
    CollectMessageSendSelectorLoweringSiteMetadataFromStatement(statement.get(), sites);
  }
}

static bool IsMessageSendSelectorLoweringSiteMetadataLess(
    const Objc3MessageSendSelectorLoweringSiteMetadata &lhs,
    const Objc3MessageSendSelectorLoweringSiteMetadata &rhs) {
  if (lhs.selector != rhs.selector) {
    return lhs.selector < rhs.selector;
  }
  if (lhs.selector_lowering_symbol != rhs.selector_lowering_symbol) {
    return lhs.selector_lowering_symbol < rhs.selector_lowering_symbol;
  }
  if (lhs.argument_count != rhs.argument_count) {
    return lhs.argument_count < rhs.argument_count;
  }
  if (lhs.selector_piece_count != rhs.selector_piece_count) {
    return lhs.selector_piece_count < rhs.selector_piece_count;
  }
  if (lhs.selector_argument_piece_count != rhs.selector_argument_piece_count) {
    return lhs.selector_argument_piece_count < rhs.selector_argument_piece_count;
  }
  if (lhs.unary_form != rhs.unary_form) {
    return lhs.unary_form < rhs.unary_form;
  }
  if (lhs.keyword_form != rhs.keyword_form) {
    return lhs.keyword_form < rhs.keyword_form;
  }
  if (lhs.selector_lowering_is_normalized != rhs.selector_lowering_is_normalized) {
    return lhs.selector_lowering_is_normalized < rhs.selector_lowering_is_normalized;
  }
  if (lhs.receiver_is_nil_literal != rhs.receiver_is_nil_literal) {
    return lhs.receiver_is_nil_literal < rhs.receiver_is_nil_literal;
  }
  if (lhs.nil_receiver_semantics_enabled != rhs.nil_receiver_semantics_enabled) {
    return lhs.nil_receiver_semantics_enabled < rhs.nil_receiver_semantics_enabled;
  }
  if (lhs.nil_receiver_foldable != rhs.nil_receiver_foldable) {
    return lhs.nil_receiver_foldable < rhs.nil_receiver_foldable;
  }
  if (lhs.nil_receiver_requires_runtime_dispatch != rhs.nil_receiver_requires_runtime_dispatch) {
    return lhs.nil_receiver_requires_runtime_dispatch < rhs.nil_receiver_requires_runtime_dispatch;
  }
  if (lhs.nil_receiver_semantics_is_normalized != rhs.nil_receiver_semantics_is_normalized) {
    return lhs.nil_receiver_semantics_is_normalized < rhs.nil_receiver_semantics_is_normalized;
  }
  if (lhs.runtime_shim_host_link_required != rhs.runtime_shim_host_link_required) {
    return lhs.runtime_shim_host_link_required < rhs.runtime_shim_host_link_required;
  }
  if (lhs.runtime_shim_host_link_elided != rhs.runtime_shim_host_link_elided) {
    return lhs.runtime_shim_host_link_elided < rhs.runtime_shim_host_link_elided;
  }
  if (lhs.runtime_shim_host_link_runtime_dispatch_arg_slots != rhs.runtime_shim_host_link_runtime_dispatch_arg_slots) {
    return lhs.runtime_shim_host_link_runtime_dispatch_arg_slots <
           rhs.runtime_shim_host_link_runtime_dispatch_arg_slots;
  }
  if (lhs.runtime_shim_host_link_declaration_parameter_count !=
      rhs.runtime_shim_host_link_declaration_parameter_count) {
    return lhs.runtime_shim_host_link_declaration_parameter_count <
           rhs.runtime_shim_host_link_declaration_parameter_count;
  }
  if (lhs.runtime_dispatch_bridge_symbol != rhs.runtime_dispatch_bridge_symbol) {
    return lhs.runtime_dispatch_bridge_symbol < rhs.runtime_dispatch_bridge_symbol;
  }
  if (lhs.runtime_shim_host_link_symbol != rhs.runtime_shim_host_link_symbol) {
    return lhs.runtime_shim_host_link_symbol < rhs.runtime_shim_host_link_symbol;
  }
  if (lhs.runtime_shim_host_link_is_normalized != rhs.runtime_shim_host_link_is_normalized) {
    return lhs.runtime_shim_host_link_is_normalized < rhs.runtime_shim_host_link_is_normalized;
  }
  if (lhs.receiver_is_super_identifier != rhs.receiver_is_super_identifier) {
    return lhs.receiver_is_super_identifier < rhs.receiver_is_super_identifier;
  }
  if (lhs.super_dispatch_enabled != rhs.super_dispatch_enabled) {
    return lhs.super_dispatch_enabled < rhs.super_dispatch_enabled;
  }
  if (lhs.super_dispatch_requires_class_context != rhs.super_dispatch_requires_class_context) {
    return lhs.super_dispatch_requires_class_context < rhs.super_dispatch_requires_class_context;
  }
  if (lhs.super_dispatch_semantics_is_normalized != rhs.super_dispatch_semantics_is_normalized) {
    return lhs.super_dispatch_semantics_is_normalized < rhs.super_dispatch_semantics_is_normalized;
  }
  if (lhs.method_family_name != rhs.method_family_name) {
    return lhs.method_family_name < rhs.method_family_name;
  }
  if (lhs.method_family_returns_retained_result != rhs.method_family_returns_retained_result) {
    return lhs.method_family_returns_retained_result < rhs.method_family_returns_retained_result;
  }
  if (lhs.method_family_returns_related_result != rhs.method_family_returns_related_result) {
    return lhs.method_family_returns_related_result < rhs.method_family_returns_related_result;
  }
  if (lhs.method_family_semantics_is_normalized != rhs.method_family_semantics_is_normalized) {
    return lhs.method_family_semantics_is_normalized < rhs.method_family_semantics_is_normalized;
  }
  if (lhs.line != rhs.line) {
    return lhs.line < rhs.line;
  }
  return lhs.column < rhs.column;
}

static std::vector<Objc3MessageSendSelectorLoweringSiteMetadata>
BuildMessageSendSelectorLoweringSiteMetadataLexicographic(const Objc3Program &ast) {
  std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> sites;
  for (const auto &global : ast.globals) {
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(global.value.get(), sites);
  }
  for (const auto &fn : ast.functions) {
    CollectMessageSendSelectorLoweringSiteMetadataFromStatements(fn.body, sites);
  }
  std::sort(sites.begin(), sites.end(), IsMessageSendSelectorLoweringSiteMetadataLess);
  return sites;
}

static Objc3AutoreleasePoolScopeSiteMetadata BuildAutoreleasePoolScopeSiteMetadata(const BlockStmt &block) {
  Objc3AutoreleasePoolScopeSiteMetadata metadata;
  metadata.scope_symbol = block.autoreleasepool_scope_symbol;
  metadata.scope_depth = block.autoreleasepool_scope_depth;
  metadata.line = block.line;
  metadata.column = block.column;
  return metadata;
}

static void CollectAutoreleasePoolScopeSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3AutoreleasePoolScopeSiteMetadata> &sites);

static void CollectAutoreleasePoolScopeSiteMetadataFromStatement(
    const Stmt *stmt, std::vector<Objc3AutoreleasePoolScopeSiteMetadata> &sites) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
    case Stmt::Kind::If:
      CollectAutoreleasePoolScopeSiteMetadataFromStatements(stmt->if_stmt->then_body, sites);
      CollectAutoreleasePoolScopeSiteMetadataFromStatements(stmt->if_stmt->else_body, sites);
      return;
    case Stmt::Kind::DoWhile:
      CollectAutoreleasePoolScopeSiteMetadataFromStatements(stmt->do_while_stmt->body, sites);
      return;
    case Stmt::Kind::For:
      CollectAutoreleasePoolScopeSiteMetadataFromStatements(stmt->for_stmt->body, sites);
      return;
    case Stmt::Kind::Switch:
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        CollectAutoreleasePoolScopeSiteMetadataFromStatements(switch_case.body, sites);
      }
      return;
    case Stmt::Kind::While:
      CollectAutoreleasePoolScopeSiteMetadataFromStatements(stmt->while_stmt->body, sites);
      return;
    case Stmt::Kind::Block:
      if (stmt->block_stmt->is_autoreleasepool_scope) {
        sites.push_back(BuildAutoreleasePoolScopeSiteMetadata(*stmt->block_stmt));
      }
      CollectAutoreleasePoolScopeSiteMetadataFromStatements(stmt->block_stmt->body, sites);
      return;
    case Stmt::Kind::Let:
    case Stmt::Kind::Assign:
    case Stmt::Kind::Return:
    case Stmt::Kind::Expr:
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectAutoreleasePoolScopeSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3AutoreleasePoolScopeSiteMetadata> &sites) {
  for (const auto &statement : statements) {
    CollectAutoreleasePoolScopeSiteMetadataFromStatement(statement.get(), sites);
  }
}

static bool IsAutoreleasePoolScopeSiteMetadataLess(const Objc3AutoreleasePoolScopeSiteMetadata &lhs,
                                                   const Objc3AutoreleasePoolScopeSiteMetadata &rhs) {
  if (lhs.scope_symbol != rhs.scope_symbol) {
    return lhs.scope_symbol < rhs.scope_symbol;
  }
  if (lhs.scope_depth != rhs.scope_depth) {
    return lhs.scope_depth < rhs.scope_depth;
  }
  if (lhs.line != rhs.line) {
    return lhs.line < rhs.line;
  }
  return lhs.column < rhs.column;
}

static std::vector<Objc3AutoreleasePoolScopeSiteMetadata>
BuildAutoreleasePoolScopeSiteMetadataLexicographic(const Objc3Program &ast) {
  std::vector<Objc3AutoreleasePoolScopeSiteMetadata> sites;
  for (const auto &fn : ast.functions) {
    CollectAutoreleasePoolScopeSiteMetadataFromStatements(fn.body, sites);
  }
  std::sort(sites.begin(), sites.end(), IsAutoreleasePoolScopeSiteMetadataLess);
  return sites;
}

static Objc3AutoreleasePoolScopeSummary BuildAutoreleasePoolScopeSummaryFromSites(
    const std::vector<Objc3AutoreleasePoolScopeSiteMetadata> &sites) {
  Objc3AutoreleasePoolScopeSummary summary;
  for (const auto &site : sites) {
    ++summary.scope_sites;
    if (site.scope_symbol.empty()) {
      ++summary.contract_violation_sites;
    } else {
      ++summary.scope_symbolized_sites;
      if (site.scope_symbol.rfind("autoreleasepool#", 0) != 0) {
        ++summary.contract_violation_sites;
      }
    }
    if (site.scope_depth == 0u) {
      ++summary.contract_violation_sites;
    } else if (site.scope_depth > summary.max_scope_depth) {
      summary.max_scope_depth = site.scope_depth;
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.scope_symbolized_sites <= summary.scope_sites &&
      summary.contract_violation_sites <= summary.scope_sites &&
      (summary.scope_sites > 0u || summary.max_scope_depth == 0u) &&
      summary.max_scope_depth <= static_cast<unsigned>(summary.scope_sites);
  return summary;
}

static Objc3MessageSendSelectorLoweringSummary BuildMessageSendSelectorLoweringSummaryFromSites(
    const std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  Objc3MessageSendSelectorLoweringSummary summary;
  for (const auto &site : sites) {
    ++summary.message_send_sites;
    if (site.unary_form) {
      ++summary.unary_form_sites;
    }
    if (site.keyword_form) {
      ++summary.keyword_form_sites;
    }
    if (!site.selector_lowering_symbol.empty()) {
      ++summary.selector_lowering_symbol_sites;
    } else {
      ++summary.selector_lowering_missing_symbol_sites;
    }
    summary.selector_lowering_piece_entries += site.selector_piece_count;
    summary.selector_lowering_argument_piece_entries += site.selector_argument_piece_count;
    if (site.selector_lowering_is_normalized) {
      ++summary.selector_lowering_normalized_sites;
    }

    bool contract_violation = false;
    if (site.unary_form == site.keyword_form) {
      ++summary.selector_lowering_form_mismatch_sites;
      summary.deterministic = false;
      contract_violation = true;
    }
    if (site.unary_form && site.argument_count != 0) {
      ++summary.selector_lowering_form_mismatch_sites;
      contract_violation = true;
    }
    if (site.keyword_form && site.argument_count == 0) {
      ++summary.selector_lowering_form_mismatch_sites;
      contract_violation = true;
    }
    if (site.selector_argument_piece_count > site.selector_piece_count) {
      ++summary.selector_lowering_arity_mismatch_sites;
      summary.deterministic = false;
      contract_violation = true;
    }
    if (site.unary_form && site.selector_argument_piece_count != 0) {
      ++summary.selector_lowering_arity_mismatch_sites;
      contract_violation = true;
    }
    if (site.keyword_form && site.selector_argument_piece_count != site.argument_count) {
      ++summary.selector_lowering_arity_mismatch_sites;
      contract_violation = true;
    }
    if (!site.selector.empty() && !site.selector_lowering_symbol.empty() &&
        site.selector != site.selector_lowering_symbol) {
      ++summary.selector_lowering_symbol_mismatch_sites;
      contract_violation = true;
    }
    if (site.selector_lowering_symbol.empty()) {
      contract_violation = true;
    }
    if (site.selector_lowering_is_normalized && site.selector_lowering_symbol.empty()) {
      summary.deterministic = false;
      contract_violation = true;
    }
    if (contract_violation) {
      ++summary.selector_lowering_contract_violation_sites;
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.unary_form_sites + summary.keyword_form_sites == summary.message_send_sites &&
      summary.selector_lowering_symbol_sites <= summary.message_send_sites &&
      summary.selector_lowering_normalized_sites <= summary.selector_lowering_symbol_sites &&
      summary.selector_lowering_argument_piece_entries <= summary.selector_lowering_piece_entries &&
      summary.selector_lowering_form_mismatch_sites <= summary.message_send_sites &&
      summary.selector_lowering_arity_mismatch_sites <= summary.message_send_sites &&
      summary.selector_lowering_symbol_mismatch_sites <= summary.message_send_sites &&
      summary.selector_lowering_missing_symbol_sites <= summary.message_send_sites &&
      summary.selector_lowering_contract_violation_sites <= summary.message_send_sites;
  return summary;
}

static Objc3MessageSendSelectorLoweringSummary BuildMessageSendSelectorLoweringSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  return BuildMessageSendSelectorLoweringSummaryFromSites(
      surface.message_send_selector_lowering_sites_lexicographic);
}

static Objc3MessageSendSelectorLoweringSummary BuildMessageSendSelectorLoweringSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  return BuildMessageSendSelectorLoweringSummaryFromSites(
      handoff.message_send_selector_lowering_sites_lexicographic);
}

static Objc3DispatchAbiMarshallingSummary BuildDispatchAbiMarshallingSummaryFromSites(
    const std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  Objc3DispatchAbiMarshallingSummary summary;
  for (const auto &site : sites) {
    ++summary.message_send_sites;
    ++summary.receiver_slots;
    summary.argument_slots += site.argument_count;
    if (site.unary_form) {
      summary.unary_argument_slots += site.argument_count;
    }
    if (site.keyword_form) {
      summary.keyword_argument_slots += site.argument_count;
    }
    if (!site.selector_lowering_symbol.empty()) {
      ++summary.selector_symbol_slots;
    } else {
      ++summary.missing_selector_symbol_sites;
    }

    bool arity_mismatch = false;
    bool contract_violation = false;
    if (site.unary_form == site.keyword_form) {
      summary.deterministic = false;
      arity_mismatch = true;
      contract_violation = true;
    }
    if (site.unary_form && (site.argument_count != 0 || site.selector_argument_piece_count != 0)) {
      arity_mismatch = true;
      contract_violation = true;
    }
    if (site.keyword_form && (site.argument_count == 0 || site.selector_argument_piece_count != site.argument_count)) {
      arity_mismatch = true;
      contract_violation = true;
    }
    if (site.selector_argument_piece_count > site.argument_count) {
      summary.deterministic = false;
      arity_mismatch = true;
      contract_violation = true;
    }
    if (site.selector_lowering_symbol.empty()) {
      contract_violation = true;
    }
    if (arity_mismatch) {
      ++summary.arity_mismatch_sites;
    }
    if (contract_violation) {
      ++summary.contract_violation_sites;
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.receiver_slots == summary.message_send_sites &&
      summary.selector_symbol_slots + summary.missing_selector_symbol_sites == summary.message_send_sites &&
      summary.keyword_argument_slots + summary.unary_argument_slots == summary.argument_slots &&
      summary.keyword_argument_slots <= summary.argument_slots &&
      summary.unary_argument_slots <= summary.argument_slots &&
      summary.selector_symbol_slots <= summary.message_send_sites &&
      summary.missing_selector_symbol_sites <= summary.message_send_sites &&
      summary.arity_mismatch_sites <= summary.message_send_sites &&
      summary.contract_violation_sites <= summary.message_send_sites;
  return summary;
}

static Objc3DispatchAbiMarshallingSummary BuildDispatchAbiMarshallingSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  return BuildDispatchAbiMarshallingSummaryFromSites(
      surface.message_send_selector_lowering_sites_lexicographic);
}

static Objc3DispatchAbiMarshallingSummary BuildDispatchAbiMarshallingSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  return BuildDispatchAbiMarshallingSummaryFromSites(
      handoff.message_send_selector_lowering_sites_lexicographic);
}

static Objc3NilReceiverSemanticsFoldabilitySummary BuildNilReceiverSemanticsFoldabilitySummaryFromSites(
    const std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  Objc3NilReceiverSemanticsFoldabilitySummary summary;
  for (const auto &site : sites) {
    ++summary.message_send_sites;
    if (site.receiver_is_nil_literal) {
      ++summary.receiver_nil_literal_sites;
    }
    if (site.nil_receiver_semantics_enabled) {
      ++summary.nil_receiver_semantics_enabled_sites;
    } else {
      ++summary.non_nil_receiver_sites;
    }
    if (site.nil_receiver_foldable) {
      ++summary.nil_receiver_foldable_sites;
    }
    if (site.nil_receiver_requires_runtime_dispatch) {
      ++summary.nil_receiver_runtime_dispatch_required_sites;
    }

    bool contract_violation = false;
    if (!site.nil_receiver_semantics_is_normalized) {
      summary.deterministic = false;
      contract_violation = true;
    }
    if (site.receiver_is_nil_literal != site.nil_receiver_semantics_enabled) {
      summary.deterministic = false;
      contract_violation = true;
    }
    if (site.nil_receiver_semantics_enabled != site.nil_receiver_foldable) {
      contract_violation = true;
    }
    if (site.nil_receiver_requires_runtime_dispatch == site.nil_receiver_foldable) {
      contract_violation = true;
    }
    if (!site.nil_receiver_semantics_enabled && site.nil_receiver_foldable) {
      contract_violation = true;
    }
    if (contract_violation) {
      ++summary.contract_violation_sites;
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.receiver_nil_literal_sites == summary.nil_receiver_semantics_enabled_sites &&
      summary.nil_receiver_foldable_sites <= summary.nil_receiver_semantics_enabled_sites &&
      summary.nil_receiver_runtime_dispatch_required_sites + summary.nil_receiver_foldable_sites ==
          summary.message_send_sites &&
      summary.nil_receiver_semantics_enabled_sites + summary.non_nil_receiver_sites == summary.message_send_sites &&
      summary.contract_violation_sites <= summary.message_send_sites;
  return summary;
}

static Objc3NilReceiverSemanticsFoldabilitySummary
BuildNilReceiverSemanticsFoldabilitySummaryFromIntegrationSurface(const Objc3SemanticIntegrationSurface &surface) {
  return BuildNilReceiverSemanticsFoldabilitySummaryFromSites(
      surface.message_send_selector_lowering_sites_lexicographic);
}

static Objc3NilReceiverSemanticsFoldabilitySummary
BuildNilReceiverSemanticsFoldabilitySummaryFromTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {
  return BuildNilReceiverSemanticsFoldabilitySummaryFromSites(
      handoff.message_send_selector_lowering_sites_lexicographic);
}

static Objc3SuperDispatchMethodFamilySummary BuildSuperDispatchMethodFamilySummaryFromSites(
    const std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  Objc3SuperDispatchMethodFamilySummary summary;
  for (const auto &site : sites) {
    ++summary.message_send_sites;
    if (site.receiver_is_super_identifier) {
      ++summary.receiver_super_identifier_sites;
    }
    if (site.super_dispatch_enabled) {
      ++summary.super_dispatch_enabled_sites;
    }
    if (site.super_dispatch_requires_class_context) {
      ++summary.super_dispatch_requires_class_context_sites;
    }
    if (site.method_family_name == "init") {
      ++summary.method_family_init_sites;
    } else if (site.method_family_name == "copy") {
      ++summary.method_family_copy_sites;
    } else if (site.method_family_name == "mutableCopy") {
      ++summary.method_family_mutable_copy_sites;
    } else if (site.method_family_name == "new") {
      ++summary.method_family_new_sites;
    } else {
      ++summary.method_family_none_sites;
    }
    if (site.method_family_returns_retained_result) {
      ++summary.method_family_returns_retained_result_sites;
    }
    if (site.method_family_returns_related_result) {
      ++summary.method_family_returns_related_result_sites;
    }

    bool contract_violation = false;
    if (!site.super_dispatch_semantics_is_normalized || !site.method_family_semantics_is_normalized) {
      summary.deterministic = false;
      contract_violation = true;
    }
    if (site.receiver_is_super_identifier != site.super_dispatch_enabled) {
      summary.deterministic = false;
      contract_violation = true;
    }
    if (site.super_dispatch_enabled != site.super_dispatch_requires_class_context) {
      contract_violation = true;
    }
    if (site.method_family_returns_related_result && site.method_family_name != "init") {
      contract_violation = true;
    }
    const bool retained_family =
        site.method_family_name == "init" || site.method_family_name == "copy" ||
        site.method_family_name == "mutableCopy" || site.method_family_name == "new";
    if (site.method_family_returns_retained_result != retained_family) {
      contract_violation = true;
    }
    if (contract_violation) {
      ++summary.contract_violation_sites;
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.receiver_super_identifier_sites == summary.super_dispatch_enabled_sites &&
      summary.super_dispatch_requires_class_context_sites == summary.super_dispatch_enabled_sites &&
      summary.method_family_init_sites + summary.method_family_copy_sites + summary.method_family_mutable_copy_sites +
              summary.method_family_new_sites + summary.method_family_none_sites ==
          summary.message_send_sites &&
      summary.method_family_returns_related_result_sites <= summary.method_family_init_sites &&
      summary.method_family_returns_retained_result_sites <= summary.message_send_sites &&
      summary.contract_violation_sites <= summary.message_send_sites;
  return summary;
}

static Objc3SuperDispatchMethodFamilySummary
BuildSuperDispatchMethodFamilySummaryFromIntegrationSurface(const Objc3SemanticIntegrationSurface &surface) {
  return BuildSuperDispatchMethodFamilySummaryFromSites(surface.message_send_selector_lowering_sites_lexicographic);
}

static Objc3SuperDispatchMethodFamilySummary
BuildSuperDispatchMethodFamilySummaryFromTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {
  return BuildSuperDispatchMethodFamilySummaryFromSites(handoff.message_send_selector_lowering_sites_lexicographic);
}

static Objc3RuntimeShimHostLinkSummary BuildRuntimeShimHostLinkSummaryFromSites(
    const std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  Objc3RuntimeShimHostLinkSummary summary;
  bool baseline_initialized = false;
  for (const auto &site : sites) {
    ++summary.message_send_sites;
    if (site.runtime_shim_host_link_required) {
      ++summary.runtime_shim_required_sites;
    }
    if (site.runtime_shim_host_link_elided) {
      ++summary.runtime_shim_elided_sites;
    }

    bool contract_violation = false;
    if (!site.runtime_shim_host_link_is_normalized) {
      summary.deterministic = false;
      contract_violation = true;
    }
    if (site.runtime_shim_host_link_required == site.runtime_shim_host_link_elided) {
      contract_violation = true;
    }
    if (site.runtime_shim_host_link_required != site.nil_receiver_requires_runtime_dispatch) {
      contract_violation = true;
    }
    if (site.runtime_shim_host_link_declaration_parameter_count !=
        site.runtime_shim_host_link_runtime_dispatch_arg_slots + 2u) {
      contract_violation = true;
    }
    if (site.runtime_dispatch_bridge_symbol.empty()) {
      summary.deterministic = false;
      contract_violation = true;
    }
    if (site.runtime_shim_host_link_symbol.empty()) {
      contract_violation = true;
    }

    const std::string site_dispatch_symbol = site.runtime_dispatch_bridge_symbol.empty()
                                                 ? std::string(kObjc3RuntimeShimHostLinkDefaultDispatchSymbol)
                                                 : site.runtime_dispatch_bridge_symbol;
    if (!baseline_initialized) {
      baseline_initialized = true;
      summary.runtime_dispatch_arg_slots = site.runtime_shim_host_link_runtime_dispatch_arg_slots;
      summary.runtime_dispatch_declaration_parameter_count =
          site.runtime_shim_host_link_declaration_parameter_count;
      summary.runtime_dispatch_symbol = site_dispatch_symbol;
      summary.default_runtime_dispatch_symbol_binding =
          summary.runtime_dispatch_symbol == kObjc3RuntimeShimHostLinkDefaultDispatchSymbol;
    } else if (summary.runtime_dispatch_arg_slots != site.runtime_shim_host_link_runtime_dispatch_arg_slots ||
               summary.runtime_dispatch_declaration_parameter_count !=
                   site.runtime_shim_host_link_declaration_parameter_count ||
               summary.runtime_dispatch_symbol != site_dispatch_symbol) {
      contract_violation = true;
    }

    if (contract_violation) {
      ++summary.contract_violation_sites;
    }
  }

  if (!baseline_initialized) {
    summary.runtime_dispatch_symbol = kObjc3RuntimeShimHostLinkDefaultDispatchSymbol;
    summary.default_runtime_dispatch_symbol_binding = true;
  } else {
    summary.default_runtime_dispatch_symbol_binding =
        summary.runtime_dispatch_symbol == kObjc3RuntimeShimHostLinkDefaultDispatchSymbol;
  }

  summary.deterministic =
      summary.deterministic &&
      summary.runtime_shim_required_sites + summary.runtime_shim_elided_sites == summary.message_send_sites &&
      summary.runtime_shim_required_sites <= summary.message_send_sites &&
      summary.runtime_shim_elided_sites <= summary.message_send_sites &&
      summary.contract_violation_sites <= summary.message_send_sites &&
      (summary.message_send_sites == 0 ||
       summary.runtime_dispatch_declaration_parameter_count == summary.runtime_dispatch_arg_slots + 2u) &&
      (summary.default_runtime_dispatch_symbol_binding ==
       (summary.runtime_dispatch_symbol == kObjc3RuntimeShimHostLinkDefaultDispatchSymbol));
  return summary;
}

static Objc3RuntimeShimHostLinkSummary
BuildRuntimeShimHostLinkSummaryFromIntegrationSurface(const Objc3SemanticIntegrationSurface &surface) {
  return BuildRuntimeShimHostLinkSummaryFromSites(surface.message_send_selector_lowering_sites_lexicographic);
}

static Objc3RuntimeShimHostLinkSummary
BuildRuntimeShimHostLinkSummaryFromTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {
  return BuildRuntimeShimHostLinkSummaryFromSites(handoff.message_send_selector_lowering_sites_lexicographic);
}

static Objc3RetainReleaseOperationSummary
BuildRetainReleaseOperationSummaryFromIntegrationSurface(const Objc3SemanticIntegrationSurface &surface) {
  Objc3RetainReleaseOperationSummary summary;
  std::size_t operation_sites = 0;

  const auto accumulate_function = [&summary, &operation_sites](const FunctionInfo &info) {
    const std::size_t arity = info.arity;
    if (info.param_has_ownership_qualifier.size() != arity ||
        info.param_ownership_insert_retain.size() != arity ||
        info.param_ownership_insert_release.size() != arity ||
        info.param_ownership_insert_autorelease.size() != arity) {
      summary.deterministic = false;
      return;
    }
    operation_sites += arity + 1u;
    for (std::size_t i = 0; i < arity; ++i) {
      const bool qualified = info.param_has_ownership_qualifier[i];
      const bool insert_retain = info.param_ownership_insert_retain[i];
      const bool insert_release = info.param_ownership_insert_release[i];
      const bool insert_autorelease = info.param_ownership_insert_autorelease[i];
      if (qualified) {
        ++summary.ownership_qualified_sites;
      }
      if (insert_retain) {
        ++summary.retain_insertion_sites;
      }
      if (insert_release) {
        ++summary.release_insertion_sites;
      }
      if (insert_autorelease) {
        ++summary.autorelease_insertion_sites;
      }
      if ((!qualified && (insert_retain || insert_release || insert_autorelease)) ||
          (insert_autorelease && (insert_retain || insert_release))) {
        ++summary.contract_violation_sites;
      }
    }
    const bool return_qualified = info.return_has_ownership_qualifier;
    if (return_qualified) {
      ++summary.ownership_qualified_sites;
    }
    if (info.return_ownership_insert_retain) {
      ++summary.retain_insertion_sites;
    }
    if (info.return_ownership_insert_release) {
      ++summary.release_insertion_sites;
    }
    if (info.return_ownership_insert_autorelease) {
      ++summary.autorelease_insertion_sites;
    }
    if ((!return_qualified && (info.return_ownership_insert_retain || info.return_ownership_insert_release ||
                               info.return_ownership_insert_autorelease)) ||
        (info.return_ownership_insert_autorelease &&
         (info.return_ownership_insert_retain || info.return_ownership_insert_release))) {
      ++summary.contract_violation_sites;
    }
  };

  const auto accumulate_method = [&summary, &operation_sites](const Objc3MethodInfo &info) {
    const std::size_t arity = info.arity;
    if (info.param_has_ownership_qualifier.size() != arity ||
        info.param_ownership_insert_retain.size() != arity ||
        info.param_ownership_insert_release.size() != arity ||
        info.param_ownership_insert_autorelease.size() != arity) {
      summary.deterministic = false;
      return;
    }
    operation_sites += arity + 1u;
    for (std::size_t i = 0; i < arity; ++i) {
      const bool qualified = info.param_has_ownership_qualifier[i];
      const bool insert_retain = info.param_ownership_insert_retain[i];
      const bool insert_release = info.param_ownership_insert_release[i];
      const bool insert_autorelease = info.param_ownership_insert_autorelease[i];
      if (qualified) {
        ++summary.ownership_qualified_sites;
      }
      if (insert_retain) {
        ++summary.retain_insertion_sites;
      }
      if (insert_release) {
        ++summary.release_insertion_sites;
      }
      if (insert_autorelease) {
        ++summary.autorelease_insertion_sites;
      }
      if ((!qualified && (insert_retain || insert_release || insert_autorelease)) ||
          (insert_autorelease && (insert_retain || insert_release))) {
        ++summary.contract_violation_sites;
      }
    }
    const bool return_qualified = info.return_has_ownership_qualifier;
    if (return_qualified) {
      ++summary.ownership_qualified_sites;
    }
    if (info.return_ownership_insert_retain) {
      ++summary.retain_insertion_sites;
    }
    if (info.return_ownership_insert_release) {
      ++summary.release_insertion_sites;
    }
    if (info.return_ownership_insert_autorelease) {
      ++summary.autorelease_insertion_sites;
    }
    if ((!return_qualified && (info.return_ownership_insert_retain || info.return_ownership_insert_release ||
                               info.return_ownership_insert_autorelease)) ||
        (info.return_ownership_insert_autorelease &&
         (info.return_ownership_insert_retain || info.return_ownership_insert_release))) {
      ++summary.contract_violation_sites;
    }
  };

  const auto accumulate_property = [&summary, &operation_sites](const Objc3PropertyInfo &info) {
    ++operation_sites;
    const bool qualified = info.has_ownership_qualifier;
    if (qualified) {
      ++summary.ownership_qualified_sites;
    }
    if (info.ownership_insert_retain) {
      ++summary.retain_insertion_sites;
    }
    if (info.ownership_insert_release) {
      ++summary.release_insertion_sites;
    }
    if (info.ownership_insert_autorelease) {
      ++summary.autorelease_insertion_sites;
    }
    if ((!qualified &&
         (info.ownership_insert_retain || info.ownership_insert_release || info.ownership_insert_autorelease)) ||
        (info.ownership_insert_autorelease && (info.ownership_insert_retain || info.ownership_insert_release))) {
      ++summary.contract_violation_sites;
    }
  };

  for (const auto &entry : surface.functions) {
    accumulate_function(entry.second);
  }
  for (const auto &entry : surface.interfaces) {
    for (const auto &method_entry : entry.second.methods) {
      accumulate_method(method_entry.second);
    }
    for (const auto &property_entry : entry.second.properties) {
      accumulate_property(property_entry.second);
    }
  }
  for (const auto &entry : surface.implementations) {
    for (const auto &method_entry : entry.second.methods) {
      accumulate_method(method_entry.second);
    }
    for (const auto &property_entry : entry.second.properties) {
      accumulate_property(property_entry.second);
    }
  }

  const std::size_t qualified_or_violation = summary.ownership_qualified_sites + summary.contract_violation_sites;
  summary.deterministic =
      summary.deterministic &&
      summary.contract_violation_sites <= operation_sites &&
      summary.retain_insertion_sites <= qualified_or_violation &&
      summary.release_insertion_sites <= qualified_or_violation &&
      summary.autorelease_insertion_sites <= qualified_or_violation;
  return summary;
}

static Objc3RetainReleaseOperationSummary
BuildRetainReleaseOperationSummaryFromTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3RetainReleaseOperationSummary summary;
  std::size_t operation_sites = 0;

  const auto accumulate_function = [&summary, &operation_sites](const Objc3SemanticFunctionTypeMetadata &metadata) {
    const std::size_t arity = metadata.arity;
    if (metadata.param_has_ownership_qualifier.size() != arity ||
        metadata.param_ownership_insert_retain.size() != arity ||
        metadata.param_ownership_insert_release.size() != arity ||
        metadata.param_ownership_insert_autorelease.size() != arity) {
      summary.deterministic = false;
      return;
    }
    operation_sites += arity + 1u;
    for (std::size_t i = 0; i < arity; ++i) {
      const bool qualified = metadata.param_has_ownership_qualifier[i];
      const bool insert_retain = metadata.param_ownership_insert_retain[i];
      const bool insert_release = metadata.param_ownership_insert_release[i];
      const bool insert_autorelease = metadata.param_ownership_insert_autorelease[i];
      if (qualified) {
        ++summary.ownership_qualified_sites;
      }
      if (insert_retain) {
        ++summary.retain_insertion_sites;
      }
      if (insert_release) {
        ++summary.release_insertion_sites;
      }
      if (insert_autorelease) {
        ++summary.autorelease_insertion_sites;
      }
      if ((!qualified && (insert_retain || insert_release || insert_autorelease)) ||
          (insert_autorelease && (insert_retain || insert_release))) {
        ++summary.contract_violation_sites;
      }
    }
    const bool return_qualified = metadata.return_has_ownership_qualifier;
    if (return_qualified) {
      ++summary.ownership_qualified_sites;
    }
    if (metadata.return_ownership_insert_retain) {
      ++summary.retain_insertion_sites;
    }
    if (metadata.return_ownership_insert_release) {
      ++summary.release_insertion_sites;
    }
    if (metadata.return_ownership_insert_autorelease) {
      ++summary.autorelease_insertion_sites;
    }
    if ((!return_qualified && (metadata.return_ownership_insert_retain || metadata.return_ownership_insert_release ||
                               metadata.return_ownership_insert_autorelease)) ||
        (metadata.return_ownership_insert_autorelease &&
         (metadata.return_ownership_insert_retain || metadata.return_ownership_insert_release))) {
      ++summary.contract_violation_sites;
    }
  };

  const auto accumulate_method = [&summary, &operation_sites](const Objc3SemanticMethodTypeMetadata &metadata) {
    const std::size_t arity = metadata.arity;
    if (metadata.param_has_ownership_qualifier.size() != arity ||
        metadata.param_ownership_insert_retain.size() != arity ||
        metadata.param_ownership_insert_release.size() != arity ||
        metadata.param_ownership_insert_autorelease.size() != arity) {
      summary.deterministic = false;
      return;
    }
    operation_sites += arity + 1u;
    for (std::size_t i = 0; i < arity; ++i) {
      const bool qualified = metadata.param_has_ownership_qualifier[i];
      const bool insert_retain = metadata.param_ownership_insert_retain[i];
      const bool insert_release = metadata.param_ownership_insert_release[i];
      const bool insert_autorelease = metadata.param_ownership_insert_autorelease[i];
      if (qualified) {
        ++summary.ownership_qualified_sites;
      }
      if (insert_retain) {
        ++summary.retain_insertion_sites;
      }
      if (insert_release) {
        ++summary.release_insertion_sites;
      }
      if (insert_autorelease) {
        ++summary.autorelease_insertion_sites;
      }
      if ((!qualified && (insert_retain || insert_release || insert_autorelease)) ||
          (insert_autorelease && (insert_retain || insert_release))) {
        ++summary.contract_violation_sites;
      }
    }
    const bool return_qualified = metadata.return_has_ownership_qualifier;
    if (return_qualified) {
      ++summary.ownership_qualified_sites;
    }
    if (metadata.return_ownership_insert_retain) {
      ++summary.retain_insertion_sites;
    }
    if (metadata.return_ownership_insert_release) {
      ++summary.release_insertion_sites;
    }
    if (metadata.return_ownership_insert_autorelease) {
      ++summary.autorelease_insertion_sites;
    }
    if ((!return_qualified && (metadata.return_ownership_insert_retain || metadata.return_ownership_insert_release ||
                               metadata.return_ownership_insert_autorelease)) ||
        (metadata.return_ownership_insert_autorelease &&
         (metadata.return_ownership_insert_retain || metadata.return_ownership_insert_release))) {
      ++summary.contract_violation_sites;
    }
  };

  const auto accumulate_property = [&summary, &operation_sites](const Objc3SemanticPropertyTypeMetadata &metadata) {
    ++operation_sites;
    const bool qualified = metadata.has_ownership_qualifier;
    if (qualified) {
      ++summary.ownership_qualified_sites;
    }
    if (metadata.ownership_insert_retain) {
      ++summary.retain_insertion_sites;
    }
    if (metadata.ownership_insert_release) {
      ++summary.release_insertion_sites;
    }
    if (metadata.ownership_insert_autorelease) {
      ++summary.autorelease_insertion_sites;
    }
    if ((!qualified && (metadata.ownership_insert_retain || metadata.ownership_insert_release ||
                        metadata.ownership_insert_autorelease)) ||
        (metadata.ownership_insert_autorelease &&
         (metadata.ownership_insert_retain || metadata.ownership_insert_release))) {
      ++summary.contract_violation_sites;
    }
  };

  for (const auto &metadata : handoff.functions_lexicographic) {
    accumulate_function(metadata);
  }
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      accumulate_method(method_metadata);
    }
    for (const auto &property_metadata : interface_metadata.properties_lexicographic) {
      accumulate_property(property_metadata);
    }
  }
  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    for (const auto &method_metadata : implementation_metadata.methods_lexicographic) {
      accumulate_method(method_metadata);
    }
    for (const auto &property_metadata : implementation_metadata.properties_lexicographic) {
      accumulate_property(property_metadata);
    }
  }

  const std::size_t qualified_or_violation = summary.ownership_qualified_sites + summary.contract_violation_sites;
  summary.deterministic =
      summary.deterministic &&
      summary.contract_violation_sites <= operation_sites &&
      summary.retain_insertion_sites <= qualified_or_violation &&
      summary.release_insertion_sites <= qualified_or_violation &&
      summary.autorelease_insertion_sites <= qualified_or_violation;
  return summary;
}

static Objc3WeakUnownedSemanticsSummary
BuildWeakUnownedSemanticsSummaryFromIntegrationSurface(const Objc3SemanticIntegrationSurface &surface) {
  Objc3WeakUnownedSemanticsSummary summary;

  const auto accumulate_site = [&summary](bool ownership_candidate,
                                          bool weak_reference,
                                          bool unowned_reference,
                                          bool unowned_safe_reference,
                                          bool conflict) {
    if (ownership_candidate) {
      ++summary.ownership_candidate_sites;
    }
    if (weak_reference) {
      ++summary.weak_reference_sites;
    }
    if (unowned_reference) {
      ++summary.unowned_reference_sites;
    }
    if (unowned_safe_reference) {
      ++summary.unowned_safe_reference_sites;
    }
    if (conflict) {
      ++summary.weak_unowned_conflict_sites;
    }
    if (conflict || (unowned_safe_reference && !unowned_reference) ||
        (!ownership_candidate && (weak_reference || unowned_reference))) {
      ++summary.contract_violation_sites;
    }
  };

  const auto accumulate_function = [&summary, &accumulate_site](const FunctionInfo &info) {
    const std::size_t arity = info.arity;
    if (info.param_has_ownership_qualifier.size() != arity ||
        info.param_ownership_is_weak_reference.size() != arity ||
        info.param_ownership_is_unowned_reference.size() != arity ||
        info.param_ownership_is_unowned_safe_reference.size() != arity) {
      summary.deterministic = false;
      return;
    }
    for (std::size_t i = 0; i < arity; ++i) {
      const bool weak_reference = info.param_ownership_is_weak_reference[i];
      const bool unowned_reference = info.param_ownership_is_unowned_reference[i];
      const bool unowned_safe_reference = info.param_ownership_is_unowned_safe_reference[i];
      const bool ownership_candidate = info.param_has_ownership_qualifier[i] || weak_reference || unowned_reference;
      accumulate_site(ownership_candidate,
                      weak_reference,
                      unowned_reference,
                      unowned_safe_reference,
                      weak_reference && unowned_reference);
    }
    const bool return_weak_reference = info.return_ownership_is_weak_reference;
    const bool return_unowned_reference = info.return_ownership_is_unowned_reference;
    const bool return_unowned_safe_reference = info.return_ownership_is_unowned_safe_reference;
    const bool return_ownership_candidate =
        info.return_has_ownership_qualifier || return_weak_reference || return_unowned_reference;
    accumulate_site(return_ownership_candidate,
                    return_weak_reference,
                    return_unowned_reference,
                    return_unowned_safe_reference,
                    return_weak_reference && return_unowned_reference);
  };

  const auto accumulate_method = [&summary, &accumulate_site](const Objc3MethodInfo &info) {
    const std::size_t arity = info.arity;
    if (info.param_has_ownership_qualifier.size() != arity ||
        info.param_ownership_is_weak_reference.size() != arity ||
        info.param_ownership_is_unowned_reference.size() != arity ||
        info.param_ownership_is_unowned_safe_reference.size() != arity) {
      summary.deterministic = false;
      return;
    }
    for (std::size_t i = 0; i < arity; ++i) {
      const bool weak_reference = info.param_ownership_is_weak_reference[i];
      const bool unowned_reference = info.param_ownership_is_unowned_reference[i];
      const bool unowned_safe_reference = info.param_ownership_is_unowned_safe_reference[i];
      const bool ownership_candidate = info.param_has_ownership_qualifier[i] || weak_reference || unowned_reference;
      accumulate_site(ownership_candidate,
                      weak_reference,
                      unowned_reference,
                      unowned_safe_reference,
                      weak_reference && unowned_reference);
    }
    const bool return_weak_reference = info.return_ownership_is_weak_reference;
    const bool return_unowned_reference = info.return_ownership_is_unowned_reference;
    const bool return_unowned_safe_reference = info.return_ownership_is_unowned_safe_reference;
    const bool return_ownership_candidate =
        info.return_has_ownership_qualifier || return_weak_reference || return_unowned_reference;
    accumulate_site(return_ownership_candidate,
                    return_weak_reference,
                    return_unowned_reference,
                    return_unowned_safe_reference,
                    return_weak_reference && return_unowned_reference);
  };

  const auto accumulate_property = [&accumulate_site](const Objc3PropertyInfo &info) {
    const bool weak_reference = info.ownership_is_weak_reference || info.is_weak;
    const bool unowned_reference = info.ownership_is_unowned_reference || info.is_unowned || info.is_assign;
    const bool unowned_safe_reference = info.ownership_is_unowned_safe_reference || info.is_unowned;
    const bool ownership_candidate = info.has_ownership_qualifier || info.is_weak || info.is_unowned || info.is_assign ||
                                     weak_reference || unowned_reference;
    const bool conflict = info.has_weak_unowned_conflict || (info.is_weak && info.is_unowned) ||
                          (weak_reference && unowned_reference);
    accumulate_site(
        ownership_candidate, weak_reference, unowned_reference, unowned_safe_reference, conflict);
  };

  for (const auto &entry : surface.functions) {
    accumulate_function(entry.second);
  }
  for (const auto &entry : surface.interfaces) {
    for (const auto &method_entry : entry.second.methods) {
      accumulate_method(method_entry.second);
    }
    for (const auto &property_entry : entry.second.properties) {
      accumulate_property(property_entry.second);
    }
  }
  for (const auto &entry : surface.implementations) {
    for (const auto &method_entry : entry.second.methods) {
      accumulate_method(method_entry.second);
    }
    for (const auto &property_entry : entry.second.properties) {
      accumulate_property(property_entry.second);
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.unowned_safe_reference_sites <= summary.unowned_reference_sites &&
      summary.weak_unowned_conflict_sites <= summary.ownership_candidate_sites &&
      summary.contract_violation_sites <= summary.ownership_candidate_sites + summary.weak_unowned_conflict_sites;
  return summary;
}

static Objc3WeakUnownedSemanticsSummary
BuildWeakUnownedSemanticsSummaryFromTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3WeakUnownedSemanticsSummary summary;

  const auto accumulate_site = [&summary](bool ownership_candidate,
                                          bool weak_reference,
                                          bool unowned_reference,
                                          bool unowned_safe_reference,
                                          bool conflict) {
    if (ownership_candidate) {
      ++summary.ownership_candidate_sites;
    }
    if (weak_reference) {
      ++summary.weak_reference_sites;
    }
    if (unowned_reference) {
      ++summary.unowned_reference_sites;
    }
    if (unowned_safe_reference) {
      ++summary.unowned_safe_reference_sites;
    }
    if (conflict) {
      ++summary.weak_unowned_conflict_sites;
    }
    if (conflict || (unowned_safe_reference && !unowned_reference) ||
        (!ownership_candidate && (weak_reference || unowned_reference))) {
      ++summary.contract_violation_sites;
    }
  };

  const auto accumulate_function = [&summary, &accumulate_site](const Objc3SemanticFunctionTypeMetadata &metadata) {
    const std::size_t arity = metadata.arity;
    if (metadata.param_has_ownership_qualifier.size() != arity ||
        metadata.param_ownership_is_weak_reference.size() != arity ||
        metadata.param_ownership_is_unowned_reference.size() != arity ||
        metadata.param_ownership_is_unowned_safe_reference.size() != arity) {
      summary.deterministic = false;
      return;
    }
    for (std::size_t i = 0; i < arity; ++i) {
      const bool weak_reference = metadata.param_ownership_is_weak_reference[i];
      const bool unowned_reference = metadata.param_ownership_is_unowned_reference[i];
      const bool unowned_safe_reference = metadata.param_ownership_is_unowned_safe_reference[i];
      const bool ownership_candidate =
          metadata.param_has_ownership_qualifier[i] || weak_reference || unowned_reference;
      accumulate_site(ownership_candidate,
                      weak_reference,
                      unowned_reference,
                      unowned_safe_reference,
                      weak_reference && unowned_reference);
    }
    const bool return_weak_reference = metadata.return_ownership_is_weak_reference;
    const bool return_unowned_reference = metadata.return_ownership_is_unowned_reference;
    const bool return_unowned_safe_reference = metadata.return_ownership_is_unowned_safe_reference;
    const bool return_ownership_candidate =
        metadata.return_has_ownership_qualifier || return_weak_reference || return_unowned_reference;
    accumulate_site(return_ownership_candidate,
                    return_weak_reference,
                    return_unowned_reference,
                    return_unowned_safe_reference,
                    return_weak_reference && return_unowned_reference);
  };

  const auto accumulate_method = [&summary, &accumulate_site](const Objc3SemanticMethodTypeMetadata &metadata) {
    const std::size_t arity = metadata.arity;
    if (metadata.param_has_ownership_qualifier.size() != arity ||
        metadata.param_ownership_is_weak_reference.size() != arity ||
        metadata.param_ownership_is_unowned_reference.size() != arity ||
        metadata.param_ownership_is_unowned_safe_reference.size() != arity) {
      summary.deterministic = false;
      return;
    }
    for (std::size_t i = 0; i < arity; ++i) {
      const bool weak_reference = metadata.param_ownership_is_weak_reference[i];
      const bool unowned_reference = metadata.param_ownership_is_unowned_reference[i];
      const bool unowned_safe_reference = metadata.param_ownership_is_unowned_safe_reference[i];
      const bool ownership_candidate =
          metadata.param_has_ownership_qualifier[i] || weak_reference || unowned_reference;
      accumulate_site(ownership_candidate,
                      weak_reference,
                      unowned_reference,
                      unowned_safe_reference,
                      weak_reference && unowned_reference);
    }
    const bool return_weak_reference = metadata.return_ownership_is_weak_reference;
    const bool return_unowned_reference = metadata.return_ownership_is_unowned_reference;
    const bool return_unowned_safe_reference = metadata.return_ownership_is_unowned_safe_reference;
    const bool return_ownership_candidate =
        metadata.return_has_ownership_qualifier || return_weak_reference || return_unowned_reference;
    accumulate_site(return_ownership_candidate,
                    return_weak_reference,
                    return_unowned_reference,
                    return_unowned_safe_reference,
                    return_weak_reference && return_unowned_reference);
  };

  const auto accumulate_property = [&accumulate_site](const Objc3SemanticPropertyTypeMetadata &metadata) {
    const bool weak_reference = metadata.ownership_is_weak_reference || metadata.is_weak;
    const bool unowned_reference = metadata.ownership_is_unowned_reference || metadata.is_unowned || metadata.is_assign;
    const bool unowned_safe_reference = metadata.ownership_is_unowned_safe_reference || metadata.is_unowned;
    const bool ownership_candidate = metadata.has_ownership_qualifier || metadata.is_weak || metadata.is_unowned ||
                                     metadata.is_assign || weak_reference || unowned_reference;
    const bool conflict = metadata.has_weak_unowned_conflict || (metadata.is_weak && metadata.is_unowned) ||
                          (weak_reference && unowned_reference);
    accumulate_site(
        ownership_candidate, weak_reference, unowned_reference, unowned_safe_reference, conflict);
  };

  for (const auto &metadata : handoff.functions_lexicographic) {
    accumulate_function(metadata);
  }
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      accumulate_method(method_metadata);
    }
    for (const auto &property_metadata : interface_metadata.properties_lexicographic) {
      accumulate_property(property_metadata);
    }
  }
  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    for (const auto &method_metadata : implementation_metadata.methods_lexicographic) {
      accumulate_method(method_metadata);
    }
    for (const auto &property_metadata : implementation_metadata.properties_lexicographic) {
      accumulate_property(property_metadata);
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.unowned_safe_reference_sites <= summary.unowned_reference_sites &&
      summary.weak_unowned_conflict_sites <= summary.ownership_candidate_sites &&
      summary.contract_violation_sites <= summary.ownership_candidate_sites + summary.weak_unowned_conflict_sites;
  return summary;
}

static Objc3ArcDiagnosticsFixitSummary
BuildArcDiagnosticsFixitSummaryFromIntegrationSurface(const Objc3SemanticIntegrationSurface &surface) {
  Objc3ArcDiagnosticsFixitSummary summary;

  const auto accumulate_site = [&summary](bool diagnostic_candidate,
                                          bool fixit_available,
                                          const std::string &diagnostic_profile,
                                          const std::string &fixit_hint,
                                          bool weak_unowned_conflict) {
    if (diagnostic_candidate) {
      ++summary.ownership_arc_diagnostic_candidate_sites;
    }
    if (fixit_available) {
      ++summary.ownership_arc_fixit_available_sites;
    }
    if (!diagnostic_profile.empty()) {
      ++summary.ownership_arc_profiled_sites;
    }
    if (diagnostic_profile == "arc-weak-unowned-conflict") {
      ++summary.ownership_arc_weak_unowned_conflict_diagnostic_sites;
    }
    if (fixit_available && fixit_hint.empty()) {
      ++summary.ownership_arc_empty_fixit_hint_sites;
    }
    if ((fixit_available && !diagnostic_candidate) ||
        (!diagnostic_profile.empty() && !diagnostic_candidate) ||
        (!fixit_hint.empty() && !fixit_available) ||
        (fixit_available && fixit_hint.empty()) ||
        (weak_unowned_conflict && diagnostic_profile != "arc-weak-unowned-conflict") ||
        (!weak_unowned_conflict && diagnostic_profile == "arc-weak-unowned-conflict")) {
      ++summary.contract_violation_sites;
    }
  };

  const auto accumulate_function = [&summary, &accumulate_site](const FunctionInfo &info) {
    const std::size_t arity = info.arity;
    if (info.param_ownership_arc_diagnostic_candidate.size() != arity ||
        info.param_ownership_arc_fixit_available.size() != arity ||
        info.param_ownership_arc_diagnostic_profile.size() != arity ||
        info.param_ownership_arc_fixit_hint.size() != arity) {
      summary.deterministic = false;
      return;
    }
    for (std::size_t i = 0; i < arity; ++i) {
      accumulate_site(info.param_ownership_arc_diagnostic_candidate[i],
                      info.param_ownership_arc_fixit_available[i],
                      info.param_ownership_arc_diagnostic_profile[i],
                      info.param_ownership_arc_fixit_hint[i],
                      false);
    }
    accumulate_site(info.return_ownership_arc_diagnostic_candidate,
                    info.return_ownership_arc_fixit_available,
                    info.return_ownership_arc_diagnostic_profile,
                    info.return_ownership_arc_fixit_hint,
                    false);
  };

  const auto accumulate_method = [&summary, &accumulate_site](const Objc3MethodInfo &info) {
    const std::size_t arity = info.arity;
    if (info.param_ownership_arc_diagnostic_candidate.size() != arity ||
        info.param_ownership_arc_fixit_available.size() != arity ||
        info.param_ownership_arc_diagnostic_profile.size() != arity ||
        info.param_ownership_arc_fixit_hint.size() != arity) {
      summary.deterministic = false;
      return;
    }
    for (std::size_t i = 0; i < arity; ++i) {
      accumulate_site(info.param_ownership_arc_diagnostic_candidate[i],
                      info.param_ownership_arc_fixit_available[i],
                      info.param_ownership_arc_diagnostic_profile[i],
                      info.param_ownership_arc_fixit_hint[i],
                      false);
    }
    accumulate_site(info.return_ownership_arc_diagnostic_candidate,
                    info.return_ownership_arc_fixit_available,
                    info.return_ownership_arc_diagnostic_profile,
                    info.return_ownership_arc_fixit_hint,
                    false);
  };

  const auto accumulate_property = [&accumulate_site](const Objc3PropertyInfo &info) {
    const bool weak_unowned_conflict = info.has_weak_unowned_conflict || (info.is_weak && info.is_unowned);
    accumulate_site(info.ownership_arc_diagnostic_candidate,
                    info.ownership_arc_fixit_available,
                    info.ownership_arc_diagnostic_profile,
                    info.ownership_arc_fixit_hint,
                    weak_unowned_conflict);
  };

  for (const auto &entry : surface.functions) {
    accumulate_function(entry.second);
  }
  for (const auto &entry : surface.interfaces) {
    for (const auto &method_entry : entry.second.methods) {
      accumulate_method(method_entry.second);
    }
    for (const auto &property_entry : entry.second.properties) {
      accumulate_property(property_entry.second);
    }
  }
  for (const auto &entry : surface.implementations) {
    for (const auto &method_entry : entry.second.methods) {
      accumulate_method(method_entry.second);
    }
    for (const auto &property_entry : entry.second.properties) {
      accumulate_property(property_entry.second);
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.ownership_arc_fixit_available_sites <=
          summary.ownership_arc_diagnostic_candidate_sites + summary.contract_violation_sites &&
      summary.ownership_arc_profiled_sites <=
          summary.ownership_arc_diagnostic_candidate_sites + summary.contract_violation_sites &&
      summary.ownership_arc_weak_unowned_conflict_diagnostic_sites <=
          summary.ownership_arc_diagnostic_candidate_sites + summary.contract_violation_sites &&
      summary.ownership_arc_empty_fixit_hint_sites <=
          summary.ownership_arc_fixit_available_sites + summary.contract_violation_sites;
  return summary;
}

static Objc3ArcDiagnosticsFixitSummary
BuildArcDiagnosticsFixitSummaryFromTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3ArcDiagnosticsFixitSummary summary;

  const auto accumulate_site = [&summary](bool diagnostic_candidate,
                                          bool fixit_available,
                                          const std::string &diagnostic_profile,
                                          const std::string &fixit_hint,
                                          bool weak_unowned_conflict) {
    if (diagnostic_candidate) {
      ++summary.ownership_arc_diagnostic_candidate_sites;
    }
    if (fixit_available) {
      ++summary.ownership_arc_fixit_available_sites;
    }
    if (!diagnostic_profile.empty()) {
      ++summary.ownership_arc_profiled_sites;
    }
    if (diagnostic_profile == "arc-weak-unowned-conflict") {
      ++summary.ownership_arc_weak_unowned_conflict_diagnostic_sites;
    }
    if (fixit_available && fixit_hint.empty()) {
      ++summary.ownership_arc_empty_fixit_hint_sites;
    }
    if ((fixit_available && !diagnostic_candidate) ||
        (!diagnostic_profile.empty() && !diagnostic_candidate) ||
        (!fixit_hint.empty() && !fixit_available) ||
        (fixit_available && fixit_hint.empty()) ||
        (weak_unowned_conflict && diagnostic_profile != "arc-weak-unowned-conflict") ||
        (!weak_unowned_conflict && diagnostic_profile == "arc-weak-unowned-conflict")) {
      ++summary.contract_violation_sites;
    }
  };

  const auto accumulate_function = [&summary, &accumulate_site](const Objc3SemanticFunctionTypeMetadata &metadata) {
    const std::size_t arity = metadata.arity;
    if (metadata.param_ownership_arc_diagnostic_candidate.size() != arity ||
        metadata.param_ownership_arc_fixit_available.size() != arity ||
        metadata.param_ownership_arc_diagnostic_profile.size() != arity ||
        metadata.param_ownership_arc_fixit_hint.size() != arity) {
      summary.deterministic = false;
      return;
    }
    for (std::size_t i = 0; i < arity; ++i) {
      accumulate_site(metadata.param_ownership_arc_diagnostic_candidate[i],
                      metadata.param_ownership_arc_fixit_available[i],
                      metadata.param_ownership_arc_diagnostic_profile[i],
                      metadata.param_ownership_arc_fixit_hint[i],
                      false);
    }
    accumulate_site(metadata.return_ownership_arc_diagnostic_candidate,
                    metadata.return_ownership_arc_fixit_available,
                    metadata.return_ownership_arc_diagnostic_profile,
                    metadata.return_ownership_arc_fixit_hint,
                    false);
  };

  const auto accumulate_method = [&summary, &accumulate_site](const Objc3SemanticMethodTypeMetadata &metadata) {
    const std::size_t arity = metadata.arity;
    if (metadata.param_ownership_arc_diagnostic_candidate.size() != arity ||
        metadata.param_ownership_arc_fixit_available.size() != arity ||
        metadata.param_ownership_arc_diagnostic_profile.size() != arity ||
        metadata.param_ownership_arc_fixit_hint.size() != arity) {
      summary.deterministic = false;
      return;
    }
    for (std::size_t i = 0; i < arity; ++i) {
      accumulate_site(metadata.param_ownership_arc_diagnostic_candidate[i],
                      metadata.param_ownership_arc_fixit_available[i],
                      metadata.param_ownership_arc_diagnostic_profile[i],
                      metadata.param_ownership_arc_fixit_hint[i],
                      false);
    }
    accumulate_site(metadata.return_ownership_arc_diagnostic_candidate,
                    metadata.return_ownership_arc_fixit_available,
                    metadata.return_ownership_arc_diagnostic_profile,
                    metadata.return_ownership_arc_fixit_hint,
                    false);
  };

  const auto accumulate_property = [&accumulate_site](const Objc3SemanticPropertyTypeMetadata &metadata) {
    const bool weak_unowned_conflict = metadata.has_weak_unowned_conflict || (metadata.is_weak && metadata.is_unowned);
    accumulate_site(metadata.ownership_arc_diagnostic_candidate,
                    metadata.ownership_arc_fixit_available,
                    metadata.ownership_arc_diagnostic_profile,
                    metadata.ownership_arc_fixit_hint,
                    weak_unowned_conflict);
  };

  for (const auto &metadata : handoff.functions_lexicographic) {
    accumulate_function(metadata);
  }
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      accumulate_method(method_metadata);
    }
    for (const auto &property_metadata : interface_metadata.properties_lexicographic) {
      accumulate_property(property_metadata);
    }
  }
  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    for (const auto &method_metadata : implementation_metadata.methods_lexicographic) {
      accumulate_method(method_metadata);
    }
    for (const auto &property_metadata : implementation_metadata.properties_lexicographic) {
      accumulate_property(property_metadata);
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.ownership_arc_fixit_available_sites <=
          summary.ownership_arc_diagnostic_candidate_sites + summary.contract_violation_sites &&
      summary.ownership_arc_profiled_sites <=
          summary.ownership_arc_diagnostic_candidate_sites + summary.contract_violation_sites &&
      summary.ownership_arc_weak_unowned_conflict_diagnostic_sites <=
          summary.ownership_arc_diagnostic_candidate_sites + summary.contract_violation_sites &&
      summary.ownership_arc_empty_fixit_hint_sites <=
          summary.ownership_arc_fixit_available_sites + summary.contract_violation_sites;
  return summary;
}

static Objc3AutoreleasePoolScopeSummary
BuildAutoreleasePoolScopeSummaryFromIntegrationSurface(const Objc3SemanticIntegrationSurface &surface) {
  return BuildAutoreleasePoolScopeSummaryFromSites(surface.autoreleasepool_scope_sites_lexicographic);
}

static Objc3AutoreleasePoolScopeSummary
BuildAutoreleasePoolScopeSummaryFromTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {
  return BuildAutoreleasePoolScopeSummaryFromSites(handoff.autoreleasepool_scope_sites_lexicographic);
}

static Objc3IdClassSelObjectPointerTypeCheckingSummary
BuildIdClassSelObjectPointerTypeCheckingSummaryFromIntegrationSurface(const Objc3SemanticIntegrationSurface &surface) {
  Objc3IdClassSelObjectPointerTypeCheckingSummary summary;

  const auto accumulate_function = [&summary](const FunctionInfo &info) {
    ++summary.return_type_sites;
    if (info.return_id_spelling) {
      ++summary.return_id_spelling_sites;
    }
    if (info.return_class_spelling) {
      ++summary.return_class_spelling_sites;
    }
    if (info.return_instancetype_spelling) {
      ++summary.return_instancetype_spelling_sites;
    }
    if (info.return_object_pointer_type_spelling) {
      ++summary.return_object_pointer_type_sites;
    }

    summary.param_type_sites += info.param_types.size();
    const std::size_t count = std::min(
        info.param_types.size(),
        std::min(info.param_id_spelling.size(),
                 std::min(info.param_class_spelling.size(),
                          std::min(info.param_instancetype_spelling.size(),
                                   info.param_object_pointer_type_spelling.size()))));
    for (std::size_t i = 0; i < count; ++i) {
      if (info.param_id_spelling[i]) {
        ++summary.param_id_spelling_sites;
      }
      if (info.param_class_spelling[i]) {
        ++summary.param_class_spelling_sites;
      }
      if (info.param_instancetype_spelling[i]) {
        ++summary.param_instancetype_spelling_sites;
      }
      if (info.param_object_pointer_type_spelling[i]) {
        ++summary.param_object_pointer_type_sites;
      }
    }
    if (count != info.param_types.size()) {
      summary.deterministic = false;
    }
  };

  const auto accumulate_method = [&summary](const Objc3MethodInfo &info) {
    ++summary.return_type_sites;
    if (info.return_id_spelling) {
      ++summary.return_id_spelling_sites;
    }
    if (info.return_class_spelling) {
      ++summary.return_class_spelling_sites;
    }
    if (info.return_instancetype_spelling) {
      ++summary.return_instancetype_spelling_sites;
    }
    if (info.return_object_pointer_type_spelling) {
      ++summary.return_object_pointer_type_sites;
    }

    summary.param_type_sites += info.param_types.size();
    const std::size_t count = std::min(
        info.param_types.size(),
        std::min(info.param_id_spelling.size(),
                 std::min(info.param_class_spelling.size(),
                          std::min(info.param_instancetype_spelling.size(),
                                   info.param_object_pointer_type_spelling.size()))));
    for (std::size_t i = 0; i < count; ++i) {
      if (info.param_id_spelling[i]) {
        ++summary.param_id_spelling_sites;
      }
      if (info.param_class_spelling[i]) {
        ++summary.param_class_spelling_sites;
      }
      if (info.param_instancetype_spelling[i]) {
        ++summary.param_instancetype_spelling_sites;
      }
      if (info.param_object_pointer_type_spelling[i]) {
        ++summary.param_object_pointer_type_sites;
      }
    }
    if (count != info.param_types.size()) {
      summary.deterministic = false;
    }
  };

  const auto accumulate_property = [&summary](const Objc3PropertyInfo &info) {
    ++summary.property_type_sites;
    if (info.id_spelling) {
      ++summary.property_id_spelling_sites;
    }
    if (info.class_spelling) {
      ++summary.property_class_spelling_sites;
    }
    if (info.instancetype_spelling) {
      ++summary.property_instancetype_spelling_sites;
    }
    if (info.object_pointer_type_spelling) {
      ++summary.property_object_pointer_type_sites;
    }
  };

  for (const auto &entry : surface.functions) {
    accumulate_function(entry.second);
  }
  for (const auto &entry : surface.interfaces) {
    for (const auto &method_entry : entry.second.methods) {
      accumulate_method(method_entry.second);
    }
    for (const auto &property_entry : entry.second.properties) {
      accumulate_property(property_entry.second);
    }
  }
  for (const auto &entry : surface.implementations) {
    for (const auto &method_entry : entry.second.methods) {
      accumulate_method(method_entry.second);
    }
    for (const auto &property_entry : entry.second.properties) {
      accumulate_property(property_entry.second);
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.param_id_spelling_sites <= summary.param_type_sites &&
      summary.param_class_spelling_sites <= summary.param_type_sites &&
      summary.param_sel_spelling_sites <= summary.param_type_sites &&
      summary.param_instancetype_spelling_sites <= summary.param_type_sites &&
      summary.param_object_pointer_type_sites <= summary.param_type_sites &&
      summary.return_id_spelling_sites <= summary.return_type_sites &&
      summary.return_class_spelling_sites <= summary.return_type_sites &&
      summary.return_sel_spelling_sites <= summary.return_type_sites &&
      summary.return_instancetype_spelling_sites <= summary.return_type_sites &&
      summary.return_object_pointer_type_sites <= summary.return_type_sites &&
      summary.property_id_spelling_sites <= summary.property_type_sites &&
      summary.property_class_spelling_sites <= summary.property_type_sites &&
      summary.property_sel_spelling_sites <= summary.property_type_sites &&
      summary.property_instancetype_spelling_sites <= summary.property_type_sites &&
      summary.property_object_pointer_type_sites <= summary.property_type_sites &&
      summary.param_id_spelling_sites + summary.param_class_spelling_sites + summary.param_sel_spelling_sites +
              summary.param_instancetype_spelling_sites + summary.param_object_pointer_type_sites <=
          summary.param_type_sites &&
      summary.return_id_spelling_sites + summary.return_class_spelling_sites + summary.return_sel_spelling_sites +
              summary.return_instancetype_spelling_sites + summary.return_object_pointer_type_sites <=
          summary.return_type_sites &&
      summary.property_id_spelling_sites + summary.property_class_spelling_sites + summary.property_sel_spelling_sites +
              summary.property_instancetype_spelling_sites + summary.property_object_pointer_type_sites <=
          summary.property_type_sites;
  return summary;
}

static Objc3IdClassSelObjectPointerTypeCheckingSummary
BuildIdClassSelObjectPointerTypeCheckingSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3IdClassSelObjectPointerTypeCheckingSummary summary;

  const auto accumulate_function = [&summary](const Objc3SemanticFunctionTypeMetadata &metadata) {
    ++summary.return_type_sites;
    if (metadata.return_id_spelling) {
      ++summary.return_id_spelling_sites;
    }
    if (metadata.return_class_spelling) {
      ++summary.return_class_spelling_sites;
    }
    if (metadata.return_instancetype_spelling) {
      ++summary.return_instancetype_spelling_sites;
    }
    if (metadata.return_object_pointer_type_spelling) {
      ++summary.return_object_pointer_type_sites;
    }

    summary.param_type_sites += metadata.param_types.size();
    const std::size_t count = std::min(
        metadata.param_types.size(),
        std::min(metadata.param_id_spelling.size(),
                 std::min(metadata.param_class_spelling.size(),
                          std::min(metadata.param_instancetype_spelling.size(),
                                   metadata.param_object_pointer_type_spelling.size()))));
    for (std::size_t i = 0; i < count; ++i) {
      if (metadata.param_id_spelling[i]) {
        ++summary.param_id_spelling_sites;
      }
      if (metadata.param_class_spelling[i]) {
        ++summary.param_class_spelling_sites;
      }
      if (metadata.param_instancetype_spelling[i]) {
        ++summary.param_instancetype_spelling_sites;
      }
      if (metadata.param_object_pointer_type_spelling[i]) {
        ++summary.param_object_pointer_type_sites;
      }
    }
    if (count != metadata.param_types.size()) {
      summary.deterministic = false;
    }
  };

  const auto accumulate_method = [&summary](const Objc3SemanticMethodTypeMetadata &metadata) {
    ++summary.return_type_sites;
    if (metadata.return_id_spelling) {
      ++summary.return_id_spelling_sites;
    }
    if (metadata.return_class_spelling) {
      ++summary.return_class_spelling_sites;
    }
    if (metadata.return_instancetype_spelling) {
      ++summary.return_instancetype_spelling_sites;
    }
    if (metadata.return_object_pointer_type_spelling) {
      ++summary.return_object_pointer_type_sites;
    }

    summary.param_type_sites += metadata.param_types.size();
    const std::size_t count = std::min(
        metadata.param_types.size(),
        std::min(metadata.param_id_spelling.size(),
                 std::min(metadata.param_class_spelling.size(),
                          std::min(metadata.param_instancetype_spelling.size(),
                                   metadata.param_object_pointer_type_spelling.size()))));
    for (std::size_t i = 0; i < count; ++i) {
      if (metadata.param_id_spelling[i]) {
        ++summary.param_id_spelling_sites;
      }
      if (metadata.param_class_spelling[i]) {
        ++summary.param_class_spelling_sites;
      }
      if (metadata.param_instancetype_spelling[i]) {
        ++summary.param_instancetype_spelling_sites;
      }
      if (metadata.param_object_pointer_type_spelling[i]) {
        ++summary.param_object_pointer_type_sites;
      }
    }
    if (count != metadata.param_types.size()) {
      summary.deterministic = false;
    }
  };

  const auto accumulate_property = [&summary](const Objc3SemanticPropertyTypeMetadata &metadata) {
    ++summary.property_type_sites;
    if (metadata.id_spelling) {
      ++summary.property_id_spelling_sites;
    }
    if (metadata.class_spelling) {
      ++summary.property_class_spelling_sites;
    }
    if (metadata.instancetype_spelling) {
      ++summary.property_instancetype_spelling_sites;
    }
    if (metadata.object_pointer_type_spelling) {
      ++summary.property_object_pointer_type_sites;
    }
  };

  for (const auto &metadata : handoff.functions_lexicographic) {
    accumulate_function(metadata);
  }
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      accumulate_method(method_metadata);
    }
    for (const auto &property_metadata : interface_metadata.properties_lexicographic) {
      accumulate_property(property_metadata);
    }
  }
  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    for (const auto &method_metadata : implementation_metadata.methods_lexicographic) {
      accumulate_method(method_metadata);
    }
    for (const auto &property_metadata : implementation_metadata.properties_lexicographic) {
      accumulate_property(property_metadata);
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.param_id_spelling_sites <= summary.param_type_sites &&
      summary.param_class_spelling_sites <= summary.param_type_sites &&
      summary.param_sel_spelling_sites <= summary.param_type_sites &&
      summary.param_instancetype_spelling_sites <= summary.param_type_sites &&
      summary.param_object_pointer_type_sites <= summary.param_type_sites &&
      summary.return_id_spelling_sites <= summary.return_type_sites &&
      summary.return_class_spelling_sites <= summary.return_type_sites &&
      summary.return_sel_spelling_sites <= summary.return_type_sites &&
      summary.return_instancetype_spelling_sites <= summary.return_type_sites &&
      summary.return_object_pointer_type_sites <= summary.return_type_sites &&
      summary.property_id_spelling_sites <= summary.property_type_sites &&
      summary.property_class_spelling_sites <= summary.property_type_sites &&
      summary.property_sel_spelling_sites <= summary.property_type_sites &&
      summary.property_instancetype_spelling_sites <= summary.property_type_sites &&
      summary.property_object_pointer_type_sites <= summary.property_type_sites &&
      summary.param_id_spelling_sites + summary.param_class_spelling_sites + summary.param_sel_spelling_sites +
              summary.param_instancetype_spelling_sites + summary.param_object_pointer_type_sites <=
          summary.param_type_sites &&
      summary.return_id_spelling_sites + summary.return_class_spelling_sites + summary.return_sel_spelling_sites +
              summary.return_instancetype_spelling_sites + summary.return_object_pointer_type_sites <=
          summary.return_type_sites &&
      summary.property_id_spelling_sites + summary.property_class_spelling_sites + summary.property_sel_spelling_sites +
              summary.property_instancetype_spelling_sites + summary.property_object_pointer_type_sites <=
          summary.property_type_sites;
  return summary;
}

Objc3SemanticIntegrationSurface BuildSemanticIntegrationSurface(const Objc3ParsedProgram &program,
                                                                        std::vector<std::string> &diagnostics) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  Objc3SemanticIntegrationSurface surface;
  std::unordered_map<std::string, int> resolved_global_values;
  Objc3InterfaceImplementationSummary interface_implementation_summary;
  interface_implementation_summary.declared_interfaces = ast.interfaces.size();
  interface_implementation_summary.declared_implementations = ast.implementations.size();

  for (const auto &global : ast.globals) {
    const bool duplicate_global = surface.globals.find(global.name) != surface.globals.end();
    if (duplicate_global) {
      diagnostics.push_back(MakeDiag(global.line, global.column, "O3S200", "duplicate global '" + global.name + "'"));
    } else {
      surface.globals.emplace(global.name, ValueType::I32);
    }
    int value = 0;
    if (!EvalConstExpr(global.value.get(), value, &resolved_global_values)) {
      diagnostics.push_back(
          MakeDiag(global.line, global.column, "O3S210", "global initializer must be constant expression"));
    } else if (!duplicate_global) {
      resolved_global_values.emplace(global.name, value);
    }
  }

  for (const auto &fn : ast.functions) {
    if (surface.globals.find(fn.name) != surface.globals.end()) {
      diagnostics.push_back(MakeDiag(fn.line, fn.column, "O3S200", "duplicate function '" + fn.name + "'"));
      continue;
    }

    auto it = surface.functions.find(fn.name);
    if (it == surface.functions.end()) {
      FunctionInfo info;
      info.arity = fn.params.size();
      info.param_types.reserve(fn.params.size());
      info.param_is_vector.reserve(fn.params.size());
      info.param_vector_base_spelling.reserve(fn.params.size());
      info.param_vector_lane_count.reserve(fn.params.size());
      info.param_has_generic_suffix.reserve(fn.params.size());
      info.param_has_pointer_declarator.reserve(fn.params.size());
      info.param_has_nullability_suffix.reserve(fn.params.size());
      info.param_has_ownership_qualifier.reserve(fn.params.size());
      info.param_object_pointer_type_spelling.reserve(fn.params.size());
      info.param_has_invalid_generic_suffix.reserve(fn.params.size());
      info.param_has_invalid_pointer_declarator.reserve(fn.params.size());
      info.param_has_invalid_nullability_suffix.reserve(fn.params.size());
      info.param_has_invalid_ownership_qualifier.reserve(fn.params.size());
      info.param_has_invalid_type_suffix.reserve(fn.params.size());
      info.param_ownership_insert_retain.reserve(fn.params.size());
      info.param_ownership_insert_release.reserve(fn.params.size());
      info.param_ownership_insert_autorelease.reserve(fn.params.size());
      info.param_ownership_is_weak_reference.reserve(fn.params.size());
      info.param_ownership_is_unowned_reference.reserve(fn.params.size());
      info.param_ownership_is_unowned_safe_reference.reserve(fn.params.size());
      info.param_ownership_arc_diagnostic_candidate.reserve(fn.params.size());
      info.param_ownership_arc_fixit_available.reserve(fn.params.size());
      info.param_ownership_arc_diagnostic_profile.reserve(fn.params.size());
      info.param_ownership_arc_fixit_hint.reserve(fn.params.size());
      info.param_has_protocol_composition.reserve(fn.params.size());
      info.param_protocol_composition_lexicographic.reserve(fn.params.size());
      info.param_has_invalid_protocol_composition.reserve(fn.params.size());
      for (const auto &param : fn.params) {
        const ProtocolCompositionInfo protocol_composition = BuildProtocolCompositionInfoFromParam(param);
        info.param_types.push_back(param.type);
        info.param_is_vector.push_back(param.vector_spelling);
        info.param_vector_base_spelling.push_back(param.vector_base_spelling);
        info.param_vector_lane_count.push_back(param.vector_lane_count);
        info.param_has_generic_suffix.push_back(param.has_generic_suffix);
        info.param_has_pointer_declarator.push_back(param.has_pointer_declarator);
        info.param_has_nullability_suffix.push_back(!param.nullability_suffix_tokens.empty());
        info.param_has_ownership_qualifier.push_back(param.has_ownership_qualifier);
        info.param_object_pointer_type_spelling.push_back(param.object_pointer_type_spelling);
        info.param_has_invalid_generic_suffix.push_back(HasInvalidGenericParamTypeSuffix(param));
        info.param_has_invalid_pointer_declarator.push_back(HasInvalidPointerParamTypeDeclarator(param));
        info.param_has_invalid_nullability_suffix.push_back(HasInvalidNullabilityParamTypeSuffix(param));
        info.param_has_invalid_ownership_qualifier.push_back(HasInvalidOwnershipQualifierParamTypeSuffix(param));
        info.param_has_invalid_type_suffix.push_back(HasInvalidParamTypeSuffix(param));
        info.param_ownership_insert_retain.push_back(param.ownership_insert_retain);
        info.param_ownership_insert_release.push_back(param.ownership_insert_release);
        info.param_ownership_insert_autorelease.push_back(param.ownership_insert_autorelease);
        info.param_ownership_is_weak_reference.push_back(param.ownership_is_weak_reference);
        info.param_ownership_is_unowned_reference.push_back(param.ownership_is_unowned_reference);
        info.param_ownership_is_unowned_safe_reference.push_back(param.ownership_is_unowned_safe_reference);
        info.param_ownership_arc_diagnostic_candidate.push_back(param.ownership_arc_diagnostic_candidate);
        info.param_ownership_arc_fixit_available.push_back(param.ownership_arc_fixit_available);
        info.param_ownership_arc_diagnostic_profile.push_back(param.ownership_arc_diagnostic_profile);
        info.param_ownership_arc_fixit_hint.push_back(param.ownership_arc_fixit_hint);
        info.param_has_protocol_composition.push_back(protocol_composition.has_protocol_composition);
        info.param_protocol_composition_lexicographic.push_back(protocol_composition.names_lexicographic);
        info.param_has_invalid_protocol_composition.push_back(protocol_composition.has_invalid_protocol_composition);
      }
      const ProtocolCompositionInfo return_protocol_composition = BuildProtocolCompositionInfoFromFunctionReturn(fn);
      info.return_has_generic_suffix = fn.has_return_generic_suffix;
      info.return_has_pointer_declarator = fn.has_return_pointer_declarator;
      info.return_has_nullability_suffix = !fn.return_nullability_suffix_tokens.empty();
      info.return_has_ownership_qualifier = fn.has_return_ownership_qualifier;
      info.return_object_pointer_type_spelling = fn.return_object_pointer_type_spelling;
      info.return_has_invalid_generic_suffix = HasInvalidGenericReturnTypeSuffix(fn);
      info.return_has_invalid_pointer_declarator = HasInvalidPointerReturnTypeDeclarator(fn);
      info.return_has_invalid_nullability_suffix = HasInvalidNullabilityReturnTypeSuffix(fn);
      info.return_has_invalid_ownership_qualifier = HasInvalidOwnershipQualifierReturnTypeSuffix(fn);
      info.return_has_invalid_type_suffix = info.return_has_invalid_generic_suffix ||
                                            info.return_has_invalid_pointer_declarator ||
                                            info.return_has_invalid_nullability_suffix ||
                                            info.return_has_invalid_ownership_qualifier;
      info.return_ownership_insert_retain = fn.return_ownership_insert_retain;
      info.return_ownership_insert_release = fn.return_ownership_insert_release;
      info.return_ownership_insert_autorelease = fn.return_ownership_insert_autorelease;
      info.return_ownership_is_weak_reference = fn.return_ownership_is_weak_reference;
      info.return_ownership_is_unowned_reference = fn.return_ownership_is_unowned_reference;
      info.return_ownership_is_unowned_safe_reference = fn.return_ownership_is_unowned_safe_reference;
      info.return_ownership_arc_diagnostic_candidate = fn.return_ownership_arc_diagnostic_candidate;
      info.return_ownership_arc_fixit_available = fn.return_ownership_arc_fixit_available;
      info.return_ownership_arc_diagnostic_profile = fn.return_ownership_arc_diagnostic_profile;
      info.return_ownership_arc_fixit_hint = fn.return_ownership_arc_fixit_hint;
      info.return_type = fn.return_type;
      info.return_is_vector = fn.return_vector_spelling;
      info.return_vector_base_spelling = fn.return_vector_base_spelling;
      info.return_vector_lane_count = fn.return_vector_lane_count;
      info.return_has_protocol_composition = return_protocol_composition.has_protocol_composition;
      info.return_protocol_composition_lexicographic = return_protocol_composition.names_lexicographic;
      info.return_has_invalid_protocol_composition = return_protocol_composition.has_invalid_protocol_composition;
      info.has_definition = !fn.is_prototype;
      info.is_pure_annotation = fn.is_pure;
      surface.functions.emplace(fn.name, std::move(info));
      continue;
    }

    FunctionInfo &existing = it->second;
    bool compatible = existing.arity == fn.params.size() && existing.return_type == fn.return_type &&
                      existing.return_is_vector == fn.return_vector_spelling &&
                      existing.return_has_ownership_qualifier == fn.has_return_ownership_qualifier &&
                      existing.return_ownership_insert_retain == fn.return_ownership_insert_retain &&
                      existing.return_ownership_insert_release == fn.return_ownership_insert_release &&
                      existing.return_ownership_insert_autorelease == fn.return_ownership_insert_autorelease &&
                      existing.return_ownership_is_weak_reference == fn.return_ownership_is_weak_reference &&
                      existing.return_ownership_is_unowned_reference == fn.return_ownership_is_unowned_reference &&
                      existing.return_ownership_is_unowned_safe_reference ==
                          fn.return_ownership_is_unowned_safe_reference &&
                      existing.return_ownership_arc_diagnostic_candidate ==
                          fn.return_ownership_arc_diagnostic_candidate &&
                      existing.return_ownership_arc_fixit_available == fn.return_ownership_arc_fixit_available &&
                      existing.return_ownership_arc_diagnostic_profile ==
                          fn.return_ownership_arc_diagnostic_profile &&
                      existing.return_ownership_arc_fixit_hint == fn.return_ownership_arc_fixit_hint;
    if (compatible && existing.return_is_vector) {
      compatible = existing.return_vector_base_spelling == fn.return_vector_base_spelling &&
                   existing.return_vector_lane_count == fn.return_vector_lane_count;
    }
    const ProtocolCompositionInfo return_protocol_composition = BuildProtocolCompositionInfoFromFunctionReturn(fn);
    if (compatible && !AreEquivalentProtocolCompositions(existing.return_has_protocol_composition,
                                                         existing.return_protocol_composition_lexicographic,
                                                         return_protocol_composition.has_protocol_composition,
                                                         return_protocol_composition.names_lexicographic)) {
      compatible = false;
    }
    if (compatible) {
      for (std::size_t i = 0; i < fn.params.size(); ++i) {
        const ProtocolCompositionInfo param_protocol_composition = BuildProtocolCompositionInfoFromParam(fn.params[i]);
        if (i >= existing.param_types.size() || i >= existing.param_is_vector.size() ||
            i >= existing.param_vector_base_spelling.size() || i >= existing.param_vector_lane_count.size() ||
            i >= existing.param_has_ownership_qualifier.size() ||
            i >= existing.param_ownership_insert_retain.size() ||
            i >= existing.param_ownership_insert_release.size() ||
            i >= existing.param_ownership_insert_autorelease.size() ||
            i >= existing.param_ownership_is_weak_reference.size() ||
            i >= existing.param_ownership_is_unowned_reference.size() ||
            i >= existing.param_ownership_is_unowned_safe_reference.size() ||
            i >= existing.param_ownership_arc_diagnostic_candidate.size() ||
            i >= existing.param_ownership_arc_fixit_available.size() ||
            i >= existing.param_ownership_arc_diagnostic_profile.size() ||
            i >= existing.param_ownership_arc_fixit_hint.size() ||
            i >= existing.param_has_protocol_composition.size() ||
            i >= existing.param_protocol_composition_lexicographic.size() ||
            existing.param_types[i] != fn.params[i].type ||
            existing.param_is_vector[i] != fn.params[i].vector_spelling) {
          compatible = false;
          break;
        }
        if (existing.param_has_ownership_qualifier[i] != fn.params[i].has_ownership_qualifier) {
          compatible = false;
          break;
        }
        if (existing.param_ownership_insert_retain[i] != fn.params[i].ownership_insert_retain ||
            existing.param_ownership_insert_release[i] != fn.params[i].ownership_insert_release ||
            existing.param_ownership_insert_autorelease[i] != fn.params[i].ownership_insert_autorelease ||
            existing.param_ownership_is_weak_reference[i] != fn.params[i].ownership_is_weak_reference ||
            existing.param_ownership_is_unowned_reference[i] != fn.params[i].ownership_is_unowned_reference ||
            existing.param_ownership_is_unowned_safe_reference[i] !=
                fn.params[i].ownership_is_unowned_safe_reference ||
            existing.param_ownership_arc_diagnostic_candidate[i] !=
                fn.params[i].ownership_arc_diagnostic_candidate ||
            existing.param_ownership_arc_fixit_available[i] !=
                fn.params[i].ownership_arc_fixit_available ||
            existing.param_ownership_arc_diagnostic_profile[i] !=
                fn.params[i].ownership_arc_diagnostic_profile ||
            existing.param_ownership_arc_fixit_hint[i] != fn.params[i].ownership_arc_fixit_hint) {
          compatible = false;
          break;
        }
        if (existing.param_is_vector[i] &&
            (existing.param_vector_base_spelling[i] != fn.params[i].vector_base_spelling ||
             existing.param_vector_lane_count[i] != fn.params[i].vector_lane_count)) {
          compatible = false;
          break;
        }
        if (!AreEquivalentProtocolCompositions(existing.param_has_protocol_composition[i],
                                               existing.param_protocol_composition_lexicographic[i],
                                               param_protocol_composition.has_protocol_composition,
                                               param_protocol_composition.names_lexicographic)) {
          compatible = false;
          break;
        }
      }
    }
    if (!compatible) {
      diagnostics.push_back(MakeDiag(fn.line, fn.column, "O3S206",
                                     "type mismatch: incompatible function signature for '" + fn.name + "'"));
      continue;
    }

    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_generic_suffix.size(); ++i) {
      existing.param_has_generic_suffix[i] =
          existing.param_has_generic_suffix[i] || fn.params[i].has_generic_suffix;
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_pointer_declarator.size(); ++i) {
      existing.param_has_pointer_declarator[i] =
          existing.param_has_pointer_declarator[i] || fn.params[i].has_pointer_declarator;
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_nullability_suffix.size(); ++i) {
      existing.param_has_nullability_suffix[i] =
          existing.param_has_nullability_suffix[i] || !fn.params[i].nullability_suffix_tokens.empty();
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_ownership_qualifier.size(); ++i) {
      existing.param_has_ownership_qualifier[i] =
          existing.param_has_ownership_qualifier[i] || fn.params[i].has_ownership_qualifier;
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_object_pointer_type_spelling.size(); ++i) {
      existing.param_object_pointer_type_spelling[i] =
          existing.param_object_pointer_type_spelling[i] || fn.params[i].object_pointer_type_spelling;
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_invalid_generic_suffix.size(); ++i) {
      existing.param_has_invalid_generic_suffix[i] =
          existing.param_has_invalid_generic_suffix[i] || HasInvalidGenericParamTypeSuffix(fn.params[i]);
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_invalid_pointer_declarator.size(); ++i) {
      existing.param_has_invalid_pointer_declarator[i] =
          existing.param_has_invalid_pointer_declarator[i] || HasInvalidPointerParamTypeDeclarator(fn.params[i]);
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_invalid_nullability_suffix.size(); ++i) {
      existing.param_has_invalid_nullability_suffix[i] =
          existing.param_has_invalid_nullability_suffix[i] || HasInvalidNullabilityParamTypeSuffix(fn.params[i]);
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_invalid_ownership_qualifier.size(); ++i) {
      existing.param_has_invalid_ownership_qualifier[i] =
          existing.param_has_invalid_ownership_qualifier[i] ||
          HasInvalidOwnershipQualifierParamTypeSuffix(fn.params[i]);
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_invalid_type_suffix.size(); ++i) {
      existing.param_has_invalid_type_suffix[i] =
          existing.param_has_invalid_type_suffix[i] || HasInvalidParamTypeSuffix(fn.params[i]);
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_invalid_protocol_composition.size(); ++i) {
      const ProtocolCompositionInfo param_protocol_composition = BuildProtocolCompositionInfoFromParam(fn.params[i]);
      existing.param_has_invalid_protocol_composition[i] =
          existing.param_has_invalid_protocol_composition[i] || param_protocol_composition.has_invalid_protocol_composition;
    }
    existing.return_has_generic_suffix = existing.return_has_generic_suffix || fn.has_return_generic_suffix;
    existing.return_has_pointer_declarator = existing.return_has_pointer_declarator || fn.has_return_pointer_declarator;
    existing.return_has_nullability_suffix =
        existing.return_has_nullability_suffix || !fn.return_nullability_suffix_tokens.empty();
    existing.return_has_ownership_qualifier =
        existing.return_has_ownership_qualifier || fn.has_return_ownership_qualifier;
    existing.return_object_pointer_type_spelling =
        existing.return_object_pointer_type_spelling || fn.return_object_pointer_type_spelling;
    existing.return_has_invalid_generic_suffix =
        existing.return_has_invalid_generic_suffix || HasInvalidGenericReturnTypeSuffix(fn);
    existing.return_has_invalid_pointer_declarator =
        existing.return_has_invalid_pointer_declarator || HasInvalidPointerReturnTypeDeclarator(fn);
    existing.return_has_invalid_nullability_suffix =
        existing.return_has_invalid_nullability_suffix || HasInvalidNullabilityReturnTypeSuffix(fn);
    existing.return_has_invalid_ownership_qualifier =
        existing.return_has_invalid_ownership_qualifier || HasInvalidOwnershipQualifierReturnTypeSuffix(fn);
    existing.return_has_invalid_type_suffix = existing.return_has_invalid_type_suffix ||
                                             existing.return_has_invalid_generic_suffix ||
                                             existing.return_has_invalid_pointer_declarator ||
                                             existing.return_has_invalid_nullability_suffix ||
                                             existing.return_has_invalid_ownership_qualifier;
    existing.return_has_invalid_protocol_composition =
        existing.return_has_invalid_protocol_composition || return_protocol_composition.has_invalid_protocol_composition;
    existing.is_pure_annotation = existing.is_pure_annotation || fn.is_pure;

    if (!fn.is_prototype) {
      if (existing.has_definition) {
        diagnostics.push_back(MakeDiag(fn.line, fn.column, "O3S200", "duplicate function '" + fn.name + "'"));
      } else {
        existing.has_definition = true;
      }
    }
  }

  for (const auto &interface_decl : ast.interfaces) {
    auto interface_it = surface.interfaces.find(interface_decl.name);
    if (interface_it != surface.interfaces.end()) {
      diagnostics.push_back(
          MakeDiag(interface_decl.line, interface_decl.column, "O3S200", "duplicate interface '" + interface_decl.name + "'"));
      continue;
    }

    Objc3InterfaceInfo interface_info;
    interface_info.super_name = interface_decl.super_name;
    for (const auto &property_decl : interface_decl.properties) {
      ValidatePropertyTypeSuffixes(property_decl, interface_decl.name, "interface", diagnostics);
      Objc3PropertyInfo property_info =
          BuildPropertyInfo(property_decl, interface_decl.name, "interface", diagnostics);
      const auto property_insert = interface_info.properties.emplace(property_decl.name, std::move(property_info));
      if (!property_insert.second) {
        diagnostics.push_back(
            MakeDiag(property_decl.line,
                     property_decl.column,
                     "O3S200",
                     "duplicate interface property '" + property_decl.name + "' in interface '" + interface_decl.name + "'"));
      }
    }

    for (const auto &method_decl : interface_decl.methods) {
      const MethodSelectorNormalizationContractInfo selector_contract =
          BuildMethodSelectorNormalizationContractInfo(method_decl);
      ValidateMethodSelectorNormalizationContract(
          method_decl, interface_decl.name, "interface", selector_contract, diagnostics);
      ValidateMethodReturnTypeSuffixes(method_decl, interface_decl.name, "interface", diagnostics);
      ValidateMethodParameterTypeSuffixes(method_decl, interface_decl.name, "interface", diagnostics);

      const std::string selector = selector_contract.normalized_selector;
      if (method_decl.has_body) {
        diagnostics.push_back(MakeDiag(method_decl.line, method_decl.column, "O3S206",
                                       "type mismatch: interface selector '" + selector + "' in '" +
                                           interface_decl.name + "' must not define a body"));
      }

      const auto method_insert =
          interface_info.methods.emplace(selector, BuildMethodInfo(method_decl, selector_contract));
      if (!method_insert.second) {
        diagnostics.push_back(MakeDiag(method_decl.line, method_decl.column, "O3S200",
                                       "duplicate interface selector '" + selector + "' in interface '" +
                                           interface_decl.name + "'"));
        continue;
      }

      ++interface_implementation_summary.interface_method_symbols;
    }

    surface.interfaces.emplace(interface_decl.name, std::move(interface_info));
  }

  for (const auto &implementation_decl : ast.implementations) {
    auto implementation_it = surface.implementations.find(implementation_decl.name);
    if (implementation_it != surface.implementations.end()) {
      diagnostics.push_back(MakeDiag(implementation_decl.line, implementation_decl.column, "O3S200",
                                     "duplicate implementation '" + implementation_decl.name + "'"));
      continue;
    }

    Objc3ImplementationInfo implementation_info;
    const auto interface_it = surface.interfaces.find(implementation_decl.name);
    if (interface_it == surface.interfaces.end()) {
      diagnostics.push_back(MakeDiag(implementation_decl.line, implementation_decl.column, "O3S206",
                                     "type mismatch: missing interface declaration for implementation '" +
                                         implementation_decl.name + "'"));
    } else {
      implementation_info.has_matching_interface = true;
    }

    for (const auto &property_decl : implementation_decl.properties) {
      ValidatePropertyTypeSuffixes(property_decl, implementation_decl.name, "implementation", diagnostics);
      Objc3PropertyInfo property_info =
          BuildPropertyInfo(property_decl, implementation_decl.name, "implementation", diagnostics);
      const auto property_insert = implementation_info.properties.emplace(property_decl.name, std::move(property_info));
      if (!property_insert.second) {
        diagnostics.push_back(
            MakeDiag(property_decl.line,
                     property_decl.column,
                     "O3S200",
                     "duplicate implementation property '" + property_decl.name + "' in implementation '" +
                         implementation_decl.name + "'"));
        continue;
      }

      if (interface_it == surface.interfaces.end()) {
        continue;
      }

      const auto interface_property_it = interface_it->second.properties.find(property_decl.name);
      if (interface_property_it == interface_it->second.properties.end()) {
        diagnostics.push_back(MakeDiag(property_decl.line,
                                       property_decl.column,
                                       "O3S206",
                                       "type mismatch: implementation property '" + property_decl.name + "' in '" +
                                           implementation_decl.name + "' is not declared in interface"));
        continue;
      }
      if (!IsCompatiblePropertySignature(interface_property_it->second, property_insert.first->second)) {
        diagnostics.push_back(MakeDiag(property_decl.line,
                                       property_decl.column,
                                       "O3S206",
                                       "type mismatch: incompatible property signature for '" + property_decl.name +
                                           "' in implementation '" + implementation_decl.name + "'"));
      }
    }

    for (const auto &method_decl : implementation_decl.methods) {
      const MethodSelectorNormalizationContractInfo selector_contract =
          BuildMethodSelectorNormalizationContractInfo(method_decl);
      ValidateMethodSelectorNormalizationContract(
          method_decl, implementation_decl.name, "implementation", selector_contract, diagnostics);
      ValidateMethodReturnTypeSuffixes(method_decl, implementation_decl.name, "implementation", diagnostics);
      ValidateMethodParameterTypeSuffixes(method_decl, implementation_decl.name, "implementation", diagnostics);

      const std::string selector = selector_contract.normalized_selector;
      if (!method_decl.has_body) {
        diagnostics.push_back(MakeDiag(method_decl.line, method_decl.column, "O3S206",
                                       "type mismatch: implementation selector '" + selector + "' in '" +
                                           implementation_decl.name + "' must define a body"));
      }

      Objc3MethodInfo method_info = BuildMethodInfo(method_decl, selector_contract);
      const auto method_insert =
          implementation_info.methods.emplace(selector, std::move(method_info));
      if (!method_insert.second) {
        diagnostics.push_back(MakeDiag(method_decl.line, method_decl.column, "O3S200",
                                       "duplicate implementation selector '" + selector + "' in implementation '" +
                                           implementation_decl.name + "'"));
        continue;
      }

      ++interface_implementation_summary.implementation_method_symbols;
      if (interface_it == surface.interfaces.end()) {
        continue;
      }

      const auto interface_method_it = interface_it->second.methods.find(selector);
      if (interface_method_it == interface_it->second.methods.end()) {
        diagnostics.push_back(MakeDiag(method_decl.line, method_decl.column, "O3S206",
                                       "type mismatch: implementation selector '" + selector + "' in '" +
                                           implementation_decl.name + "' is not declared in interface"));
        continue;
      }

      if (!IsCompatibleMethodSignature(interface_method_it->second, method_insert.first->second)) {
        diagnostics.push_back(MakeDiag(method_decl.line, method_decl.column, "O3S206",
                                       "type mismatch: incompatible method signature for selector '" + selector +
                                           "' in implementation '" + implementation_decl.name + "'"));
        continue;
      }

      ++interface_implementation_summary.linked_implementation_symbols;
    }

    surface.implementations.emplace(implementation_decl.name, std::move(implementation_info));
  }

  interface_implementation_summary.resolved_interfaces = surface.interfaces.size();
  interface_implementation_summary.resolved_implementations = surface.implementations.size();
  interface_implementation_summary.deterministic =
      interface_implementation_summary.linked_implementation_symbols <=
          interface_implementation_summary.implementation_method_symbols &&
      interface_implementation_summary.linked_implementation_symbols <=
          interface_implementation_summary.interface_method_symbols;
  surface.interface_implementation_summary = interface_implementation_summary;
  surface.protocol_category_composition_summary = BuildProtocolCategoryCompositionSummaryFromSurface(surface);
  surface.class_protocol_category_linking_summary =
      BuildClassProtocolCategoryLinkingSummary(surface.interface_implementation_summary,
                                               surface.protocol_category_composition_summary);
  surface.selector_normalization_summary = BuildSelectorNormalizationSummaryFromSurface(surface);
  surface.property_attribute_summary = BuildPropertyAttributeSummaryFromSurface(surface);
  surface.type_annotation_surface_summary = BuildTypeAnnotationSurfaceSummaryFromIntegrationSurface(surface);
  surface.symbol_graph_scope_resolution_summary = BuildSymbolGraphScopeResolutionSummaryFromIntegrationSurface(surface);
  surface.method_lookup_override_conflict_summary =
      BuildMethodLookupOverrideConflictSummaryFromIntegrationSurface(surface);
  surface.property_synthesis_ivar_binding_summary =
      BuildPropertySynthesisIvarBindingSummaryFromIntegrationSurface(surface);
  surface.id_class_sel_object_pointer_type_checking_summary =
      BuildIdClassSelObjectPointerTypeCheckingSummaryFromIntegrationSurface(surface);
  surface.block_literal_capture_sites_lexicographic =
      BuildBlockLiteralCaptureSiteMetadataLexicographic(ast);
  surface.block_literal_capture_semantics_summary =
      BuildBlockLiteralCaptureSemanticsSummaryFromIntegrationSurface(surface);
  surface.block_abi_invoke_trampoline_sites_lexicographic =
      BuildBlockAbiInvokeTrampolineSiteMetadataLexicographic(ast);
  surface.block_abi_invoke_trampoline_semantics_summary =
      BuildBlockAbiInvokeTrampolineSemanticsSummaryFromIntegrationSurface(surface);
  surface.block_storage_escape_sites_lexicographic =
      BuildBlockStorageEscapeSiteMetadataLexicographic(ast);
  surface.block_storage_escape_semantics_summary =
      BuildBlockStorageEscapeSemanticsSummaryFromIntegrationSurface(surface);
  surface.message_send_selector_lowering_sites_lexicographic =
      BuildMessageSendSelectorLoweringSiteMetadataLexicographic(ast);
  surface.message_send_selector_lowering_summary =
      BuildMessageSendSelectorLoweringSummaryFromIntegrationSurface(surface);
  surface.dispatch_abi_marshalling_summary = BuildDispatchAbiMarshallingSummaryFromIntegrationSurface(surface);
  surface.nil_receiver_semantics_foldability_summary =
      BuildNilReceiverSemanticsFoldabilitySummaryFromIntegrationSurface(surface);
  surface.super_dispatch_method_family_summary =
      BuildSuperDispatchMethodFamilySummaryFromIntegrationSurface(surface);
  surface.runtime_shim_host_link_summary =
      BuildRuntimeShimHostLinkSummaryFromIntegrationSurface(surface);
  surface.retain_release_operation_summary =
      BuildRetainReleaseOperationSummaryFromIntegrationSurface(surface);
  surface.weak_unowned_semantics_summary =
      BuildWeakUnownedSemanticsSummaryFromIntegrationSurface(surface);
  surface.arc_diagnostics_fixit_summary =
      BuildArcDiagnosticsFixitSummaryFromIntegrationSurface(surface);
  surface.autoreleasepool_scope_sites_lexicographic =
      BuildAutoreleasePoolScopeSiteMetadataLexicographic(ast);
  surface.autoreleasepool_scope_summary =
      BuildAutoreleasePoolScopeSummaryFromIntegrationSurface(surface);
  surface.built = true;
  return surface;
}

Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(const Objc3SemanticIntegrationSurface &surface) {
  Objc3SemanticTypeMetadataHandoff handoff;
  handoff.global_names_lexicographic.reserve(surface.globals.size());
  for (const auto &global : surface.globals) {
    handoff.global_names_lexicographic.push_back(global.first);
  }
  std::sort(handoff.global_names_lexicographic.begin(), handoff.global_names_lexicographic.end());

  std::vector<std::string> function_names;
  function_names.reserve(surface.functions.size());
  for (const auto &entry : surface.functions) {
    function_names.push_back(entry.first);
  }
  std::sort(function_names.begin(), function_names.end());

  handoff.functions_lexicographic.reserve(function_names.size());
  for (const std::string &name : function_names) {
    const auto function_it = surface.functions.find(name);
    if (function_it == surface.functions.end()) {
      continue;
    }
    const FunctionInfo &source = function_it->second;
    Objc3SemanticFunctionTypeMetadata metadata;
    metadata.name = name;
    metadata.arity = source.arity;
    metadata.param_types = source.param_types;
    metadata.param_is_vector = source.param_is_vector;
    metadata.param_vector_base_spelling = source.param_vector_base_spelling;
    metadata.param_vector_lane_count = source.param_vector_lane_count;
    metadata.param_has_generic_suffix = source.param_has_generic_suffix;
    metadata.param_has_pointer_declarator = source.param_has_pointer_declarator;
    metadata.param_has_nullability_suffix = source.param_has_nullability_suffix;
    metadata.param_has_ownership_qualifier = source.param_has_ownership_qualifier;
    metadata.param_object_pointer_type_spelling = source.param_object_pointer_type_spelling;
    metadata.param_has_invalid_generic_suffix = source.param_has_invalid_generic_suffix;
    metadata.param_has_invalid_pointer_declarator = source.param_has_invalid_pointer_declarator;
    metadata.param_has_invalid_nullability_suffix = source.param_has_invalid_nullability_suffix;
    metadata.param_has_invalid_ownership_qualifier = source.param_has_invalid_ownership_qualifier;
    metadata.param_has_invalid_type_suffix = source.param_has_invalid_type_suffix;
    metadata.param_ownership_insert_retain = source.param_ownership_insert_retain;
    metadata.param_ownership_insert_release = source.param_ownership_insert_release;
    metadata.param_ownership_insert_autorelease = source.param_ownership_insert_autorelease;
    metadata.param_ownership_is_weak_reference = source.param_ownership_is_weak_reference;
    metadata.param_ownership_is_unowned_reference = source.param_ownership_is_unowned_reference;
    metadata.param_ownership_is_unowned_safe_reference = source.param_ownership_is_unowned_safe_reference;
    metadata.param_ownership_arc_diagnostic_candidate = source.param_ownership_arc_diagnostic_candidate;
    metadata.param_ownership_arc_fixit_available = source.param_ownership_arc_fixit_available;
    metadata.param_ownership_arc_diagnostic_profile = source.param_ownership_arc_diagnostic_profile;
    metadata.param_ownership_arc_fixit_hint = source.param_ownership_arc_fixit_hint;
    metadata.param_has_protocol_composition = source.param_has_protocol_composition;
    metadata.param_protocol_composition_lexicographic = source.param_protocol_composition_lexicographic;
    metadata.param_has_invalid_protocol_composition = source.param_has_invalid_protocol_composition;
    metadata.return_has_generic_suffix = source.return_has_generic_suffix;
    metadata.return_has_pointer_declarator = source.return_has_pointer_declarator;
    metadata.return_has_nullability_suffix = source.return_has_nullability_suffix;
    metadata.return_has_ownership_qualifier = source.return_has_ownership_qualifier;
    metadata.return_object_pointer_type_spelling = source.return_object_pointer_type_spelling;
    metadata.return_has_invalid_generic_suffix = source.return_has_invalid_generic_suffix;
    metadata.return_has_invalid_pointer_declarator = source.return_has_invalid_pointer_declarator;
    metadata.return_has_invalid_nullability_suffix = source.return_has_invalid_nullability_suffix;
    metadata.return_has_invalid_ownership_qualifier = source.return_has_invalid_ownership_qualifier;
    metadata.return_has_invalid_type_suffix = source.return_has_invalid_type_suffix;
    metadata.return_ownership_insert_retain = source.return_ownership_insert_retain;
    metadata.return_ownership_insert_release = source.return_ownership_insert_release;
    metadata.return_ownership_insert_autorelease = source.return_ownership_insert_autorelease;
    metadata.return_ownership_is_weak_reference = source.return_ownership_is_weak_reference;
    metadata.return_ownership_is_unowned_reference = source.return_ownership_is_unowned_reference;
    metadata.return_ownership_is_unowned_safe_reference = source.return_ownership_is_unowned_safe_reference;
    metadata.return_ownership_arc_diagnostic_candidate = source.return_ownership_arc_diagnostic_candidate;
    metadata.return_ownership_arc_fixit_available = source.return_ownership_arc_fixit_available;
    metadata.return_ownership_arc_diagnostic_profile = source.return_ownership_arc_diagnostic_profile;
    metadata.return_ownership_arc_fixit_hint = source.return_ownership_arc_fixit_hint;
    metadata.return_type = source.return_type;
    metadata.return_is_vector = source.return_is_vector;
    metadata.return_vector_base_spelling = source.return_vector_base_spelling;
    metadata.return_vector_lane_count = source.return_vector_lane_count;
    metadata.return_has_protocol_composition = source.return_has_protocol_composition;
    metadata.return_protocol_composition_lexicographic = source.return_protocol_composition_lexicographic;
    metadata.return_has_invalid_protocol_composition = source.return_has_invalid_protocol_composition;
    metadata.has_definition = source.has_definition;
    metadata.is_pure_annotation = source.is_pure_annotation;
    handoff.functions_lexicographic.push_back(std::move(metadata));
  }

  std::vector<std::string> interface_names;
  interface_names.reserve(surface.interfaces.size());
  for (const auto &entry : surface.interfaces) {
    interface_names.push_back(entry.first);
  }
  std::sort(interface_names.begin(), interface_names.end());

  handoff.interfaces_lexicographic.reserve(interface_names.size());
  for (const std::string &name : interface_names) {
    const auto interface_it = surface.interfaces.find(name);
    if (interface_it == surface.interfaces.end()) {
      continue;
    }

    Objc3SemanticInterfaceTypeMetadata metadata;
    metadata.name = name;
    metadata.super_name = interface_it->second.super_name;

    std::vector<std::string> property_names;
    property_names.reserve(interface_it->second.properties.size());
    for (const auto &property_entry : interface_it->second.properties) {
      property_names.push_back(property_entry.first);
    }
    std::sort(property_names.begin(), property_names.end());

    metadata.properties_lexicographic.reserve(property_names.size());
    for (const std::string &property_name : property_names) {
      const auto property_it = interface_it->second.properties.find(property_name);
      if (property_it == interface_it->second.properties.end()) {
        continue;
      }
      const Objc3PropertyInfo &source = property_it->second;
      Objc3SemanticPropertyTypeMetadata property_metadata;
      property_metadata.name = property_name;
      property_metadata.type = source.type;
      property_metadata.is_vector = source.is_vector;
      property_metadata.vector_base_spelling = source.vector_base_spelling;
      property_metadata.vector_lane_count = source.vector_lane_count;
      property_metadata.id_spelling = source.id_spelling;
      property_metadata.class_spelling = source.class_spelling;
      property_metadata.instancetype_spelling = source.instancetype_spelling;
      property_metadata.object_pointer_type_spelling = source.object_pointer_type_spelling;
      property_metadata.has_generic_suffix = source.has_generic_suffix;
      property_metadata.has_pointer_declarator = source.has_pointer_declarator;
      property_metadata.has_nullability_suffix = source.has_nullability_suffix;
      property_metadata.has_ownership_qualifier = source.has_ownership_qualifier;
      property_metadata.has_invalid_generic_suffix = source.has_invalid_generic_suffix;
      property_metadata.has_invalid_pointer_declarator = source.has_invalid_pointer_declarator;
      property_metadata.has_invalid_nullability_suffix = source.has_invalid_nullability_suffix;
      property_metadata.has_invalid_ownership_qualifier = source.has_invalid_ownership_qualifier;
      property_metadata.has_invalid_type_suffix = source.has_invalid_type_suffix;
      property_metadata.ownership_insert_retain = source.ownership_insert_retain;
      property_metadata.ownership_insert_release = source.ownership_insert_release;
      property_metadata.ownership_insert_autorelease = source.ownership_insert_autorelease;
      property_metadata.ownership_is_weak_reference = source.ownership_is_weak_reference;
      property_metadata.ownership_is_unowned_reference = source.ownership_is_unowned_reference;
      property_metadata.ownership_is_unowned_safe_reference = source.ownership_is_unowned_safe_reference;
      property_metadata.ownership_arc_diagnostic_candidate = source.ownership_arc_diagnostic_candidate;
      property_metadata.ownership_arc_fixit_available = source.ownership_arc_fixit_available;
      property_metadata.ownership_arc_diagnostic_profile = source.ownership_arc_diagnostic_profile;
      property_metadata.ownership_arc_fixit_hint = source.ownership_arc_fixit_hint;
      property_metadata.attribute_entries = source.attribute_entries;
      property_metadata.attribute_names_lexicographic = source.attribute_names_lexicographic;
      property_metadata.is_readonly = source.is_readonly;
      property_metadata.is_readwrite = source.is_readwrite;
      property_metadata.is_atomic = source.is_atomic;
      property_metadata.is_nonatomic = source.is_nonatomic;
      property_metadata.is_copy = source.is_copy;
      property_metadata.is_strong = source.is_strong;
      property_metadata.is_weak = source.is_weak;
      property_metadata.is_unowned = source.is_unowned;
      property_metadata.is_assign = source.is_assign;
      property_metadata.has_getter = source.has_getter;
      property_metadata.has_setter = source.has_setter;
      property_metadata.getter_selector = source.getter_selector;
      property_metadata.setter_selector = source.setter_selector;
      property_metadata.invalid_attribute_entries = source.invalid_attribute_entries;
      property_metadata.property_contract_violations = source.property_contract_violations;
      property_metadata.has_unknown_attribute = source.has_unknown_attribute;
      property_metadata.has_duplicate_attribute = source.has_duplicate_attribute;
      property_metadata.has_readwrite_conflict = source.has_readwrite_conflict;
      property_metadata.has_atomicity_conflict = source.has_atomicity_conflict;
      property_metadata.has_ownership_conflict = source.has_ownership_conflict;
      property_metadata.has_weak_unowned_conflict = source.has_weak_unowned_conflict;
      property_metadata.has_accessor_selector_contract_violation = source.has_accessor_selector_contract_violation;
      property_metadata.has_invalid_attribute_contract = source.has_invalid_attribute_contract;
      metadata.properties_lexicographic.push_back(std::move(property_metadata));
    }

    std::vector<std::string> selectors;
    selectors.reserve(interface_it->second.methods.size());
    for (const auto &method_entry : interface_it->second.methods) {
      selectors.push_back(method_entry.first);
    }
    std::sort(selectors.begin(), selectors.end());

    metadata.methods_lexicographic.reserve(selectors.size());
    for (const std::string &selector : selectors) {
      const auto method_it = interface_it->second.methods.find(selector);
      if (method_it == interface_it->second.methods.end()) {
        continue;
      }
      const Objc3MethodInfo &source = method_it->second;
      Objc3SemanticMethodTypeMetadata method_metadata;
      method_metadata.selector = selector;
      method_metadata.selector_normalized = source.selector_normalized;
      method_metadata.selector_piece_count = source.selector_piece_count;
      method_metadata.selector_parameter_piece_count = source.selector_parameter_piece_count;
      method_metadata.selector_contract_normalized = source.selector_contract_normalized;
      method_metadata.selector_had_pieceless_form = source.selector_had_pieceless_form;
      method_metadata.selector_has_spelling_mismatch = source.selector_has_spelling_mismatch;
      method_metadata.selector_has_arity_mismatch = source.selector_has_arity_mismatch;
      method_metadata.selector_has_parameter_linkage_mismatch = source.selector_has_parameter_linkage_mismatch;
      method_metadata.selector_has_normalization_flag_mismatch = source.selector_has_normalization_flag_mismatch;
      method_metadata.selector_has_missing_piece_keyword = source.selector_has_missing_piece_keyword;
      method_metadata.arity = source.arity;
      method_metadata.param_types = source.param_types;
      method_metadata.param_is_vector = source.param_is_vector;
      method_metadata.param_vector_base_spelling = source.param_vector_base_spelling;
      method_metadata.param_vector_lane_count = source.param_vector_lane_count;
      method_metadata.param_has_generic_suffix = source.param_has_generic_suffix;
      method_metadata.param_has_pointer_declarator = source.param_has_pointer_declarator;
      method_metadata.param_has_nullability_suffix = source.param_has_nullability_suffix;
      method_metadata.param_has_ownership_qualifier = source.param_has_ownership_qualifier;
      method_metadata.param_object_pointer_type_spelling = source.param_object_pointer_type_spelling;
      method_metadata.param_has_invalid_generic_suffix = source.param_has_invalid_generic_suffix;
      method_metadata.param_has_invalid_pointer_declarator = source.param_has_invalid_pointer_declarator;
      method_metadata.param_has_invalid_nullability_suffix = source.param_has_invalid_nullability_suffix;
      method_metadata.param_has_invalid_ownership_qualifier = source.param_has_invalid_ownership_qualifier;
      method_metadata.param_has_invalid_type_suffix = source.param_has_invalid_type_suffix;
      method_metadata.param_ownership_insert_retain = source.param_ownership_insert_retain;
      method_metadata.param_ownership_insert_release = source.param_ownership_insert_release;
      method_metadata.param_ownership_insert_autorelease = source.param_ownership_insert_autorelease;
      method_metadata.param_ownership_is_weak_reference = source.param_ownership_is_weak_reference;
      method_metadata.param_ownership_is_unowned_reference = source.param_ownership_is_unowned_reference;
      method_metadata.param_ownership_is_unowned_safe_reference = source.param_ownership_is_unowned_safe_reference;
      method_metadata.param_ownership_arc_diagnostic_candidate = source.param_ownership_arc_diagnostic_candidate;
      method_metadata.param_ownership_arc_fixit_available = source.param_ownership_arc_fixit_available;
      method_metadata.param_ownership_arc_diagnostic_profile = source.param_ownership_arc_diagnostic_profile;
      method_metadata.param_ownership_arc_fixit_hint = source.param_ownership_arc_fixit_hint;
      method_metadata.param_has_protocol_composition = source.param_has_protocol_composition;
      method_metadata.param_protocol_composition_lexicographic = source.param_protocol_composition_lexicographic;
      method_metadata.param_has_invalid_protocol_composition = source.param_has_invalid_protocol_composition;
      method_metadata.return_has_generic_suffix = source.return_has_generic_suffix;
      method_metadata.return_has_pointer_declarator = source.return_has_pointer_declarator;
      method_metadata.return_has_nullability_suffix = source.return_has_nullability_suffix;
      method_metadata.return_has_ownership_qualifier = source.return_has_ownership_qualifier;
      method_metadata.return_object_pointer_type_spelling = source.return_object_pointer_type_spelling;
      method_metadata.return_has_invalid_generic_suffix = source.return_has_invalid_generic_suffix;
      method_metadata.return_has_invalid_pointer_declarator = source.return_has_invalid_pointer_declarator;
      method_metadata.return_has_invalid_nullability_suffix = source.return_has_invalid_nullability_suffix;
      method_metadata.return_has_invalid_ownership_qualifier = source.return_has_invalid_ownership_qualifier;
      method_metadata.return_has_invalid_type_suffix = source.return_has_invalid_type_suffix;
      method_metadata.return_ownership_insert_retain = source.return_ownership_insert_retain;
      method_metadata.return_ownership_insert_release = source.return_ownership_insert_release;
      method_metadata.return_ownership_insert_autorelease = source.return_ownership_insert_autorelease;
      method_metadata.return_ownership_is_weak_reference = source.return_ownership_is_weak_reference;
      method_metadata.return_ownership_is_unowned_reference = source.return_ownership_is_unowned_reference;
      method_metadata.return_ownership_is_unowned_safe_reference = source.return_ownership_is_unowned_safe_reference;
      method_metadata.return_ownership_arc_diagnostic_candidate = source.return_ownership_arc_diagnostic_candidate;
      method_metadata.return_ownership_arc_fixit_available = source.return_ownership_arc_fixit_available;
      method_metadata.return_ownership_arc_diagnostic_profile = source.return_ownership_arc_diagnostic_profile;
      method_metadata.return_ownership_arc_fixit_hint = source.return_ownership_arc_fixit_hint;
      method_metadata.return_type = source.return_type;
      method_metadata.return_is_vector = source.return_is_vector;
      method_metadata.return_vector_base_spelling = source.return_vector_base_spelling;
      method_metadata.return_vector_lane_count = source.return_vector_lane_count;
      method_metadata.return_has_protocol_composition = source.return_has_protocol_composition;
      method_metadata.return_protocol_composition_lexicographic = source.return_protocol_composition_lexicographic;
      method_metadata.return_has_invalid_protocol_composition = source.return_has_invalid_protocol_composition;
      method_metadata.is_class_method = source.is_class_method;
      method_metadata.has_definition = source.has_definition;
      metadata.methods_lexicographic.push_back(std::move(method_metadata));
    }

    handoff.interfaces_lexicographic.push_back(std::move(metadata));
  }

  std::vector<std::string> implementation_names;
  implementation_names.reserve(surface.implementations.size());
  for (const auto &entry : surface.implementations) {
    implementation_names.push_back(entry.first);
  }
  std::sort(implementation_names.begin(), implementation_names.end());

  handoff.implementations_lexicographic.reserve(implementation_names.size());
  for (const std::string &name : implementation_names) {
    const auto implementation_it = surface.implementations.find(name);
    if (implementation_it == surface.implementations.end()) {
      continue;
    }

    Objc3SemanticImplementationTypeMetadata metadata;
    metadata.name = name;
    metadata.has_matching_interface = implementation_it->second.has_matching_interface;

    std::vector<std::string> property_names;
    property_names.reserve(implementation_it->second.properties.size());
    for (const auto &property_entry : implementation_it->second.properties) {
      property_names.push_back(property_entry.first);
    }
    std::sort(property_names.begin(), property_names.end());

    metadata.properties_lexicographic.reserve(property_names.size());
    for (const std::string &property_name : property_names) {
      const auto property_it = implementation_it->second.properties.find(property_name);
      if (property_it == implementation_it->second.properties.end()) {
        continue;
      }
      const Objc3PropertyInfo &source = property_it->second;
      Objc3SemanticPropertyTypeMetadata property_metadata;
      property_metadata.name = property_name;
      property_metadata.type = source.type;
      property_metadata.is_vector = source.is_vector;
      property_metadata.vector_base_spelling = source.vector_base_spelling;
      property_metadata.vector_lane_count = source.vector_lane_count;
      property_metadata.id_spelling = source.id_spelling;
      property_metadata.class_spelling = source.class_spelling;
      property_metadata.instancetype_spelling = source.instancetype_spelling;
      property_metadata.object_pointer_type_spelling = source.object_pointer_type_spelling;
      property_metadata.has_generic_suffix = source.has_generic_suffix;
      property_metadata.has_pointer_declarator = source.has_pointer_declarator;
      property_metadata.has_nullability_suffix = source.has_nullability_suffix;
      property_metadata.has_ownership_qualifier = source.has_ownership_qualifier;
      property_metadata.has_invalid_generic_suffix = source.has_invalid_generic_suffix;
      property_metadata.has_invalid_pointer_declarator = source.has_invalid_pointer_declarator;
      property_metadata.has_invalid_nullability_suffix = source.has_invalid_nullability_suffix;
      property_metadata.has_invalid_ownership_qualifier = source.has_invalid_ownership_qualifier;
      property_metadata.has_invalid_type_suffix = source.has_invalid_type_suffix;
      property_metadata.ownership_insert_retain = source.ownership_insert_retain;
      property_metadata.ownership_insert_release = source.ownership_insert_release;
      property_metadata.ownership_insert_autorelease = source.ownership_insert_autorelease;
      property_metadata.ownership_is_weak_reference = source.ownership_is_weak_reference;
      property_metadata.ownership_is_unowned_reference = source.ownership_is_unowned_reference;
      property_metadata.ownership_is_unowned_safe_reference = source.ownership_is_unowned_safe_reference;
      property_metadata.ownership_arc_diagnostic_candidate = source.ownership_arc_diagnostic_candidate;
      property_metadata.ownership_arc_fixit_available = source.ownership_arc_fixit_available;
      property_metadata.ownership_arc_diagnostic_profile = source.ownership_arc_diagnostic_profile;
      property_metadata.ownership_arc_fixit_hint = source.ownership_arc_fixit_hint;
      property_metadata.attribute_entries = source.attribute_entries;
      property_metadata.attribute_names_lexicographic = source.attribute_names_lexicographic;
      property_metadata.is_readonly = source.is_readonly;
      property_metadata.is_readwrite = source.is_readwrite;
      property_metadata.is_atomic = source.is_atomic;
      property_metadata.is_nonatomic = source.is_nonatomic;
      property_metadata.is_copy = source.is_copy;
      property_metadata.is_strong = source.is_strong;
      property_metadata.is_weak = source.is_weak;
      property_metadata.is_unowned = source.is_unowned;
      property_metadata.is_assign = source.is_assign;
      property_metadata.has_getter = source.has_getter;
      property_metadata.has_setter = source.has_setter;
      property_metadata.getter_selector = source.getter_selector;
      property_metadata.setter_selector = source.setter_selector;
      property_metadata.invalid_attribute_entries = source.invalid_attribute_entries;
      property_metadata.property_contract_violations = source.property_contract_violations;
      property_metadata.has_unknown_attribute = source.has_unknown_attribute;
      property_metadata.has_duplicate_attribute = source.has_duplicate_attribute;
      property_metadata.has_readwrite_conflict = source.has_readwrite_conflict;
      property_metadata.has_atomicity_conflict = source.has_atomicity_conflict;
      property_metadata.has_ownership_conflict = source.has_ownership_conflict;
      property_metadata.has_weak_unowned_conflict = source.has_weak_unowned_conflict;
      property_metadata.has_accessor_selector_contract_violation = source.has_accessor_selector_contract_violation;
      property_metadata.has_invalid_attribute_contract = source.has_invalid_attribute_contract;
      metadata.properties_lexicographic.push_back(std::move(property_metadata));
    }

    std::vector<std::string> selectors;
    selectors.reserve(implementation_it->second.methods.size());
    for (const auto &method_entry : implementation_it->second.methods) {
      selectors.push_back(method_entry.first);
    }
    std::sort(selectors.begin(), selectors.end());

    metadata.methods_lexicographic.reserve(selectors.size());
    for (const std::string &selector : selectors) {
      const auto method_it = implementation_it->second.methods.find(selector);
      if (method_it == implementation_it->second.methods.end()) {
        continue;
      }
      const Objc3MethodInfo &source = method_it->second;
      Objc3SemanticMethodTypeMetadata method_metadata;
      method_metadata.selector = selector;
      method_metadata.selector_normalized = source.selector_normalized;
      method_metadata.selector_piece_count = source.selector_piece_count;
      method_metadata.selector_parameter_piece_count = source.selector_parameter_piece_count;
      method_metadata.selector_contract_normalized = source.selector_contract_normalized;
      method_metadata.selector_had_pieceless_form = source.selector_had_pieceless_form;
      method_metadata.selector_has_spelling_mismatch = source.selector_has_spelling_mismatch;
      method_metadata.selector_has_arity_mismatch = source.selector_has_arity_mismatch;
      method_metadata.selector_has_parameter_linkage_mismatch = source.selector_has_parameter_linkage_mismatch;
      method_metadata.selector_has_normalization_flag_mismatch = source.selector_has_normalization_flag_mismatch;
      method_metadata.selector_has_missing_piece_keyword = source.selector_has_missing_piece_keyword;
      method_metadata.arity = source.arity;
      method_metadata.param_types = source.param_types;
      method_metadata.param_is_vector = source.param_is_vector;
      method_metadata.param_vector_base_spelling = source.param_vector_base_spelling;
      method_metadata.param_vector_lane_count = source.param_vector_lane_count;
      method_metadata.param_has_generic_suffix = source.param_has_generic_suffix;
      method_metadata.param_has_pointer_declarator = source.param_has_pointer_declarator;
      method_metadata.param_has_nullability_suffix = source.param_has_nullability_suffix;
      method_metadata.param_has_ownership_qualifier = source.param_has_ownership_qualifier;
      method_metadata.param_object_pointer_type_spelling = source.param_object_pointer_type_spelling;
      method_metadata.param_has_invalid_generic_suffix = source.param_has_invalid_generic_suffix;
      method_metadata.param_has_invalid_pointer_declarator = source.param_has_invalid_pointer_declarator;
      method_metadata.param_has_invalid_nullability_suffix = source.param_has_invalid_nullability_suffix;
      method_metadata.param_has_invalid_ownership_qualifier = source.param_has_invalid_ownership_qualifier;
      method_metadata.param_has_invalid_type_suffix = source.param_has_invalid_type_suffix;
      method_metadata.param_ownership_insert_retain = source.param_ownership_insert_retain;
      method_metadata.param_ownership_insert_release = source.param_ownership_insert_release;
      method_metadata.param_ownership_insert_autorelease = source.param_ownership_insert_autorelease;
      method_metadata.param_ownership_is_weak_reference = source.param_ownership_is_weak_reference;
      method_metadata.param_ownership_is_unowned_reference = source.param_ownership_is_unowned_reference;
      method_metadata.param_ownership_is_unowned_safe_reference = source.param_ownership_is_unowned_safe_reference;
      method_metadata.param_ownership_arc_diagnostic_candidate = source.param_ownership_arc_diagnostic_candidate;
      method_metadata.param_ownership_arc_fixit_available = source.param_ownership_arc_fixit_available;
      method_metadata.param_ownership_arc_diagnostic_profile = source.param_ownership_arc_diagnostic_profile;
      method_metadata.param_ownership_arc_fixit_hint = source.param_ownership_arc_fixit_hint;
      method_metadata.param_has_protocol_composition = source.param_has_protocol_composition;
      method_metadata.param_protocol_composition_lexicographic = source.param_protocol_composition_lexicographic;
      method_metadata.param_has_invalid_protocol_composition = source.param_has_invalid_protocol_composition;
      method_metadata.return_has_generic_suffix = source.return_has_generic_suffix;
      method_metadata.return_has_pointer_declarator = source.return_has_pointer_declarator;
      method_metadata.return_has_nullability_suffix = source.return_has_nullability_suffix;
      method_metadata.return_has_ownership_qualifier = source.return_has_ownership_qualifier;
      method_metadata.return_object_pointer_type_spelling = source.return_object_pointer_type_spelling;
      method_metadata.return_has_invalid_generic_suffix = source.return_has_invalid_generic_suffix;
      method_metadata.return_has_invalid_pointer_declarator = source.return_has_invalid_pointer_declarator;
      method_metadata.return_has_invalid_nullability_suffix = source.return_has_invalid_nullability_suffix;
      method_metadata.return_has_invalid_ownership_qualifier = source.return_has_invalid_ownership_qualifier;
      method_metadata.return_has_invalid_type_suffix = source.return_has_invalid_type_suffix;
      method_metadata.return_ownership_insert_retain = source.return_ownership_insert_retain;
      method_metadata.return_ownership_insert_release = source.return_ownership_insert_release;
      method_metadata.return_ownership_insert_autorelease = source.return_ownership_insert_autorelease;
      method_metadata.return_ownership_is_weak_reference = source.return_ownership_is_weak_reference;
      method_metadata.return_ownership_is_unowned_reference = source.return_ownership_is_unowned_reference;
      method_metadata.return_ownership_is_unowned_safe_reference = source.return_ownership_is_unowned_safe_reference;
      method_metadata.return_ownership_arc_diagnostic_candidate = source.return_ownership_arc_diagnostic_candidate;
      method_metadata.return_ownership_arc_fixit_available = source.return_ownership_arc_fixit_available;
      method_metadata.return_ownership_arc_diagnostic_profile = source.return_ownership_arc_diagnostic_profile;
      method_metadata.return_ownership_arc_fixit_hint = source.return_ownership_arc_fixit_hint;
      method_metadata.return_type = source.return_type;
      method_metadata.return_is_vector = source.return_is_vector;
      method_metadata.return_vector_base_spelling = source.return_vector_base_spelling;
      method_metadata.return_vector_lane_count = source.return_vector_lane_count;
      method_metadata.return_has_protocol_composition = source.return_has_protocol_composition;
      method_metadata.return_protocol_composition_lexicographic = source.return_protocol_composition_lexicographic;
      method_metadata.return_has_invalid_protocol_composition = source.return_has_invalid_protocol_composition;
      method_metadata.is_class_method = source.is_class_method;
      method_metadata.has_definition = source.has_definition;
      metadata.methods_lexicographic.push_back(std::move(method_metadata));
    }

    handoff.implementations_lexicographic.push_back(std::move(metadata));
  }

  handoff.interface_implementation_summary = surface.interface_implementation_summary;
  handoff.interface_implementation_summary.resolved_interfaces = handoff.interfaces_lexicographic.size();
  handoff.interface_implementation_summary.resolved_implementations = handoff.implementations_lexicographic.size();
  handoff.interface_implementation_summary.interface_method_symbols = 0;
  for (const auto &metadata : handoff.interfaces_lexicographic) {
    handoff.interface_implementation_summary.interface_method_symbols += metadata.methods_lexicographic.size();
  }
  handoff.interface_implementation_summary.implementation_method_symbols = 0;
  for (const auto &metadata : handoff.implementations_lexicographic) {
    handoff.interface_implementation_summary.implementation_method_symbols += metadata.methods_lexicographic.size();
  }
  handoff.interface_implementation_summary.linked_implementation_symbols = 0;
  const auto are_compatible_method_metadata = [](const Objc3SemanticMethodTypeMetadata &lhs,
                                                 const Objc3SemanticMethodTypeMetadata &rhs) {
    if (lhs.arity != rhs.arity || lhs.return_type != rhs.return_type || lhs.return_is_vector != rhs.return_is_vector ||
        lhs.is_class_method != rhs.is_class_method ||
        lhs.return_ownership_insert_retain != rhs.return_ownership_insert_retain ||
        lhs.return_ownership_insert_release != rhs.return_ownership_insert_release ||
        lhs.return_ownership_insert_autorelease != rhs.return_ownership_insert_autorelease ||
        lhs.return_ownership_is_weak_reference != rhs.return_ownership_is_weak_reference ||
        lhs.return_ownership_is_unowned_reference != rhs.return_ownership_is_unowned_reference ||
        lhs.return_ownership_is_unowned_safe_reference != rhs.return_ownership_is_unowned_safe_reference ||
        lhs.return_ownership_arc_diagnostic_candidate != rhs.return_ownership_arc_diagnostic_candidate ||
        lhs.return_ownership_arc_fixit_available != rhs.return_ownership_arc_fixit_available ||
        lhs.return_ownership_arc_diagnostic_profile != rhs.return_ownership_arc_diagnostic_profile ||
        lhs.return_ownership_arc_fixit_hint != rhs.return_ownership_arc_fixit_hint) {
      return false;
    }
    if (lhs.return_is_vector &&
        (lhs.return_vector_base_spelling != rhs.return_vector_base_spelling ||
         lhs.return_vector_lane_count != rhs.return_vector_lane_count)) {
      return false;
    }
    if (!AreEquivalentProtocolCompositions(lhs.return_has_protocol_composition,
                                           lhs.return_protocol_composition_lexicographic,
                                           rhs.return_has_protocol_composition,
                                           rhs.return_protocol_composition_lexicographic)) {
      return false;
    }
    for (std::size_t i = 0; i < lhs.arity; ++i) {
      if (i >= lhs.param_types.size() || i >= lhs.param_is_vector.size() || i >= lhs.param_vector_base_spelling.size() ||
          i >= lhs.param_vector_lane_count.size() || i >= lhs.param_has_protocol_composition.size() ||
          i >= lhs.param_protocol_composition_lexicographic.size() || i >= lhs.param_ownership_insert_retain.size() ||
          i >= lhs.param_ownership_insert_release.size() || i >= lhs.param_ownership_insert_autorelease.size() ||
          i >= lhs.param_ownership_is_weak_reference.size() ||
          i >= lhs.param_ownership_is_unowned_reference.size() ||
          i >= lhs.param_ownership_is_unowned_safe_reference.size() ||
          i >= lhs.param_ownership_arc_diagnostic_candidate.size() ||
          i >= lhs.param_ownership_arc_fixit_available.size() ||
          i >= lhs.param_ownership_arc_diagnostic_profile.size() ||
          i >= lhs.param_ownership_arc_fixit_hint.size() ||
          i >= rhs.param_types.size() ||
          i >= rhs.param_is_vector.size() || i >= rhs.param_vector_base_spelling.size() ||
          i >= rhs.param_vector_lane_count.size() || i >= rhs.param_has_protocol_composition.size() ||
          i >= rhs.param_protocol_composition_lexicographic.size() || i >= rhs.param_ownership_insert_retain.size() ||
          i >= rhs.param_ownership_insert_release.size() || i >= rhs.param_ownership_insert_autorelease.size() ||
          i >= rhs.param_ownership_is_weak_reference.size() ||
          i >= rhs.param_ownership_is_unowned_reference.size() ||
          i >= rhs.param_ownership_is_unowned_safe_reference.size() ||
          i >= rhs.param_ownership_arc_diagnostic_candidate.size() ||
          i >= rhs.param_ownership_arc_fixit_available.size() ||
          i >= rhs.param_ownership_arc_diagnostic_profile.size() ||
          i >= rhs.param_ownership_arc_fixit_hint.size()) {
        return false;
      }
      if (lhs.param_types[i] != rhs.param_types[i] || lhs.param_is_vector[i] != rhs.param_is_vector[i]) {
        return false;
      }
      if (lhs.param_ownership_insert_retain[i] != rhs.param_ownership_insert_retain[i] ||
          lhs.param_ownership_insert_release[i] != rhs.param_ownership_insert_release[i] ||
          lhs.param_ownership_insert_autorelease[i] != rhs.param_ownership_insert_autorelease[i] ||
          lhs.param_ownership_is_weak_reference[i] != rhs.param_ownership_is_weak_reference[i] ||
          lhs.param_ownership_is_unowned_reference[i] != rhs.param_ownership_is_unowned_reference[i] ||
          lhs.param_ownership_is_unowned_safe_reference[i] != rhs.param_ownership_is_unowned_safe_reference[i] ||
          lhs.param_ownership_arc_diagnostic_candidate[i] != rhs.param_ownership_arc_diagnostic_candidate[i] ||
          lhs.param_ownership_arc_fixit_available[i] != rhs.param_ownership_arc_fixit_available[i] ||
          lhs.param_ownership_arc_diagnostic_profile[i] != rhs.param_ownership_arc_diagnostic_profile[i] ||
          lhs.param_ownership_arc_fixit_hint[i] != rhs.param_ownership_arc_fixit_hint[i]) {
        return false;
      }
      if (lhs.param_is_vector[i] &&
          (lhs.param_vector_base_spelling[i] != rhs.param_vector_base_spelling[i] ||
           lhs.param_vector_lane_count[i] != rhs.param_vector_lane_count[i])) {
        return false;
      }
      if (!AreEquivalentProtocolCompositions(lhs.param_has_protocol_composition[i],
                                             lhs.param_protocol_composition_lexicographic[i],
                                             rhs.param_has_protocol_composition[i],
                                             rhs.param_protocol_composition_lexicographic[i])) {
        return false;
      }
    }
    return true;
  };
  std::unordered_map<std::string, const Objc3SemanticInterfaceTypeMetadata *> interfaces_by_name;
  interfaces_by_name.reserve(handoff.interfaces_lexicographic.size());
  for (const auto &metadata : handoff.interfaces_lexicographic) {
    interfaces_by_name[metadata.name] = &metadata;
  }
  for (const auto &implementation : handoff.implementations_lexicographic) {
    if (!implementation.has_matching_interface) {
      continue;
    }
    const auto interface_it = interfaces_by_name.find(implementation.name);
    if (interface_it == interfaces_by_name.end()) {
      continue;
    }
    const Objc3SemanticInterfaceTypeMetadata &interface_metadata = *interface_it->second;
    for (const auto &implementation_method : implementation.methods_lexicographic) {
      const auto interface_method_it = std::find_if(
          interface_metadata.methods_lexicographic.begin(),
          interface_metadata.methods_lexicographic.end(),
          [&implementation_method](const Objc3SemanticMethodTypeMetadata &candidate) {
            return candidate.selector == implementation_method.selector;
          });
      if (interface_method_it == interface_metadata.methods_lexicographic.end()) {
        continue;
      }
      if (are_compatible_method_metadata(*interface_method_it, implementation_method)) {
        ++handoff.interface_implementation_summary.linked_implementation_symbols;
      }
    }
  }
  handoff.interface_implementation_summary.deterministic =
      handoff.interface_implementation_summary.deterministic &&
      handoff.interface_implementation_summary.linked_implementation_symbols <=
          handoff.interface_implementation_summary.implementation_method_symbols &&
      handoff.interface_implementation_summary.linked_implementation_symbols <=
          handoff.interface_implementation_summary.interface_method_symbols;

  handoff.selector_normalization_summary = Objc3SelectorNormalizationSummary{};
  const auto accumulate_method_selector_metadata =
      [&handoff](const Objc3SemanticMethodTypeMetadata &metadata) {
        ++handoff.selector_normalization_summary.methods_total;
        if (metadata.selector_contract_normalized) {
          ++handoff.selector_normalization_summary.normalized_methods;
        }
        handoff.selector_normalization_summary.selector_piece_entries += metadata.selector_piece_count;
        handoff.selector_normalization_summary.selector_parameter_piece_entries +=
            metadata.selector_parameter_piece_count;
        if (metadata.selector_had_pieceless_form) {
          ++handoff.selector_normalization_summary.selector_pieceless_methods;
        }
        if (metadata.selector_has_spelling_mismatch) {
          ++handoff.selector_normalization_summary.selector_spelling_mismatches;
        }
        if (metadata.selector_has_arity_mismatch) {
          ++handoff.selector_normalization_summary.selector_arity_mismatches;
        }
        if (metadata.selector_has_parameter_linkage_mismatch) {
          ++handoff.selector_normalization_summary.selector_parameter_linkage_mismatches;
        }
        if (metadata.selector_has_normalization_flag_mismatch) {
          ++handoff.selector_normalization_summary.selector_normalization_flag_mismatches;
        }
        if (metadata.selector_has_missing_piece_keyword) {
          ++handoff.selector_normalization_summary.selector_missing_keyword_pieces;
        }
        if (metadata.selector_normalized.empty() ||
            metadata.selector_parameter_piece_count > metadata.selector_piece_count) {
          handoff.selector_normalization_summary.deterministic = false;
        }
      };
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      accumulate_method_selector_metadata(method_metadata);
    }
  }
  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    for (const auto &method_metadata : implementation_metadata.methods_lexicographic) {
      accumulate_method_selector_metadata(method_metadata);
    }
  }
  handoff.selector_normalization_summary.deterministic =
      handoff.selector_normalization_summary.deterministic &&
      handoff.selector_normalization_summary.normalized_methods <=
          handoff.selector_normalization_summary.methods_total &&
      handoff.selector_normalization_summary.selector_parameter_piece_entries <=
          handoff.selector_normalization_summary.selector_piece_entries &&
      handoff.selector_normalization_summary.contract_violations() <=
          handoff.selector_normalization_summary.methods_total;

  handoff.protocol_category_composition_summary = Objc3ProtocolCategoryCompositionSummary{};
  const auto accumulate_function_metadata_composition =
      [&handoff](const Objc3SemanticFunctionTypeMetadata &metadata) {
        if (metadata.param_has_protocol_composition.size() != metadata.arity ||
            metadata.param_protocol_composition_lexicographic.size() != metadata.arity ||
            metadata.param_has_invalid_protocol_composition.size() != metadata.arity) {
          handoff.protocol_category_composition_summary.deterministic = false;
          return;
        }
        for (std::size_t i = 0; i < metadata.arity; ++i) {
          AccumulateProtocolCompositionSite(metadata.param_has_protocol_composition[i],
                                           metadata.param_protocol_composition_lexicographic[i],
                                           metadata.param_has_invalid_protocol_composition[i],
                                           false,
                                           handoff.protocol_category_composition_summary);
        }
        AccumulateProtocolCompositionSite(metadata.return_has_protocol_composition,
                                         metadata.return_protocol_composition_lexicographic,
                                         metadata.return_has_invalid_protocol_composition,
                                         false,
                                         handoff.protocol_category_composition_summary);
      };
  const auto accumulate_method_metadata_composition =
      [&handoff](const Objc3SemanticMethodTypeMetadata &metadata) {
        if (metadata.param_has_protocol_composition.size() != metadata.arity ||
            metadata.param_protocol_composition_lexicographic.size() != metadata.arity ||
            metadata.param_has_invalid_protocol_composition.size() != metadata.arity) {
          handoff.protocol_category_composition_summary.deterministic = false;
          return;
        }
        for (std::size_t i = 0; i < metadata.arity; ++i) {
          AccumulateProtocolCompositionSite(metadata.param_has_protocol_composition[i],
                                           metadata.param_protocol_composition_lexicographic[i],
                                           metadata.param_has_invalid_protocol_composition[i],
                                           true,
                                           handoff.protocol_category_composition_summary);
        }
        AccumulateProtocolCompositionSite(metadata.return_has_protocol_composition,
                                         metadata.return_protocol_composition_lexicographic,
                                         metadata.return_has_invalid_protocol_composition,
                                         true,
                                         handoff.protocol_category_composition_summary);
      };

  for (const auto &function_metadata : handoff.functions_lexicographic) {
    accumulate_function_metadata_composition(function_metadata);
  }
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      accumulate_method_metadata_composition(method_metadata);
    }
  }
  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    for (const auto &method_metadata : implementation_metadata.methods_lexicographic) {
      accumulate_method_metadata_composition(method_metadata);
    }
  }
  handoff.protocol_category_composition_summary.deterministic =
      handoff.protocol_category_composition_summary.deterministic &&
      handoff.protocol_category_composition_summary.invalid_protocol_composition_sites <=
          handoff.protocol_category_composition_summary.total_composition_sites() &&
      handoff.protocol_category_composition_summary.category_composition_sites <=
          handoff.protocol_category_composition_summary.protocol_composition_sites &&
      handoff.protocol_category_composition_summary.category_composition_symbols <=
          handoff.protocol_category_composition_summary.protocol_composition_symbols;
  handoff.class_protocol_category_linking_summary =
      BuildClassProtocolCategoryLinkingSummary(handoff.interface_implementation_summary,
                                               handoff.protocol_category_composition_summary);

  handoff.type_annotation_surface_summary = Objc3TypeAnnotationSurfaceSummary{};
  const auto accumulate_function_type_annotations =
      [&handoff](const Objc3SemanticFunctionTypeMetadata &metadata) {
        if (metadata.param_has_generic_suffix.size() != metadata.arity ||
            metadata.param_has_pointer_declarator.size() != metadata.arity ||
            metadata.param_has_nullability_suffix.size() != metadata.arity ||
            metadata.param_has_ownership_qualifier.size() != metadata.arity ||
            metadata.param_object_pointer_type_spelling.size() != metadata.arity ||
            metadata.param_has_invalid_generic_suffix.size() != metadata.arity ||
            metadata.param_has_invalid_pointer_declarator.size() != metadata.arity ||
            metadata.param_has_invalid_nullability_suffix.size() != metadata.arity ||
            metadata.param_has_invalid_ownership_qualifier.size() != metadata.arity ||
            metadata.param_has_invalid_type_suffix.size() != metadata.arity) {
          handoff.type_annotation_surface_summary.deterministic = false;
          return;
        }
        for (std::size_t i = 0; i < metadata.arity; ++i) {
          if (metadata.param_has_generic_suffix[i]) {
            ++handoff.type_annotation_surface_summary.generic_suffix_sites;
          }
          if (metadata.param_has_pointer_declarator[i]) {
            ++handoff.type_annotation_surface_summary.pointer_declarator_sites;
          }
          if (metadata.param_has_nullability_suffix[i]) {
            ++handoff.type_annotation_surface_summary.nullability_suffix_sites;
          }
          if (metadata.param_has_ownership_qualifier[i]) {
            ++handoff.type_annotation_surface_summary.ownership_qualifier_sites;
          }
          if (metadata.param_object_pointer_type_spelling[i]) {
            ++handoff.type_annotation_surface_summary.object_pointer_type_sites;
          }
          if (metadata.param_has_invalid_generic_suffix[i]) {
            ++handoff.type_annotation_surface_summary.invalid_generic_suffix_sites;
          }
          if (metadata.param_has_invalid_pointer_declarator[i]) {
            ++handoff.type_annotation_surface_summary.invalid_pointer_declarator_sites;
          }
          if (metadata.param_has_invalid_nullability_suffix[i]) {
            ++handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites;
          }
          if (metadata.param_has_invalid_ownership_qualifier[i]) {
            ++handoff.type_annotation_surface_summary.invalid_ownership_qualifier_sites;
          }
          const bool expected_invalid = metadata.param_has_invalid_generic_suffix[i] ||
                                        metadata.param_has_invalid_pointer_declarator[i] ||
                                        metadata.param_has_invalid_nullability_suffix[i] ||
                                        metadata.param_has_invalid_ownership_qualifier[i];
          if (metadata.param_has_invalid_type_suffix[i] != expected_invalid) {
            handoff.type_annotation_surface_summary.deterministic = false;
          }
        }
        if (metadata.return_has_generic_suffix) {
          ++handoff.type_annotation_surface_summary.generic_suffix_sites;
        }
        if (metadata.return_has_pointer_declarator) {
          ++handoff.type_annotation_surface_summary.pointer_declarator_sites;
        }
        if (metadata.return_has_nullability_suffix) {
          ++handoff.type_annotation_surface_summary.nullability_suffix_sites;
        }
        if (metadata.return_has_ownership_qualifier) {
          ++handoff.type_annotation_surface_summary.ownership_qualifier_sites;
        }
        if (metadata.return_object_pointer_type_spelling) {
          ++handoff.type_annotation_surface_summary.object_pointer_type_sites;
        }
        if (metadata.return_has_invalid_generic_suffix) {
          ++handoff.type_annotation_surface_summary.invalid_generic_suffix_sites;
        }
        if (metadata.return_has_invalid_pointer_declarator) {
          ++handoff.type_annotation_surface_summary.invalid_pointer_declarator_sites;
        }
        if (metadata.return_has_invalid_nullability_suffix) {
          ++handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites;
        }
        if (metadata.return_has_invalid_ownership_qualifier) {
          ++handoff.type_annotation_surface_summary.invalid_ownership_qualifier_sites;
        }
        const bool expected_return_invalid = metadata.return_has_invalid_generic_suffix ||
                                             metadata.return_has_invalid_pointer_declarator ||
                                             metadata.return_has_invalid_nullability_suffix ||
                                             metadata.return_has_invalid_ownership_qualifier;
        if (metadata.return_has_invalid_type_suffix != expected_return_invalid) {
          handoff.type_annotation_surface_summary.deterministic = false;
        }
      };
  const auto accumulate_method_type_annotations =
      [&handoff](const Objc3SemanticMethodTypeMetadata &metadata) {
        if (metadata.param_has_generic_suffix.size() != metadata.arity ||
            metadata.param_has_pointer_declarator.size() != metadata.arity ||
            metadata.param_has_nullability_suffix.size() != metadata.arity ||
            metadata.param_has_ownership_qualifier.size() != metadata.arity ||
            metadata.param_object_pointer_type_spelling.size() != metadata.arity ||
            metadata.param_has_invalid_generic_suffix.size() != metadata.arity ||
            metadata.param_has_invalid_pointer_declarator.size() != metadata.arity ||
            metadata.param_has_invalid_nullability_suffix.size() != metadata.arity ||
            metadata.param_has_invalid_ownership_qualifier.size() != metadata.arity ||
            metadata.param_has_invalid_type_suffix.size() != metadata.arity) {
          handoff.type_annotation_surface_summary.deterministic = false;
          return;
        }
        for (std::size_t i = 0; i < metadata.arity; ++i) {
          if (metadata.param_has_generic_suffix[i]) {
            ++handoff.type_annotation_surface_summary.generic_suffix_sites;
          }
          if (metadata.param_has_pointer_declarator[i]) {
            ++handoff.type_annotation_surface_summary.pointer_declarator_sites;
          }
          if (metadata.param_has_nullability_suffix[i]) {
            ++handoff.type_annotation_surface_summary.nullability_suffix_sites;
          }
          if (metadata.param_has_ownership_qualifier[i]) {
            ++handoff.type_annotation_surface_summary.ownership_qualifier_sites;
          }
          if (metadata.param_object_pointer_type_spelling[i]) {
            ++handoff.type_annotation_surface_summary.object_pointer_type_sites;
          }
          if (metadata.param_has_invalid_generic_suffix[i]) {
            ++handoff.type_annotation_surface_summary.invalid_generic_suffix_sites;
          }
          if (metadata.param_has_invalid_pointer_declarator[i]) {
            ++handoff.type_annotation_surface_summary.invalid_pointer_declarator_sites;
          }
          if (metadata.param_has_invalid_nullability_suffix[i]) {
            ++handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites;
          }
          if (metadata.param_has_invalid_ownership_qualifier[i]) {
            ++handoff.type_annotation_surface_summary.invalid_ownership_qualifier_sites;
          }
          const bool expected_invalid = metadata.param_has_invalid_generic_suffix[i] ||
                                        metadata.param_has_invalid_pointer_declarator[i] ||
                                        metadata.param_has_invalid_nullability_suffix[i] ||
                                        metadata.param_has_invalid_ownership_qualifier[i];
          if (metadata.param_has_invalid_type_suffix[i] != expected_invalid) {
            handoff.type_annotation_surface_summary.deterministic = false;
          }
        }
        if (metadata.return_has_generic_suffix) {
          ++handoff.type_annotation_surface_summary.generic_suffix_sites;
        }
        if (metadata.return_has_pointer_declarator) {
          ++handoff.type_annotation_surface_summary.pointer_declarator_sites;
        }
        if (metadata.return_has_nullability_suffix) {
          ++handoff.type_annotation_surface_summary.nullability_suffix_sites;
        }
        if (metadata.return_has_ownership_qualifier) {
          ++handoff.type_annotation_surface_summary.ownership_qualifier_sites;
        }
        if (metadata.return_object_pointer_type_spelling) {
          ++handoff.type_annotation_surface_summary.object_pointer_type_sites;
        }
        if (metadata.return_has_invalid_generic_suffix) {
          ++handoff.type_annotation_surface_summary.invalid_generic_suffix_sites;
        }
        if (metadata.return_has_invalid_pointer_declarator) {
          ++handoff.type_annotation_surface_summary.invalid_pointer_declarator_sites;
        }
        if (metadata.return_has_invalid_nullability_suffix) {
          ++handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites;
        }
        if (metadata.return_has_invalid_ownership_qualifier) {
          ++handoff.type_annotation_surface_summary.invalid_ownership_qualifier_sites;
        }
        const bool expected_return_invalid = metadata.return_has_invalid_generic_suffix ||
                                             metadata.return_has_invalid_pointer_declarator ||
                                             metadata.return_has_invalid_nullability_suffix ||
                                             metadata.return_has_invalid_ownership_qualifier;
        if (metadata.return_has_invalid_type_suffix != expected_return_invalid) {
          handoff.type_annotation_surface_summary.deterministic = false;
        }
      };
  const auto accumulate_property_type_annotations =
      [&handoff](const Objc3SemanticPropertyTypeMetadata &metadata) {
        if (metadata.has_generic_suffix) {
          ++handoff.type_annotation_surface_summary.generic_suffix_sites;
        }
        if (metadata.has_pointer_declarator) {
          ++handoff.type_annotation_surface_summary.pointer_declarator_sites;
        }
        if (metadata.has_nullability_suffix) {
          ++handoff.type_annotation_surface_summary.nullability_suffix_sites;
        }
        if (metadata.has_ownership_qualifier) {
          ++handoff.type_annotation_surface_summary.ownership_qualifier_sites;
        }
        if (metadata.object_pointer_type_spelling) {
          ++handoff.type_annotation_surface_summary.object_pointer_type_sites;
        }
        if (metadata.has_invalid_generic_suffix) {
          ++handoff.type_annotation_surface_summary.invalid_generic_suffix_sites;
        }
        if (metadata.has_invalid_pointer_declarator) {
          ++handoff.type_annotation_surface_summary.invalid_pointer_declarator_sites;
        }
        if (metadata.has_invalid_nullability_suffix) {
          ++handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites;
        }
        if (metadata.has_invalid_ownership_qualifier) {
          ++handoff.type_annotation_surface_summary.invalid_ownership_qualifier_sites;
        }
        const bool expected_invalid = metadata.has_invalid_generic_suffix ||
                                      metadata.has_invalid_pointer_declarator ||
                                      metadata.has_invalid_nullability_suffix ||
                                      metadata.has_invalid_ownership_qualifier;
        if (metadata.has_invalid_type_suffix != expected_invalid) {
          handoff.type_annotation_surface_summary.deterministic = false;
        }
      };
  for (const auto &function_metadata : handoff.functions_lexicographic) {
    accumulate_function_type_annotations(function_metadata);
  }
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      accumulate_method_type_annotations(method_metadata);
    }
    for (const auto &property_metadata : interface_metadata.properties_lexicographic) {
      accumulate_property_type_annotations(property_metadata);
    }
  }
  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    for (const auto &method_metadata : implementation_metadata.methods_lexicographic) {
      accumulate_method_type_annotations(method_metadata);
    }
    for (const auto &property_metadata : implementation_metadata.properties_lexicographic) {
      accumulate_property_type_annotations(property_metadata);
    }
  }
  handoff.type_annotation_surface_summary.deterministic =
      handoff.type_annotation_surface_summary.deterministic &&
      handoff.type_annotation_surface_summary.invalid_generic_suffix_sites <=
          handoff.type_annotation_surface_summary.generic_suffix_sites &&
      handoff.type_annotation_surface_summary.invalid_pointer_declarator_sites <=
          handoff.type_annotation_surface_summary.pointer_declarator_sites &&
      handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites <=
          handoff.type_annotation_surface_summary.nullability_suffix_sites &&
      handoff.type_annotation_surface_summary.invalid_ownership_qualifier_sites <=
          handoff.type_annotation_surface_summary.ownership_qualifier_sites &&
      handoff.type_annotation_surface_summary.invalid_type_annotation_sites() <=
          handoff.type_annotation_surface_summary.total_type_annotation_sites();
  handoff.symbol_graph_scope_resolution_summary =
      BuildSymbolGraphScopeResolutionSummaryFromTypeMetadataHandoff(handoff);
  handoff.method_lookup_override_conflict_summary =
      BuildMethodLookupOverrideConflictSummaryFromTypeMetadataHandoff(handoff);
  handoff.property_synthesis_ivar_binding_summary =
      BuildPropertySynthesisIvarBindingSummaryFromTypeMetadataHandoff(handoff);
  handoff.id_class_sel_object_pointer_type_checking_summary =
      BuildIdClassSelObjectPointerTypeCheckingSummaryFromTypeMetadataHandoff(handoff);
  handoff.block_literal_capture_sites_lexicographic =
      surface.block_literal_capture_sites_lexicographic;
  handoff.block_literal_capture_semantics_summary =
      BuildBlockLiteralCaptureSemanticsSummaryFromTypeMetadataHandoff(handoff);
  handoff.block_abi_invoke_trampoline_sites_lexicographic =
      surface.block_abi_invoke_trampoline_sites_lexicographic;
  handoff.block_abi_invoke_trampoline_semantics_summary =
      BuildBlockAbiInvokeTrampolineSemanticsSummaryFromTypeMetadataHandoff(handoff);
  handoff.block_storage_escape_sites_lexicographic =
      surface.block_storage_escape_sites_lexicographic;
  handoff.block_storage_escape_semantics_summary =
      BuildBlockStorageEscapeSemanticsSummaryFromTypeMetadataHandoff(handoff);
  handoff.message_send_selector_lowering_sites_lexicographic =
      surface.message_send_selector_lowering_sites_lexicographic;
  handoff.message_send_selector_lowering_summary =
      BuildMessageSendSelectorLoweringSummaryFromTypeMetadataHandoff(handoff);
  handoff.dispatch_abi_marshalling_summary =
      BuildDispatchAbiMarshallingSummaryFromTypeMetadataHandoff(handoff);
  handoff.nil_receiver_semantics_foldability_summary =
      BuildNilReceiverSemanticsFoldabilitySummaryFromTypeMetadataHandoff(handoff);
  handoff.super_dispatch_method_family_summary =
      BuildSuperDispatchMethodFamilySummaryFromTypeMetadataHandoff(handoff);
  handoff.runtime_shim_host_link_summary =
      BuildRuntimeShimHostLinkSummaryFromTypeMetadataHandoff(handoff);
  handoff.retain_release_operation_summary =
      BuildRetainReleaseOperationSummaryFromTypeMetadataHandoff(handoff);
  handoff.weak_unowned_semantics_summary =
      BuildWeakUnownedSemanticsSummaryFromTypeMetadataHandoff(handoff);
  handoff.arc_diagnostics_fixit_summary =
      BuildArcDiagnosticsFixitSummaryFromTypeMetadataHandoff(handoff);
  handoff.autoreleasepool_scope_sites_lexicographic =
      surface.autoreleasepool_scope_sites_lexicographic;
  handoff.autoreleasepool_scope_summary =
      BuildAutoreleasePoolScopeSummaryFromTypeMetadataHandoff(handoff);
  return handoff;
}

bool IsDeterministicSemanticTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff) {
  if (!std::is_sorted(handoff.global_names_lexicographic.begin(), handoff.global_names_lexicographic.end())) {
    return false;
  }
  if (!std::is_sorted(handoff.functions_lexicographic.begin(), handoff.functions_lexicographic.end(),
                       [](const Objc3SemanticFunctionTypeMetadata &lhs, const Objc3SemanticFunctionTypeMetadata &rhs) {
                         return lhs.name < rhs.name;
                       })) {
    return false;
  }
  if (!std::is_sorted(handoff.interfaces_lexicographic.begin(), handoff.interfaces_lexicographic.end(),
                      [](const Objc3SemanticInterfaceTypeMetadata &lhs, const Objc3SemanticInterfaceTypeMetadata &rhs) {
                        return lhs.name < rhs.name;
                      })) {
    return false;
  }
  if (!std::is_sorted(
          handoff.implementations_lexicographic.begin(),
          handoff.implementations_lexicographic.end(),
          [](const Objc3SemanticImplementationTypeMetadata &lhs, const Objc3SemanticImplementationTypeMetadata &rhs) {
            return lhs.name < rhs.name;
          })) {
    return false;
  }
  if (!std::is_sorted(handoff.message_send_selector_lowering_sites_lexicographic.begin(),
                      handoff.message_send_selector_lowering_sites_lexicographic.end(),
                      IsMessageSendSelectorLoweringSiteMetadataLess)) {
    return false;
  }
  if (!std::is_sorted(handoff.block_literal_capture_sites_lexicographic.begin(),
                      handoff.block_literal_capture_sites_lexicographic.end(),
                      IsBlockLiteralCaptureSiteMetadataLess)) {
    return false;
  }
  if (!std::is_sorted(handoff.block_abi_invoke_trampoline_sites_lexicographic.begin(),
                      handoff.block_abi_invoke_trampoline_sites_lexicographic.end(),
                      IsBlockAbiInvokeTrampolineSiteMetadataLess)) {
    return false;
  }
  if (!std::is_sorted(handoff.block_storage_escape_sites_lexicographic.begin(),
                      handoff.block_storage_escape_sites_lexicographic.end(),
                      IsBlockStorageEscapeSiteMetadataLess)) {
    return false;
  }
  if (!std::is_sorted(handoff.autoreleasepool_scope_sites_lexicographic.begin(),
                      handoff.autoreleasepool_scope_sites_lexicographic.end(),
                      IsAutoreleasePoolScopeSiteMetadataLess)) {
    return false;
  }

  const auto is_deterministic_method_metadata = [](const Objc3SemanticMethodTypeMetadata &metadata) {
    if (metadata.selector.empty() ||
        metadata.selector_normalized.empty() ||
        metadata.selector != metadata.selector_normalized ||
        metadata.selector_parameter_piece_count > metadata.selector_piece_count ||
        metadata.selector_had_pieceless_form != (metadata.selector_piece_count == 0u) ||
        metadata.selector_has_arity_mismatch != (metadata.selector_parameter_piece_count != metadata.arity)) {
      return false;
    }
    if (metadata.selector_contract_normalized &&
        (metadata.selector_had_pieceless_form ||
         metadata.selector_has_spelling_mismatch ||
         metadata.selector_has_arity_mismatch ||
         metadata.selector_has_parameter_linkage_mismatch ||
         metadata.selector_has_normalization_flag_mismatch ||
         metadata.selector_has_missing_piece_keyword)) {
      return false;
    }
    if (metadata.selector_has_missing_piece_keyword && metadata.selector_contract_normalized) {
      return false;
    }
    if (metadata.param_types.size() != metadata.arity ||
        metadata.param_is_vector.size() != metadata.arity ||
        metadata.param_vector_base_spelling.size() != metadata.arity ||
        metadata.param_vector_lane_count.size() != metadata.arity ||
        metadata.param_has_generic_suffix.size() != metadata.arity ||
        metadata.param_has_pointer_declarator.size() != metadata.arity ||
        metadata.param_has_nullability_suffix.size() != metadata.arity ||
        metadata.param_has_ownership_qualifier.size() != metadata.arity ||
        metadata.param_object_pointer_type_spelling.size() != metadata.arity ||
        metadata.param_has_invalid_generic_suffix.size() != metadata.arity ||
        metadata.param_has_invalid_pointer_declarator.size() != metadata.arity ||
        metadata.param_has_invalid_nullability_suffix.size() != metadata.arity ||
        metadata.param_has_invalid_ownership_qualifier.size() != metadata.arity ||
        metadata.param_has_invalid_type_suffix.size() != metadata.arity ||
        metadata.param_ownership_insert_retain.size() != metadata.arity ||
        metadata.param_ownership_insert_release.size() != metadata.arity ||
        metadata.param_ownership_insert_autorelease.size() != metadata.arity ||
        metadata.param_ownership_arc_diagnostic_candidate.size() != metadata.arity ||
        metadata.param_ownership_arc_fixit_available.size() != metadata.arity ||
        metadata.param_ownership_arc_diagnostic_profile.size() != metadata.arity ||
        metadata.param_ownership_arc_fixit_hint.size() != metadata.arity ||
        metadata.param_has_protocol_composition.size() != metadata.arity ||
        metadata.param_protocol_composition_lexicographic.size() != metadata.arity ||
        metadata.param_has_invalid_protocol_composition.size() != metadata.arity) {
      return false;
    }
    if ((!metadata.return_ownership_arc_diagnostic_candidate &&
         (metadata.return_ownership_arc_fixit_available ||
          !metadata.return_ownership_arc_diagnostic_profile.empty() ||
          !metadata.return_ownership_arc_fixit_hint.empty())) ||
        (metadata.return_ownership_arc_fixit_available &&
         metadata.return_ownership_arc_fixit_hint.empty())) {
      return false;
    }
    if ((metadata.return_has_invalid_generic_suffix && !metadata.return_has_generic_suffix) ||
        (metadata.return_has_invalid_pointer_declarator && !metadata.return_has_pointer_declarator) ||
        (metadata.return_has_invalid_nullability_suffix && !metadata.return_has_nullability_suffix) ||
        (metadata.return_has_invalid_ownership_qualifier && !metadata.return_has_ownership_qualifier) ||
        (metadata.return_has_invalid_type_suffix !=
         (metadata.return_has_invalid_generic_suffix || metadata.return_has_invalid_pointer_declarator ||
          metadata.return_has_invalid_nullability_suffix || metadata.return_has_invalid_ownership_qualifier))) {
      return false;
    }
    if (metadata.return_has_invalid_protocol_composition && !metadata.return_has_protocol_composition) {
      return false;
    }
    if (metadata.return_has_protocol_composition &&
        !IsSortedUniqueStrings(metadata.return_protocol_composition_lexicographic)) {
      return false;
    }
    for (std::size_t i = 0; i < metadata.arity; ++i) {
      const bool expected_invalid = metadata.param_has_invalid_generic_suffix[i] ||
                                    metadata.param_has_invalid_pointer_declarator[i] ||
                                    metadata.param_has_invalid_nullability_suffix[i] ||
                                    metadata.param_has_invalid_ownership_qualifier[i];
      if ((metadata.param_has_invalid_generic_suffix[i] && !metadata.param_has_generic_suffix[i]) ||
          (metadata.param_has_invalid_pointer_declarator[i] && !metadata.param_has_pointer_declarator[i]) ||
          (metadata.param_has_invalid_nullability_suffix[i] && !metadata.param_has_nullability_suffix[i]) ||
          (metadata.param_has_invalid_ownership_qualifier[i] && !metadata.param_has_ownership_qualifier[i]) ||
          metadata.param_has_invalid_type_suffix[i] != expected_invalid) {
        return false;
      }
      if ((!metadata.param_has_ownership_qualifier[i] &&
           (metadata.param_ownership_insert_retain[i] || metadata.param_ownership_insert_release[i] ||
            metadata.param_ownership_insert_autorelease[i])) ||
          (metadata.param_ownership_insert_autorelease[i] &&
           (metadata.param_ownership_insert_retain[i] || metadata.param_ownership_insert_release[i]))) {
        return false;
      }
      if ((!metadata.param_ownership_arc_diagnostic_candidate[i] &&
           (metadata.param_ownership_arc_fixit_available[i] ||
            !metadata.param_ownership_arc_diagnostic_profile[i].empty() ||
            !metadata.param_ownership_arc_fixit_hint[i].empty())) ||
          (metadata.param_ownership_arc_fixit_available[i] &&
           metadata.param_ownership_arc_fixit_hint[i].empty())) {
        return false;
      }
      if (!IsSortedUniqueStrings(metadata.param_protocol_composition_lexicographic[i])) {
        return false;
      }
      if (metadata.param_has_invalid_protocol_composition[i] && !metadata.param_has_protocol_composition[i]) {
        return false;
      }
    }
    if ((!metadata.return_has_ownership_qualifier &&
         (metadata.return_ownership_insert_retain || metadata.return_ownership_insert_release ||
          metadata.return_ownership_insert_autorelease)) ||
        (metadata.return_ownership_insert_autorelease &&
         (metadata.return_ownership_insert_retain || metadata.return_ownership_insert_release))) {
      return false;
    }
    return true;
  };

  const bool deterministic_functions =
      std::all_of(handoff.functions_lexicographic.begin(),
                  handoff.functions_lexicographic.end(),
                  [](const Objc3SemanticFunctionTypeMetadata &metadata) {
                    if (metadata.param_types.size() != metadata.arity ||
                        metadata.param_is_vector.size() != metadata.arity ||
                        metadata.param_vector_base_spelling.size() != metadata.arity ||
                        metadata.param_vector_lane_count.size() != metadata.arity ||
                        metadata.param_has_generic_suffix.size() != metadata.arity ||
                        metadata.param_has_pointer_declarator.size() != metadata.arity ||
                        metadata.param_has_nullability_suffix.size() != metadata.arity ||
                        metadata.param_has_ownership_qualifier.size() != metadata.arity ||
                        metadata.param_object_pointer_type_spelling.size() != metadata.arity ||
                        metadata.param_has_invalid_generic_suffix.size() != metadata.arity ||
                        metadata.param_has_invalid_pointer_declarator.size() != metadata.arity ||
                        metadata.param_has_invalid_nullability_suffix.size() != metadata.arity ||
                        metadata.param_has_invalid_ownership_qualifier.size() != metadata.arity ||
                        metadata.param_has_invalid_type_suffix.size() != metadata.arity ||
                        metadata.param_ownership_insert_retain.size() != metadata.arity ||
                        metadata.param_ownership_insert_release.size() != metadata.arity ||
                        metadata.param_ownership_insert_autorelease.size() != metadata.arity ||
                        metadata.param_ownership_arc_diagnostic_candidate.size() != metadata.arity ||
                        metadata.param_ownership_arc_fixit_available.size() != metadata.arity ||
                        metadata.param_ownership_arc_diagnostic_profile.size() != metadata.arity ||
                        metadata.param_ownership_arc_fixit_hint.size() != metadata.arity ||
                        metadata.param_has_protocol_composition.size() != metadata.arity ||
                        metadata.param_protocol_composition_lexicographic.size() != metadata.arity ||
                        metadata.param_has_invalid_protocol_composition.size() != metadata.arity) {
                      return false;
                    }
                    if ((!metadata.return_ownership_arc_diagnostic_candidate &&
                         (metadata.return_ownership_arc_fixit_available ||
                          !metadata.return_ownership_arc_diagnostic_profile.empty() ||
                          !metadata.return_ownership_arc_fixit_hint.empty())) ||
                        (metadata.return_ownership_arc_fixit_available &&
                         metadata.return_ownership_arc_fixit_hint.empty())) {
                      return false;
                    }
                    if ((metadata.return_has_invalid_generic_suffix && !metadata.return_has_generic_suffix) ||
                        (metadata.return_has_invalid_pointer_declarator && !metadata.return_has_pointer_declarator) ||
                        (metadata.return_has_invalid_nullability_suffix && !metadata.return_has_nullability_suffix) ||
                        (metadata.return_has_invalid_ownership_qualifier && !metadata.return_has_ownership_qualifier) ||
                        (metadata.return_has_invalid_type_suffix !=
                         (metadata.return_has_invalid_generic_suffix || metadata.return_has_invalid_pointer_declarator ||
                          metadata.return_has_invalid_nullability_suffix ||
                          metadata.return_has_invalid_ownership_qualifier))) {
                      return false;
                    }
                    if (metadata.return_has_invalid_protocol_composition && !metadata.return_has_protocol_composition) {
                      return false;
                    }
                    if (metadata.return_has_protocol_composition &&
                        !IsSortedUniqueStrings(metadata.return_protocol_composition_lexicographic)) {
                      return false;
                    }
                    for (std::size_t i = 0; i < metadata.arity; ++i) {
                      const bool expected_invalid = metadata.param_has_invalid_generic_suffix[i] ||
                                                    metadata.param_has_invalid_pointer_declarator[i] ||
                                                    metadata.param_has_invalid_nullability_suffix[i] ||
                                                    metadata.param_has_invalid_ownership_qualifier[i];
                      if ((metadata.param_has_invalid_generic_suffix[i] && !metadata.param_has_generic_suffix[i]) ||
                          (metadata.param_has_invalid_pointer_declarator[i] && !metadata.param_has_pointer_declarator[i]) ||
                          (metadata.param_has_invalid_nullability_suffix[i] && !metadata.param_has_nullability_suffix[i]) ||
                          (metadata.param_has_invalid_ownership_qualifier[i] &&
                           !metadata.param_has_ownership_qualifier[i]) ||
                          metadata.param_has_invalid_type_suffix[i] != expected_invalid) {
                        return false;
                      }
                      if ((!metadata.param_has_ownership_qualifier[i] &&
                           (metadata.param_ownership_insert_retain[i] || metadata.param_ownership_insert_release[i] ||
                            metadata.param_ownership_insert_autorelease[i])) ||
                          (metadata.param_ownership_insert_autorelease[i] &&
                           (metadata.param_ownership_insert_retain[i] ||
                            metadata.param_ownership_insert_release[i]))) {
                        return false;
                      }
                      if ((!metadata.param_ownership_arc_diagnostic_candidate[i] &&
                           (metadata.param_ownership_arc_fixit_available[i] ||
                            !metadata.param_ownership_arc_diagnostic_profile[i].empty() ||
                            !metadata.param_ownership_arc_fixit_hint[i].empty())) ||
                          (metadata.param_ownership_arc_fixit_available[i] &&
                           metadata.param_ownership_arc_fixit_hint[i].empty())) {
                        return false;
                      }
                      if (!IsSortedUniqueStrings(metadata.param_protocol_composition_lexicographic[i])) {
                        return false;
                      }
                      if (metadata.param_has_invalid_protocol_composition[i] &&
                          !metadata.param_has_protocol_composition[i]) {
                        return false;
                      }
                    }
                    if ((!metadata.return_has_ownership_qualifier &&
                         (metadata.return_ownership_insert_retain || metadata.return_ownership_insert_release ||
                          metadata.return_ownership_insert_autorelease)) ||
                        (metadata.return_ownership_insert_autorelease &&
                         (metadata.return_ownership_insert_retain || metadata.return_ownership_insert_release))) {
                      return false;
                    }
                    return true;
                  });

  const bool deterministic_interfaces =
      std::all_of(handoff.interfaces_lexicographic.begin(),
                  handoff.interfaces_lexicographic.end(),
                  [&is_deterministic_method_metadata](const Objc3SemanticInterfaceTypeMetadata &metadata) {
                    return std::is_sorted(metadata.methods_lexicographic.begin(),
                                          metadata.methods_lexicographic.end(),
                                          [](const Objc3SemanticMethodTypeMetadata &lhs,
                                             const Objc3SemanticMethodTypeMetadata &rhs) {
                                            return lhs.selector < rhs.selector;
                                          }) &&
                           std::all_of(metadata.methods_lexicographic.begin(),
                                       metadata.methods_lexicographic.end(),
                                       is_deterministic_method_metadata);
                  });

  const bool deterministic_implementations =
      std::all_of(
          handoff.implementations_lexicographic.begin(),
          handoff.implementations_lexicographic.end(),
          [&is_deterministic_method_metadata](const Objc3SemanticImplementationTypeMetadata &metadata) {
            return std::is_sorted(metadata.methods_lexicographic.begin(),
                                  metadata.methods_lexicographic.end(),
                                  [](const Objc3SemanticMethodTypeMetadata &lhs,
                                     const Objc3SemanticMethodTypeMetadata &rhs) {
                                    return lhs.selector < rhs.selector;
                                  }) &&
                   std::all_of(metadata.methods_lexicographic.begin(),
                               metadata.methods_lexicographic.end(),
                               is_deterministic_method_metadata);
          });

  if (!deterministic_functions || !deterministic_interfaces || !deterministic_implementations) {
    return false;
  }

  Objc3ProtocolCategoryCompositionSummary protocol_category_summary;
  const auto accumulate_function_metadata_composition =
      [&protocol_category_summary](const Objc3SemanticFunctionTypeMetadata &metadata) {
        if (metadata.param_has_protocol_composition.size() != metadata.arity ||
            metadata.param_protocol_composition_lexicographic.size() != metadata.arity ||
            metadata.param_has_invalid_protocol_composition.size() != metadata.arity) {
          protocol_category_summary.deterministic = false;
          return;
        }
        for (std::size_t i = 0; i < metadata.arity; ++i) {
          AccumulateProtocolCompositionSite(metadata.param_has_protocol_composition[i],
                                           metadata.param_protocol_composition_lexicographic[i],
                                           metadata.param_has_invalid_protocol_composition[i],
                                           false,
                                           protocol_category_summary);
        }
        AccumulateProtocolCompositionSite(metadata.return_has_protocol_composition,
                                         metadata.return_protocol_composition_lexicographic,
                                         metadata.return_has_invalid_protocol_composition,
                                         false,
                                         protocol_category_summary);
      };
  const auto accumulate_method_metadata_composition =
      [&protocol_category_summary](const Objc3SemanticMethodTypeMetadata &metadata) {
        if (metadata.param_has_protocol_composition.size() != metadata.arity ||
            metadata.param_protocol_composition_lexicographic.size() != metadata.arity ||
            metadata.param_has_invalid_protocol_composition.size() != metadata.arity) {
          protocol_category_summary.deterministic = false;
          return;
        }
        for (std::size_t i = 0; i < metadata.arity; ++i) {
          AccumulateProtocolCompositionSite(metadata.param_has_protocol_composition[i],
                                           metadata.param_protocol_composition_lexicographic[i],
                                           metadata.param_has_invalid_protocol_composition[i],
                                           true,
                                           protocol_category_summary);
        }
        AccumulateProtocolCompositionSite(metadata.return_has_protocol_composition,
                                         metadata.return_protocol_composition_lexicographic,
                                         metadata.return_has_invalid_protocol_composition,
                                         true,
                                         protocol_category_summary);
      };
  for (const auto &metadata : handoff.functions_lexicographic) {
    accumulate_function_metadata_composition(metadata);
  }
  for (const auto &metadata : handoff.interfaces_lexicographic) {
    for (const auto &method : metadata.methods_lexicographic) {
      accumulate_method_metadata_composition(method);
    }
  }
  for (const auto &metadata : handoff.implementations_lexicographic) {
    for (const auto &method : metadata.methods_lexicographic) {
      accumulate_method_metadata_composition(method);
    }
  }
  protocol_category_summary.deterministic =
      protocol_category_summary.deterministic &&
      protocol_category_summary.invalid_protocol_composition_sites <=
          protocol_category_summary.total_composition_sites() &&
      protocol_category_summary.category_composition_sites <= protocol_category_summary.protocol_composition_sites &&
      protocol_category_summary.category_composition_symbols <= protocol_category_summary.protocol_composition_symbols;

  Objc3SelectorNormalizationSummary selector_summary;
  const auto accumulate_selector_summary = [&selector_summary](const Objc3SemanticMethodTypeMetadata &metadata) {
    ++selector_summary.methods_total;
    if (metadata.selector_contract_normalized) {
      ++selector_summary.normalized_methods;
    }
    selector_summary.selector_piece_entries += metadata.selector_piece_count;
    selector_summary.selector_parameter_piece_entries += metadata.selector_parameter_piece_count;
    if (metadata.selector_had_pieceless_form) {
      ++selector_summary.selector_pieceless_methods;
    }
    if (metadata.selector_has_spelling_mismatch) {
      ++selector_summary.selector_spelling_mismatches;
    }
    if (metadata.selector_has_arity_mismatch) {
      ++selector_summary.selector_arity_mismatches;
    }
    if (metadata.selector_has_parameter_linkage_mismatch) {
      ++selector_summary.selector_parameter_linkage_mismatches;
    }
    if (metadata.selector_has_normalization_flag_mismatch) {
      ++selector_summary.selector_normalization_flag_mismatches;
    }
    if (metadata.selector_has_missing_piece_keyword) {
      ++selector_summary.selector_missing_keyword_pieces;
    }
    if (metadata.selector.empty() ||
        metadata.selector_normalized.empty() ||
        metadata.selector != metadata.selector_normalized ||
        metadata.selector_parameter_piece_count > metadata.selector_piece_count) {
      selector_summary.deterministic = false;
    }
  };
  for (const auto &metadata : handoff.interfaces_lexicographic) {
    for (const auto &method : metadata.methods_lexicographic) {
      accumulate_selector_summary(method);
    }
  }
  for (const auto &metadata : handoff.implementations_lexicographic) {
    for (const auto &method : metadata.methods_lexicographic) {
      accumulate_selector_summary(method);
    }
  }
  selector_summary.deterministic =
      selector_summary.deterministic &&
      selector_summary.normalized_methods <= selector_summary.methods_total &&
      selector_summary.selector_parameter_piece_entries <= selector_summary.selector_piece_entries &&
      selector_summary.contract_violations() <= selector_summary.methods_total;

  Objc3TypeAnnotationSurfaceSummary type_annotation_summary;
  const auto accumulate_function_type_annotations = [&type_annotation_summary](
                                                        const Objc3SemanticFunctionTypeMetadata &metadata) {
    if (metadata.param_has_generic_suffix.size() != metadata.arity ||
        metadata.param_has_pointer_declarator.size() != metadata.arity ||
        metadata.param_has_nullability_suffix.size() != metadata.arity ||
        metadata.param_has_ownership_qualifier.size() != metadata.arity ||
        metadata.param_object_pointer_type_spelling.size() != metadata.arity ||
        metadata.param_has_invalid_generic_suffix.size() != metadata.arity ||
        metadata.param_has_invalid_pointer_declarator.size() != metadata.arity ||
        metadata.param_has_invalid_nullability_suffix.size() != metadata.arity ||
        metadata.param_has_invalid_ownership_qualifier.size() != metadata.arity ||
        metadata.param_has_invalid_type_suffix.size() != metadata.arity) {
      type_annotation_summary.deterministic = false;
      return;
    }
    for (std::size_t i = 0; i < metadata.arity; ++i) {
      if (metadata.param_has_generic_suffix[i]) {
        ++type_annotation_summary.generic_suffix_sites;
      }
      if (metadata.param_has_pointer_declarator[i]) {
        ++type_annotation_summary.pointer_declarator_sites;
      }
      if (metadata.param_has_nullability_suffix[i]) {
        ++type_annotation_summary.nullability_suffix_sites;
      }
      if (metadata.param_has_ownership_qualifier[i]) {
        ++type_annotation_summary.ownership_qualifier_sites;
      }
      if (metadata.param_object_pointer_type_spelling[i]) {
        ++type_annotation_summary.object_pointer_type_sites;
      }
      if (metadata.param_has_invalid_generic_suffix[i]) {
        ++type_annotation_summary.invalid_generic_suffix_sites;
      }
      if (metadata.param_has_invalid_pointer_declarator[i]) {
        ++type_annotation_summary.invalid_pointer_declarator_sites;
      }
      if (metadata.param_has_invalid_nullability_suffix[i]) {
        ++type_annotation_summary.invalid_nullability_suffix_sites;
      }
      if (metadata.param_has_invalid_ownership_qualifier[i]) {
        ++type_annotation_summary.invalid_ownership_qualifier_sites;
      }
      const bool expected_invalid = metadata.param_has_invalid_generic_suffix[i] ||
                                    metadata.param_has_invalid_pointer_declarator[i] ||
                                    metadata.param_has_invalid_nullability_suffix[i] ||
                                    metadata.param_has_invalid_ownership_qualifier[i];
      if (metadata.param_has_invalid_type_suffix[i] != expected_invalid) {
        type_annotation_summary.deterministic = false;
      }
    }
    if (metadata.return_has_generic_suffix) {
      ++type_annotation_summary.generic_suffix_sites;
    }
    if (metadata.return_has_pointer_declarator) {
      ++type_annotation_summary.pointer_declarator_sites;
    }
    if (metadata.return_has_nullability_suffix) {
      ++type_annotation_summary.nullability_suffix_sites;
    }
    if (metadata.return_has_ownership_qualifier) {
      ++type_annotation_summary.ownership_qualifier_sites;
    }
    if (metadata.return_object_pointer_type_spelling) {
      ++type_annotation_summary.object_pointer_type_sites;
    }
    if (metadata.return_has_invalid_generic_suffix) {
      ++type_annotation_summary.invalid_generic_suffix_sites;
    }
    if (metadata.return_has_invalid_pointer_declarator) {
      ++type_annotation_summary.invalid_pointer_declarator_sites;
    }
    if (metadata.return_has_invalid_nullability_suffix) {
      ++type_annotation_summary.invalid_nullability_suffix_sites;
    }
    if (metadata.return_has_invalid_ownership_qualifier) {
      ++type_annotation_summary.invalid_ownership_qualifier_sites;
    }
    const bool expected_return_invalid = metadata.return_has_invalid_generic_suffix ||
                                         metadata.return_has_invalid_pointer_declarator ||
                                         metadata.return_has_invalid_nullability_suffix ||
                                         metadata.return_has_invalid_ownership_qualifier;
    if (metadata.return_has_invalid_type_suffix != expected_return_invalid) {
      type_annotation_summary.deterministic = false;
    }
  };
  const auto accumulate_method_type_annotations = [&type_annotation_summary](
                                                      const Objc3SemanticMethodTypeMetadata &metadata) {
    if (metadata.param_has_generic_suffix.size() != metadata.arity ||
        metadata.param_has_pointer_declarator.size() != metadata.arity ||
        metadata.param_has_nullability_suffix.size() != metadata.arity ||
        metadata.param_has_ownership_qualifier.size() != metadata.arity ||
        metadata.param_object_pointer_type_spelling.size() != metadata.arity ||
        metadata.param_has_invalid_generic_suffix.size() != metadata.arity ||
        metadata.param_has_invalid_pointer_declarator.size() != metadata.arity ||
        metadata.param_has_invalid_nullability_suffix.size() != metadata.arity ||
        metadata.param_has_invalid_ownership_qualifier.size() != metadata.arity ||
        metadata.param_has_invalid_type_suffix.size() != metadata.arity) {
      type_annotation_summary.deterministic = false;
      return;
    }
    for (std::size_t i = 0; i < metadata.arity; ++i) {
      if (metadata.param_has_generic_suffix[i]) {
        ++type_annotation_summary.generic_suffix_sites;
      }
      if (metadata.param_has_pointer_declarator[i]) {
        ++type_annotation_summary.pointer_declarator_sites;
      }
      if (metadata.param_has_nullability_suffix[i]) {
        ++type_annotation_summary.nullability_suffix_sites;
      }
      if (metadata.param_has_ownership_qualifier[i]) {
        ++type_annotation_summary.ownership_qualifier_sites;
      }
      if (metadata.param_object_pointer_type_spelling[i]) {
        ++type_annotation_summary.object_pointer_type_sites;
      }
      if (metadata.param_has_invalid_generic_suffix[i]) {
        ++type_annotation_summary.invalid_generic_suffix_sites;
      }
      if (metadata.param_has_invalid_pointer_declarator[i]) {
        ++type_annotation_summary.invalid_pointer_declarator_sites;
      }
      if (metadata.param_has_invalid_nullability_suffix[i]) {
        ++type_annotation_summary.invalid_nullability_suffix_sites;
      }
      if (metadata.param_has_invalid_ownership_qualifier[i]) {
        ++type_annotation_summary.invalid_ownership_qualifier_sites;
      }
      const bool expected_invalid = metadata.param_has_invalid_generic_suffix[i] ||
                                    metadata.param_has_invalid_pointer_declarator[i] ||
                                    metadata.param_has_invalid_nullability_suffix[i] ||
                                    metadata.param_has_invalid_ownership_qualifier[i];
      if (metadata.param_has_invalid_type_suffix[i] != expected_invalid) {
        type_annotation_summary.deterministic = false;
      }
    }
    if (metadata.return_has_generic_suffix) {
      ++type_annotation_summary.generic_suffix_sites;
    }
    if (metadata.return_has_pointer_declarator) {
      ++type_annotation_summary.pointer_declarator_sites;
    }
    if (metadata.return_has_nullability_suffix) {
      ++type_annotation_summary.nullability_suffix_sites;
    }
    if (metadata.return_has_ownership_qualifier) {
      ++type_annotation_summary.ownership_qualifier_sites;
    }
    if (metadata.return_object_pointer_type_spelling) {
      ++type_annotation_summary.object_pointer_type_sites;
    }
    if (metadata.return_has_invalid_generic_suffix) {
      ++type_annotation_summary.invalid_generic_suffix_sites;
    }
    if (metadata.return_has_invalid_pointer_declarator) {
      ++type_annotation_summary.invalid_pointer_declarator_sites;
    }
    if (metadata.return_has_invalid_nullability_suffix) {
      ++type_annotation_summary.invalid_nullability_suffix_sites;
    }
    if (metadata.return_has_invalid_ownership_qualifier) {
      ++type_annotation_summary.invalid_ownership_qualifier_sites;
    }
    const bool expected_return_invalid = metadata.return_has_invalid_generic_suffix ||
                                         metadata.return_has_invalid_pointer_declarator ||
                                         metadata.return_has_invalid_nullability_suffix ||
                                         metadata.return_has_invalid_ownership_qualifier;
    if (metadata.return_has_invalid_type_suffix != expected_return_invalid) {
      type_annotation_summary.deterministic = false;
    }
  };
  const auto accumulate_property_type_annotations = [&type_annotation_summary](
                                                        const Objc3SemanticPropertyTypeMetadata &metadata) {
    if (metadata.has_generic_suffix) {
      ++type_annotation_summary.generic_suffix_sites;
    }
    if (metadata.has_pointer_declarator) {
      ++type_annotation_summary.pointer_declarator_sites;
    }
    if (metadata.has_nullability_suffix) {
      ++type_annotation_summary.nullability_suffix_sites;
    }
    if (metadata.has_ownership_qualifier) {
      ++type_annotation_summary.ownership_qualifier_sites;
    }
    if (metadata.object_pointer_type_spelling) {
      ++type_annotation_summary.object_pointer_type_sites;
    }
    if (metadata.has_invalid_generic_suffix) {
      ++type_annotation_summary.invalid_generic_suffix_sites;
    }
    if (metadata.has_invalid_pointer_declarator) {
      ++type_annotation_summary.invalid_pointer_declarator_sites;
    }
    if (metadata.has_invalid_nullability_suffix) {
      ++type_annotation_summary.invalid_nullability_suffix_sites;
    }
    if (metadata.has_invalid_ownership_qualifier) {
      ++type_annotation_summary.invalid_ownership_qualifier_sites;
    }
    const bool expected_invalid = metadata.has_invalid_generic_suffix ||
                                  metadata.has_invalid_pointer_declarator ||
                                  metadata.has_invalid_nullability_suffix ||
                                  metadata.has_invalid_ownership_qualifier;
    if (metadata.has_invalid_type_suffix != expected_invalid) {
      type_annotation_summary.deterministic = false;
    }
  };
  for (const auto &metadata : handoff.functions_lexicographic) {
    accumulate_function_type_annotations(metadata);
  }
  for (const auto &metadata : handoff.interfaces_lexicographic) {
    for (const auto &method : metadata.methods_lexicographic) {
      accumulate_method_type_annotations(method);
    }
    for (const auto &property : metadata.properties_lexicographic) {
      accumulate_property_type_annotations(property);
    }
  }
  for (const auto &metadata : handoff.implementations_lexicographic) {
    for (const auto &method : metadata.methods_lexicographic) {
      accumulate_method_type_annotations(method);
    }
    for (const auto &property : metadata.properties_lexicographic) {
      accumulate_property_type_annotations(property);
    }
  }
  type_annotation_summary.deterministic =
      type_annotation_summary.deterministic &&
      type_annotation_summary.invalid_generic_suffix_sites <= type_annotation_summary.generic_suffix_sites &&
      type_annotation_summary.invalid_pointer_declarator_sites <= type_annotation_summary.pointer_declarator_sites &&
      type_annotation_summary.invalid_nullability_suffix_sites <= type_annotation_summary.nullability_suffix_sites &&
      type_annotation_summary.invalid_ownership_qualifier_sites <= type_annotation_summary.ownership_qualifier_sites &&
      type_annotation_summary.invalid_type_annotation_sites() <= type_annotation_summary.total_type_annotation_sites();

  std::size_t interface_method_symbols = 0;
  for (const auto &metadata : handoff.interfaces_lexicographic) {
    interface_method_symbols += metadata.methods_lexicographic.size();
  }
  std::size_t implementation_method_symbols = 0;
  for (const auto &metadata : handoff.implementations_lexicographic) {
    implementation_method_symbols += metadata.methods_lexicographic.size();
  }
  const Objc3SymbolGraphScopeResolutionSummary symbol_graph_scope_summary =
      BuildSymbolGraphScopeResolutionSummaryFromTypeMetadataHandoff(handoff);
  const Objc3MethodLookupOverrideConflictSummary method_lookup_override_conflict_summary =
      BuildMethodLookupOverrideConflictSummaryFromTypeMetadataHandoff(handoff);
  const Objc3PropertySynthesisIvarBindingSummary property_synthesis_ivar_binding_summary =
      BuildPropertySynthesisIvarBindingSummaryFromTypeMetadataHandoff(handoff);
  const Objc3IdClassSelObjectPointerTypeCheckingSummary id_class_sel_object_pointer_type_checking_summary =
      BuildIdClassSelObjectPointerTypeCheckingSummaryFromTypeMetadataHandoff(handoff);
  const Objc3BlockLiteralCaptureSemanticsSummary block_literal_capture_semantics_summary =
      BuildBlockLiteralCaptureSemanticsSummaryFromTypeMetadataHandoff(handoff);
  const Objc3BlockAbiInvokeTrampolineSemanticsSummary block_abi_invoke_trampoline_semantics_summary =
      BuildBlockAbiInvokeTrampolineSemanticsSummaryFromTypeMetadataHandoff(handoff);
  const Objc3BlockStorageEscapeSemanticsSummary block_storage_escape_semantics_summary =
      BuildBlockStorageEscapeSemanticsSummaryFromTypeMetadataHandoff(handoff);
  const Objc3MessageSendSelectorLoweringSummary message_send_selector_lowering_summary =
      BuildMessageSendSelectorLoweringSummaryFromTypeMetadataHandoff(handoff);
  const Objc3DispatchAbiMarshallingSummary dispatch_abi_marshalling_summary =
      BuildDispatchAbiMarshallingSummaryFromTypeMetadataHandoff(handoff);
  const Objc3NilReceiverSemanticsFoldabilitySummary nil_receiver_semantics_foldability_summary =
      BuildNilReceiverSemanticsFoldabilitySummaryFromTypeMetadataHandoff(handoff);
  const Objc3SuperDispatchMethodFamilySummary super_dispatch_method_family_summary =
      BuildSuperDispatchMethodFamilySummaryFromTypeMetadataHandoff(handoff);
  const Objc3RuntimeShimHostLinkSummary runtime_shim_host_link_summary =
      BuildRuntimeShimHostLinkSummaryFromTypeMetadataHandoff(handoff);
  const Objc3RetainReleaseOperationSummary retain_release_operation_summary =
      BuildRetainReleaseOperationSummaryFromTypeMetadataHandoff(handoff);
  const Objc3WeakUnownedSemanticsSummary weak_unowned_semantics_summary =
      BuildWeakUnownedSemanticsSummaryFromTypeMetadataHandoff(handoff);
  const Objc3ArcDiagnosticsFixitSummary arc_diagnostics_fixit_summary =
      BuildArcDiagnosticsFixitSummaryFromTypeMetadataHandoff(handoff);
  const Objc3AutoreleasePoolScopeSummary autoreleasepool_scope_summary =
      BuildAutoreleasePoolScopeSummaryFromTypeMetadataHandoff(handoff);

  const Objc3InterfaceImplementationSummary &summary = handoff.interface_implementation_summary;
  const Objc3ClassProtocolCategoryLinkingSummary class_protocol_category_linking_summary =
      BuildClassProtocolCategoryLinkingSummary(summary, protocol_category_summary);
  return summary.deterministic &&
         summary.resolved_interfaces == handoff.interfaces_lexicographic.size() &&
         summary.resolved_implementations == handoff.implementations_lexicographic.size() &&
         summary.interface_method_symbols == interface_method_symbols &&
         summary.implementation_method_symbols == implementation_method_symbols &&
         summary.linked_implementation_symbols <= summary.implementation_method_symbols &&
         summary.linked_implementation_symbols <= summary.interface_method_symbols &&
         handoff.class_protocol_category_linking_summary.deterministic &&
         handoff.class_protocol_category_linking_summary.declared_interfaces ==
             class_protocol_category_linking_summary.declared_interfaces &&
         handoff.class_protocol_category_linking_summary.resolved_interfaces ==
             class_protocol_category_linking_summary.resolved_interfaces &&
         handoff.class_protocol_category_linking_summary.declared_implementations ==
             class_protocol_category_linking_summary.declared_implementations &&
         handoff.class_protocol_category_linking_summary.resolved_implementations ==
             class_protocol_category_linking_summary.resolved_implementations &&
         handoff.class_protocol_category_linking_summary.interface_method_symbols ==
             class_protocol_category_linking_summary.interface_method_symbols &&
         handoff.class_protocol_category_linking_summary.implementation_method_symbols ==
             class_protocol_category_linking_summary.implementation_method_symbols &&
         handoff.class_protocol_category_linking_summary.linked_implementation_symbols ==
             class_protocol_category_linking_summary.linked_implementation_symbols &&
         handoff.class_protocol_category_linking_summary.protocol_composition_sites ==
             class_protocol_category_linking_summary.protocol_composition_sites &&
         handoff.class_protocol_category_linking_summary.protocol_composition_symbols ==
             class_protocol_category_linking_summary.protocol_composition_symbols &&
         handoff.class_protocol_category_linking_summary.category_composition_sites ==
             class_protocol_category_linking_summary.category_composition_sites &&
         handoff.class_protocol_category_linking_summary.category_composition_symbols ==
             class_protocol_category_linking_summary.category_composition_symbols &&
         handoff.class_protocol_category_linking_summary.invalid_protocol_composition_sites ==
             class_protocol_category_linking_summary.invalid_protocol_composition_sites &&
         handoff.protocol_category_composition_summary.deterministic &&
         handoff.protocol_category_composition_summary.protocol_composition_sites ==
             protocol_category_summary.protocol_composition_sites &&
         handoff.protocol_category_composition_summary.protocol_composition_symbols ==
             protocol_category_summary.protocol_composition_symbols &&
         handoff.protocol_category_composition_summary.category_composition_sites ==
             protocol_category_summary.category_composition_sites &&
         handoff.protocol_category_composition_summary.category_composition_symbols ==
              protocol_category_summary.category_composition_symbols &&
         handoff.protocol_category_composition_summary.invalid_protocol_composition_sites ==
              protocol_category_summary.invalid_protocol_composition_sites &&
         handoff.selector_normalization_summary.deterministic &&
         handoff.selector_normalization_summary.methods_total == selector_summary.methods_total &&
         handoff.selector_normalization_summary.normalized_methods == selector_summary.normalized_methods &&
         handoff.selector_normalization_summary.selector_piece_entries == selector_summary.selector_piece_entries &&
         handoff.selector_normalization_summary.selector_parameter_piece_entries ==
             selector_summary.selector_parameter_piece_entries &&
         handoff.selector_normalization_summary.selector_pieceless_methods ==
             selector_summary.selector_pieceless_methods &&
         handoff.selector_normalization_summary.selector_spelling_mismatches ==
             selector_summary.selector_spelling_mismatches &&
         handoff.selector_normalization_summary.selector_arity_mismatches ==
             selector_summary.selector_arity_mismatches &&
         handoff.selector_normalization_summary.selector_parameter_linkage_mismatches ==
             selector_summary.selector_parameter_linkage_mismatches &&
         handoff.selector_normalization_summary.selector_normalization_flag_mismatches ==
             selector_summary.selector_normalization_flag_mismatches &&
         handoff.selector_normalization_summary.selector_missing_keyword_pieces ==
             selector_summary.selector_missing_keyword_pieces &&
         handoff.type_annotation_surface_summary.deterministic &&
         handoff.type_annotation_surface_summary.generic_suffix_sites == type_annotation_summary.generic_suffix_sites &&
         handoff.type_annotation_surface_summary.pointer_declarator_sites ==
             type_annotation_summary.pointer_declarator_sites &&
         handoff.type_annotation_surface_summary.nullability_suffix_sites ==
             type_annotation_summary.nullability_suffix_sites &&
         handoff.type_annotation_surface_summary.ownership_qualifier_sites ==
             type_annotation_summary.ownership_qualifier_sites &&
         handoff.type_annotation_surface_summary.object_pointer_type_sites ==
             type_annotation_summary.object_pointer_type_sites &&
         handoff.type_annotation_surface_summary.invalid_generic_suffix_sites ==
             type_annotation_summary.invalid_generic_suffix_sites &&
         handoff.type_annotation_surface_summary.invalid_pointer_declarator_sites ==
             type_annotation_summary.invalid_pointer_declarator_sites &&
         handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites ==
             type_annotation_summary.invalid_nullability_suffix_sites &&
         handoff.type_annotation_surface_summary.invalid_ownership_qualifier_sites ==
             type_annotation_summary.invalid_ownership_qualifier_sites &&
         handoff.symbol_graph_scope_resolution_summary.deterministic &&
         handoff.symbol_graph_scope_resolution_summary.global_symbol_nodes ==
             symbol_graph_scope_summary.global_symbol_nodes &&
         handoff.symbol_graph_scope_resolution_summary.function_symbol_nodes ==
             symbol_graph_scope_summary.function_symbol_nodes &&
         handoff.symbol_graph_scope_resolution_summary.interface_symbol_nodes ==
             symbol_graph_scope_summary.interface_symbol_nodes &&
         handoff.symbol_graph_scope_resolution_summary.implementation_symbol_nodes ==
             symbol_graph_scope_summary.implementation_symbol_nodes &&
         handoff.symbol_graph_scope_resolution_summary.interface_property_symbol_nodes ==
             symbol_graph_scope_summary.interface_property_symbol_nodes &&
         handoff.symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes ==
             symbol_graph_scope_summary.implementation_property_symbol_nodes &&
         handoff.symbol_graph_scope_resolution_summary.interface_method_symbol_nodes ==
             symbol_graph_scope_summary.interface_method_symbol_nodes &&
         handoff.symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes ==
             symbol_graph_scope_summary.implementation_method_symbol_nodes &&
         handoff.symbol_graph_scope_resolution_summary.top_level_scope_symbols ==
             symbol_graph_scope_summary.top_level_scope_symbols &&
         handoff.symbol_graph_scope_resolution_summary.nested_scope_symbols ==
             symbol_graph_scope_summary.nested_scope_symbols &&
         handoff.symbol_graph_scope_resolution_summary.scope_frames_total ==
             symbol_graph_scope_summary.scope_frames_total &&
         handoff.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites ==
             symbol_graph_scope_summary.implementation_interface_resolution_sites &&
         handoff.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits ==
             symbol_graph_scope_summary.implementation_interface_resolution_hits &&
         handoff.symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses ==
             symbol_graph_scope_summary.implementation_interface_resolution_misses &&
         handoff.symbol_graph_scope_resolution_summary.method_resolution_sites ==
             symbol_graph_scope_summary.method_resolution_sites &&
         handoff.symbol_graph_scope_resolution_summary.method_resolution_hits ==
             symbol_graph_scope_summary.method_resolution_hits &&
         handoff.symbol_graph_scope_resolution_summary.method_resolution_misses ==
             symbol_graph_scope_summary.method_resolution_misses &&
         handoff.symbol_graph_scope_resolution_summary.symbol_nodes_total() ==
             symbol_graph_scope_summary.symbol_nodes_total() &&
         handoff.symbol_graph_scope_resolution_summary.resolution_hits_total() ==
             symbol_graph_scope_summary.resolution_hits_total() &&
         handoff.symbol_graph_scope_resolution_summary.resolution_misses_total() ==
             symbol_graph_scope_summary.resolution_misses_total() &&
         handoff.method_lookup_override_conflict_summary.deterministic &&
         handoff.method_lookup_override_conflict_summary.method_lookup_sites ==
             method_lookup_override_conflict_summary.method_lookup_sites &&
         handoff.method_lookup_override_conflict_summary.method_lookup_hits ==
             method_lookup_override_conflict_summary.method_lookup_hits &&
         handoff.method_lookup_override_conflict_summary.method_lookup_misses ==
             method_lookup_override_conflict_summary.method_lookup_misses &&
         handoff.method_lookup_override_conflict_summary.override_lookup_sites ==
             method_lookup_override_conflict_summary.override_lookup_sites &&
         handoff.method_lookup_override_conflict_summary.override_lookup_hits ==
             method_lookup_override_conflict_summary.override_lookup_hits &&
         handoff.method_lookup_override_conflict_summary.override_lookup_misses ==
             method_lookup_override_conflict_summary.override_lookup_misses &&
         handoff.method_lookup_override_conflict_summary.override_conflicts ==
             method_lookup_override_conflict_summary.override_conflicts &&
         handoff.method_lookup_override_conflict_summary.unresolved_base_interfaces ==
             method_lookup_override_conflict_summary.unresolved_base_interfaces &&
         handoff.method_lookup_override_conflict_summary.method_lookup_hits <=
             handoff.method_lookup_override_conflict_summary.method_lookup_sites &&
         handoff.method_lookup_override_conflict_summary.method_lookup_hits +
                 handoff.method_lookup_override_conflict_summary.method_lookup_misses ==
             handoff.method_lookup_override_conflict_summary.method_lookup_sites &&
         handoff.method_lookup_override_conflict_summary.override_lookup_hits <=
             handoff.method_lookup_override_conflict_summary.override_lookup_sites &&
         handoff.method_lookup_override_conflict_summary.override_lookup_hits +
                 handoff.method_lookup_override_conflict_summary.override_lookup_misses ==
             handoff.method_lookup_override_conflict_summary.override_lookup_sites &&
         handoff.method_lookup_override_conflict_summary.override_conflicts <=
             handoff.method_lookup_override_conflict_summary.override_lookup_hits &&
         handoff.property_synthesis_ivar_binding_summary.deterministic &&
         handoff.property_synthesis_ivar_binding_summary.property_synthesis_sites ==
             property_synthesis_ivar_binding_summary.property_synthesis_sites &&
         handoff.property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings ==
             property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings &&
         handoff.property_synthesis_ivar_binding_summary.property_synthesis_default_ivar_bindings ==
             property_synthesis_ivar_binding_summary.property_synthesis_default_ivar_bindings &&
         handoff.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
             property_synthesis_ivar_binding_summary.ivar_binding_sites &&
         handoff.property_synthesis_ivar_binding_summary.ivar_binding_resolved ==
             property_synthesis_ivar_binding_summary.ivar_binding_resolved &&
         handoff.property_synthesis_ivar_binding_summary.ivar_binding_missing ==
             property_synthesis_ivar_binding_summary.ivar_binding_missing &&
         handoff.property_synthesis_ivar_binding_summary.ivar_binding_conflicts ==
             property_synthesis_ivar_binding_summary.ivar_binding_conflicts &&
         handoff.property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings +
                 handoff.property_synthesis_ivar_binding_summary.property_synthesis_default_ivar_bindings ==
             handoff.property_synthesis_ivar_binding_summary.property_synthesis_sites &&
         handoff.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
             handoff.property_synthesis_ivar_binding_summary.property_synthesis_sites &&
         handoff.property_synthesis_ivar_binding_summary.ivar_binding_resolved +
                 handoff.property_synthesis_ivar_binding_summary.ivar_binding_missing +
                 handoff.property_synthesis_ivar_binding_summary.ivar_binding_conflicts ==
             handoff.property_synthesis_ivar_binding_summary.ivar_binding_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.deterministic &&
         handoff.id_class_sel_object_pointer_type_checking_summary.param_type_sites ==
             id_class_sel_object_pointer_type_checking_summary.param_type_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.param_id_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.param_id_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.param_class_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.param_class_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.param_sel_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.param_sel_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.param_instancetype_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.param_instancetype_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.param_object_pointer_type_sites ==
             id_class_sel_object_pointer_type_checking_summary.param_object_pointer_type_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.return_type_sites ==
             id_class_sel_object_pointer_type_checking_summary.return_type_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.return_id_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.return_id_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.return_class_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.return_class_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.return_sel_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.return_sel_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.return_instancetype_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.return_instancetype_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.return_object_pointer_type_sites ==
             id_class_sel_object_pointer_type_checking_summary.return_object_pointer_type_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.property_type_sites ==
             id_class_sel_object_pointer_type_checking_summary.property_type_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.property_id_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.property_id_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.property_class_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.property_class_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.property_sel_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.property_sel_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.property_instancetype_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.property_instancetype_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.property_object_pointer_type_sites ==
             id_class_sel_object_pointer_type_checking_summary.property_object_pointer_type_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.param_id_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.param_class_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.param_sel_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.param_instancetype_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.param_object_pointer_type_sites <=
             handoff.id_class_sel_object_pointer_type_checking_summary.param_type_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.return_id_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.return_class_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.return_sel_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.return_instancetype_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.return_object_pointer_type_sites <=
             handoff.id_class_sel_object_pointer_type_checking_summary.return_type_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.property_id_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.property_class_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.property_sel_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.property_instancetype_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.property_object_pointer_type_sites <=
             handoff.id_class_sel_object_pointer_type_checking_summary.property_type_sites &&
         handoff.block_literal_capture_semantics_summary.deterministic &&
         handoff.block_literal_capture_semantics_summary.block_literal_sites ==
             block_literal_capture_semantics_summary.block_literal_sites &&
         handoff.block_literal_capture_semantics_summary.block_parameter_entries ==
             block_literal_capture_semantics_summary.block_parameter_entries &&
         handoff.block_literal_capture_semantics_summary.block_capture_entries ==
             block_literal_capture_semantics_summary.block_capture_entries &&
         handoff.block_literal_capture_semantics_summary.block_body_statement_entries ==
             block_literal_capture_semantics_summary.block_body_statement_entries &&
         handoff.block_literal_capture_semantics_summary.block_empty_capture_sites ==
             block_literal_capture_semantics_summary.block_empty_capture_sites &&
         handoff.block_literal_capture_semantics_summary.block_nondeterministic_capture_sites ==
             block_literal_capture_semantics_summary.block_nondeterministic_capture_sites &&
         handoff.block_literal_capture_semantics_summary.block_non_normalized_sites ==
             block_literal_capture_semantics_summary.block_non_normalized_sites &&
         handoff.block_literal_capture_semantics_summary.contract_violation_sites ==
             block_literal_capture_semantics_summary.contract_violation_sites &&
         handoff.block_literal_capture_semantics_summary.block_empty_capture_sites <=
             handoff.block_literal_capture_semantics_summary.block_literal_sites &&
         handoff.block_literal_capture_semantics_summary.block_nondeterministic_capture_sites <=
             handoff.block_literal_capture_semantics_summary.block_literal_sites &&
         handoff.block_literal_capture_semantics_summary.block_non_normalized_sites <=
             handoff.block_literal_capture_semantics_summary.block_literal_sites &&
         handoff.block_literal_capture_semantics_summary.contract_violation_sites <=
             handoff.block_literal_capture_semantics_summary.block_literal_sites &&
         handoff.block_abi_invoke_trampoline_semantics_summary.deterministic &&
         handoff.block_abi_invoke_trampoline_semantics_summary.block_literal_sites ==
             block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
         handoff.block_abi_invoke_trampoline_semantics_summary.invoke_argument_slots_total ==
             block_abi_invoke_trampoline_semantics_summary.invoke_argument_slots_total &&
         handoff.block_abi_invoke_trampoline_semantics_summary.capture_word_count_total ==
             block_abi_invoke_trampoline_semantics_summary.capture_word_count_total &&
         handoff.block_abi_invoke_trampoline_semantics_summary.parameter_entries_total ==
             block_abi_invoke_trampoline_semantics_summary.parameter_entries_total &&
         handoff.block_abi_invoke_trampoline_semantics_summary.capture_entries_total ==
             block_abi_invoke_trampoline_semantics_summary.capture_entries_total &&
         handoff.block_abi_invoke_trampoline_semantics_summary.body_statement_entries_total ==
             block_abi_invoke_trampoline_semantics_summary.body_statement_entries_total &&
         handoff.block_abi_invoke_trampoline_semantics_summary.descriptor_symbolized_sites ==
             block_abi_invoke_trampoline_semantics_summary.descriptor_symbolized_sites &&
         handoff.block_abi_invoke_trampoline_semantics_summary.invoke_trampoline_symbolized_sites ==
             block_abi_invoke_trampoline_semantics_summary.invoke_trampoline_symbolized_sites &&
         handoff.block_abi_invoke_trampoline_semantics_summary.missing_invoke_trampoline_sites ==
             block_abi_invoke_trampoline_semantics_summary.missing_invoke_trampoline_sites &&
         handoff.block_abi_invoke_trampoline_semantics_summary.non_normalized_layout_sites ==
             block_abi_invoke_trampoline_semantics_summary.non_normalized_layout_sites &&
         handoff.block_abi_invoke_trampoline_semantics_summary.contract_violation_sites ==
             block_abi_invoke_trampoline_semantics_summary.contract_violation_sites &&
         handoff.block_abi_invoke_trampoline_semantics_summary.descriptor_symbolized_sites <=
             handoff.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
         handoff.block_abi_invoke_trampoline_semantics_summary.invoke_trampoline_symbolized_sites <=
             handoff.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
         handoff.block_abi_invoke_trampoline_semantics_summary.missing_invoke_trampoline_sites <=
             handoff.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
         handoff.block_abi_invoke_trampoline_semantics_summary.non_normalized_layout_sites <=
             handoff.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
         handoff.block_abi_invoke_trampoline_semantics_summary.contract_violation_sites <=
             handoff.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
         handoff.block_abi_invoke_trampoline_semantics_summary.invoke_trampoline_symbolized_sites +
                 handoff.block_abi_invoke_trampoline_semantics_summary.missing_invoke_trampoline_sites ==
             handoff.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
         handoff.block_abi_invoke_trampoline_semantics_summary.invoke_argument_slots_total ==
             handoff.block_abi_invoke_trampoline_semantics_summary.parameter_entries_total &&
         handoff.block_abi_invoke_trampoline_semantics_summary.capture_word_count_total ==
             handoff.block_abi_invoke_trampoline_semantics_summary.capture_entries_total &&
         handoff.block_storage_escape_semantics_summary.deterministic &&
         handoff.block_storage_escape_semantics_summary.block_literal_sites ==
             block_storage_escape_semantics_summary.block_literal_sites &&
         handoff.block_storage_escape_semantics_summary.mutable_capture_count_total ==
             block_storage_escape_semantics_summary.mutable_capture_count_total &&
         handoff.block_storage_escape_semantics_summary.byref_slot_count_total ==
             block_storage_escape_semantics_summary.byref_slot_count_total &&
         handoff.block_storage_escape_semantics_summary.parameter_entries_total ==
             block_storage_escape_semantics_summary.parameter_entries_total &&
         handoff.block_storage_escape_semantics_summary.capture_entries_total ==
             block_storage_escape_semantics_summary.capture_entries_total &&
         handoff.block_storage_escape_semantics_summary.body_statement_entries_total ==
             block_storage_escape_semantics_summary.body_statement_entries_total &&
         handoff.block_storage_escape_semantics_summary.requires_byref_cells_sites ==
             block_storage_escape_semantics_summary.requires_byref_cells_sites &&
         handoff.block_storage_escape_semantics_summary.escape_analysis_enabled_sites ==
             block_storage_escape_semantics_summary.escape_analysis_enabled_sites &&
         handoff.block_storage_escape_semantics_summary.escape_to_heap_sites ==
             block_storage_escape_semantics_summary.escape_to_heap_sites &&
         handoff.block_storage_escape_semantics_summary.escape_profile_normalized_sites ==
             block_storage_escape_semantics_summary.escape_profile_normalized_sites &&
         handoff.block_storage_escape_semantics_summary.byref_layout_symbolized_sites ==
             block_storage_escape_semantics_summary.byref_layout_symbolized_sites &&
         handoff.block_storage_escape_semantics_summary.contract_violation_sites ==
             block_storage_escape_semantics_summary.contract_violation_sites &&
         handoff.block_storage_escape_semantics_summary.requires_byref_cells_sites <=
             handoff.block_storage_escape_semantics_summary.block_literal_sites &&
         handoff.block_storage_escape_semantics_summary.escape_analysis_enabled_sites <=
             handoff.block_storage_escape_semantics_summary.block_literal_sites &&
         handoff.block_storage_escape_semantics_summary.escape_to_heap_sites <=
             handoff.block_storage_escape_semantics_summary.block_literal_sites &&
         handoff.block_storage_escape_semantics_summary.escape_profile_normalized_sites <=
             handoff.block_storage_escape_semantics_summary.block_literal_sites &&
         handoff.block_storage_escape_semantics_summary.byref_layout_symbolized_sites <=
             handoff.block_storage_escape_semantics_summary.block_literal_sites &&
         handoff.block_storage_escape_semantics_summary.contract_violation_sites <=
             handoff.block_storage_escape_semantics_summary.block_literal_sites &&
         handoff.block_storage_escape_semantics_summary.mutable_capture_count_total ==
             handoff.block_storage_escape_semantics_summary.capture_entries_total &&
         handoff.block_storage_escape_semantics_summary.byref_slot_count_total ==
             handoff.block_storage_escape_semantics_summary.capture_entries_total &&
         handoff.block_storage_escape_semantics_summary.escape_analysis_enabled_sites ==
             handoff.block_storage_escape_semantics_summary.block_literal_sites &&
         handoff.block_storage_escape_semantics_summary.requires_byref_cells_sites ==
             handoff.block_storage_escape_semantics_summary.escape_to_heap_sites &&
         handoff.message_send_selector_lowering_summary.deterministic &&
         handoff.message_send_selector_lowering_summary.message_send_sites ==
             message_send_selector_lowering_summary.message_send_sites &&
         handoff.message_send_selector_lowering_summary.unary_form_sites ==
             message_send_selector_lowering_summary.unary_form_sites &&
         handoff.message_send_selector_lowering_summary.keyword_form_sites ==
             message_send_selector_lowering_summary.keyword_form_sites &&
         handoff.message_send_selector_lowering_summary.selector_lowering_symbol_sites ==
             message_send_selector_lowering_summary.selector_lowering_symbol_sites &&
         handoff.message_send_selector_lowering_summary.selector_lowering_piece_entries ==
             message_send_selector_lowering_summary.selector_lowering_piece_entries &&
         handoff.message_send_selector_lowering_summary.selector_lowering_argument_piece_entries ==
             message_send_selector_lowering_summary.selector_lowering_argument_piece_entries &&
         handoff.message_send_selector_lowering_summary.selector_lowering_normalized_sites ==
             message_send_selector_lowering_summary.selector_lowering_normalized_sites &&
         handoff.message_send_selector_lowering_summary.selector_lowering_form_mismatch_sites ==
             message_send_selector_lowering_summary.selector_lowering_form_mismatch_sites &&
         handoff.message_send_selector_lowering_summary.selector_lowering_arity_mismatch_sites ==
             message_send_selector_lowering_summary.selector_lowering_arity_mismatch_sites &&
         handoff.message_send_selector_lowering_summary.selector_lowering_symbol_mismatch_sites ==
             message_send_selector_lowering_summary.selector_lowering_symbol_mismatch_sites &&
         handoff.message_send_selector_lowering_summary.selector_lowering_missing_symbol_sites ==
             message_send_selector_lowering_summary.selector_lowering_missing_symbol_sites &&
         handoff.message_send_selector_lowering_summary.selector_lowering_contract_violation_sites ==
             message_send_selector_lowering_summary.selector_lowering_contract_violation_sites &&
         handoff.message_send_selector_lowering_summary.unary_form_sites +
                 handoff.message_send_selector_lowering_summary.keyword_form_sites ==
             handoff.message_send_selector_lowering_summary.message_send_sites &&
         handoff.message_send_selector_lowering_summary.selector_lowering_symbol_sites <=
             handoff.message_send_selector_lowering_summary.message_send_sites &&
         handoff.message_send_selector_lowering_summary.selector_lowering_argument_piece_entries <=
             handoff.message_send_selector_lowering_summary.selector_lowering_piece_entries &&
         handoff.message_send_selector_lowering_summary.selector_lowering_normalized_sites <=
             handoff.message_send_selector_lowering_summary.selector_lowering_symbol_sites &&
         handoff.message_send_selector_lowering_summary.selector_lowering_form_mismatch_sites <=
             handoff.message_send_selector_lowering_summary.message_send_sites &&
         handoff.message_send_selector_lowering_summary.selector_lowering_arity_mismatch_sites <=
             handoff.message_send_selector_lowering_summary.message_send_sites &&
         handoff.message_send_selector_lowering_summary.selector_lowering_symbol_mismatch_sites <=
             handoff.message_send_selector_lowering_summary.message_send_sites &&
         handoff.message_send_selector_lowering_summary.selector_lowering_missing_symbol_sites <=
             handoff.message_send_selector_lowering_summary.message_send_sites &&
         handoff.message_send_selector_lowering_summary.selector_lowering_contract_violation_sites <=
             handoff.message_send_selector_lowering_summary.message_send_sites &&
         handoff.dispatch_abi_marshalling_summary.deterministic &&
         handoff.dispatch_abi_marshalling_summary.message_send_sites ==
             dispatch_abi_marshalling_summary.message_send_sites &&
         handoff.dispatch_abi_marshalling_summary.receiver_slots ==
             dispatch_abi_marshalling_summary.receiver_slots &&
         handoff.dispatch_abi_marshalling_summary.selector_symbol_slots ==
             dispatch_abi_marshalling_summary.selector_symbol_slots &&
         handoff.dispatch_abi_marshalling_summary.argument_slots ==
             dispatch_abi_marshalling_summary.argument_slots &&
         handoff.dispatch_abi_marshalling_summary.keyword_argument_slots ==
             dispatch_abi_marshalling_summary.keyword_argument_slots &&
         handoff.dispatch_abi_marshalling_summary.unary_argument_slots ==
             dispatch_abi_marshalling_summary.unary_argument_slots &&
         handoff.dispatch_abi_marshalling_summary.arity_mismatch_sites ==
             dispatch_abi_marshalling_summary.arity_mismatch_sites &&
         handoff.dispatch_abi_marshalling_summary.missing_selector_symbol_sites ==
             dispatch_abi_marshalling_summary.missing_selector_symbol_sites &&
         handoff.dispatch_abi_marshalling_summary.contract_violation_sites ==
             dispatch_abi_marshalling_summary.contract_violation_sites &&
         handoff.dispatch_abi_marshalling_summary.receiver_slots ==
             handoff.dispatch_abi_marshalling_summary.message_send_sites &&
         handoff.dispatch_abi_marshalling_summary.selector_symbol_slots +
                 handoff.dispatch_abi_marshalling_summary.missing_selector_symbol_sites ==
             handoff.dispatch_abi_marshalling_summary.message_send_sites &&
         handoff.dispatch_abi_marshalling_summary.keyword_argument_slots +
                 handoff.dispatch_abi_marshalling_summary.unary_argument_slots ==
             handoff.dispatch_abi_marshalling_summary.argument_slots &&
         handoff.dispatch_abi_marshalling_summary.keyword_argument_slots <=
             handoff.dispatch_abi_marshalling_summary.argument_slots &&
         handoff.dispatch_abi_marshalling_summary.unary_argument_slots <=
             handoff.dispatch_abi_marshalling_summary.argument_slots &&
         handoff.dispatch_abi_marshalling_summary.selector_symbol_slots <=
             handoff.dispatch_abi_marshalling_summary.message_send_sites &&
         handoff.dispatch_abi_marshalling_summary.missing_selector_symbol_sites <=
             handoff.dispatch_abi_marshalling_summary.message_send_sites &&
         handoff.dispatch_abi_marshalling_summary.arity_mismatch_sites <=
             handoff.dispatch_abi_marshalling_summary.message_send_sites &&
         handoff.dispatch_abi_marshalling_summary.contract_violation_sites <=
             handoff.dispatch_abi_marshalling_summary.message_send_sites &&
         handoff.nil_receiver_semantics_foldability_summary.deterministic &&
         handoff.nil_receiver_semantics_foldability_summary.message_send_sites ==
             nil_receiver_semantics_foldability_summary.message_send_sites &&
         handoff.nil_receiver_semantics_foldability_summary.receiver_nil_literal_sites ==
             nil_receiver_semantics_foldability_summary.receiver_nil_literal_sites &&
         handoff.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites ==
             nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites &&
         handoff.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites ==
             nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites &&
         handoff.nil_receiver_semantics_foldability_summary.nil_receiver_runtime_dispatch_required_sites ==
             nil_receiver_semantics_foldability_summary.nil_receiver_runtime_dispatch_required_sites &&
         handoff.nil_receiver_semantics_foldability_summary.non_nil_receiver_sites ==
             nil_receiver_semantics_foldability_summary.non_nil_receiver_sites &&
         handoff.nil_receiver_semantics_foldability_summary.contract_violation_sites ==
             nil_receiver_semantics_foldability_summary.contract_violation_sites &&
         handoff.nil_receiver_semantics_foldability_summary.receiver_nil_literal_sites ==
             handoff.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites &&
         handoff.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites <=
             handoff.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites &&
         handoff.nil_receiver_semantics_foldability_summary.nil_receiver_runtime_dispatch_required_sites +
                 handoff.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites ==
             handoff.nil_receiver_semantics_foldability_summary.message_send_sites &&
         handoff.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites +
                 handoff.nil_receiver_semantics_foldability_summary.non_nil_receiver_sites ==
             handoff.nil_receiver_semantics_foldability_summary.message_send_sites &&
         handoff.nil_receiver_semantics_foldability_summary.contract_violation_sites <=
             handoff.nil_receiver_semantics_foldability_summary.message_send_sites &&
         handoff.super_dispatch_method_family_summary.deterministic &&
         handoff.super_dispatch_method_family_summary.message_send_sites ==
             super_dispatch_method_family_summary.message_send_sites &&
         handoff.super_dispatch_method_family_summary.receiver_super_identifier_sites ==
             super_dispatch_method_family_summary.receiver_super_identifier_sites &&
         handoff.super_dispatch_method_family_summary.super_dispatch_enabled_sites ==
             super_dispatch_method_family_summary.super_dispatch_enabled_sites &&
         handoff.super_dispatch_method_family_summary.super_dispatch_requires_class_context_sites ==
             super_dispatch_method_family_summary.super_dispatch_requires_class_context_sites &&
         handoff.super_dispatch_method_family_summary.method_family_init_sites ==
             super_dispatch_method_family_summary.method_family_init_sites &&
         handoff.super_dispatch_method_family_summary.method_family_copy_sites ==
             super_dispatch_method_family_summary.method_family_copy_sites &&
         handoff.super_dispatch_method_family_summary.method_family_mutable_copy_sites ==
             super_dispatch_method_family_summary.method_family_mutable_copy_sites &&
         handoff.super_dispatch_method_family_summary.method_family_new_sites ==
             super_dispatch_method_family_summary.method_family_new_sites &&
         handoff.super_dispatch_method_family_summary.method_family_none_sites ==
             super_dispatch_method_family_summary.method_family_none_sites &&
         handoff.super_dispatch_method_family_summary.method_family_returns_retained_result_sites ==
             super_dispatch_method_family_summary.method_family_returns_retained_result_sites &&
         handoff.super_dispatch_method_family_summary.method_family_returns_related_result_sites ==
             super_dispatch_method_family_summary.method_family_returns_related_result_sites &&
         handoff.super_dispatch_method_family_summary.contract_violation_sites ==
             super_dispatch_method_family_summary.contract_violation_sites &&
         handoff.super_dispatch_method_family_summary.receiver_super_identifier_sites ==
             handoff.super_dispatch_method_family_summary.super_dispatch_enabled_sites &&
         handoff.super_dispatch_method_family_summary.super_dispatch_requires_class_context_sites ==
             handoff.super_dispatch_method_family_summary.super_dispatch_enabled_sites &&
         handoff.super_dispatch_method_family_summary.method_family_init_sites +
                 handoff.super_dispatch_method_family_summary.method_family_copy_sites +
                 handoff.super_dispatch_method_family_summary.method_family_mutable_copy_sites +
                 handoff.super_dispatch_method_family_summary.method_family_new_sites +
                 handoff.super_dispatch_method_family_summary.method_family_none_sites ==
             handoff.super_dispatch_method_family_summary.message_send_sites &&
         handoff.super_dispatch_method_family_summary.method_family_returns_related_result_sites <=
             handoff.super_dispatch_method_family_summary.method_family_init_sites &&
         handoff.super_dispatch_method_family_summary.method_family_returns_retained_result_sites <=
             handoff.super_dispatch_method_family_summary.message_send_sites &&
         handoff.super_dispatch_method_family_summary.contract_violation_sites <=
             handoff.super_dispatch_method_family_summary.message_send_sites &&
         handoff.runtime_shim_host_link_summary.deterministic &&
         handoff.runtime_shim_host_link_summary.message_send_sites ==
             runtime_shim_host_link_summary.message_send_sites &&
         handoff.runtime_shim_host_link_summary.runtime_shim_required_sites ==
             runtime_shim_host_link_summary.runtime_shim_required_sites &&
         handoff.runtime_shim_host_link_summary.runtime_shim_elided_sites ==
             runtime_shim_host_link_summary.runtime_shim_elided_sites &&
         handoff.runtime_shim_host_link_summary.runtime_dispatch_arg_slots ==
             runtime_shim_host_link_summary.runtime_dispatch_arg_slots &&
         handoff.runtime_shim_host_link_summary.runtime_dispatch_declaration_parameter_count ==
             runtime_shim_host_link_summary.runtime_dispatch_declaration_parameter_count &&
         handoff.runtime_shim_host_link_summary.contract_violation_sites ==
             runtime_shim_host_link_summary.contract_violation_sites &&
         handoff.runtime_shim_host_link_summary.runtime_dispatch_symbol ==
             runtime_shim_host_link_summary.runtime_dispatch_symbol &&
         handoff.runtime_shim_host_link_summary.default_runtime_dispatch_symbol_binding ==
             runtime_shim_host_link_summary.default_runtime_dispatch_symbol_binding &&
         handoff.runtime_shim_host_link_summary.runtime_shim_required_sites +
                 handoff.runtime_shim_host_link_summary.runtime_shim_elided_sites ==
             handoff.runtime_shim_host_link_summary.message_send_sites &&
         handoff.runtime_shim_host_link_summary.contract_violation_sites <=
             handoff.runtime_shim_host_link_summary.message_send_sites &&
         (handoff.runtime_shim_host_link_summary.message_send_sites == 0 ||
          handoff.runtime_shim_host_link_summary.runtime_dispatch_declaration_parameter_count ==
              handoff.runtime_shim_host_link_summary.runtime_dispatch_arg_slots + 2u) &&
         (handoff.runtime_shim_host_link_summary.default_runtime_dispatch_symbol_binding ==
          (handoff.runtime_shim_host_link_summary.runtime_dispatch_symbol ==
           kObjc3RuntimeShimHostLinkDefaultDispatchSymbol)) &&
         handoff.retain_release_operation_summary.deterministic &&
         handoff.retain_release_operation_summary.ownership_qualified_sites ==
             retain_release_operation_summary.ownership_qualified_sites &&
         handoff.retain_release_operation_summary.retain_insertion_sites ==
             retain_release_operation_summary.retain_insertion_sites &&
         handoff.retain_release_operation_summary.release_insertion_sites ==
             retain_release_operation_summary.release_insertion_sites &&
         handoff.retain_release_operation_summary.autorelease_insertion_sites ==
             retain_release_operation_summary.autorelease_insertion_sites &&
         handoff.retain_release_operation_summary.contract_violation_sites ==
             retain_release_operation_summary.contract_violation_sites &&
         handoff.weak_unowned_semantics_summary.deterministic &&
         handoff.weak_unowned_semantics_summary.ownership_candidate_sites ==
             weak_unowned_semantics_summary.ownership_candidate_sites &&
         handoff.weak_unowned_semantics_summary.weak_reference_sites ==
             weak_unowned_semantics_summary.weak_reference_sites &&
         handoff.weak_unowned_semantics_summary.unowned_reference_sites ==
             weak_unowned_semantics_summary.unowned_reference_sites &&
         handoff.weak_unowned_semantics_summary.unowned_safe_reference_sites ==
             weak_unowned_semantics_summary.unowned_safe_reference_sites &&
         handoff.weak_unowned_semantics_summary.weak_unowned_conflict_sites ==
             weak_unowned_semantics_summary.weak_unowned_conflict_sites &&
         handoff.weak_unowned_semantics_summary.contract_violation_sites ==
             weak_unowned_semantics_summary.contract_violation_sites &&
         handoff.weak_unowned_semantics_summary.unowned_safe_reference_sites <=
             handoff.weak_unowned_semantics_summary.unowned_reference_sites &&
         handoff.weak_unowned_semantics_summary.weak_unowned_conflict_sites <=
             handoff.weak_unowned_semantics_summary.ownership_candidate_sites &&
         handoff.weak_unowned_semantics_summary.contract_violation_sites <=
             handoff.weak_unowned_semantics_summary.ownership_candidate_sites +
                 handoff.weak_unowned_semantics_summary.weak_unowned_conflict_sites &&
         handoff.arc_diagnostics_fixit_summary.deterministic &&
         handoff.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites ==
             arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites &&
         handoff.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites ==
             arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites &&
         handoff.arc_diagnostics_fixit_summary.ownership_arc_profiled_sites ==
             arc_diagnostics_fixit_summary.ownership_arc_profiled_sites &&
         handoff.arc_diagnostics_fixit_summary.ownership_arc_weak_unowned_conflict_diagnostic_sites ==
             arc_diagnostics_fixit_summary.ownership_arc_weak_unowned_conflict_diagnostic_sites &&
         handoff.arc_diagnostics_fixit_summary.ownership_arc_empty_fixit_hint_sites ==
             arc_diagnostics_fixit_summary.ownership_arc_empty_fixit_hint_sites &&
         handoff.arc_diagnostics_fixit_summary.contract_violation_sites ==
             arc_diagnostics_fixit_summary.contract_violation_sites &&
         handoff.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites <=
             handoff.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
                 handoff.arc_diagnostics_fixit_summary.contract_violation_sites &&
         handoff.arc_diagnostics_fixit_summary.ownership_arc_profiled_sites <=
             handoff.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
                 handoff.arc_diagnostics_fixit_summary.contract_violation_sites &&
         handoff.arc_diagnostics_fixit_summary.ownership_arc_weak_unowned_conflict_diagnostic_sites <=
             handoff.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
                 handoff.arc_diagnostics_fixit_summary.contract_violation_sites &&
         handoff.arc_diagnostics_fixit_summary.ownership_arc_empty_fixit_hint_sites <=
             handoff.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites +
                 handoff.arc_diagnostics_fixit_summary.contract_violation_sites &&
         handoff.autoreleasepool_scope_summary.deterministic &&
         handoff.autoreleasepool_scope_summary.scope_sites ==
             autoreleasepool_scope_summary.scope_sites &&
         handoff.autoreleasepool_scope_summary.scope_symbolized_sites ==
             autoreleasepool_scope_summary.scope_symbolized_sites &&
         handoff.autoreleasepool_scope_summary.contract_violation_sites ==
             autoreleasepool_scope_summary.contract_violation_sites &&
         handoff.autoreleasepool_scope_summary.max_scope_depth ==
             autoreleasepool_scope_summary.max_scope_depth &&
         handoff.autoreleasepool_scope_summary.scope_symbolized_sites <=
             handoff.autoreleasepool_scope_summary.scope_sites &&
         handoff.autoreleasepool_scope_summary.contract_violation_sites <=
             handoff.autoreleasepool_scope_summary.scope_sites &&
         (handoff.autoreleasepool_scope_summary.scope_sites > 0u ||
          handoff.autoreleasepool_scope_summary.max_scope_depth == 0u) &&
         handoff.autoreleasepool_scope_summary.max_scope_depth <=
             static_cast<unsigned>(handoff.autoreleasepool_scope_summary.scope_sites);
}

void ValidateSemanticBodies(const Objc3ParsedProgram &program, const Objc3SemanticIntegrationSurface &surface,
                            const Objc3SemanticValidationOptions &options,
                            std::vector<std::string> &diagnostics) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  StaticScalarBindings global_static_bindings;
  std::unordered_set<std::string> assigned_identifier_names;
  for (const auto &fn : ast.functions) {
    CollectAssignedIdentifiers(fn.body, assigned_identifier_names);
  }
  std::vector<int> global_initializer_values;
  if (ResolveGlobalInitializerValues(ast.globals, global_initializer_values)) {
    const std::size_t count = std::min(ast.globals.size(), global_initializer_values.size());
    for (std::size_t i = 0; i < count; ++i) {
      const std::string &name = ast.globals[i].name;
      if (assigned_identifier_names.find(name) != assigned_identifier_names.end()) {
        continue;
      }
      global_static_bindings[name] = global_initializer_values[i];
    }
  }

  for (const auto &fn : ast.functions) {
    ValidateReturnTypeSuffixes(fn, diagnostics);
    ValidateParameterTypeSuffixes(fn, diagnostics);

    std::vector<SemanticScope> scopes;
    scopes.push_back({});
    for (const auto &param : fn.params) {
      if (scopes.back().find(param.name) != scopes.back().end()) {
        diagnostics.push_back(MakeDiag(param.line, param.column, "O3S201", "duplicate parameter '" + param.name + "'"));
      } else {
        scopes.back().emplace(param.name, MakeSemanticTypeFromParam(param));
      }
    }

    if (!fn.is_prototype) {
      const SemanticTypeInfo expected_return_type = MakeSemanticTypeFromFunctionReturn(fn);
      const StaticScalarBindings static_scalar_bindings = CollectFunctionStaticScalarBindings(fn, &global_static_bindings);
      ValidateStatements(fn.body, scopes, surface.globals, surface.functions, expected_return_type, fn.name, diagnostics,
                         0, 0, options.max_message_send_args);
      if (!(expected_return_type.type == ValueType::Void && !expected_return_type.is_vector) &&
          !BlockAlwaysReturns(fn.body, &static_scalar_bindings)) {
        diagnostics.push_back(
            MakeDiag(fn.line, fn.column, "O3S205", "missing return path in function '" + fn.name + "'"));
      }
    }
  }
}
