#include "sema/objc3_semantic_passes.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

static std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message) {
  std::ostringstream out;
  out << "error:" << line << ":" << column << ": " << message << " [" << code << "]";
  return out.str();
}

struct PureContractEffectInfo {
  struct SourceLoc {
    unsigned line = 1;
    unsigned column = 1;
    bool present = false;
  };

  SourceLoc global_write_site;
  SourceLoc message_send_site;
  std::unordered_map<std::string, SourceLoc> called_functions;
};

struct PureContractCause {
  std::string token;
  unsigned line = 1;
  unsigned column = 1;
  bool present = false;
  std::string detail_token;
  unsigned detail_line = 1;
  unsigned detail_column = 1;
  bool detail_present = false;
};

static bool IsEarlierPureContractSourceLoc(unsigned line, unsigned column, const PureContractEffectInfo::SourceLoc &loc) {
  if (!loc.present) {
    return true;
  }
  if (line < loc.line) {
    return true;
  }
  if (line > loc.line) {
    return false;
  }
  return column < loc.column;
}

static void RecordPureContractSourceLoc(PureContractEffectInfo::SourceLoc &loc, unsigned line, unsigned column) {
  if (!IsEarlierPureContractSourceLoc(line, column, loc)) {
    return;
  }
  loc.line = line;
  loc.column = column;
  loc.present = true;
}

static std::vector<std::string> SortedPureContractNames(const std::unordered_map<std::string, PureContractEffectInfo::SourceLoc> &names) {
  std::vector<std::string> ordered;
  ordered.reserve(names.size());
  for (const auto &entry : names) {
    ordered.push_back(entry.first);
  }
  std::sort(ordered.begin(), ordered.end());
  return ordered;
}

static PureContractCause DetermineDirectPureContractImpurityCause(const PureContractEffectInfo &info) {
  PureContractCause cause;
  if (info.global_write_site.present) {
    cause.token = "global-write";
    cause.line = info.global_write_site.line;
    cause.column = info.global_write_site.column;
    cause.present = true;
    cause.detail_token = cause.token;
    cause.detail_line = cause.line;
    cause.detail_column = cause.column;
    cause.detail_present = true;
    return cause;
  }
  if (info.message_send_site.present) {
    cause.token = "message-send";
    cause.line = info.message_send_site.line;
    cause.column = info.message_send_site.column;
    cause.present = true;
    cause.detail_token = cause.token;
    cause.detail_line = cause.line;
    cause.detail_column = cause.column;
    cause.detail_present = true;
    return cause;
  }
  return cause;
}

static bool IsBetterPureContractCause(const PureContractCause &candidate, const PureContractCause &current) {
  if (!candidate.present) {
    return false;
  }
  if (!current.present) {
    return true;
  }
  if (candidate.token != current.token) {
    return candidate.token < current.token;
  }
  if (candidate.line != current.line) {
    return candidate.line < current.line;
  }
  return candidate.column < current.column;
}

static bool IsNameBoundInSemanticScopes(const std::vector<std::unordered_set<std::string>> &scopes,
                                        const std::string &name) {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    if (it->find(name) != it->end()) {
      return true;
    }
  }
  return false;
}

static bool IsPureContractGlobalWriteTarget(const std::string &name,
                                            const std::vector<std::unordered_set<std::string>> &scopes,
                                            const std::unordered_set<std::string> &globals) {
  if (name.empty() || IsNameBoundInSemanticScopes(scopes, name)) {
    return false;
  }
  return globals.find(name) != globals.end();
}

static void CollectPureContractEffectExpr(const Expr *expr, std::vector<std::unordered_set<std::string>> &scopes,
                                          PureContractEffectInfo &info);

