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
    case Stmt::Kind::ForIn:
      if (stmt->for_in_stmt != nullptr) {
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->for_in_stmt->collection.get(), contract);
        for (const auto &child : stmt->for_in_stmt->body) {
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
    case Stmt::Kind::CollectionMutation:
      if (stmt->collection_mutation_stmt != nullptr) {
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->collection_mutation_stmt->key_or_index.get(), contract);
        AccumulateBlockSourceModelCompletionFromExpr(
            stmt->collection_mutation_stmt->value.get(), contract);
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
  for (const auto &key : expr->collection_keys) {
    AccumulateBlockSourceModelCompletionFromExpr(key.get(), contract);
  }
  for (const auto &value : expr->collection_values) {
    AccumulateBlockSourceModelCompletionFromExpr(value.get(), contract);
  }
}
