#include "sema/objc3_static_analysis.h"

#include <limits>

bool BlockAlwaysReturns(const std::vector<std::unique_ptr<Stmt>> &statements,
                        const StaticScalarBindings *bindings);
bool StatementAlwaysReturns(const Stmt *stmt, const StaticScalarBindings *bindings);

bool IsBoolLikeI32Literal(const Expr *expr) {
  if (expr == nullptr) {
    return false;
  }
  if (expr->kind == Expr::Kind::NilLiteral) {
    return true;
  }
  return expr->kind == Expr::Kind::Number && (expr->number == 0 || expr->number == 1);
}

static bool TryEvalStaticTruthiness(const Expr *expr, bool &value, const StaticScalarBindings *bindings = nullptr);

static bool TryEvalStaticArithmeticBinary(const std::string &op, int lhs, int rhs, int &value) {
  const auto int_min = std::numeric_limits<int>::min();
  const auto int_max = std::numeric_limits<int>::max();
  if (op == "/" || op == "%") {
    if (rhs == 0) {
      return false;
    }
    if (lhs == int_min && rhs == -1) {
      return false;
    }
    if (op == "/") {
      value = lhs / rhs;
      return true;
    }
    value = lhs % rhs;
    return true;
  }
  long long result = 0;
  if (op == "+") {
    result = static_cast<long long>(lhs) + static_cast<long long>(rhs);
  } else if (op == "-") {
    result = static_cast<long long>(lhs) - static_cast<long long>(rhs);
  } else if (op == "*") {
    result = static_cast<long long>(lhs) * static_cast<long long>(rhs);
  } else {
    return false;
  }
  if (result < static_cast<long long>(int_min) || result > static_cast<long long>(int_max)) {
    return false;
  }
  value = static_cast<int>(result);
  return true;
}

static bool TryEvalStaticBitwiseShiftBinary(const std::string &op, int lhs, int rhs, int &value) {
  if (op == "&") {
    value = lhs & rhs;
    return true;
  }
  if (op == "|") {
    value = lhs | rhs;
    return true;
  }
  if (op == "^") {
    value = lhs ^ rhs;
    return true;
  }
  if (op == "<<" || op == ">>") {
    if (rhs < 0 || rhs >= std::numeric_limits<int>::digits || lhs < 0) {
      return false;
    }
    if (op == "<<") {
      const auto shifted = static_cast<unsigned long long>(lhs) << rhs;
      if (shifted > static_cast<unsigned long long>(std::numeric_limits<int>::max())) {
        return false;
      }
      value = static_cast<int>(shifted);
      return true;
    }
    value = lhs >> rhs;
    return true;
  }
  return false;
}

