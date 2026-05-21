#include "sema/objc3_static_analysis.h"

bool BlockAlwaysReturns(const std::vector<std::unique_ptr<Stmt>> &statements,
                        const StaticScalarBindings *bindings);
bool StatementAlwaysReturns(const Stmt *stmt, const StaticScalarBindings *bindings);

static bool StatementReturnsOrFallsThroughToNextCase(const Stmt *stmt, const StaticScalarBindings *bindings = nullptr);

static bool BlockReturnsOrFallsThroughToNextCase(const std::vector<std::unique_ptr<Stmt>> &statements,
                                                 const StaticScalarBindings *bindings = nullptr) {
  for (const auto &stmt : statements) {
    if (StatementAlwaysReturns(stmt.get(), bindings)) {
      return true;
    }
    if (!StatementReturnsOrFallsThroughToNextCase(stmt.get(), bindings)) {
      return false;
    }
  }
  return true;
}

static bool StatementReturnsOrFallsThroughToNextCase(const Stmt *stmt, const StaticScalarBindings *bindings) {
  if (stmt == nullptr) {
    return false;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
    case Stmt::Kind::Assign:
    case Stmt::Kind::CollectionMutation:
    case Stmt::Kind::Expr:
    case Stmt::Kind::Empty:
      return true;
    case Stmt::Kind::Block:
      if (stmt->block_stmt == nullptr) {
        return false;
      }
      if (stmt->block_stmt->is_do_catch_scope) {
        if (!BlockReturnsOrFallsThroughToNextCase(stmt->block_stmt->body,
                                                  bindings)) {
          return false;
        }
        for (const auto &clause : stmt->block_stmt->catch_clauses) {
          if (!BlockReturnsOrFallsThroughToNextCase(clause.body, bindings)) {
            return false;
          }
        }
        return true;
      }
      return BlockReturnsOrFallsThroughToNextCase(stmt->block_stmt->body, bindings);
    case Stmt::Kind::Defer:
      return true;
    case Stmt::Kind::If: {
      if (stmt->if_stmt == nullptr) {
        return false;
      }
      const IfStmt *if_stmt = stmt->if_stmt.get();
      const bool then_ok = BlockReturnsOrFallsThroughToNextCase(if_stmt->then_body, bindings);
      const bool else_ok =
          if_stmt->else_body.empty() ? true : BlockReturnsOrFallsThroughToNextCase(if_stmt->else_body, bindings);
      if (ExprIsStaticallyTrue(if_stmt->condition.get(), bindings)) {
        return then_ok;
      }
      if (ExprIsStaticallyFalse(if_stmt->condition.get(), bindings)) {
        return else_ok;
      }
      return then_ok && else_ok;
    }
    case Stmt::Kind::Switch:
      // Nested switches that do not already guarantee return may still complete and
      // continue with deterministic fallthrough into subsequent outer case-body statements.
      return true;
    case Stmt::Kind::Return:
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
      return false;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt == nullptr) {
        return false;
      }
      if (!ExprIsStaticallyFalse(stmt->do_while_stmt->condition.get(), bindings)) {
        return false;
      }
      return BlockReturnsOrFallsThroughToNextCase(stmt->do_while_stmt->body, bindings);
    case Stmt::Kind::For:
      if (stmt->for_stmt == nullptr || stmt->for_stmt->condition == nullptr) {
        return false;
      }
      return ExprIsStaticallyFalse(stmt->for_stmt->condition.get(), bindings);
    case Stmt::Kind::ForIn:
      return false;
    case Stmt::Kind::While:
      if (stmt->while_stmt == nullptr) {
        return false;
      }
      return ExprIsStaticallyFalse(stmt->while_stmt->condition.get(), bindings);
  }
  return false;
}

