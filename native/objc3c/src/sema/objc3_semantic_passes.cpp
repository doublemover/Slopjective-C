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

static bool IsCompoundAssignmentOperator(const std::string &op) {
  return op == "+=" || op == "-=" || op == "*=" || op == "/=" || op == "%=" || op == "&=" || op == "|=" ||
         op == "^=" || op == "<<=" || op == ">>=";
}

static bool IsMessageI32CompatibleType(ValueType type) {
  return type == ValueType::I32 || type == ValueType::Bool;
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

bool ResolveGlobalInitializerValues(const std::vector<GlobalDecl> &globals, std::vector<int> &values) {
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

static ValueType ScopeLookupType(const std::vector<std::unordered_map<std::string, ValueType>> &scopes,
                                 const std::string &name) {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    auto found = it->find(name);
    if (found != it->end()) {
      return found->second;
    }
  }
  return ValueType::Unknown;
}

static bool SupportsGenericParamTypeSuffix(const FuncParam &param) {
  return param.id_spelling || param.class_spelling || param.instancetype_spelling;
}

static bool SupportsNullabilityParamTypeSuffix(const FuncParam &param) {
  return param.id_spelling || param.class_spelling || param.instancetype_spelling;
}

static bool SupportsGenericReturnTypeSuffix(const FunctionDecl &fn) {
  return fn.return_id_spelling || fn.return_class_spelling || fn.return_instancetype_spelling;
}

static bool SupportsNullabilityReturnTypeSuffix(const FunctionDecl &fn) {
  return fn.return_id_spelling || fn.return_class_spelling || fn.return_instancetype_spelling;
}

static bool HasInvalidParamTypeSuffix(const FuncParam &param) {
  const bool has_unsupported_generic_suffix = param.has_generic_suffix && !SupportsGenericParamTypeSuffix(param);
  const bool has_unsupported_nullability_suffix =
      !param.nullability_suffix_tokens.empty() && !SupportsNullabilityParamTypeSuffix(param);
  return has_unsupported_generic_suffix || has_unsupported_nullability_suffix;
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
  if (!SupportsNullabilityReturnTypeSuffix(fn)) {
    for (const auto &token : fn.return_nullability_suffix_tokens) {
      diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                     "type mismatch: unsupported function return type suffix '" + token.text +
                                         "' for non-id/Class/instancetype return annotation in function '" + fn.name +
                                         "'"));
    }
  }
}

static ValueType ValidateMessageSendExpr(const Expr *expr,
                                         const std::vector<std::unordered_map<std::string, ValueType>> &scopes,
                                         const std::unordered_map<std::string, ValueType> &globals,
                                         const std::unordered_map<std::string, FunctionInfo> &functions,
                                         std::vector<std::string> &diagnostics,
                                         std::size_t max_message_send_args);

