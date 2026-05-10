#include "sema/objc3_semantic_passes.h"

#include "sema/objc3_semantic_error_handling_bridge_helpers.h"

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

namespace {

struct Objc3ErrorHandlingSemanticWalkContext {
  bool in_throws_callable = false;
  bool local_handler_active = false;
  bool in_catch_body = false;
};

static void ResolveMessageSendErrorSurface(const Objc3Program &program,
                                           const std::string &selector,
                                           bool &throwing,
                                           bool &bridged) {
  const auto inspect_method = [&](const Objc3MethodDecl &method) {
    if (method.selector != selector) {
      return;
    }
    throwing = throwing || method.throws_declared;
    bridged =
        bridged || CallableHasSemanticallyValidNSErrorStatusBridge(method, program);
  };
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method : protocol_decl.methods) {
      inspect_method(method);
    }
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      inspect_method(method);
    }
  }
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &method : implementation_decl.methods) {
      inspect_method(method);
    }
  }
}

static void ResolveTryOperandSurface(const Expr *operand,
                                     const Objc3Program &program,
                                     bool &throwing_callable,
                                     bool &bridged_callable) {
  if (operand == nullptr) {
    return;
  }
  if (operand->kind == Expr::Kind::Call) {
    if (const FunctionDecl *fn =
            FindFunctionDeclByName(program, operand->ident)) {
      throwing_callable = fn->throws_declared;
      bridged_callable =
          CallableHasSemanticallyValidNSErrorStatusBridge(*fn, program);
    }
    return;
  }
  if (operand->kind == Expr::Kind::MessageSend) {
    ResolveMessageSendErrorSurface(program, operand->selector, throwing_callable,
                                   bridged_callable);
  }
}

static void WalkErrorHandlingTryDoCatchExpr(
    const Expr *expr,
    const Objc3Program &program,
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    std::vector<std::string> &diagnostics,
    bool allow_source_only_error_runtime_surface,
    const Objc3ErrorHandlingSemanticWalkContext &context);

