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

static bool SupportsPointerPropertyTypeDeclarator(const Objc3PropertyDecl &property) {
  return property.id_spelling || property.class_spelling || property.instancetype_spelling;
}

static bool HasInvalidPropertyTypeSuffix(const Objc3PropertyDecl &property) {
  const bool has_unsupported_generic_suffix =
      property.has_generic_suffix && !SupportsGenericPropertyTypeSuffix(property);
  const bool has_unsupported_pointer_declarator =
      property.has_pointer_declarator && !SupportsPointerPropertyTypeDeclarator(property);
  const bool has_unsupported_nullability_suffix =
      !property.nullability_suffix_tokens.empty() && !SupportsNullabilityPropertyTypeSuffix(property);
  return has_unsupported_generic_suffix || has_unsupported_pointer_declarator || has_unsupported_nullability_suffix;
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
  const bool has_unsupported_generic_suffix = param.has_generic_suffix && !SupportsGenericParamTypeSuffix(param);
  const bool has_unsupported_pointer_declarator =
      param.has_pointer_declarator && !SupportsPointerParamTypeDeclarator(param);
  const bool has_unsupported_nullability_suffix =
      !param.nullability_suffix_tokens.empty() && !SupportsNullabilityParamTypeSuffix(param);
  return has_unsupported_generic_suffix || has_unsupported_pointer_declarator || has_unsupported_nullability_suffix;
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
  info.has_invalid_type_suffix = HasInvalidPropertyTypeSuffix(property);
  info.attribute_entries = property.attributes.size();
  info.is_readonly = property.is_readonly;
  info.is_readwrite = property.is_readwrite;
  info.is_atomic = property.is_atomic;
  info.is_nonatomic = property.is_nonatomic;
  info.is_copy = property.is_copy;
  info.is_strong = property.is_strong;
  info.is_weak = property.is_weak;
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
                                          (info.is_weak ? 1u : 0u) + (info.is_assign ? 1u : 0u);
  if (ownership_modifiers > 1u) {
    info.has_ownership_conflict = true;
    emit_property_contract_violation(
        property.line,
        property.column,
        "type mismatch: @property ownership modifiers conflict for property '" + property.name + "' in " + owner_kind +
            " '" + owner_name + "'");
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
  info.param_has_invalid_type_suffix.reserve(method.params.size());
  info.param_has_protocol_composition.reserve(method.params.size());
  info.param_protocol_composition_lexicographic.reserve(method.params.size());
  info.param_has_invalid_protocol_composition.reserve(method.params.size());
  for (const auto &param : method.params) {
    const ProtocolCompositionInfo protocol_composition = BuildProtocolCompositionInfoFromParam(param);
    info.param_types.push_back(param.type);
    info.param_is_vector.push_back(param.vector_spelling);
    info.param_vector_base_spelling.push_back(param.vector_base_spelling);
    info.param_vector_lane_count.push_back(param.vector_lane_count);
    info.param_has_invalid_type_suffix.push_back(HasInvalidParamTypeSuffix(param));
    info.param_has_protocol_composition.push_back(protocol_composition.has_protocol_composition);
    info.param_protocol_composition_lexicographic.push_back(protocol_composition.names_lexicographic);
    info.param_has_invalid_protocol_composition.push_back(protocol_composition.has_invalid_protocol_composition);
  }
  const ProtocolCompositionInfo return_protocol_composition = BuildProtocolCompositionInfoFromMethodReturn(method);
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
      lhs.is_class_method != rhs.is_class_method) {
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
        i >= lhs.param_protocol_composition_lexicographic.size() || i >= rhs.param_types.size() ||
        i >= rhs.param_is_vector.size() || i >= rhs.param_vector_base_spelling.size() ||
        i >= rhs.param_vector_lane_count.size() || i >= rhs.param_has_protocol_composition.size() ||
        i >= rhs.param_protocol_composition_lexicographic.size()) {
      return false;
    }
    if (lhs.param_types[i] != rhs.param_types[i] || lhs.param_is_vector[i] != rhs.param_is_vector[i]) {
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
      info.param_has_invalid_type_suffix.reserve(fn.params.size());
      info.param_has_protocol_composition.reserve(fn.params.size());
      info.param_protocol_composition_lexicographic.reserve(fn.params.size());
      info.param_has_invalid_protocol_composition.reserve(fn.params.size());
      for (const auto &param : fn.params) {
        const ProtocolCompositionInfo protocol_composition = BuildProtocolCompositionInfoFromParam(param);
        info.param_types.push_back(param.type);
        info.param_is_vector.push_back(param.vector_spelling);
        info.param_vector_base_spelling.push_back(param.vector_base_spelling);
        info.param_vector_lane_count.push_back(param.vector_lane_count);
        info.param_has_invalid_type_suffix.push_back(HasInvalidParamTypeSuffix(param));
        info.param_has_protocol_composition.push_back(protocol_composition.has_protocol_composition);
        info.param_protocol_composition_lexicographic.push_back(protocol_composition.names_lexicographic);
        info.param_has_invalid_protocol_composition.push_back(protocol_composition.has_invalid_protocol_composition);
      }
      const ProtocolCompositionInfo return_protocol_composition = BuildProtocolCompositionInfoFromFunctionReturn(fn);
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
                      existing.return_is_vector == fn.return_vector_spelling;
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
            i >= existing.param_has_protocol_composition.size() ||
            i >= existing.param_protocol_composition_lexicographic.size() ||
            existing.param_types[i] != fn.params[i].type ||
            existing.param_is_vector[i] != fn.params[i].vector_spelling) {
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

    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_invalid_type_suffix.size(); ++i) {
      existing.param_has_invalid_type_suffix[i] =
          existing.param_has_invalid_type_suffix[i] || HasInvalidParamTypeSuffix(fn.params[i]);
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_invalid_protocol_composition.size(); ++i) {
      const ProtocolCompositionInfo param_protocol_composition = BuildProtocolCompositionInfoFromParam(fn.params[i]);
      existing.param_has_invalid_protocol_composition[i] =
          existing.param_has_invalid_protocol_composition[i] || param_protocol_composition.has_invalid_protocol_composition;
    }
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
  surface.selector_normalization_summary = BuildSelectorNormalizationSummaryFromSurface(surface);
  surface.property_attribute_summary = BuildPropertyAttributeSummaryFromSurface(surface);
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
    metadata.param_has_invalid_type_suffix = source.param_has_invalid_type_suffix;
    metadata.param_has_protocol_composition = source.param_has_protocol_composition;
    metadata.param_protocol_composition_lexicographic = source.param_protocol_composition_lexicographic;
    metadata.param_has_invalid_protocol_composition = source.param_has_invalid_protocol_composition;
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
      property_metadata.has_invalid_type_suffix = source.has_invalid_type_suffix;
      property_metadata.attribute_entries = source.attribute_entries;
      property_metadata.attribute_names_lexicographic = source.attribute_names_lexicographic;
      property_metadata.is_readonly = source.is_readonly;
      property_metadata.is_readwrite = source.is_readwrite;
      property_metadata.is_atomic = source.is_atomic;
      property_metadata.is_nonatomic = source.is_nonatomic;
      property_metadata.is_copy = source.is_copy;
      property_metadata.is_strong = source.is_strong;
      property_metadata.is_weak = source.is_weak;
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
      method_metadata.param_has_invalid_type_suffix = source.param_has_invalid_type_suffix;
      method_metadata.param_has_protocol_composition = source.param_has_protocol_composition;
      method_metadata.param_protocol_composition_lexicographic = source.param_protocol_composition_lexicographic;
      method_metadata.param_has_invalid_protocol_composition = source.param_has_invalid_protocol_composition;
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
      property_metadata.has_invalid_type_suffix = source.has_invalid_type_suffix;
      property_metadata.attribute_entries = source.attribute_entries;
      property_metadata.attribute_names_lexicographic = source.attribute_names_lexicographic;
      property_metadata.is_readonly = source.is_readonly;
      property_metadata.is_readwrite = source.is_readwrite;
      property_metadata.is_atomic = source.is_atomic;
      property_metadata.is_nonatomic = source.is_nonatomic;
      property_metadata.is_copy = source.is_copy;
      property_metadata.is_strong = source.is_strong;
      property_metadata.is_weak = source.is_weak;
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
      method_metadata.param_has_invalid_type_suffix = source.param_has_invalid_type_suffix;
      method_metadata.param_has_protocol_composition = source.param_has_protocol_composition;
      method_metadata.param_protocol_composition_lexicographic = source.param_protocol_composition_lexicographic;
      method_metadata.param_has_invalid_protocol_composition = source.param_has_invalid_protocol_composition;
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
        lhs.is_class_method != rhs.is_class_method) {
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
          i >= lhs.param_protocol_composition_lexicographic.size() || i >= rhs.param_types.size() ||
          i >= rhs.param_is_vector.size() || i >= rhs.param_vector_base_spelling.size() ||
          i >= rhs.param_vector_lane_count.size() || i >= rhs.param_has_protocol_composition.size() ||
          i >= rhs.param_protocol_composition_lexicographic.size()) {
        return false;
      }
      if (lhs.param_types[i] != rhs.param_types[i] || lhs.param_is_vector[i] != rhs.param_is_vector[i]) {
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
        metadata.param_has_invalid_type_suffix.size() != metadata.arity ||
        metadata.param_has_protocol_composition.size() != metadata.arity ||
        metadata.param_protocol_composition_lexicographic.size() != metadata.arity ||
        metadata.param_has_invalid_protocol_composition.size() != metadata.arity) {
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
      if (!IsSortedUniqueStrings(metadata.param_protocol_composition_lexicographic[i])) {
        return false;
      }
      if (metadata.param_has_invalid_protocol_composition[i] && !metadata.param_has_protocol_composition[i]) {
        return false;
      }
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
                        metadata.param_has_invalid_type_suffix.size() != metadata.arity ||
                        metadata.param_has_protocol_composition.size() != metadata.arity ||
                        metadata.param_protocol_composition_lexicographic.size() != metadata.arity ||
                        metadata.param_has_invalid_protocol_composition.size() != metadata.arity) {
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
                      if (!IsSortedUniqueStrings(metadata.param_protocol_composition_lexicographic[i])) {
                        return false;
                      }
                      if (metadata.param_has_invalid_protocol_composition[i] &&
                          !metadata.param_has_protocol_composition[i]) {
                        return false;
                      }
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

  std::size_t interface_method_symbols = 0;
  for (const auto &metadata : handoff.interfaces_lexicographic) {
    interface_method_symbols += metadata.methods_lexicographic.size();
  }
  std::size_t implementation_method_symbols = 0;
  for (const auto &metadata : handoff.implementations_lexicographic) {
    implementation_method_symbols += metadata.methods_lexicographic.size();
  }

  const Objc3InterfaceImplementationSummary &summary = handoff.interface_implementation_summary;
  return summary.deterministic &&
         summary.resolved_interfaces == handoff.interfaces_lexicographic.size() &&
         summary.resolved_implementations == handoff.implementations_lexicographic.size() &&
         summary.interface_method_symbols == interface_method_symbols &&
         summary.implementation_method_symbols == implementation_method_symbols &&
         summary.linked_implementation_symbols <= summary.implementation_method_symbols &&
         summary.linked_implementation_symbols <= summary.interface_method_symbols &&
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
             selector_summary.selector_missing_keyword_pieces;
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
