#include "artifacts/objc3_frontend_ownership_semantic_artifacts.h"

#include <algorithm>
#include <string>
#include <vector>

#include "ast/objc3_ast_core.h"
#include "ast/objc3_ast_declarations.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {
namespace {

bool IsStrictlySortedUniqueStrings(const std::vector<std::string> &entries) {
  if (!std::is_sorted(entries.begin(), entries.end())) {
    return false;
  }
  return std::adjacent_find(entries.begin(), entries.end()) == entries.end();
}

void AccumulateBlockSourceModelCompletionFromExpr(
    const Expr *expr,
    Objc3BlockSourceModelCompletionContract &contract);

void AccumulateBlockSourceModelCompletionFromStmt(
    const Stmt *stmt,
    Objc3BlockSourceModelCompletionContract &contract) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (stmt->let_stmt != nullptr) {
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->let_stmt->value.get(), contract);
      }
      break;
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->assign_stmt->value.get(), contract);
      }
      break;
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->return_stmt->value.get(), contract);
      }
      break;
    case Stmt::Kind::If:
      if (stmt->if_stmt != nullptr) {
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->if_stmt->condition.get(), contract);
        for (const auto &child : stmt->if_stmt->then_body) {
          AccumulateBlockSourceModelCompletionFromStmt(child.get(), contract);
        }
        for (const auto &child : stmt->if_stmt->else_body) {
          AccumulateBlockSourceModelCompletionFromStmt(child.get(), contract);
        }
      }
      break;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt != nullptr) {
        for (const auto &child : stmt->do_while_stmt->body) {
          AccumulateBlockSourceModelCompletionFromStmt(child.get(), contract);
        }
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->do_while_stmt->condition.get(), contract);
      }
      break;
    case Stmt::Kind::For:
      if (stmt->for_stmt != nullptr) {
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->for_stmt->init.value.get(), contract);
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->for_stmt->condition.get(), contract);
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->for_stmt->step.value.get(), contract);
        for (const auto &child : stmt->for_stmt->body) {
          AccumulateBlockSourceModelCompletionFromStmt(child.get(), contract);
        }
      }
      break;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt != nullptr) {
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->switch_stmt->condition.get(), contract);
        for (const auto &switch_case : stmt->switch_stmt->cases) {
          for (const auto &child : switch_case.body) {
            AccumulateBlockSourceModelCompletionFromStmt(child.get(), contract);
          }
        }
      }
      break;
    case Stmt::Kind::While:
      if (stmt->while_stmt != nullptr) {
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->while_stmt->condition.get(), contract);
        for (const auto &child : stmt->while_stmt->body) {
          AccumulateBlockSourceModelCompletionFromStmt(child.get(), contract);
        }
      }
      break;
    case Stmt::Kind::Block:
    case Stmt::Kind::Defer:
      if (stmt->block_stmt != nullptr) {
        for (const auto &child : stmt->block_stmt->body) {
          AccumulateBlockSourceModelCompletionFromStmt(child.get(), contract);
        }
      }
      break;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->expr_stmt->value.get(), contract);
      }
      break;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      break;
  }
}

void AccumulateBlockSourceModelCompletionFromExpr(
    const Expr *expr,
    Objc3BlockSourceModelCompletionContract &contract) {
  if (expr == nullptr) {
    return;
  }

  if (expr->kind == Expr::Kind::BlockLiteral) {
    ++contract.block_literal_sites;
    contract.signature_entries_total +=
        expr->block_parameter_signature_entries_lexicographic.size();
    contract.explicit_typed_parameter_entries_total +=
        expr->block_explicit_typed_parameter_count;
    contract.implicit_parameter_entries_total +=
        expr->block_implicit_parameter_count;
    contract.capture_inventory_entries_total +=
        expr->block_capture_inventory_entries_lexicographic.size();
    contract.byvalue_readonly_capture_entries_total +=
        expr->block_byvalue_readonly_capture_count;
    contract.invoke_surface_entries_total +=
        expr->block_invoke_surface_entries_lexicographic.size();

    if (!expr->block_source_model_is_normalized) {
      ++contract.non_normalized_sites;
    }

    bool contract_violation = false;
    contract_violation |=
        expr->block_parameter_signature_entries_lexicographic.size() !=
        expr->block_parameter_count;
    contract_violation |=
        expr->block_explicit_typed_parameter_count +
                expr->block_implicit_parameter_count !=
        expr->block_parameter_count;
    contract_violation |=
        expr->block_capture_inventory_entries_lexicographic.size() !=
        expr->block_capture_count;
    contract_violation |=
        expr->block_byvalue_readonly_capture_count != expr->block_capture_count;
    contract_violation |=
        expr->block_invoke_surface_entries_lexicographic.size() != 2u;
    contract_violation |= expr->block_signature_profile.empty();
    contract_violation |= expr->block_capture_inventory_profile.empty();
    contract_violation |= expr->block_invoke_surface_profile.empty();
    contract_violation |= expr->block_abi_descriptor_symbol.empty();
    contract_violation |= expr->block_invoke_trampoline_symbol.empty();
    contract_violation |= expr->block_source_model_replay_key.empty();
    contract_violation |= !IsStrictlySortedUniqueStrings(
        expr->block_parameter_signature_entries_lexicographic);
    contract_violation |= !IsStrictlySortedUniqueStrings(
        expr->block_capture_inventory_entries_lexicographic);
    contract_violation |= !IsStrictlySortedUniqueStrings(
        expr->block_invoke_surface_entries_lexicographic);

    if (contract_violation) {
      ++contract.contract_violation_sites;
    }
  }

  AccumulateBlockSourceModelCompletionFromExpr(expr->receiver.get(), contract);
  AccumulateBlockSourceModelCompletionFromExpr(expr->left.get(), contract);
  AccumulateBlockSourceModelCompletionFromExpr(expr->right.get(), contract);
  AccumulateBlockSourceModelCompletionFromExpr(expr->third.get(), contract);
  for (const auto &arg : expr->args) {
    AccumulateBlockSourceModelCompletionFromExpr(arg.get(), contract);
  }
}

