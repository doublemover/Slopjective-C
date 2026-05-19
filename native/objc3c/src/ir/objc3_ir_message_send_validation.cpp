#include "ir/objc3_ir_message_send_validation.h"

#include "ast/objc3_ast.h"

#include <string>

namespace {

bool ValidateObjc3IRMessageSendArityExpr(const Expr *expr,
                                         std::size_t runtime_dispatch_arg_slots,
                                         std::string &error) {
  if (expr == nullptr) {
    return true;
  }
  switch (expr->kind) {
    case Expr::Kind::Number:
    case Expr::Kind::BoolLiteral:
    case Expr::Kind::NilLiteral:
    case Expr::Kind::Identifier:
    case Expr::Kind::KeyPathLiteral:
    case Expr::Kind::BlockLiteral:
      return true;
    case Expr::Kind::Binary:
      return ValidateObjc3IRMessageSendArityExpr(
                 expr->left.get(), runtime_dispatch_arg_slots, error) &&
             ValidateObjc3IRMessageSendArityExpr(
                 expr->right.get(), runtime_dispatch_arg_slots, error);
    case Expr::Kind::Conditional:
      return ValidateObjc3IRMessageSendArityExpr(
                 expr->left.get(), runtime_dispatch_arg_slots, error) &&
             ValidateObjc3IRMessageSendArityExpr(
                 expr->right.get(), runtime_dispatch_arg_slots, error) &&
             ValidateObjc3IRMessageSendArityExpr(
                 expr->third.get(), runtime_dispatch_arg_slots, error);
    case Expr::Kind::Call:
    case Expr::Kind::Try:
    case Expr::Kind::Throw:
      for (const auto &arg : expr->args) {
        if (!ValidateObjc3IRMessageSendArityExpr(
                arg.get(), runtime_dispatch_arg_slots, error)) {
          return false;
        }
      }
      return true;
    case Expr::Kind::MessageSend:
      if (expr->args.size() > runtime_dispatch_arg_slots) {
        error = "message send exceeds runtime dispatch arg slots: got " +
                std::to_string(expr->args.size()) + ", max " +
                std::to_string(runtime_dispatch_arg_slots) + " at " +
                std::to_string(expr->line) + ":" +
                std::to_string(expr->column);
        return false;
      }
      if (!ValidateObjc3IRMessageSendArityExpr(
              expr->receiver.get(), runtime_dispatch_arg_slots, error)) {
        return false;
      }
      for (const auto &arg : expr->args) {
        if (!ValidateObjc3IRMessageSendArityExpr(
                arg.get(), runtime_dispatch_arg_slots, error)) {
          return false;
        }
      }
      return true;
  }
  return true;
}

bool ValidateObjc3IRMessageSendArityForClause(
    const ForClause &clause, std::size_t runtime_dispatch_arg_slots,
    std::string &error) {
  switch (clause.kind) {
    case ForClause::Kind::None:
      return true;
    case ForClause::Kind::Expr:
    case ForClause::Kind::Let:
    case ForClause::Kind::Assign:
      return ValidateObjc3IRMessageSendArityExpr(
          clause.value.get(), runtime_dispatch_arg_slots, error);
  }
  return true;
}

bool ValidateObjc3IRMessageSendArityStmt(const Stmt *stmt,
                                         std::size_t runtime_dispatch_arg_slots,
                                         std::string &error) {
  if (stmt == nullptr) {
    return true;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      return stmt->let_stmt == nullptr ||
             ValidateObjc3IRMessageSendArityExpr(
                 stmt->let_stmt->value.get(), runtime_dispatch_arg_slots,
                 error);
    case Stmt::Kind::Assign:
      return stmt->assign_stmt == nullptr ||
             ValidateObjc3IRMessageSendArityExpr(
                 stmt->assign_stmt->value.get(), runtime_dispatch_arg_slots,
                 error);
    case Stmt::Kind::Return:
      return stmt->return_stmt == nullptr ||
             ValidateObjc3IRMessageSendArityExpr(
                 stmt->return_stmt->value.get(), runtime_dispatch_arg_slots,
                 error);
    case Stmt::Kind::Expr:
      return stmt->expr_stmt == nullptr ||
             ValidateObjc3IRMessageSendArityExpr(
                 stmt->expr_stmt->value.get(), runtime_dispatch_arg_slots,
                 error);
    case Stmt::Kind::If:
      if (stmt->if_stmt == nullptr) {
        return true;
      }
      if (!ValidateObjc3IRMessageSendArityExpr(
              stmt->if_stmt->condition.get(), runtime_dispatch_arg_slots,
              error)) {
        return false;
      }
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        if (!ValidateObjc3IRMessageSendArityStmt(
                then_stmt.get(), runtime_dispatch_arg_slots, error)) {
          return false;
        }
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        if (!ValidateObjc3IRMessageSendArityStmt(
                else_stmt.get(), runtime_dispatch_arg_slots, error)) {
          return false;
        }
      }
      return true;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt == nullptr) {
        return true;
      }
      for (const auto &loop_stmt : stmt->do_while_stmt->body) {
        if (!ValidateObjc3IRMessageSendArityStmt(
                loop_stmt.get(), runtime_dispatch_arg_slots, error)) {
          return false;
        }
      }
      return ValidateObjc3IRMessageSendArityExpr(
          stmt->do_while_stmt->condition.get(), runtime_dispatch_arg_slots,
          error);
    case Stmt::Kind::For:
      if (stmt->for_stmt == nullptr) {
        return true;
      }
      if (!ValidateObjc3IRMessageSendArityForClause(
              stmt->for_stmt->init, runtime_dispatch_arg_slots, error) ||
          !ValidateObjc3IRMessageSendArityExpr(
              stmt->for_stmt->condition.get(), runtime_dispatch_arg_slots,
              error) ||
          !ValidateObjc3IRMessageSendArityForClause(
              stmt->for_stmt->step, runtime_dispatch_arg_slots, error)) {
        return false;
      }
      for (const auto &loop_stmt : stmt->for_stmt->body) {
        if (!ValidateObjc3IRMessageSendArityStmt(
                loop_stmt.get(), runtime_dispatch_arg_slots, error)) {
          return false;
        }
      }
      return true;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt == nullptr) {
        return true;
      }
      if (!ValidateObjc3IRMessageSendArityExpr(
              stmt->switch_stmt->condition.get(), runtime_dispatch_arg_slots,
              error)) {
        return false;
      }
      for (const auto &case_stmt : stmt->switch_stmt->cases) {
        for (const auto &case_body_stmt : case_stmt.body) {
          if (!ValidateObjc3IRMessageSendArityStmt(
                  case_body_stmt.get(), runtime_dispatch_arg_slots, error)) {
            return false;
          }
        }
      }
      return true;
    case Stmt::Kind::While:
      if (stmt->while_stmt == nullptr) {
        return true;
      }
      if (!ValidateObjc3IRMessageSendArityExpr(
              stmt->while_stmt->condition.get(), runtime_dispatch_arg_slots,
              error)) {
        return false;
      }
      for (const auto &loop_stmt : stmt->while_stmt->body) {
        if (!ValidateObjc3IRMessageSendArityStmt(
                loop_stmt.get(), runtime_dispatch_arg_slots, error)) {
          return false;
        }
      }
      return true;
    case Stmt::Kind::Block:
    case Stmt::Kind::Defer:
      if (stmt->block_stmt == nullptr) {
        return true;
      }
      for (const auto &nested_stmt : stmt->block_stmt->body) {
        if (!ValidateObjc3IRMessageSendArityStmt(
                nested_stmt.get(), runtime_dispatch_arg_slots, error)) {
          return false;
        }
      }
      return true;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return true;
  }
  return true;
}

}  // namespace

bool ValidateObjc3IRMessageSendArityContract(
    const Objc3Program &program, std::size_t runtime_dispatch_arg_slots,
    std::string &error) {
  for (const auto &global : program.globals) {
    if (!ValidateObjc3IRMessageSendArityExpr(
            global.value.get(), runtime_dispatch_arg_slots, error)) {
      return false;
    }
  }
  for (const auto &fn : program.functions) {
    for (const auto &stmt : fn.body) {
      if (!ValidateObjc3IRMessageSendArityStmt(
              stmt.get(), runtime_dispatch_arg_slots, error)) {
        return false;
      }
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      if (!method.has_body) {
        continue;
      }
      for (const auto &stmt : method.body) {
        if (!ValidateObjc3IRMessageSendArityStmt(
                stmt.get(), runtime_dispatch_arg_slots, error)) {
          return false;
        }
      }
    }
  }
  return true;
}
