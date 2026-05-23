#include "artifacts/objc3_frontend_dispatch_semantic_artifacts.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <unordered_set>

namespace objc3::artifacts::frontend {
namespace {

std::size_t CountSelectorPieces(const std::string &selector) {
  if (selector.empty()) {
    return 0;
  }
  std::size_t colons = 0;
  for (char c : selector) {
    if (c == ':') {
      ++colons;
    }
  }
  return colons == 0 ? 1 : colons;
}

template <typename Visitor>
void WalkMessageSendLoweringExpr(const Expr *expr, Visitor &visitor) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
    case Expr::Kind::MessageSend: {
      visitor(*expr);
      WalkMessageSendLoweringExpr(expr->receiver.get(), visitor);
      for (const auto &arg : expr->args) {
        WalkMessageSendLoweringExpr(arg.get(), visitor);
      }
      return;
    }
    case Expr::Kind::Binary:
      WalkMessageSendLoweringExpr(expr->left.get(), visitor);
      WalkMessageSendLoweringExpr(expr->right.get(), visitor);
      return;
    case Expr::Kind::Conditional:
      WalkMessageSendLoweringExpr(expr->left.get(), visitor);
      WalkMessageSendLoweringExpr(expr->right.get(), visitor);
      WalkMessageSendLoweringExpr(expr->third.get(), visitor);
      return;
    case Expr::Kind::Call:
      for (const auto &arg : expr->args) {
        WalkMessageSendLoweringExpr(arg.get(), visitor);
      }
      return;
    case Expr::Kind::CollectionLiteral:
      for (const auto &key : expr->collection_keys) {
        WalkMessageSendLoweringExpr(key.get(), visitor);
      }
      for (const auto &value : expr->collection_values) {
        WalkMessageSendLoweringExpr(value.get(), visitor);
      }
      return;
    case Expr::Kind::IndexAccess:
      WalkMessageSendLoweringExpr(expr->left.get(), visitor);
      WalkMessageSendLoweringExpr(expr->right.get(), visitor);
      return;
    default:
      return;
  }
}

template <typename Visitor>
void WalkMessageSendLoweringForClause(
    const ForClause &clause, Visitor &visitor) {
  if (clause.value != nullptr) {
    WalkMessageSendLoweringExpr(clause.value.get(), visitor);
  }
}

template <typename Visitor>
void WalkMessageSendLoweringStmt(const Stmt *stmt, Visitor &visitor) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (stmt->let_stmt != nullptr) {
        WalkMessageSendLoweringExpr(stmt->let_stmt->value.get(), visitor);
      }
      return;
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        WalkMessageSendLoweringExpr(stmt->assign_stmt->value.get(), visitor);
      }
      return;
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        WalkMessageSendLoweringExpr(stmt->return_stmt->value.get(), visitor);
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        WalkMessageSendLoweringExpr(stmt->expr_stmt->value.get(), visitor);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt == nullptr) {
        return;
      }
      WalkMessageSendLoweringExpr(stmt->if_stmt->condition.get(), visitor);
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        WalkMessageSendLoweringStmt(then_stmt.get(), visitor);
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        WalkMessageSendLoweringStmt(else_stmt.get(), visitor);
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt == nullptr) {
        return;
      }
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        WalkMessageSendLoweringStmt(body_stmt.get(), visitor);
      }
      WalkMessageSendLoweringExpr(stmt->do_while_stmt->condition.get(),
                                  visitor);
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt == nullptr) {
        return;
      }
      WalkMessageSendLoweringForClause(stmt->for_stmt->init, visitor);
      WalkMessageSendLoweringExpr(stmt->for_stmt->condition.get(), visitor);
      WalkMessageSendLoweringForClause(stmt->for_stmt->step, visitor);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        WalkMessageSendLoweringStmt(body_stmt.get(), visitor);
      }
      return;
    case Stmt::Kind::ForIn:
      if (stmt->for_in_stmt == nullptr) {
        return;
      }
      WalkMessageSendLoweringExpr(stmt->for_in_stmt->collection.get(),
                                  visitor);
      for (const auto &body_stmt : stmt->for_in_stmt->body) {
        WalkMessageSendLoweringStmt(body_stmt.get(), visitor);
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt == nullptr) {
        return;
      }
      WalkMessageSendLoweringExpr(stmt->switch_stmt->condition.get(),
                                  visitor);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          WalkMessageSendLoweringStmt(case_stmt.get(), visitor);
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt == nullptr) {
        return;
      }
      WalkMessageSendLoweringExpr(stmt->while_stmt->condition.get(), visitor);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        WalkMessageSendLoweringStmt(body_stmt.get(), visitor);
      }
      return;
    case Stmt::Kind::Block:
    case Stmt::Kind::Defer:
      if (stmt->block_stmt == nullptr) {
        return;
      }
      for (const auto &body_stmt : stmt->block_stmt->body) {
        WalkMessageSendLoweringStmt(body_stmt.get(), visitor);
      }
      return;
    case Stmt::Kind::CollectionMutation:
      if (stmt->collection_mutation_stmt == nullptr) {
        return;
      }
      WalkMessageSendLoweringExpr(
          stmt->collection_mutation_stmt->key_or_index.get(), visitor);
      WalkMessageSendLoweringExpr(
          stmt->collection_mutation_stmt->value.get(), visitor);
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

