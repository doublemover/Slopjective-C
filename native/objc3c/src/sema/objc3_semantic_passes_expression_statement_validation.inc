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
#include "sema/objc3_semantic_passes_expression_statement_validation_statement_local_helpers.inc"

  switch (stmt->kind) {
#include "sema/objc3_semantic_passes_expression_statement_validation_statement_basic_cases.inc"
#include "sema/objc3_semantic_passes_expression_statement_validation_statement_if_case.inc"
