#include "sema/objc3_semantic_passes_concurrency_task_callable_models.inc"
#include "sema/objc3_semantic_passes_concurrency_task_callable_site_collection.inc"
#include "sema/objc3_semantic_passes_concurrency_task_callable_diagnostics.inc"

static void DiagnoseConcurrencyTaskCallableLegality(
    const std::vector<std::unique_ptr<Stmt>> &body, bool inside_async_context,
    std::vector<std::string> &diagnostics) {
  Objc3ConcurrencyTaskCallableLegalityProfile profile;
  for (const auto &stmt : body) {
    CollectConcurrencyTaskCallableLegalityStmtSites(stmt.get(), profile);
  }
  for (const auto &stmt : body) {
    DiagnoseConcurrencyTaskCallableLegalityStmt(stmt.get(), profile,
                                          inside_async_context, diagnostics);
  }
}

static Objc3ConcurrencyTaskCallableLegalityProfile
BuildConcurrencyTaskCallableLegalityProfile(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3ConcurrencyTaskCallableLegalityProfile profile;
  for (const auto &stmt : body) {
    CollectConcurrencyTaskCallableLegalityStmtSites(stmt.get(), profile);
  }
  return profile;
}