template <typename Visitor>
void WalkMessageSendLoweringProgram(
    const Objc3Program &program, Visitor &visitor) {
  for (const auto &global : program.globals) {
    WalkMessageSendLoweringExpr(global.value.get(), visitor);
  }
  for (const auto &function : program.functions) {
    for (const auto &stmt : function.body) {
      WalkMessageSendLoweringStmt(stmt.get(), visitor);
    }
  }
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &method_decl : implementation_decl.methods) {
      if (!method_decl.has_body) {
        continue;
      }
      for (const auto &stmt : method_decl.body) {
        WalkMessageSendLoweringStmt(stmt.get(), visitor);
      }
    }
  }
}

void AccumulateMessageSendSelectorLoweringSite(
    const Expr &expr, Objc3MessageSendSelectorLoweringContract &contract,
    std::unordered_set<std::string> &selector_literals) {
  ++contract.message_send_sites;
  ++contract.receiver_expression_sites;
  if (expr.args.empty()) {
    ++contract.unary_selector_sites;
  } else {
    ++contract.keyword_selector_sites;
  }
  contract.argument_expression_sites += expr.args.size();
  const std::size_t selector_pieces = CountSelectorPieces(expr.selector);
  contract.selector_piece_sites += selector_pieces;
  if (selector_pieces == 0u) {
    contract.deterministic = false;
  } else {
    selector_literals.insert(expr.selector);
  }
}

void AccumulateDispatchAbiMarshallingSite(
    const Expr &expr, std::size_t runtime_dispatch_arg_slots,
    Objc3DispatchAbiMarshallingContract &contract) {
  ++contract.message_send_sites;
  ++contract.receiver_slots_marshaled;
  ++contract.selector_slots_marshaled;
  const std::size_t actual_args = expr.args.size();
  const std::size_t marshalled_args =
      std::min(actual_args, runtime_dispatch_arg_slots);
  contract.argument_value_slots_marshaled += marshalled_args;
  if (actual_args > runtime_dispatch_arg_slots) {
    contract.deterministic = false;
  }
  contract.argument_padding_slots_marshaled +=
      (runtime_dispatch_arg_slots - marshalled_args);
  contract.argument_total_slots_marshaled += runtime_dispatch_arg_slots;
}

}  // namespace

Objc3MessageSendSelectorLoweringContract
BuildMessageSendSelectorLoweringContract(const Objc3Program &program) {
  Objc3MessageSendSelectorLoweringContract contract;
  std::unordered_set<std::string> selector_literals;
  auto accumulate_message_send = [&](const Expr &expr) {
    AccumulateMessageSendSelectorLoweringSite(expr, contract,
                                              selector_literals);
  };
  WalkMessageSendLoweringProgram(program, accumulate_message_send);

  contract.selector_literal_entries = selector_literals.size();
  for (const auto &selector : selector_literals) {
    contract.selector_literal_characters += selector.size();
  }
  return contract;
}

Objc3DispatchAbiMarshallingContract BuildDispatchAbiMarshallingContract(
    const Objc3Program &program, std::size_t runtime_dispatch_arg_slots) {
  Objc3DispatchAbiMarshallingContract contract;
  contract.runtime_dispatch_arg_slots = runtime_dispatch_arg_slots;
  auto accumulate_message_send = [&](const Expr &expr) {
    AccumulateDispatchAbiMarshallingSite(expr, runtime_dispatch_arg_slots,
                                         contract);
  };
  WalkMessageSendLoweringProgram(program, accumulate_message_send);

  contract.total_marshaled_slots = contract.receiver_slots_marshaled +
                                   contract.selector_slots_marshaled +
                                   contract.argument_total_slots_marshaled;
  return contract;
}

}  // namespace objc3::artifacts::frontend
