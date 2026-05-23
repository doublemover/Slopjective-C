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
  for (const auto &key : expr->collection_keys) {
    AccumulateBlockSourceStorageAnnotationFromExpr(key.get(), contract);
  }
  for (const auto &value : expr->collection_values) {
    AccumulateBlockSourceStorageAnnotationFromExpr(value.get(), contract);
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
    case Stmt::Kind::ForIn:
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->for_in_stmt->collection.get(), contract);
      for (const auto &body_stmt : stmt->for_in_stmt->body) {
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
    case Stmt::Kind::CollectionMutation:
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->collection_mutation_stmt->key_or_index.get(), contract);
      AccumulateBlockSourceStorageAnnotationFromExpr(
          stmt->collection_mutation_stmt->value.get(), contract);
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}
