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
    const Objc3ErrorHandlingSemanticWalkContext &context);

#include "sema/objc3_semantic_error_handling_try_do_catch_diagnostics.inc"
#include "sema/objc3_semantic_error_handling_try_do_catch_operand_surface.inc"
#include "sema/objc3_semantic_error_handling_try_do_catch_do_scope.inc"
#include "sema/objc3_semantic_error_handling_try_do_catch_statement_walk.inc"
#include "sema/objc3_semantic_error_handling_try_do_catch_expression_walk.inc"
#include "sema/objc3_semantic_error_handling_try_do_catch_readiness.inc"

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

  FinalizeErrorHandlingTryDoCatchSummary(summary,
                                         allow_source_only_error_runtime_surface);
  return summary;
}