static ValueType ValidateExpr(const Expr *expr, const std::vector<std::unordered_map<std::string, ValueType>> &scopes,
                              const std::unordered_map<std::string, ValueType> &globals,
                              const std::unordered_map<std::string, FunctionInfo> &functions,
                              std::vector<std::string> &diagnostics,
                              std::size_t max_message_send_args) {
  if (expr == nullptr) {
    return ValueType::Unknown;
  }
  switch (expr->kind) {
    case Expr::Kind::Number:
      return ValueType::I32;
    case Expr::Kind::BoolLiteral:
      return ValueType::Bool;
    case Expr::Kind::NilLiteral:
      return ValueType::I32;
    case Expr::Kind::Identifier: {
      const ValueType local_type = ScopeLookupType(scopes, expr->ident);
      if (local_type != ValueType::Unknown) {
        return local_type;
      }
      auto global_it = globals.find(expr->ident);
      if (global_it != globals.end()) {
        return global_it->second;
      }
      if (functions.find(expr->ident) != functions.end()) {
        diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                       "type mismatch: function '" + expr->ident +
                                           "' cannot be used as a value"));
        return ValueType::Function;
      }
      diagnostics.push_back(
          MakeDiag(expr->line, expr->column, "O3S202", "undefined identifier '" + expr->ident + "'"));
      return ValueType::Unknown;
    }
    case Expr::Kind::Binary: {
      const ValueType lhs =
          ValidateExpr(expr->left.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      const ValueType rhs =
          ValidateExpr(expr->right.get(), scopes, globals, functions, diagnostics, max_message_send_args);

      if (expr->op == "+" || expr->op == "-" || expr->op == "*" || expr->op == "/" || expr->op == "%") {
        if (lhs != ValueType::Unknown && lhs != ValueType::I32) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for arithmetic lhs, got '" +
                                             std::string(TypeName(lhs)) + "'"));
        }
        if (rhs != ValueType::Unknown && rhs != ValueType::I32) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for arithmetic rhs, got '" +
                                             std::string(TypeName(rhs)) + "'"));
        }
        return ValueType::I32;
      }

      if (expr->op == "&" || expr->op == "|" || expr->op == "^" || expr->op == "<<" || expr->op == ">>") {
        if (lhs != ValueType::Unknown && lhs != ValueType::I32) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for bitwise lhs, got '" +
                                             std::string(TypeName(lhs)) + "'"));
        }
        if (rhs != ValueType::Unknown && rhs != ValueType::I32) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for bitwise rhs, got '" +
                                             std::string(TypeName(rhs)) + "'"));
        }
        return ValueType::I32;
      }

      if (expr->op == "==" || expr->op == "!=") {
        const bool bool_to_i32_literal =
            (lhs == ValueType::Bool && rhs == ValueType::I32 && IsBoolLikeI32Literal(expr->right.get())) ||
            (rhs == ValueType::Bool && lhs == ValueType::I32 && IsBoolLikeI32Literal(expr->left.get()));
        if (lhs != ValueType::Unknown && rhs != ValueType::Unknown && lhs != rhs && !bool_to_i32_literal) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: equality compares '" + std::string(TypeName(lhs)) +
                                             "' with '" + std::string(TypeName(rhs)) + "'"));
        }
        return ValueType::Bool;
      }

      if (expr->op == "<" || expr->op == "<=" || expr->op == ">" || expr->op == ">=") {
        if (lhs != ValueType::Unknown && lhs != ValueType::I32) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for relational lhs, got '" +
                                             std::string(TypeName(lhs)) + "'"));
        }
        if (rhs != ValueType::Unknown && rhs != ValueType::I32) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for relational rhs, got '" +
                                             std::string(TypeName(rhs)) + "'"));
        }
        return ValueType::Bool;
      }

      if (expr->op == "&&" || expr->op == "||") {
        if (lhs != ValueType::Unknown && lhs != ValueType::Bool && lhs != ValueType::I32) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected bool for logical lhs, got '" +
                                             std::string(TypeName(lhs)) + "'"));
        }
        if (rhs != ValueType::Unknown && rhs != ValueType::Bool && rhs != ValueType::I32) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected bool for logical rhs, got '" +
                                             std::string(TypeName(rhs)) + "'"));
        }
        return ValueType::Bool;
      }

      return ValueType::Unknown;
    }
    case Expr::Kind::Conditional: {
      if (expr->left == nullptr || expr->right == nullptr || expr->third == nullptr) {
        return ValueType::Unknown;
      }

      const ValueType condition_type =
          ValidateExpr(expr->left.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      if (condition_type != ValueType::Unknown && condition_type != ValueType::Bool &&
          condition_type != ValueType::I32) {
        diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                       "type mismatch: conditional condition must be bool-compatible"));
      }

      const ValueType then_type =
          ValidateExpr(expr->right.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      const ValueType else_type =
          ValidateExpr(expr->third.get(), scopes, globals, functions, diagnostics, max_message_send_args);

      if (then_type == ValueType::Unknown) {
        return else_type;
      }
      if (else_type == ValueType::Unknown) {
        return then_type;
      }
      const bool then_scalar = then_type == ValueType::I32 || then_type == ValueType::Bool;
      const bool else_scalar = else_type == ValueType::I32 || else_type == ValueType::Bool;
      if (then_scalar && else_scalar) {
        if (then_type == else_type) {
          return then_type;
        }
        return ValueType::I32;
      }
      if (then_type != else_type) {
        diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                       "type mismatch: conditional branches must be scalar-compatible"));
      }
      return then_type == else_type ? then_type : ValueType::Unknown;
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
        const ValueType arg_type = ValidateExpr(expr->args[i].get(), scopes, globals, functions, diagnostics,
                                                max_message_send_args);
        if (fn_it != functions.end() && i < fn_it->second.param_types.size()) {
          if (i < fn_it->second.param_has_invalid_type_suffix.size() &&
              fn_it->second.param_has_invalid_type_suffix[i]) {
            continue;
          }
          const ValueType expected = fn_it->second.param_types[i];
          const bool bool_coercion = expected == ValueType::Bool && arg_type == ValueType::I32;
          if (arg_type != ValueType::Unknown && expected != ValueType::Unknown && arg_type != expected &&
              !bool_coercion) {
            diagnostics.push_back(MakeDiag(expr->args[i]->line, expr->args[i]->column, "O3S206",
                                           "type mismatch: expected '" + std::string(TypeName(expected)) +
                                               "' argument for parameter " + std::to_string(i) + " of '" +
                                               expr->ident + "', got '" + std::string(TypeName(arg_type)) + "'"));
          }
        }
      }
      if (fn_it != functions.end()) {
        return fn_it->second.return_type;
      }
      return ValueType::Unknown;
    }
    case Expr::Kind::MessageSend: {
      return ValidateMessageSendExpr(expr, scopes, globals, functions, diagnostics, max_message_send_args);
    }
  }
  return ValueType::Unknown;
}