static void WalkErrorHandlingTryDoCatchStmt(
    const Stmt *stmt,
    const Objc3Program &program,
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    std::vector<std::string> &diagnostics,
    bool allow_source_only_error_runtime_surface,
    const Objc3ErrorHandlingSemanticWalkContext &context) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
  case Stmt::Kind::Let:
    WalkErrorHandlingTryDoCatchExpr(
        stmt->let_stmt != nullptr ? stmt->let_stmt->value.get() : nullptr,
        program, summary, diagnostics, allow_source_only_error_runtime_surface,
        context);
    return;
  case Stmt::Kind::Assign:
    WalkErrorHandlingTryDoCatchExpr(
        stmt->assign_stmt != nullptr ? stmt->assign_stmt->value.get() : nullptr,
        program, summary, diagnostics, allow_source_only_error_runtime_surface,
        context);
    return;
  case Stmt::Kind::Return:
    WalkErrorHandlingTryDoCatchExpr(
        stmt->return_stmt != nullptr ? stmt->return_stmt->value.get() : nullptr,
        program, summary, diagnostics, allow_source_only_error_runtime_surface,
        context);
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    WalkErrorHandlingTryDoCatchExpr(
        stmt->if_stmt->condition.get(), program, summary, diagnostics,
        allow_source_only_error_runtime_surface, context);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      WalkErrorHandlingTryDoCatchStmt(then_stmt.get(), program, summary,
                                      diagnostics,
                                      allow_source_only_error_runtime_surface,
                                      context);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      WalkErrorHandlingTryDoCatchStmt(else_stmt.get(), program, summary,
                                      diagnostics,
                                      allow_source_only_error_runtime_surface,
                                      context);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      WalkErrorHandlingTryDoCatchStmt(body_stmt.get(), program, summary,
                                      diagnostics,
                                      allow_source_only_error_runtime_surface,
                                      context);
    }
    WalkErrorHandlingTryDoCatchExpr(
        stmt->do_while_stmt->condition.get(), program, summary, diagnostics,
        allow_source_only_error_runtime_surface, context);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    WalkErrorHandlingTryDoCatchExpr(
        stmt->for_stmt->init.value.get(), program, summary, diagnostics,
        allow_source_only_error_runtime_surface, context);
    WalkErrorHandlingTryDoCatchExpr(
        stmt->for_stmt->condition.get(), program, summary, diagnostics,
        allow_source_only_error_runtime_surface, context);
    WalkErrorHandlingTryDoCatchExpr(
        stmt->for_stmt->step.value.get(), program, summary, diagnostics,
        allow_source_only_error_runtime_surface, context);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      WalkErrorHandlingTryDoCatchStmt(body_stmt.get(), program, summary,
                                      diagnostics,
                                      allow_source_only_error_runtime_surface,
                                      context);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    WalkErrorHandlingTryDoCatchExpr(
        stmt->switch_stmt->condition.get(), program, summary, diagnostics,
        allow_source_only_error_runtime_surface, context);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        WalkErrorHandlingTryDoCatchStmt(case_stmt.get(), program, summary,
                                        diagnostics,
                                        allow_source_only_error_runtime_surface,
                                        context);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    WalkErrorHandlingTryDoCatchExpr(
        stmt->while_stmt->condition.get(), program, summary, diagnostics,
        allow_source_only_error_runtime_surface, context);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      WalkErrorHandlingTryDoCatchStmt(body_stmt.get(), program, summary,
                                      diagnostics,
                                      allow_source_only_error_runtime_surface,
                                      context);
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    if (stmt->block_stmt->is_do_catch_scope) {
      ++summary.do_catch_sites;
      summary.catch_clause_sites += stmt->block_stmt->catch_clauses.size();
      for (const auto &clause : stmt->block_stmt->catch_clauses) {
        if (clause.catch_all) {
          ++summary.catch_all_sites;
        }
        if (clause.has_binding) {
          ++summary.catch_binding_sites;
        }
      }
      if (!allow_source_only_error_runtime_surface) {
        RecordErrorHandlingSemanticDiagnostic(
            summary.contract_violation_sites, stmt->line, stmt->column,
            "O3S267",
            "unsupported feature claim: do/catch statements are not yet runnable in Objective-C 3 native mode",
            diagnostics);
      }
      if (stmt->block_stmt->catch_clauses.empty()) {
        RecordErrorHandlingSemanticDiagnostic(
            summary.contract_violation_sites, stmt->line, stmt->column,
            "O3S268", "do/catch requires at least one catch clause",
            diagnostics);
      }
      bool saw_catch_all = false;
      for (std::size_t index = 0;
           index < stmt->block_stmt->catch_clauses.size(); ++index) {
        const auto &clause = stmt->block_stmt->catch_clauses[index];
        if (saw_catch_all) {
          RecordErrorHandlingSemanticDiagnostic(
              summary.contract_violation_sites, clause.line, clause.column,
              "O3S269", "catch clauses after a catch-all are unreachable",
              diagnostics);
        }
        if (clause.catch_all) {
          saw_catch_all = true;
        }
      }

      Objc3ErrorHandlingSemanticWalkContext body_context = context;
      body_context.local_handler_active = true;
      for (const auto &body_stmt : stmt->block_stmt->body) {
        WalkErrorHandlingTryDoCatchStmt(body_stmt.get(), program, summary,
                                        diagnostics,
                                        allow_source_only_error_runtime_surface,
                                        body_context);
      }
      for (const auto &clause : stmt->block_stmt->catch_clauses) {
        Objc3ErrorHandlingSemanticWalkContext catch_context = context;
        catch_context.local_handler_active = false;
        catch_context.in_catch_body = true;
        for (const auto &body_stmt : clause.body) {
          WalkErrorHandlingTryDoCatchStmt(
              body_stmt.get(), program, summary, diagnostics,
              allow_source_only_error_runtime_surface, catch_context);
        }
      }
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      WalkErrorHandlingTryDoCatchStmt(body_stmt.get(), program, summary,
                                      diagnostics,
                                      allow_source_only_error_runtime_surface,
                                      context);
    }
    return;
  case Stmt::Kind::Expr:
    WalkErrorHandlingTryDoCatchExpr(
        stmt->expr_stmt != nullptr ? stmt->expr_stmt->value.get() : nullptr,
        program, summary, diagnostics, allow_source_only_error_runtime_surface,
        context);
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