bool StatementAlwaysReturns(const Stmt *stmt, const StaticScalarBindings *bindings) {
  if (stmt == nullptr) {
    return false;
  }
  if (stmt->kind == Stmt::Kind::Switch && stmt->switch_stmt != nullptr) {
    const auto &cases = stmt->switch_stmt->cases;
    if (cases.empty()) {
      return false;
    }

    bool has_default = false;
    std::vector<bool> arm_guarantees(cases.size(), false);
    bool next_arm_guarantees_return = false;

    for (std::size_t offset = 0; offset < cases.size(); ++offset) {
      const std::size_t i = cases.size() - 1 - offset;
      const auto &case_stmt = cases[i];
      has_default = has_default || case_stmt.is_default;

      const bool body_guarantees_return = BlockAlwaysReturns(case_stmt.body, bindings);
      if (body_guarantees_return) {
        arm_guarantees[i] = true;
      } else if (BlockReturnsOrFallsThroughToNextCase(case_stmt.body, bindings)) {
        // Case bodies that either return or fall through chain deterministically to the next case arm.
        arm_guarantees[i] = next_arm_guarantees_return;
      } else {
        arm_guarantees[i] = false;
      }
      next_arm_guarantees_return = arm_guarantees[i];
    }

    int static_switch_value = 0;
    if (TryEvalStaticScalarValue(stmt->switch_stmt->condition.get(), static_switch_value, bindings)) {
      std::size_t default_index = cases.size();
      std::size_t selected_index = cases.size();
      for (std::size_t i = 0; i < cases.size(); ++i) {
        const auto &case_stmt = cases[i];
        if (case_stmt.is_default) {
          if (default_index == cases.size()) {
            default_index = i;
          }
          continue;
        }
        if (static_switch_value == case_stmt.value) {
          selected_index = i;
          break;
        }
      }
      if (selected_index == cases.size()) {
        selected_index = default_index;
      }
      if (selected_index == cases.size()) {
        return false;
      }
      return arm_guarantees[selected_index];
    }

    if (!has_default) {
      return false;
    }
    for (bool arm_guarantees_return : arm_guarantees) {
      if (!arm_guarantees_return) {
        return false;
      }
    }
    return true;
  }
  if (stmt->kind == Stmt::Kind::Return) {
    return true;
  }
  if (stmt->kind == Stmt::Kind::Block && stmt->block_stmt != nullptr) {
    if (stmt->block_stmt->is_do_catch_scope) {
      if (!BlockAlwaysReturns(stmt->block_stmt->body, bindings)) {
        return false;
      }
      for (const auto &clause : stmt->block_stmt->catch_clauses) {
        if (!BlockAlwaysReturns(clause.body, bindings)) {
          return false;
        }
      }
      return true;
    }
    return BlockAlwaysReturns(stmt->block_stmt->body, bindings);
  }
  if (stmt->kind == Stmt::Kind::Defer) {
    return false;
  }
  if (stmt->kind == Stmt::Kind::If && stmt->if_stmt != nullptr) {
    const IfStmt *if_stmt = stmt->if_stmt.get();
    if (ExprIsStaticallyTrue(if_stmt->condition.get(), bindings)) {
      if (if_stmt->then_body.empty()) {
        return false;
      }
      return BlockAlwaysReturns(if_stmt->then_body, bindings);
    }
    if (ExprIsStaticallyFalse(if_stmt->condition.get(), bindings)) {
      if (if_stmt->else_body.empty()) {
        return false;
      }
      return BlockAlwaysReturns(if_stmt->else_body, bindings);
    }
    if (if_stmt->then_body.empty() || if_stmt->else_body.empty()) {
      return false;
    }
    return BlockAlwaysReturns(if_stmt->then_body, bindings) && BlockAlwaysReturns(if_stmt->else_body, bindings);
  }
  if (stmt->kind == Stmt::Kind::While && stmt->while_stmt != nullptr) {
    if (!ExprIsStaticallyTrue(stmt->while_stmt->condition.get(), bindings)) {
      return false;
    }
    return BlockAlwaysReturns(stmt->while_stmt->body, bindings);
  }
  if (stmt->kind == Stmt::Kind::For && stmt->for_stmt != nullptr) {
    const bool guaranteed_entry = (stmt->for_stmt->condition == nullptr) ||
                                  ExprIsStaticallyTrue(stmt->for_stmt->condition.get(), bindings);
    if (!guaranteed_entry) {
      return false;
    }
    return BlockAlwaysReturns(stmt->for_stmt->body, bindings);
  }
  if (stmt->kind == Stmt::Kind::ForIn && stmt->for_in_stmt != nullptr) {
    return false;
  }
  if (stmt->kind == Stmt::Kind::DoWhile && stmt->do_while_stmt != nullptr) {
    return BlockAlwaysReturns(stmt->do_while_stmt->body, bindings);
  }
  return false;
}

bool BlockAlwaysReturns(const std::vector<std::unique_ptr<Stmt>> &statements,
                        const StaticScalarBindings *bindings) {
  for (const auto &stmt : statements) {
    if (StatementAlwaysReturns(stmt.get(), bindings)) {
      return true;
    }
  }
  return false;
}
