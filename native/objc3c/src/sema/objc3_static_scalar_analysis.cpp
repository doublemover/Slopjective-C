#include "sema/objc3_static_scalar_analysis.h"

#include <limits>

namespace {

bool TryEvalStaticTruthiness(const Expr *expr, bool &value,
                             const StaticScalarBindings *bindings = nullptr);

bool TryEvalStaticArithmeticBinary(const std::string &op, int lhs, int rhs,
                                   int &value) {
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
  if (result < static_cast<long long>(int_min) ||
      result > static_cast<long long>(int_max)) {
    return false;
  }
  value = static_cast<int>(result);
  return true;
}

bool TryEvalStaticBitwiseShiftBinary(const std::string &op, int lhs, int rhs,
                                     int &value) {
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
      if (shifted >
          static_cast<unsigned long long>(std::numeric_limits<int>::max())) {
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

bool TryEvalStaticTruthiness(const Expr *expr, bool &value,
                             const StaticScalarBindings *bindings) {
  int scalar = 0;
  if (!TryEvalStaticScalarValue(expr, scalar, bindings)) {
    return false;
  }
  value = scalar != 0;
  return true;
}

}  // namespace

bool IsBoolLikeI32Literal(const Expr *expr) {
  if (expr == nullptr) {
    return false;
  }
  if (expr->kind == Expr::Kind::NilLiteral) {
    return true;
  }
  return expr->kind == Expr::Kind::Number &&
         (expr->number == 0 || expr->number == 1);
}

bool TryEvalStaticScalarValue(const Expr *expr, int &value,
                              const StaticScalarBindings *bindings) {
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
  if (expr->kind == Expr::Kind::Binary && expr->left != nullptr &&
      expr->right != nullptr &&
      (expr->op == "+" || expr->op == "-" || expr->op == "*" ||
       expr->op == "/" || expr->op == "%")) {
    int lhs = 0;
    int rhs = 0;
    if (!TryEvalStaticScalarValue(expr->left.get(), lhs, bindings) ||
        !TryEvalStaticScalarValue(expr->right.get(), rhs, bindings)) {
      return false;
    }
    return TryEvalStaticArithmeticBinary(expr->op, lhs, rhs, value);
  }
  if (expr->kind == Expr::Kind::Binary && expr->left != nullptr &&
      expr->right != nullptr &&
      (expr->op == "&" || expr->op == "|" || expr->op == "^" ||
       expr->op == "<<" || expr->op == ">>")) {
    int lhs = 0;
    int rhs = 0;
    if (!TryEvalStaticScalarValue(expr->left.get(), lhs, bindings) ||
        !TryEvalStaticScalarValue(expr->right.get(), rhs, bindings)) {
      return false;
    }
    return TryEvalStaticBitwiseShiftBinary(expr->op, lhs, rhs, value);
  }
  if (expr->kind == Expr::Kind::Binary && expr->left != nullptr &&
      expr->right != nullptr && (expr->op == "&&" || expr->op == "||")) {
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
  if (expr->kind == Expr::Kind::Binary && expr->left != nullptr &&
      expr->right != nullptr &&
      (expr->op == "==" || expr->op == "!=" || expr->op == "<" ||
       expr->op == "<=" || expr->op == ">" || expr->op == ">=")) {
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

bool ExprIsStaticallyFalse(const Expr *expr,
                           const StaticScalarBindings *bindings) {
  bool truthy = false;
  return TryEvalStaticTruthiness(expr, truthy, bindings) && !truthy;
}

bool ExprIsStaticallyTrue(const Expr *expr,
                          const StaticScalarBindings *bindings) {
  bool truthy = false;
  return TryEvalStaticTruthiness(expr, truthy, bindings) && truthy;
}
