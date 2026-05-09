#include "parse/objc3_parser_profile_symbol_walk.h"

namespace objc3c::parse {

void Objc3ProfileSymbolWalker::Visit(const std::string &symbol) const {
  if (visit != nullptr) {
    visit(symbol, context);
  }
}

void WalkObjc3ProfileSymbolsInExpr(
    const Expr *expr,
    const Objc3ProfileSymbolWalker &walker) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Call:
    walker.Visit(expr->ident);
    for (const auto &arg : expr->args) {
      WalkObjc3ProfileSymbolsInExpr(arg.get(), walker);
    }
    return;
  case Expr::Kind::MessageSend:
    walker.Visit(expr->selector);
    WalkObjc3ProfileSymbolsInExpr(expr->receiver.get(), walker);
    for (const auto &arg : expr->args) {
      WalkObjc3ProfileSymbolsInExpr(arg.get(), walker);
    }
    return;
  case Expr::Kind::Binary:
    WalkObjc3ProfileSymbolsInExpr(expr->left.get(), walker);
    WalkObjc3ProfileSymbolsInExpr(expr->right.get(), walker);
    return;
  case Expr::Kind::Conditional:
    WalkObjc3ProfileSymbolsInExpr(expr->left.get(), walker);
    WalkObjc3ProfileSymbolsInExpr(expr->right.get(), walker);
    WalkObjc3ProfileSymbolsInExpr(expr->third.get(), walker);
    return;
  case Expr::Kind::BlockLiteral:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::NilLiteral:
  case Expr::Kind::Number:
  default:
    return;
  }
}

namespace {

void WalkObjc3ProfileSymbolsInForClause(
    const ForClause &clause,
    const Objc3ProfileSymbolWalker &walker) {
  WalkObjc3ProfileSymbolsInExpr(clause.value.get(), walker);
}

}  // namespace

void WalkObjc3ProfileSymbolsInStmt(
    const Stmt *stmt,
    const Objc3ProfileSymbolWalker &walker) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      WalkObjc3ProfileSymbolsInExpr(stmt->let_stmt->value.get(), walker);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      WalkObjc3ProfileSymbolsInExpr(stmt->assign_stmt->value.get(), walker);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      WalkObjc3ProfileSymbolsInExpr(stmt->return_stmt->value.get(), walker);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    WalkObjc3ProfileSymbolsInExpr(stmt->if_stmt->condition.get(), walker);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      WalkObjc3ProfileSymbolsInStmt(then_stmt.get(), walker);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      WalkObjc3ProfileSymbolsInStmt(else_stmt.get(), walker);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      WalkObjc3ProfileSymbolsInStmt(body_stmt.get(), walker);
    }
    WalkObjc3ProfileSymbolsInExpr(stmt->do_while_stmt->condition.get(), walker);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    WalkObjc3ProfileSymbolsInForClause(stmt->for_stmt->init, walker);
    WalkObjc3ProfileSymbolsInExpr(stmt->for_stmt->condition.get(), walker);
    WalkObjc3ProfileSymbolsInForClause(stmt->for_stmt->step, walker);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      WalkObjc3ProfileSymbolsInStmt(body_stmt.get(), walker);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    WalkObjc3ProfileSymbolsInExpr(stmt->switch_stmt->condition.get(), walker);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        WalkObjc3ProfileSymbolsInStmt(case_stmt.get(), walker);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    WalkObjc3ProfileSymbolsInExpr(stmt->while_stmt->condition.get(), walker);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      WalkObjc3ProfileSymbolsInStmt(body_stmt.get(), walker);
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      WalkObjc3ProfileSymbolsInStmt(body_stmt.get(), walker);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      WalkObjc3ProfileSymbolsInExpr(stmt->expr_stmt->value.get(), walker);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

void WalkObjc3ProfileSymbolsInBody(
    const std::vector<std::unique_ptr<Stmt>> &body,
    const Objc3ProfileSymbolWalker &walker) {
  for (const auto &stmt : body) {
    WalkObjc3ProfileSymbolsInStmt(stmt.get(), walker);
  }
}

void WalkObjc3ProfileSymbolsInOpaqueMethodBody(
    const Objc3MethodDecl &method,
    const Objc3ProfileSymbolWalker &walker) {
  if (method.has_body) {
    walker.Visit(method.selector);
  }
}

}  // namespace objc3c::parse