static void CollectPureContractEffectForClause(const ForClause &clause,
                                               std::vector<std::unordered_set<std::string>> &scopes,
                                               const std::unordered_set<std::string> &globals,
                                               PureContractEffectInfo &info) {
  switch (clause.kind) {
    case ForClause::Kind::None:
      return;
    case ForClause::Kind::Expr:
      CollectPureContractEffectExpr(clause.value.get(), scopes, info);
      return;
    case ForClause::Kind::Let:
      CollectPureContractEffectExpr(clause.value.get(), scopes, info);
      if (!scopes.empty() && !clause.name.empty()) {
        scopes.back().insert(clause.name);
      }
      return;
    case ForClause::Kind::Assign:
      if (IsPureContractGlobalWriteTarget(clause.name, scopes, globals)) {
        RecordPureContractSourceLoc(info.global_write_site, clause.line, clause.column);
      }
      CollectPureContractEffectExpr(clause.value.get(), scopes, info);
      return;
  }
}

static void CollectPureContractEffectStmt(const Stmt *stmt, std::vector<std::unordered_set<std::string>> &scopes,
                                          const std::unordered_set<std::string> &globals, PureContractEffectInfo &info) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (stmt->let_stmt == nullptr) {
        return;
      }
      CollectPureContractEffectExpr(stmt->let_stmt->value.get(), scopes, info);
      if (!scopes.empty() && !stmt->let_stmt->name.empty()) {
        scopes.back().insert(stmt->let_stmt->name);
      }
      return;
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt == nullptr) {
        return;
      }
      if (IsPureContractGlobalWriteTarget(stmt->assign_stmt->name, scopes, globals)) {
        RecordPureContractSourceLoc(info.global_write_site, stmt->assign_stmt->line, stmt->assign_stmt->column);
      }
      CollectPureContractEffectExpr(stmt->assign_stmt->value.get(), scopes, info);
      return;
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        CollectPureContractEffectExpr(stmt->return_stmt->value.get(), scopes, info);
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        CollectPureContractEffectExpr(stmt->expr_stmt->value.get(), scopes, info);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt == nullptr) {
        return;
      }
      CollectPureContractEffectExpr(stmt->if_stmt->condition.get(), scopes, info);
      scopes.push_back({});
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        CollectPureContractEffectStmt(then_stmt.get(), scopes, globals, info);
      }
      scopes.pop_back();
      scopes.push_back({});
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        CollectPureContractEffectStmt(else_stmt.get(), scopes, globals, info);
      }
      scopes.pop_back();
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      for (const auto &loop_stmt : stmt->do_while_stmt->body) {
        CollectPureContractEffectStmt(loop_stmt.get(), scopes, globals, info);
      }
      scopes.pop_back();
      CollectPureContractEffectExpr(stmt->do_while_stmt->condition.get(), scopes, info);
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      CollectPureContractEffectForClause(stmt->for_stmt->init, scopes, globals, info);
      CollectPureContractEffectExpr(stmt->for_stmt->condition.get(), scopes, info);
      scopes.push_back({});
      for (const auto &loop_stmt : stmt->for_stmt->body) {
        CollectPureContractEffectStmt(loop_stmt.get(), scopes, globals, info);
      }
      scopes.pop_back();
      CollectPureContractEffectForClause(stmt->for_stmt->step, scopes, globals, info);
      scopes.pop_back();
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt == nullptr) {
        return;
      }
      CollectPureContractEffectExpr(stmt->switch_stmt->condition.get(), scopes, info);
      for (const auto &case_stmt : stmt->switch_stmt->cases) {
        scopes.push_back({});
        for (const auto &case_body_stmt : case_stmt.body) {
          CollectPureContractEffectStmt(case_body_stmt.get(), scopes, globals, info);
        }
        scopes.pop_back();
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt == nullptr) {
        return;
      }
      CollectPureContractEffectExpr(stmt->while_stmt->condition.get(), scopes, info);
      scopes.push_back({});
      for (const auto &loop_stmt : stmt->while_stmt->body) {
        CollectPureContractEffectStmt(loop_stmt.get(), scopes, globals, info);
      }
      scopes.pop_back();
      return;
    case Stmt::Kind::Block:
      if (stmt->block_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      for (const auto &nested_stmt : stmt->block_stmt->body) {
        CollectPureContractEffectStmt(nested_stmt.get(), scopes, globals, info);
      }
      scopes.pop_back();
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectPureContractEffectExpr(const Expr *expr, std::vector<std::unordered_set<std::string>> &scopes,
                                          PureContractEffectInfo &info) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
    case Expr::Kind::Number:
    case Expr::Kind::BoolLiteral:
    case Expr::Kind::NilLiteral:
    case Expr::Kind::Identifier:
      return;
    case Expr::Kind::Binary:
      CollectPureContractEffectExpr(expr->left.get(), scopes, info);
      CollectPureContractEffectExpr(expr->right.get(), scopes, info);
      return;
    case Expr::Kind::Conditional:
      CollectPureContractEffectExpr(expr->left.get(), scopes, info);
      CollectPureContractEffectExpr(expr->right.get(), scopes, info);
      CollectPureContractEffectExpr(expr->third.get(), scopes, info);
      return;
    case Expr::Kind::Call:
      RecordPureContractSourceLoc(info.called_functions[expr->ident], expr->line, expr->column);
      for (const auto &arg : expr->args) {
        CollectPureContractEffectExpr(arg.get(), scopes, info);
      }
      return;
    case Expr::Kind::MessageSend:
      RecordPureContractSourceLoc(info.message_send_site, expr->line, expr->column);
      CollectPureContractEffectExpr(expr->receiver.get(), scopes, info);
      for (const auto &arg : expr->args) {
        CollectPureContractEffectExpr(arg.get(), scopes, info);
      }
      return;
  }
}