static ValueType ValidateMessageSendExpr(const Expr *expr,
                                         const std::vector<std::unordered_map<std::string, ValueType>> &scopes,
                                         const std::unordered_map<std::string, ValueType> &globals,
                                         const std::unordered_map<std::string, FunctionInfo> &functions,
                                         std::vector<std::string> &diagnostics,
                                         std::size_t max_message_send_args) {
  const ValueType receiver_type =
      ValidateExpr(expr->receiver.get(), scopes, globals, functions, diagnostics, max_message_send_args);
  const std::string selector = expr->selector.empty() ? "<unknown>" : expr->selector;
  if (receiver_type != ValueType::Unknown && !IsMessageI32CompatibleType(receiver_type)) {
    const unsigned diag_line = expr->receiver != nullptr ? expr->receiver->line : expr->line;
    const unsigned diag_column = expr->receiver != nullptr ? expr->receiver->column : expr->column;
    diagnostics.push_back(MakeDiag(diag_line, diag_column, "O3S207",
                                   "type mismatch: message receiver for selector '" + selector +
                                       "' must be i32-compatible, got '" +
                                       std::string(TypeName(receiver_type)) + "'"));
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
    const ValueType arg_type =
        ValidateExpr(arg.get(), scopes, globals, functions, diagnostics, max_message_send_args);
    if (arg_type != ValueType::Unknown && !IsMessageI32CompatibleType(arg_type)) {
      diagnostics.push_back(MakeDiag(arg->line, arg->column, "O3S209",
                                     "type mismatch: message argument " + std::to_string(i) +
                                         " for selector '" + selector +
                                         "' must be i32-compatible, got '" +
                                         std::string(TypeName(arg_type)) + "'"));
    }
  }
  return ValueType::I32;
}

static void ValidateStatements(const std::vector<std::unique_ptr<Stmt>> &statements,
                               std::vector<std::unordered_map<std::string, ValueType>> &scopes,
                               const std::unordered_map<std::string, ValueType> &globals,
                               const std::unordered_map<std::string, FunctionInfo> &functions,
                               ValueType expected_return_type, const std::string &function_name,
                               std::vector<std::string> &diagnostics, int loop_depth, int switch_depth,
                               std::size_t max_message_send_args);

static void ValidateAssignmentCompatibility(const std::string &target_name, const std::string &op,
                                           const Expr *value_expr, unsigned line, unsigned column,
                                           bool found_target, ValueType target_type, ValueType value_type,
                                           std::vector<std::string> &diagnostics) {
  if (op == "=") {
    const bool target_known_scalar = target_type == ValueType::I32 || target_type == ValueType::Bool;
    const bool value_known_scalar = value_type == ValueType::I32 || value_type == ValueType::Bool;
    const bool assign_matches =
        target_type == value_type || (target_type == ValueType::I32 && value_type == ValueType::Bool) ||
        (target_type == ValueType::Bool && value_type == ValueType::I32 && IsBoolLikeI32Literal(value_expr));
    if (found_target && target_known_scalar && value_type != ValueType::Unknown && !value_known_scalar) {
      diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                     "type mismatch: assignment to '" + target_name + "' expects '" +
                                         std::string(TypeName(target_type)) + "', got '" +
                                         std::string(TypeName(value_type)) + "'"));
      return;
    }
    if (found_target && target_known_scalar && value_known_scalar && !assign_matches) {
      diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                     "type mismatch: assignment to '" + target_name + "' expects '" +
                                         std::string(TypeName(target_type)) + "', got '" +
                                         std::string(TypeName(value_type)) + "'"));
    }
    return;
  }

  if (!IsCompoundAssignmentOperator(op)) {
    if (op == "++" || op == "--") {
      if (found_target && target_type != ValueType::Unknown && target_type != ValueType::I32) {
        diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                       "type mismatch: update operator '" + op + "' target '" + target_name +
                                           "' must be 'i32', got '" + std::string(TypeName(target_type)) + "'"));
      }
      return;
    }
    diagnostics.push_back(
        MakeDiag(line, column, "O3S206", "type mismatch: unsupported assignment operator '" + op + "'"));
    return;
  }
  if (!found_target) {
    return;
  }
  if (target_type != ValueType::Unknown && target_type != ValueType::I32) {
    diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                   "type mismatch: compound assignment '" + op + "' target '" + target_name +
                                       "' must be 'i32', got '" + std::string(TypeName(target_type)) + "'"));
  }
  if (target_type == ValueType::I32 && value_type != ValueType::Unknown && value_type != ValueType::I32) {
    diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                   "type mismatch: compound assignment '" + op + "' value for '" + target_name +
                                       "' must be 'i32', got '" + std::string(TypeName(value_type)) + "'"));
  }
}