void AccumulateBlockSourceStorageAnnotationFromStmt(
    const Stmt *stmt,
    Objc3BlockSourceStorageAnnotationContract &contract);

void AccumulateBlockSourceStorageAnnotationFromExpr(
    const Expr *expr,
    Objc3BlockSourceStorageAnnotationContract &contract) {
  if (expr == nullptr) {
    return;
  }

  if (expr->kind == Expr::Kind::BlockLiteral) {
    ++contract.block_literal_sites;
    contract.capture_entries_total += expr->block_capture_count;
    contract.mutated_capture_entries_total += expr->block_mutated_capture_count;
    contract.byref_capture_entries_total += expr->block_byref_capture_count;
    if (expr->block_copy_helper_intent_required) {
      ++contract.copy_helper_intent_sites;
    }
    if (expr->block_dispose_helper_intent_required) {
      ++contract.dispose_helper_intent_sites;
    }
    if (expr->block_escape_shape_promotes_to_heap_candidate) {
      ++contract.heap_candidate_sites;
    }

    if (expr->block_escape_shape_symbol == "expression-site") {
      ++contract.expression_sites;
    } else if (expr->block_escape_shape_symbol == "global-initializer") {
      ++contract.global_initializer_sites;
    } else if (expr->block_escape_shape_symbol == "binding-initializer") {
      ++contract.binding_initializer_sites;
    } else if (expr->block_escape_shape_symbol == "assignment-value") {
      ++contract.assignment_value_sites;
    } else if (expr->block_escape_shape_symbol == "return-value") {
      ++contract.return_value_sites;
    } else if (expr->block_escape_shape_symbol == "call-argument") {
      ++contract.call_argument_sites;
    } else if (expr->block_escape_shape_symbol == "message-argument") {
      ++contract.message_argument_sites;
    } else {
      ++contract.contract_violation_sites;
    }

    if (!expr->block_source_storage_annotations_are_normalized) {
      ++contract.non_normalized_sites;
    }

    bool contract_violation = false;
    contract_violation |= expr->block_mutated_capture_count >
                          expr->block_capture_count;
    contract_violation |= expr->block_byref_capture_count >
                          expr->block_mutated_capture_count;
    contract_violation |= !IsStrictlySortedUniqueStrings(
        expr->block_mutated_capture_names_lexicographic);
    contract_violation |= !IsStrictlySortedUniqueStrings(
        expr->block_byref_capture_names_lexicographic);
    contract_violation |= expr->block_escape_shape_symbol.empty();
    contract_violation |= expr->block_escape_shape_profile.empty();
    contract_violation |= expr->block_helper_intent_profile.empty();
    contract_violation |= expr->block_copy_helper_intent_required !=
                          (expr->block_byref_capture_count > 0u);
    contract_violation |= expr->block_dispose_helper_intent_required !=
                          (expr->block_byref_capture_count > 0u);
    contract_violation |= expr->block_escape_shape_promotes_to_heap_candidate !=
                          (expr->block_escape_shape_symbol !=
                           "expression-site");
    if (contract_violation) {
      ++contract.contract_violation_sites;
    }
  }

  AccumulateBlockSourceStorageAnnotationFromExpr(expr->receiver.get(),
                                                 contract);
  AccumulateBlockSourceStorageAnnotationFromExpr(expr->left.get(), contract);
  AccumulateBlockSourceStorageAnnotationFromExpr(expr->right.get(), contract);
  AccumulateBlockSourceStorageAnnotationFromExpr(expr->third.get(), contract);
  for (const auto &arg : expr->args) {
    AccumulateBlockSourceStorageAnnotationFromExpr(arg.get(), contract);
  }
}

