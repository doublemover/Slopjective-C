      }
      if (AreObjCReferenceTypesAssignmentCompatible(then_type, else_type)) {
        if (then_type.type == else_type.type) {
          return then_type;
        }
        return MakeScalarSemanticType(ValueType::ObjCId);
      }
      if (!IsSameSemanticType(then_type, else_type)) {
        diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                       "type mismatch: conditional branches must be type-compatible"));
      }
      return IsSameSemanticType(then_type, else_type) ? then_type : MakeScalarSemanticType(ValueType::Unknown);
    }
#include "sema/objc3_semantic_passes_expression_statement_validation_block_literal_case.inc"
#include "sema/objc3_semantic_passes_expression_statement_validation_callable_checks.inc"
    case Expr::Kind::MessageSend: {
      return ValidateMessageSendExpr(expr, scopes, globals, functions,
                                     diagnostics, max_message_send_args,
                                     message_send_context);
    }
  }
  return MakeScalarSemanticType(ValueType::Unknown);
}

#include "sema/objc3_semantic_passes_expression_statement_validation_message_send.inc"

static void ValidateStatements(const std::vector<std::unique_ptr<Stmt>> &statements,
                               std::vector<SemanticScope> &scopes,
                               const std::unordered_map<std::string, ValueType> &globals,
                               const std::unordered_map<std::string, FunctionInfo> &functions,
                               const SemanticTypeInfo &expected_return_type, const std::string &function_name,
                               std::vector<std::string> &diagnostics, int loop_depth, int switch_depth,
                               int defer_loop_base_depth, int defer_switch_base_depth,
                               bool inside_defer_body,
                               std::size_t max_message_send_args,
                               const Objc3MessageSendResolutionContext &message_send_context);

static void ValidateStatement(
    const Stmt *stmt, std::vector<SemanticScope> &scopes,
    const std::unordered_map<std::string, ValueType> &globals,
    const std::unordered_map<std::string, FunctionInfo> &functions,
    const SemanticTypeInfo &expected_return_type,
    const std::string &function_name, std::vector<std::string> &diagnostics,
    int loop_depth, int switch_depth, int defer_loop_base_depth,
    int defer_switch_base_depth, bool inside_defer_body,
    std::size_t max_message_send_args,
    const Objc3MessageSendResolutionContext &message_send_context);

static void ValidateStatementsFromIndex(
    const std::vector<std::unique_ptr<Stmt>> &statements, std::size_t start_index,
    std::vector<SemanticScope> &scopes,
    const std::unordered_map<std::string, ValueType> &globals,
    const std::unordered_map<std::string, FunctionInfo> &functions,
    const SemanticTypeInfo &expected_return_type, const std::string &function_name,
    std::vector<std::string> &diagnostics, int loop_depth, int switch_depth,
    int defer_loop_base_depth, int defer_switch_base_depth,
    bool inside_defer_body,
    std::size_t max_message_send_args,
    const Objc3MessageSendResolutionContext &message_send_context) {
  for (std::size_t index = start_index; index < statements.size(); ++index) {
    ValidateStatement(statements[index].get(), scopes, globals, functions,
                      expected_return_type, function_name, diagnostics,
                      loop_depth, switch_depth, defer_loop_base_depth,
                      defer_switch_base_depth, inside_defer_body,
                      max_message_send_args,
                      message_send_context);
  }
}

#include "sema/objc3_semantic_passes_expression_statement_validation_assignment.inc"

#include "sema/objc3_semantic_passes_expression_statement_validation_atomic_mappings.inc"

#include "sema/objc3_semantic_passes_expression_statement_validation_statement_flow.inc"

static void ValidateStatement(const Stmt *stmt, std::vector<SemanticScope> &scopes,
                              const std::unordered_map<std::string, ValueType> &globals,
                              const std::unordered_map<std::string, FunctionInfo> &functions,
                              const SemanticTypeInfo &expected_return_type, const std::string &function_name,
                              std::vector<std::string> &diagnostics, int loop_depth, int switch_depth,
                              int defer_loop_base_depth, int defer_switch_base_depth,
                              bool inside_defer_body,
                              std::size_t max_message_send_args,
                              const Objc3MessageSendResolutionContext &message_send_context) {
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
        (void)ValidateExpr(clause.value.get(), scopes, globals, functions, diagnostics,
                           max_message_send_args, message_send_context);
        return;
      case ForClause::Kind::Let: {
        if (scopes.empty()) {
          return;
        }
        const SemanticTypeInfo value_type = ValidateExpr(
            clause.value.get(), scopes, globals, functions, diagnostics,
            max_message_send_args, message_send_context);
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
        const SemanticTypeInfo value_type = ValidateExpr(
            clause.value.get(), scopes, globals, functions, diagnostics,
            max_message_send_args, message_send_context);
        ValidateAssignmentCompatibility(clause.name, clause.op, clause.value.get(), clause.line, clause.column,
                                        found_target, target_type, value_type,
                                        diagnostics, message_send_context);
        return;
      }
    }
  };

  switch (stmt->kind) {
#include "sema/objc3_semantic_passes_expression_statement_validation_statement_basic_cases.inc"
#include "sema/objc3_semantic_passes_expression_statement_validation_statement_if_case.inc"
