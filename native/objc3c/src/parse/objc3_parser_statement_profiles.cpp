#include "parse/objc3_parser_statement_profiles.h"

#include <cstddef>
#include <sstream>

namespace objc3c::parse {

const char *Objc3TryOperatorKindSpelling(Expr::TryOperatorKind kind) {
  switch (kind) {
  case Expr::TryOperatorKind::Propagate:
    return "try";
  case Expr::TryOperatorKind::Optional:
    return "try?";
  case Expr::TryOperatorKind::Forced:
    return "try!";
  case Expr::TryOperatorKind::None:
  default:
    return "none";
  }
}

std::string BuildObjc3TryExpressionProfile(const Expr &expr) {
  std::ostringstream out;
  out << "try-expression:kind="
      << Objc3TryOperatorKindSpelling(expr.try_operator_kind)
      << ";requires_throwing_context="
      << (expr.try_expression_requires_throwing_context ? "true" : "false")
      << ";normalized="
      << (expr.try_expression_is_normalized ? "true" : "false");
  return out.str();
}

std::string BuildObjc3ThrowStatementProfile(const Expr &expr) {
  std::ostringstream out;
  out << "throw-statement:normalized="
      << (expr.throw_statement_is_normalized ? "true" : "false");
  return out.str();
}

std::string BuildObjc3DoCatchProfile(const BlockStmt &block) {
  std::ostringstream out;
  out << "do-catch:clauses=" << block.catch_clauses.size()
      << ";catch_all_sites=";
  std::size_t catch_all_sites = 0;
  for (const auto &clause : block.catch_clauses) {
    if (clause.catch_all) {
      ++catch_all_sites;
    }
  }
  out << catch_all_sites
      << ";normalized="
      << (block.do_catch_is_normalized ? "true" : "false");
  return out.str();
}

}  // namespace objc3c::parse
