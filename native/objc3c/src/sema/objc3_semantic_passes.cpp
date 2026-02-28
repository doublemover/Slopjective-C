#include "sema/objc3_semantic_passes.h"
#include "sema/objc3_static_analysis.h"

#include <algorithm>
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

static bool HasInvalidParamTypeSuffix(const FuncParam &param) {
  const bool has_unsupported_generic_suffix = param.has_generic_suffix && !SupportsGenericParamTypeSuffix(param);
  const bool has_unsupported_pointer_declarator =
      param.has_pointer_declarator && !SupportsPointerParamTypeDeclarator(param);
  const bool has_unsupported_nullability_suffix =
      !param.nullability_suffix_tokens.empty() && !SupportsNullabilityParamTypeSuffix(param);
  return has_unsupported_generic_suffix || has_unsupported_pointer_declarator || has_unsupported_nullability_suffix;
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

static std::string MethodSelectorName(const Objc3MethodDecl &method) {
  return method.selector.empty() ? std::string("<unknown>") : method.selector;
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

static Objc3MethodInfo BuildMethodInfo(const Objc3MethodDecl &method) {
  Objc3MethodInfo info;
  info.arity = method.params.size();
  info.param_types.reserve(method.params.size());
  info.param_is_vector.reserve(method.params.size());
  info.param_vector_base_spelling.reserve(method.params.size());
  info.param_vector_lane_count.reserve(method.params.size());
  info.param_has_invalid_type_suffix.reserve(method.params.size());
  for (const auto &param : method.params) {
    info.param_types.push_back(param.type);
    info.param_is_vector.push_back(param.vector_spelling);
    info.param_vector_base_spelling.push_back(param.vector_base_spelling);
    info.param_vector_lane_count.push_back(param.vector_lane_count);
    info.param_has_invalid_type_suffix.push_back(HasInvalidParamTypeSuffix(param));
  }
  info.return_type = method.return_type;
  info.return_is_vector = method.return_vector_spelling;
  info.return_vector_base_spelling = method.return_vector_base_spelling;
  info.return_vector_lane_count = method.return_vector_lane_count;
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
  for (std::size_t i = 0; i < lhs.arity; ++i) {
    if (i >= lhs.param_types.size() || i >= lhs.param_is_vector.size() || i >= lhs.param_vector_base_spelling.size() ||
        i >= lhs.param_vector_lane_count.size() || i >= rhs.param_types.size() || i >= rhs.param_is_vector.size() ||
        i >= rhs.param_vector_base_spelling.size() || i >= rhs.param_vector_lane_count.size()) {
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
      for (const auto &param : fn.params) {
        info.param_types.push_back(param.type);
        info.param_is_vector.push_back(param.vector_spelling);
        info.param_vector_base_spelling.push_back(param.vector_base_spelling);
        info.param_vector_lane_count.push_back(param.vector_lane_count);
        info.param_has_invalid_type_suffix.push_back(HasInvalidParamTypeSuffix(param));
      }
      info.return_type = fn.return_type;
      info.return_is_vector = fn.return_vector_spelling;
      info.return_vector_base_spelling = fn.return_vector_base_spelling;
      info.return_vector_lane_count = fn.return_vector_lane_count;
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
    if (compatible) {
      for (std::size_t i = 0; i < fn.params.size(); ++i) {
        if (i >= existing.param_types.size() || i >= existing.param_is_vector.size() ||
            i >= existing.param_vector_base_spelling.size() || i >= existing.param_vector_lane_count.size() ||
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
    for (const auto &method_decl : interface_decl.methods) {
      ValidateMethodReturnTypeSuffixes(method_decl, interface_decl.name, "interface", diagnostics);
      ValidateMethodParameterTypeSuffixes(method_decl, interface_decl.name, "interface", diagnostics);

      const std::string selector = MethodSelectorName(method_decl);
      if (method_decl.has_body) {
        diagnostics.push_back(MakeDiag(method_decl.line, method_decl.column, "O3S206",
                                       "type mismatch: interface selector '" + selector + "' in '" +
                                           interface_decl.name + "' must not define a body"));
      }

      const auto method_insert =
          interface_info.methods.emplace(selector, BuildMethodInfo(method_decl));
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

    for (const auto &method_decl : implementation_decl.methods) {
      ValidateMethodReturnTypeSuffixes(method_decl, implementation_decl.name, "implementation", diagnostics);
      ValidateMethodParameterTypeSuffixes(method_decl, implementation_decl.name, "implementation", diagnostics);

      const std::string selector = MethodSelectorName(method_decl);
      if (!method_decl.has_body) {
        diagnostics.push_back(MakeDiag(method_decl.line, method_decl.column, "O3S206",
                                       "type mismatch: implementation selector '" + selector + "' in '" +
                                           implementation_decl.name + "' must define a body"));
      }

      Objc3MethodInfo method_info = BuildMethodInfo(method_decl);
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
    metadata.return_type = source.return_type;
    metadata.return_is_vector = source.return_is_vector;
    metadata.return_vector_base_spelling = source.return_vector_base_spelling;
    metadata.return_vector_lane_count = source.return_vector_lane_count;
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
      method_metadata.arity = source.arity;
      method_metadata.param_types = source.param_types;
      method_metadata.param_is_vector = source.param_is_vector;
      method_metadata.param_vector_base_spelling = source.param_vector_base_spelling;
      method_metadata.param_vector_lane_count = source.param_vector_lane_count;
      method_metadata.param_has_invalid_type_suffix = source.param_has_invalid_type_suffix;
      method_metadata.return_type = source.return_type;
      method_metadata.return_is_vector = source.return_is_vector;
      method_metadata.return_vector_base_spelling = source.return_vector_base_spelling;
      method_metadata.return_vector_lane_count = source.return_vector_lane_count;
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
      method_metadata.arity = source.arity;
      method_metadata.param_types = source.param_types;
      method_metadata.param_is_vector = source.param_is_vector;
      method_metadata.param_vector_base_spelling = source.param_vector_base_spelling;
      method_metadata.param_vector_lane_count = source.param_vector_lane_count;
      method_metadata.param_has_invalid_type_suffix = source.param_has_invalid_type_suffix;
      method_metadata.return_type = source.return_type;
      method_metadata.return_is_vector = source.return_is_vector;
      method_metadata.return_vector_base_spelling = source.return_vector_base_spelling;
      method_metadata.return_vector_lane_count = source.return_vector_lane_count;
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
    for (std::size_t i = 0; i < lhs.arity; ++i) {
      if (i >= lhs.param_types.size() || i >= lhs.param_is_vector.size() || i >= lhs.param_vector_base_spelling.size() ||
          i >= lhs.param_vector_lane_count.size() || i >= rhs.param_types.size() || i >= rhs.param_is_vector.size() ||
          i >= rhs.param_vector_base_spelling.size() || i >= rhs.param_vector_lane_count.size()) {
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
    return metadata.param_types.size() == metadata.arity &&
           metadata.param_is_vector.size() == metadata.arity &&
           metadata.param_vector_base_spelling.size() == metadata.arity &&
           metadata.param_vector_lane_count.size() == metadata.arity &&
           metadata.param_has_invalid_type_suffix.size() == metadata.arity;
  };

  const bool deterministic_functions =
      std::all_of(handoff.functions_lexicographic.begin(),
                  handoff.functions_lexicographic.end(),
                  [](const Objc3SemanticFunctionTypeMetadata &metadata) {
                    return metadata.param_types.size() == metadata.arity &&
                           metadata.param_is_vector.size() == metadata.arity &&
                           metadata.param_vector_base_spelling.size() == metadata.arity &&
                           metadata.param_vector_lane_count.size() == metadata.arity &&
                           metadata.param_has_invalid_type_suffix.size() == metadata.arity;
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
         summary.linked_implementation_symbols <= summary.interface_method_symbols;
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