void AccumulateBlockSourceStorageAnnotationFromStmt(
    const Stmt *stmt,
    Objc3BlockSourceStorageAnnotationContract &contract) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->let_stmt->value.get(), contract);
      return;
    case Stmt::Kind::Assign:
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->assign_stmt->value.get(), contract);
      return;
    case Stmt::Kind::Return:
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->return_stmt->value.get(), contract);
      return;
    case Stmt::Kind::Expr:
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->expr_stmt->value.get(), contract);
      return;
    case Stmt::Kind::If:
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->if_stmt->condition.get(), contract);
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        AccumulateBlockSourceStorageAnnotationFromStmt(then_stmt.get(),
                                                       contract);
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        AccumulateBlockSourceStorageAnnotationFromStmt(else_stmt.get(),
                                                       contract);
      }
      return;
    case Stmt::Kind::DoWhile:
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        AccumulateBlockSourceStorageAnnotationFromStmt(body_stmt.get(),
                                                       contract);
      }
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->do_while_stmt->condition.get(), contract);
      return;
    case Stmt::Kind::For:
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->for_stmt->init.value.get(), contract);
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->for_stmt->condition.get(), contract);
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->for_stmt->step.value.get(), contract);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        AccumulateBlockSourceStorageAnnotationFromStmt(body_stmt.get(),
                                                       contract);
      }
      return;
    case Stmt::Kind::Switch:
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->switch_stmt->condition.get(), contract);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          AccumulateBlockSourceStorageAnnotationFromStmt(case_stmt.get(),
                                                         contract);
        }
      }
      return;
    case Stmt::Kind::While:
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->while_stmt->condition.get(), contract);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        AccumulateBlockSourceStorageAnnotationFromStmt(body_stmt.get(),
                                                       contract);
      }
      return;
    case Stmt::Kind::Block:
    case Stmt::Kind::Defer:
      for (const auto &body_stmt : stmt->block_stmt->body) {
        AccumulateBlockSourceStorageAnnotationFromStmt(body_stmt.get(),
                                                       contract);
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

}  // namespace

Objc3BlockSourceModelCompletionContract BuildBlockSourceModelCompletionContract(
    const Objc3Program &program) {
  Objc3BlockSourceModelCompletionContract contract;

  for (const auto &global : program.globals) {
    AccumulateBlockSourceModelCompletionFromExpr(global.value.get(), contract);
  }
  for (const auto &function : program.functions) {
    for (const auto &stmt : function.body) {
      AccumulateBlockSourceModelCompletionFromStmt(stmt.get(), contract);
    }
  }
  for (const auto &protocol : program.protocols) {
    for (const auto &method : protocol.methods) {
      for (const auto &stmt : method.body) {
        AccumulateBlockSourceModelCompletionFromStmt(stmt.get(), contract);
      }
    }
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      for (const auto &stmt : method.body) {
        AccumulateBlockSourceModelCompletionFromStmt(stmt.get(), contract);
      }
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      for (const auto &stmt : method.body) {
        AccumulateBlockSourceModelCompletionFromStmt(stmt.get(), contract);
      }
    }
  }

  contract.deterministic = contract.non_normalized_sites == 0u &&
                           contract.contract_violation_sites == 0u;
  return contract;
}

Objc3BlockSourceStorageAnnotationContract
BuildBlockSourceStorageAnnotationContract(const Objc3Program &program) {
  Objc3BlockSourceStorageAnnotationContract contract;

  for (const auto &global : program.globals) {
    AccumulateBlockSourceStorageAnnotationFromExpr(global.value.get(), contract);
  }
  for (const auto &function : program.functions) {
    for (const auto &stmt : function.body) {
      AccumulateBlockSourceStorageAnnotationFromStmt(stmt.get(), contract);
    }
  }
  for (const auto &protocol : program.protocols) {
    for (const auto &method : protocol.methods) {
      for (const auto &stmt : method.body) {
        AccumulateBlockSourceStorageAnnotationFromStmt(stmt.get(), contract);
      }
    }
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      for (const auto &stmt : method.body) {
        AccumulateBlockSourceStorageAnnotationFromStmt(stmt.get(), contract);
      }
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      for (const auto &stmt : method.body) {
        AccumulateBlockSourceStorageAnnotationFromStmt(stmt.get(), contract);
      }
    }
  }

  contract.deterministic = contract.non_normalized_sites == 0u &&
                           contract.contract_violation_sites == 0u;
  return contract;
}

}  // namespace objc3::artifacts::frontend