bool TryEvalStaticScalarValue(const Expr *expr, int &value, const StaticScalarBindings *bindings) {
  if (expr == nullptr) {
    return false;
  }
  if (expr->kind == Expr::Kind::BoolLiteral) {
    value = expr->bool_value ? 1 : 0;
    return true;
  }
  if (expr->kind == Expr::Kind::NilLiteral) {
    value = 0;
    return true;
  }
  if (expr->kind == Expr::Kind::Number) {
    value = expr->number;
    return true;
  }
  if (expr->kind == Expr::Kind::Identifier && bindings != nullptr) {
    auto it = bindings->find(expr->ident);
    if (it != bindings->end()) {
      value = it->second;
      return true;
    }
  }
  if (expr->kind == Expr::Kind::Conditional) {
    bool cond_truthy = false;
    if (!TryEvalStaticTruthiness(expr->left.get(), cond_truthy, bindings)) {
      return false;
    }
    const Expr *selected = cond_truthy ? expr->right.get() : expr->third.get();
    if (selected == nullptr) {
      return false;
    }
    return TryEvalStaticScalarValue(selected, value, bindings);
  }
  if (expr->kind == Expr::Kind::Binary && expr->left != nullptr && expr->right != nullptr &&
      (expr->op == "+" || expr->op == "-" || expr->op == "*" || expr->op == "/" || expr->op == "%")) {
    int lhs = 0;
    int rhs = 0;
    if (!TryEvalStaticScalarValue(expr->left.get(), lhs, bindings) ||
        !TryEvalStaticScalarValue(expr->right.get(), rhs, bindings)) {
      return false;
    }
    return TryEvalStaticArithmeticBinary(expr->op, lhs, rhs, value);
  }
  if (expr->kind == Expr::Kind::Binary && expr->left != nullptr && expr->right != nullptr &&
      (expr->op == "&" || expr->op == "|" || expr->op == "^" || expr->op == "<<" || expr->op == ">>")) {
    int lhs = 0;
    int rhs = 0;
    if (!TryEvalStaticScalarValue(expr->left.get(), lhs, bindings) ||
        !TryEvalStaticScalarValue(expr->right.get(), rhs, bindings)) {
      return false;
    }
    return TryEvalStaticBitwiseShiftBinary(expr->op, lhs, rhs, value);
  }
  if (expr->kind == Expr::Kind::Binary && expr->left != nullptr && expr->right != nullptr &&
      (expr->op == "&&" || expr->op == "||")) {
    bool lhs_truthy = false;
    if (!TryEvalStaticTruthiness(expr->left.get(), lhs_truthy, bindings)) {
      return false;
    }
    if (expr->op == "&&") {
      if (!lhs_truthy) {
        value = 0;
        return true;
      }
      bool rhs_truthy = false;
      if (!TryEvalStaticTruthiness(expr->right.get(), rhs_truthy, bindings)) {
        return false;
      }
      value = rhs_truthy ? 1 : 0;
      return true;
    }
    if (lhs_truthy) {
      value = 1;
      return true;
    }
    bool rhs_truthy = false;
    if (!TryEvalStaticTruthiness(expr->right.get(), rhs_truthy, bindings)) {
      return false;
    }
    value = rhs_truthy ? 1 : 0;
    return true;
  }
  if (expr->kind == Expr::Kind::Binary && expr->left != nullptr && expr->right != nullptr &&
      (expr->op == "==" || expr->op == "!=" || expr->op == "<" || expr->op == "<=" || expr->op == ">" ||
       expr->op == ">=")) {
    int lhs = 0;
    int rhs = 0;
    if (!TryEvalStaticScalarValue(expr->left.get(), lhs, bindings) ||
        !TryEvalStaticScalarValue(expr->right.get(), rhs, bindings)) {
      return false;
    }
    bool cmp = false;
    if (expr->op == "==") {
      cmp = lhs == rhs;
    } else if (expr->op == "!=") {
      cmp = lhs != rhs;
    } else if (expr->op == "<") {
      cmp = lhs < rhs;
    } else if (expr->op == "<=") {
      cmp = lhs <= rhs;
    } else if (expr->op == ">") {
      cmp = lhs > rhs;
    } else if (expr->op == ">=") {
      cmp = lhs >= rhs;
    }
    value = cmp ? 1 : 0;
    return true;
  }
  return false;
}

static bool TryEvalStaticTruthiness(const Expr *expr, bool &value, const StaticScalarBindings *bindings) {
  int scalar = 0;
  if (!TryEvalStaticScalarValue(expr, scalar, bindings)) {
    return false;
  }
  value = scalar != 0;
  return true;
}

bool ExprIsStaticallyFalse(const Expr *expr, const StaticScalarBindings *bindings) {
  bool truthy = false;
  return TryEvalStaticTruthiness(expr, truthy, bindings) && !truthy;
}

bool ExprIsStaticallyTrue(const Expr *expr, const StaticScalarBindings *bindings) {
  bool truthy = false;
  return TryEvalStaticTruthiness(expr, truthy, bindings) && truthy;
}


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
    case Stmt::Kind::Expr:
    case Stmt::Kind::Empty:
      return true;
    case Stmt::Kind::Block:
      if (stmt->block_stmt == nullptr) {
        return false;
      }
      return BlockReturnsOrFallsThroughToNextCase(stmt->block_stmt->body, bindings);
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
    return BlockAlwaysReturns(stmt->block_stmt->body, bindings);
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

