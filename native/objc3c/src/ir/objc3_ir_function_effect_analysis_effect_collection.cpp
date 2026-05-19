#include "ir/objc3_ir_function_effect_analysis_effect_collection.h"

#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_function_effect_analysis.h"

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

bool IsGlobalSymbolWriteTarget(
    const std::string &name, const ScopeStack &scopes,
    const std::unordered_set<std::string> &global_symbols) {
  if (name.empty() || IsNameBoundInScopes(scopes, name)) {
    return false;
  }
  return global_symbols.find(name) != global_symbols.end();
}

void CollectFunctionEffectExpr(const Expr *expr, ScopeStack &scopes,
                               FunctionEffectInfo &info) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
    case Expr::Kind::Number:
    case Expr::Kind::BoolLiteral:
    case Expr::Kind::NilLiteral:
    case Expr::Kind::Identifier:
    case Expr::Kind::KeyPathLiteral:
    case Expr::Kind::BlockLiteral:
      return;
    case Expr::Kind::Binary:
      CollectFunctionEffectExpr(expr->left.get(), scopes, info);
      CollectFunctionEffectExpr(expr->right.get(), scopes, info);
      return;
    case Expr::Kind::Conditional:
      CollectFunctionEffectExpr(expr->left.get(), scopes, info);
      CollectFunctionEffectExpr(expr->right.get(), scopes, info);
      CollectFunctionEffectExpr(expr->third.get(), scopes, info);
      return;
    case Expr::Kind::Call:
      info.called_functions.insert(expr->ident);
      for (const auto &arg : expr->args) {
        CollectFunctionEffectExpr(arg.get(), scopes, info);
      }
      return;
    case Expr::Kind::Try:
    case Expr::Kind::Throw:
      for (const auto &arg : expr->args) {
        CollectFunctionEffectExpr(arg.get(), scopes, info);
      }
      return;
    case Expr::Kind::MessageSend:
      info.has_message_send = true;
      CollectFunctionEffectExpr(expr->receiver.get(), scopes, info);
      for (const auto &arg : expr->args) {
        CollectFunctionEffectExpr(arg.get(), scopes, info);
      }
      return;
  }
}

void CollectFunctionEffectForClause(
    const ForClause &clause, ScopeStack &scopes, FunctionEffectInfo &info,
    const std::unordered_set<std::string> &global_symbols) {
  switch (clause.kind) {
    case ForClause::Kind::None:
      return;
    case ForClause::Kind::Expr:
      CollectFunctionEffectExpr(clause.value.get(), scopes, info);
      return;
    case ForClause::Kind::Let:
      CollectFunctionEffectExpr(clause.value.get(), scopes, info);
      if (!scopes.empty() && !clause.name.empty()) {
        scopes.back().insert(clause.name);
      }
      return;
    case ForClause::Kind::Assign:
      if (IsGlobalSymbolWriteTarget(clause.name, scopes, global_symbols)) {
        info.has_global_write = true;
      }
      CollectFunctionEffectExpr(clause.value.get(), scopes, info);
      return;
  }
}