static void ValidateStatement(const Stmt *stmt, std::vector<std::unordered_map<std::string, ValueType>> &scopes,
                              const std::unordered_map<std::string, ValueType> &globals,
                              const std::unordered_map<std::string, FunctionInfo> &functions,
                              ValueType expected_return_type, const std::string &function_name,
                              std::vector<std::string> &diagnostics, int loop_depth, int switch_depth,
                              std::size_t max_message_send_args) {
  if (stmt == nullptr) {
    return;
  }
  const auto resolve_assignment_target_type = [&](const std::string &target_name, ValueType &target_type) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
      auto found = it->find(target_name);
      if (found != it->end()) {
        target_type = found->second;
        return true;
      }
    }
    auto global_it = globals.find(target_name);
    if (global_it != globals.end()) {
      target_type = global_it->second;
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
        const ValueType value_type =
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
        ValueType target_type = ValueType::Unknown;
        const bool found_target = resolve_assignment_target_type(clause.name, target_type);
        if (!found_target) {
          diagnostics.push_back(MakeDiag(clause.line, clause.column, "O3S214",
                                         "invalid assignment target '" + clause.name +
                                             "': target must be a mutable symbol"));
        }
        const ValueType value_type =
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
      const ValueType value_type =
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
      ValueType target_type = ValueType::Unknown;
      const bool found_target = resolve_assignment_target_type(assign->name, target_type);
      if (!found_target) {
        diagnostics.push_back(MakeDiag(assign->line, assign->column, "O3S214",
                                       "invalid assignment target '" + assign->name +
                                           "': target must be a mutable symbol"));
      }
      const ValueType value_type =
          ValidateExpr(assign->value.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      ValidateAssignmentCompatibility(assign->name, assign->op, assign->value.get(), assign->line, assign->column,
                                      found_target, target_type, value_type, diagnostics);
      return;
    }
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        const ReturnStmt *ret = stmt->return_stmt.get();
        if (ret->value == nullptr) {
          if (expected_return_type != ValueType::Void) {
            diagnostics.push_back(MakeDiag(ret->line, ret->column, "O3S211",
                                           "type mismatch: function '" + function_name + "' must return '" +
                                               std::string(TypeName(expected_return_type)) + "'"));
          }
          return;
        }

        if (expected_return_type == ValueType::Void) {
          diagnostics.push_back(MakeDiag(ret->line, ret->column, "O3S211",
                                         "type mismatch: void function '" + function_name +
                                             "' must use 'return;'"));
          (void)ValidateExpr(ret->value.get(), scopes, globals, functions, diagnostics, max_message_send_args);
          return;
        }

        const ValueType return_type =
            ValidateExpr(ret->value.get(), scopes, globals, functions, diagnostics, max_message_send_args);
        const bool return_matches =
            return_type == expected_return_type ||
            (expected_return_type == ValueType::I32 && return_type == ValueType::Bool) ||
            (expected_return_type == ValueType::Bool && return_type == ValueType::I32 &&
             IsBoolLikeI32Literal(ret->value.get()));
        if (!return_matches && return_type != ValueType::Unknown && return_type != ValueType::Function) {
          diagnostics.push_back(MakeDiag(ret->line, ret->column, "O3S211",
                                         "type mismatch: return expression in function '" + function_name +
                                             "' must be '" + std::string(TypeName(expected_return_type)) +
                                             "', got '" + std::string(TypeName(return_type)) + "'"));
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
      const ValueType condition_type =
          ValidateExpr(if_stmt->condition.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      if (condition_type != ValueType::Unknown && condition_type != ValueType::Bool &&
          condition_type != ValueType::I32) {
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

      const ValueType condition_type =
          ValidateExpr(do_while_stmt->condition.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      if (condition_type != ValueType::Unknown && condition_type != ValueType::Bool &&
          condition_type != ValueType::I32) {
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
        const ValueType condition_type =
            ValidateExpr(for_stmt->condition.get(), scopes, globals, functions, diagnostics, max_message_send_args);
        if (condition_type != ValueType::Unknown && condition_type != ValueType::Bool &&
            condition_type != ValueType::I32) {
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
      const ValueType condition_type =
          ValidateExpr(switch_stmt->condition.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      if (condition_type != ValueType::Unknown && condition_type != ValueType::Bool &&
          condition_type != ValueType::I32) {
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
      const ValueType condition_type =
          ValidateExpr(while_stmt->condition.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      if (condition_type != ValueType::Unknown && condition_type != ValueType::Bool &&
          condition_type != ValueType::I32) {
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
                               std::vector<std::unordered_map<std::string, ValueType>> &scopes,
                               const std::unordered_map<std::string, ValueType> &globals,
                               const std::unordered_map<std::string, FunctionInfo> &functions,
                               ValueType expected_return_type, const std::string &function_name,
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

Objc3SemanticIntegrationSurface BuildSemanticIntegrationSurface(const Objc3Program &program,
                                                                       std::vector<std::string> &diagnostics) {
  Objc3SemanticIntegrationSurface surface;
  std::unordered_map<std::string, int> resolved_global_values;

  for (const auto &global : program.globals) {
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

  for (const auto &fn : program.functions) {
    if (surface.globals.find(fn.name) != surface.globals.end()) {
      diagnostics.push_back(MakeDiag(fn.line, fn.column, "O3S200", "duplicate function '" + fn.name + "'"));
      continue;
    }

    auto it = surface.functions.find(fn.name);
    if (it == surface.functions.end()) {
      FunctionInfo info;
      info.arity = fn.params.size();
      info.param_types.reserve(fn.params.size());
      info.param_has_invalid_type_suffix.reserve(fn.params.size());
      for (const auto &param : fn.params) {
        info.param_types.push_back(param.type);
        info.param_has_invalid_type_suffix.push_back(HasInvalidParamTypeSuffix(param));
      }
      info.return_type = fn.return_type;
      info.has_definition = !fn.is_prototype;
      info.is_pure_annotation = fn.is_pure;
      surface.functions.emplace(fn.name, std::move(info));
      continue;
    }

    FunctionInfo &existing = it->second;
    bool compatible = existing.arity == fn.params.size() && existing.return_type == fn.return_type;
    if (compatible) {
      for (std::size_t i = 0; i < fn.params.size(); ++i) {
        if (existing.param_types[i] != fn.params[i].type) {
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

  surface.built = true;
  return surface;
}

void ValidateSemanticBodies(const Objc3Program &program, const Objc3SemanticIntegrationSurface &surface,
                            const Objc3SemanticValidationOptions &options,
                            std::vector<std::string> &diagnostics) {
  StaticScalarBindings global_static_bindings;
  std::unordered_set<std::string> assigned_identifier_names;
  for (const auto &fn : program.functions) {
    CollectAssignedIdentifiers(fn.body, assigned_identifier_names);
  }
  std::vector<int> global_initializer_values;
  if (ResolveGlobalInitializerValues(program.globals, global_initializer_values)) {
    const std::size_t count = std::min(program.globals.size(), global_initializer_values.size());
    for (std::size_t i = 0; i < count; ++i) {
      const std::string &name = program.globals[i].name;
      if (assigned_identifier_names.find(name) != assigned_identifier_names.end()) {
        continue;
      }
      global_static_bindings[name] = global_initializer_values[i];
    }
  }

  for (const auto &fn : program.functions) {
    ValidateReturnTypeSuffixes(fn, diagnostics);
    ValidateParameterTypeSuffixes(fn, diagnostics);

    std::vector<std::unordered_map<std::string, ValueType>> scopes;
    scopes.push_back({});
    for (const auto &param : fn.params) {
      if (scopes.back().find(param.name) != scopes.back().end()) {
        diagnostics.push_back(MakeDiag(param.line, param.column, "O3S201", "duplicate parameter '" + param.name + "'"));
      } else {
        scopes.back().emplace(param.name, param.type);
      }
    }

    if (!fn.is_prototype) {
      const StaticScalarBindings static_scalar_bindings = CollectFunctionStaticScalarBindings(fn, &global_static_bindings);
      ValidateStatements(fn.body, scopes, surface.globals, surface.functions, fn.return_type, fn.name, diagnostics,
                         0, 0, options.max_message_send_args);
      if (fn.return_type != ValueType::Void && !BlockAlwaysReturns(fn.body, &static_scalar_bindings)) {
        diagnostics.push_back(
            MakeDiag(fn.line, fn.column, "O3S205", "missing return path in function '" + fn.name + "'"));
      }
    }
  }
}




