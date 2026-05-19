#include "ir/objc3_ir_function_effect_analysis.h"

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_function_effect_analysis_effect_collection.h"

namespace {

using ScopeStack = std::vector<std::unordered_set<std::string>>;

bool IsNameBoundInScopes(const ScopeStack &scopes, const std::string &name) {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    if (it->find(name) != it->end()) {
      return true;
    }
  }
  return false;
}

void NotePotentialGlobalMutation(
    const std::string &name, const ScopeStack &scopes,
    const std::unordered_set<std::string> &global_symbols,
    std::unordered_set<std::string> &mutable_global_symbols) {
  if (name.empty() || IsNameBoundInScopes(scopes, name)) {
    return;
  }
  if (global_symbols.find(name) != global_symbols.end()) {
    mutable_global_symbols.insert(name);
  }
}

void CollectMutableGlobalSymbolsForClause(
    const ForClause &clause, ScopeStack &scopes,
    const std::unordered_set<std::string> &global_symbols,
    std::unordered_set<std::string> &mutable_global_symbols) {
  switch (clause.kind) {
    case ForClause::Kind::None:
    case ForClause::Kind::Expr:
      return;
    case ForClause::Kind::Let:
      if (!scopes.empty() && !clause.name.empty()) {
        scopes.back().insert(clause.name);
      }
      return;
    case ForClause::Kind::Assign:
      NotePotentialGlobalMutation(
          clause.name, scopes, global_symbols, mutable_global_symbols);
      return;
  }
}

void CollectMutableGlobalSymbolsStmt(
    const Stmt *stmt, ScopeStack &scopes,
    const std::unordered_set<std::string> &global_symbols,
    std::unordered_set<std::string> &mutable_global_symbols) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (stmt->let_stmt != nullptr && !stmt->let_stmt->name.empty() &&
          !scopes.empty()) {
        scopes.back().insert(stmt->let_stmt->name);
      }
      return;
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        NotePotentialGlobalMutation(stmt->assign_stmt->name, scopes,
                                    global_symbols, mutable_global_symbols);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        CollectMutableGlobalSymbolsStmt(
            then_stmt.get(), scopes, global_symbols, mutable_global_symbols);
      }
      scopes.pop_back();
      scopes.push_back({});
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        CollectMutableGlobalSymbolsStmt(
            else_stmt.get(), scopes, global_symbols, mutable_global_symbols);
      }
      scopes.pop_back();
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      for (const auto &loop_stmt : stmt->do_while_stmt->body) {
        CollectMutableGlobalSymbolsStmt(
            loop_stmt.get(), scopes, global_symbols, mutable_global_symbols);
      }
      scopes.pop_back();
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      CollectMutableGlobalSymbolsForClause(
          stmt->for_stmt->init, scopes, global_symbols,
          mutable_global_symbols);
      scopes.push_back({});
      for (const auto &loop_stmt : stmt->for_stmt->body) {
        CollectMutableGlobalSymbolsStmt(
            loop_stmt.get(), scopes, global_symbols, mutable_global_symbols);
      }
      scopes.pop_back();
      CollectMutableGlobalSymbolsForClause(
          stmt->for_stmt->step, scopes, global_symbols,
          mutable_global_symbols);
      scopes.pop_back();
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt == nullptr) {
        return;
      }
      for (const auto &case_stmt : stmt->switch_stmt->cases) {
        scopes.push_back({});
        for (const auto &case_body_stmt : case_stmt.body) {
          CollectMutableGlobalSymbolsStmt(case_body_stmt.get(), scopes,
                                          global_symbols,
                                          mutable_global_symbols);
        }
        scopes.pop_back();
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      for (const auto &loop_stmt : stmt->while_stmt->body) {
        CollectMutableGlobalSymbolsStmt(
            loop_stmt.get(), scopes, global_symbols, mutable_global_symbols);
      }
      scopes.pop_back();
      return;
    case Stmt::Kind::Block:
    case Stmt::Kind::Defer:
      if (stmt->block_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      for (const auto &nested_stmt : stmt->block_stmt->body) {
        CollectMutableGlobalSymbolsStmt(
            nested_stmt.get(), scopes, global_symbols, mutable_global_symbols);
      }
      scopes.pop_back();
      return;
    case Stmt::Kind::Return:
    case Stmt::Kind::Expr:
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

void CollectMutableGlobalSymbols(
    const Objc3IRFunctionEffectAnalysisOptions &options,
    Objc3IRFunctionEffectAnalysis &analysis) {
  for (const FunctionDecl *fn : options.function_definitions) {
    if (fn == nullptr) {
      continue;
    }
    ScopeStack scopes;
    scopes.push_back({});
    for (const auto &param : fn->params) {
      scopes.back().insert(param.name);
    }
    for (const auto &stmt : fn->body) {
      CollectMutableGlobalSymbolsStmt(
          stmt.get(), scopes, options.global_symbols,
          analysis.mutable_global_symbols);
    }
  }
}

}  // namespace

Objc3IRFunctionEffectAnalysis BuildObjc3IRFunctionEffectAnalysis(
    const Objc3IRFunctionEffectAnalysisOptions &options) {
  Objc3IRFunctionEffectAnalysis analysis;
  CollectMutableGlobalSymbols(options, analysis);
  CollectObjc3IRFunctionEffects(options, analysis);
  return analysis;
}

bool Objc3IRFunctionMayHaveGlobalSideEffects(
    const std::string &name,
    const std::unordered_set<std::string> &defined_functions,
    const std::unordered_set<std::string> &declared_pure_functions,
    const std::unordered_set<std::string> &impure_functions) {
  if (name.empty()) {
    return true;
  }
  if (defined_functions.find(name) == defined_functions.end()) {
    return declared_pure_functions.find(name) == declared_pure_functions.end();
  }
  return impure_functions.find(name) != impure_functions.end();
}