static void WalkErrorHandlingTryDoCatchExpr(
    const Expr *expr,
    const Objc3Program &program,
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    std::vector<std::string> &diagnostics,
    bool allow_source_only_error_runtime_surface,
    const Objc3ErrorHandlingSemanticWalkContext &context) {
  if (expr == nullptr) {
    return;
  }

  if (expr->try_expression_enabled) {
    ++summary.try_expression_sites;
    switch (expr->try_operator_kind) {
    case Expr::TryOperatorKind::Propagate:
      ++summary.try_propagating_sites;
      break;
    case Expr::TryOperatorKind::Optional:
      ++summary.try_optional_sites;
      break;
    case Expr::TryOperatorKind::Forced:
      ++summary.try_forced_sites;
      break;
    case Expr::TryOperatorKind::None:
      break;
    }

    if (!allow_source_only_error_runtime_surface) {
      RecordErrorHandlingSemanticDiagnostic(
          summary.contract_violation_sites, expr->line, expr->column, "O3S270",
          "unsupported feature claim: try expressions are not yet runnable in Objective-C 3 native mode",
          diagnostics);
    }

    const Expr *operand =
        !expr->args.empty() ? expr->args.front().get() : expr->left.get();
    bool throwing_callable = false;
    bool bridged_callable = false;
    ResolveTryOperandSurface(operand, program, throwing_callable,
                             bridged_callable);
    if (throwing_callable) {
      ++summary.throwing_callable_try_sites;
    }
    if (bridged_callable) {
      ++summary.bridged_callable_try_sites;
    }
    if (!throwing_callable && !bridged_callable) {
      RecordErrorHandlingSemanticDiagnostic(
          summary.contract_violation_sites, expr->line, expr->column, "O3S271",
          "try operand must be a throwing or NSError-bridged call surface",
          diagnostics);
    }
    if (expr->try_expression_requires_throwing_context) {
      if (context.local_handler_active) {
        ++summary.local_handler_sites;
      } else if (context.in_throws_callable) {
        ++summary.caller_propagation_sites;
      } else {
        RecordErrorHandlingSemanticDiagnostic(
            summary.contract_violation_sites, expr->line, expr->column,
            "O3S272",
            "propagating try requires a throws function or an enclosing do/catch",
            diagnostics);
      }
    }
  }

  if (expr->throw_statement_enabled) {
    ++summary.throw_statement_sites;
    if (context.in_catch_body) {
      ++summary.rethrow_sites;
    }
    if (!allow_source_only_error_runtime_surface) {
      RecordErrorHandlingSemanticDiagnostic(
          summary.contract_violation_sites, expr->line, expr->column, "O3S273",
          "unsupported feature claim: throw statements are not yet runnable in Objective-C 3 native mode",
          diagnostics);
    }
    if (!(context.in_throws_callable || context.in_catch_body)) {
      RecordErrorHandlingSemanticDiagnostic(
          summary.contract_violation_sites, expr->line, expr->column, "O3S274",
          "throw statements require a throws function or a catch body",
          diagnostics);
    }
  }

  WalkErrorHandlingTryDoCatchExpr(expr->receiver.get(), program, summary,
                                  diagnostics,
                                  allow_source_only_error_runtime_surface,
                                  context);
  WalkErrorHandlingTryDoCatchExpr(expr->left.get(), program, summary,
                                  diagnostics,
                                  allow_source_only_error_runtime_surface,
                                  context);
  WalkErrorHandlingTryDoCatchExpr(expr->right.get(), program, summary,
                                  diagnostics,
                                  allow_source_only_error_runtime_surface,
                                  context);
  WalkErrorHandlingTryDoCatchExpr(expr->third.get(), program, summary,
                                  diagnostics,
                                  allow_source_only_error_runtime_surface,
                                  context);
  for (const auto &arg : expr->args) {
    WalkErrorHandlingTryDoCatchExpr(arg.get(), program, summary, diagnostics,
                                    allow_source_only_error_runtime_surface,
                                    context);
  }
}

} // namespace

Objc3ErrorHandlingTryDoCatchSemanticSummary
BuildErrorHandlingTryDoCatchSemanticSummary(
    const Objc3Program &program,
    const Objc3SemanticIntegrationSurface &surface,
    bool allow_source_only_error_runtime_surface,
    std::vector<std::string> &diagnostics) {
  (void)surface;
  Objc3ErrorHandlingTryDoCatchSemanticSummary summary;
  for (const auto &fn : program.functions) {
    Objc3ErrorHandlingSemanticWalkContext context;
    context.in_throws_callable = fn.throws_declared;
    for (const auto &stmt : fn.body) {
      WalkErrorHandlingTryDoCatchStmt(stmt.get(), program, summary, diagnostics,
                                      allow_source_only_error_runtime_surface,
                                      context);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      Objc3ErrorHandlingSemanticWalkContext context;
      context.in_throws_callable = method.throws_declared;
      for (const auto &stmt : method.body) {
        WalkErrorHandlingTryDoCatchStmt(stmt.get(), program, summary,
                                        diagnostics,
                                        allow_source_only_error_runtime_surface,
                                        context);
      }
    }
  }

  summary.try_surface_landed =
      summary.try_expression_sites ==
          summary.try_propagating_sites + summary.try_optional_sites +
              summary.try_forced_sites &&
      summary.throwing_callable_try_sites <= summary.try_expression_sites &&
      summary.bridged_callable_try_sites <= summary.try_expression_sites;
  summary.throw_surface_landed =
      summary.rethrow_sites <= summary.throw_statement_sites;
  summary.do_catch_surface_landed =
      summary.catch_binding_sites <= summary.catch_clause_sites &&
      summary.catch_all_sites <= summary.catch_clause_sites;
  summary.throwing_context_legality_enforced = true;
  summary.deterministic =
      summary.try_surface_landed && summary.throw_surface_landed &&
      summary.do_catch_surface_landed &&
      summary.contract_violation_sites <=
          summary.try_expression_sites + summary.throw_statement_sites +
              summary.do_catch_sites + summary.catch_clause_sites;
  summary.ready_for_lowering_and_runtime = false;

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";try=" << summary.try_expression_sites
      << ";throw=" << summary.throw_statement_sites
      << ";do-catch=" << summary.do_catch_sites
      << ";violations=" << summary.contract_violation_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}