void ValidatePureContractSemanticDiagnostics(const Objc3Program &program,
                                                    const std::unordered_map<std::string, FunctionInfo> &surface_functions,
                                                    std::vector<std::string> &diagnostics) {
  std::unordered_set<std::string> globals;
  for (const auto &global : program.globals) {
    globals.insert(global.name);
  }

  std::unordered_set<std::string> defined_functions;
  std::unordered_map<std::string, bool> pure_annotations;
  for (const auto &entry : surface_functions) {
    pure_annotations[entry.first] = entry.second.is_pure_annotation;
  }

  std::unordered_map<std::string, PureContractEffectInfo> function_effects;
  for (const auto &fn : program.functions) {
    if (fn.is_prototype) {
      continue;
    }
    defined_functions.insert(fn.name);

    PureContractEffectInfo info;
    std::vector<std::unordered_set<std::string>> scopes;
    scopes.push_back({});
    for (const auto &param : fn.params) {
      scopes.back().insert(param.name);
    }
    for (const auto &stmt : fn.body) {
      CollectPureContractEffectStmt(stmt.get(), scopes, globals, info);
    }
    function_effects[fn.name] = std::move(info);
  }

  std::vector<std::string> ordered_functions;
  ordered_functions.reserve(function_effects.size());
  for (const auto &entry : function_effects) {
    ordered_functions.push_back(entry.first);
  }
  std::sort(ordered_functions.begin(), ordered_functions.end());

  std::unordered_set<std::string> impure_functions;
  std::unordered_map<std::string, PureContractCause> impure_causes;
  for (const std::string &name : ordered_functions) {
    const auto effect_it = function_effects.find(name);
    if (effect_it == function_effects.end()) {
      continue;
    }
    const PureContractCause direct_cause = DetermineDirectPureContractImpurityCause(effect_it->second);
    if (direct_cause.present) {
      impure_functions.insert(name);
      impure_causes[name] = direct_cause;
    }
  }

  bool changed = true;
  while (changed) {
    changed = false;
    for (const std::string &name : ordered_functions) {
      if (impure_functions.find(name) != impure_functions.end()) {
        continue;
      }
      const auto effect_it = function_effects.find(name);
      if (effect_it == function_effects.end()) {
        continue;
      }
      const std::vector<std::string> callees = SortedPureContractNames(effect_it->second.called_functions);
      PureContractCause selected_cause;
      for (const std::string &callee : callees) {
        const bool callee_defined = defined_functions.find(callee) != defined_functions.end();
        const bool callee_pure = pure_annotations.find(callee) != pure_annotations.end() && pure_annotations[callee];
        PureContractCause candidate_cause;
        const auto call_site_it = effect_it->second.called_functions.find(callee);
        if (call_site_it != effect_it->second.called_functions.end() && call_site_it->second.present) {
          candidate_cause.line = call_site_it->second.line;
          candidate_cause.column = call_site_it->second.column;
          candidate_cause.present = true;
        }
        if (!callee_defined && !callee_pure) {
          candidate_cause.token = "unannotated-extern-call:" + callee;
          if (candidate_cause.present) {
            candidate_cause.detail_token = candidate_cause.token;
            candidate_cause.detail_line = candidate_cause.line;
            candidate_cause.detail_column = candidate_cause.column;
            candidate_cause.detail_present = true;
          }
        } else if (impure_functions.find(callee) != impure_functions.end()) {
          candidate_cause.token = "impure-callee:" + callee;
          const auto callee_cause_it = impure_causes.find(callee);
          if (callee_cause_it != impure_causes.end()) {
            const PureContractCause &callee_cause = callee_cause_it->second;
            if (callee_cause.detail_present) {
              candidate_cause.detail_token = callee_cause.detail_token;
              candidate_cause.detail_line = callee_cause.detail_line;
              candidate_cause.detail_column = callee_cause.detail_column;
              candidate_cause.detail_present = true;
            } else if (callee_cause.present) {
              candidate_cause.detail_token = callee_cause.token;
              candidate_cause.detail_line = callee_cause.line;
              candidate_cause.detail_column = callee_cause.column;
              candidate_cause.detail_present = true;
            }
          }
        }

        if (IsBetterPureContractCause(candidate_cause, selected_cause)) {
          selected_cause = candidate_cause;
        }
      }
      if (!selected_cause.present) {
        continue;
      }
      if (!selected_cause.detail_present) {
        selected_cause.detail_token = selected_cause.token;
        selected_cause.detail_line = selected_cause.line;
        selected_cause.detail_column = selected_cause.column;
        selected_cause.detail_present = true;
      }
      impure_functions.insert(name);
      impure_causes[name] = selected_cause;
      changed = true;
    }
  }

  std::unordered_set<std::string> reported;
  for (const auto &fn : program.functions) {
    if (fn.is_prototype || !fn.is_pure) {
      continue;
    }
    if (impure_functions.find(fn.name) == impure_functions.end()) {
      continue;
    }
    if (!reported.insert(fn.name).second) {
      continue;
    }
    PureContractCause cause;
    const auto cause_it = impure_causes.find(fn.name);
    if (cause_it != impure_causes.end()) {
      cause = cause_it->second;
    }
    if (!cause.present) {
      cause.token = "unknown";
      cause.line = fn.line;
      cause.column = fn.column;
      cause.present = true;
      cause.detail_token = cause.token;
      cause.detail_line = cause.line;
      cause.detail_column = cause.column;
      cause.detail_present = true;
    }
    if (!cause.detail_present) {
      cause.detail_token = cause.token;
      cause.detail_line = cause.line;
      cause.detail_column = cause.column;
      cause.detail_present = true;
    }
    diagnostics.push_back(MakeDiag(fn.line, fn.column, "O3S215",
                                   "pure contract violation: function '" + fn.name +
                                       "' declared 'pure' has side effects (cause: " + cause.token +
                                       "; cause-site:" + std::to_string(cause.line) + ":" +
                                       std::to_string(cause.column) + "; detail:" + cause.detail_token + "@" +
                                       std::to_string(cause.detail_line) + ":" +
                                       std::to_string(cause.detail_column) + ")"));
  }
}