void CollectFunctionEffectStmt(
    const Stmt *stmt, ScopeStack &scopes, FunctionEffectInfo &info,
    const std::unordered_set<std::string> &global_symbols) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (stmt->let_stmt == nullptr) {
        return;
      }
      CollectFunctionEffectExpr(stmt->let_stmt->value.get(), scopes, info);
      if (!scopes.empty() && !stmt->let_stmt->name.empty()) {
        scopes.back().insert(stmt->let_stmt->name);
      }
      return;
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt == nullptr) {
        return;
      }
      if (IsGlobalSymbolWriteTarget(stmt->assign_stmt->name, scopes,
                                    global_symbols)) {
        info.has_global_write = true;
      }
      CollectFunctionEffectExpr(stmt->assign_stmt->value.get(), scopes, info);
      return;
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        CollectFunctionEffectExpr(stmt->return_stmt->value.get(), scopes, info);
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        CollectFunctionEffectExpr(stmt->expr_stmt->value.get(), scopes, info);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt == nullptr) {
        return;
      }
      CollectFunctionEffectExpr(stmt->if_stmt->condition.get(), scopes, info);
      scopes.push_back({});
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        CollectFunctionEffectStmt(
            then_stmt.get(), scopes, info, global_symbols);
      }
      scopes.pop_back();
      scopes.push_back({});
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        CollectFunctionEffectStmt(
            else_stmt.get(), scopes, info, global_symbols);
      }
      scopes.pop_back();
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      for (const auto &loop_stmt : stmt->do_while_stmt->body) {
        CollectFunctionEffectStmt(
            loop_stmt.get(), scopes, info, global_symbols);
      }
      scopes.pop_back();
      CollectFunctionEffectExpr(
          stmt->do_while_stmt->condition.get(), scopes, info);
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      CollectFunctionEffectForClause(
          stmt->for_stmt->init, scopes, info, global_symbols);
      CollectFunctionEffectExpr(
          stmt->for_stmt->condition.get(), scopes, info);
      scopes.push_back({});
      for (const auto &loop_stmt : stmt->for_stmt->body) {
        CollectFunctionEffectStmt(
            loop_stmt.get(), scopes, info, global_symbols);
      }
      scopes.pop_back();
      CollectFunctionEffectForClause(
          stmt->for_stmt->step, scopes, info, global_symbols);
      scopes.pop_back();
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt == nullptr) {
        return;
      }
      CollectFunctionEffectExpr(stmt->switch_stmt->condition.get(), scopes,
                                info);
      for (const auto &case_stmt : stmt->switch_stmt->cases) {
        scopes.push_back({});
        for (const auto &case_body_stmt : case_stmt.body) {
          CollectFunctionEffectStmt(
              case_body_stmt.get(), scopes, info, global_symbols);
        }
        scopes.pop_back();
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt == nullptr) {
        return;
      }
      CollectFunctionEffectExpr(stmt->while_stmt->condition.get(), scopes,
                                info);
      scopes.push_back({});
      for (const auto &loop_stmt : stmt->while_stmt->body) {
        CollectFunctionEffectStmt(
            loop_stmt.get(), scopes, info, global_symbols);
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
        CollectFunctionEffectStmt(
            nested_stmt.get(), scopes, info, global_symbols);
      }
      scopes.pop_back();
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

}  // namespace

void CollectObjc3IRFunctionEffects(
    const Objc3IRFunctionEffectAnalysisOptions &options,
    Objc3IRFunctionEffectAnalysis &analysis) {
  for (const FunctionDecl *fn : options.function_definitions) {
    if (fn == nullptr) {
      continue;
    }
    FunctionEffectInfo info;
    ScopeStack scopes;
    scopes.push_back({});
    for (const auto &param : fn->params) {
      scopes.back().insert(param.name);
    }
    for (const auto &stmt : fn->body) {
      CollectFunctionEffectStmt(
          stmt.get(), scopes, info, options.global_symbols);
    }
    analysis.function_effects[fn->name] = std::move(info);
  }

  for (const auto &entry : analysis.function_effects) {
    const FunctionEffectInfo &info = entry.second;
    if (info.has_global_write || info.has_message_send) {
      analysis.impure_functions.insert(entry.first);
    }
  }

  bool changed = true;
  while (changed) {
    changed = false;
    for (const auto &entry : analysis.function_effects) {
      const std::string &name = entry.first;
      if (analysis.impure_functions.find(name) !=
          analysis.impure_functions.end()) {
        continue;
      }
      for (const std::string &callee : entry.second.called_functions) {
        const bool callee_defined =
            options.defined_functions.find(callee) !=
            options.defined_functions.end();
        const bool callee_declared_pure =
            options.declared_pure_functions.find(callee) !=
            options.declared_pure_functions.end();
        if ((!callee_defined && !callee_declared_pure) ||
            analysis.impure_functions.find(callee) !=
                analysis.impure_functions.end()) {
          analysis.impure_functions.insert(name);
          changed = true;
          break;
        }
      }
    }
  }
}
