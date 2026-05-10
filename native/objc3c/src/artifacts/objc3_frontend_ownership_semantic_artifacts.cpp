#include "artifacts/objc3_frontend_ownership_semantic_artifacts.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

inline constexpr const char
    *kObjc3OwnershipBorrowedRetainableAbiCompletionArtifactModel =
        "borrowed-return-contracts-and-retainable-family-call-boundaries-now-publish-a-dedicated-ownership-abi-and-replay-packet-above-the-frozen-lowering-contract";
inline constexpr const char
    *kObjc3OwnershipBorrowedRetainableAbiCompletionProofModel =
        "the-supported-proof-slice-remains-direct-call-abi-emission-and-replay-stability-without-claiming-lane-d-runtime-helper-integration";

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

Objc3OwnershipQualifierLoweringContract BuildOwnershipQualifierLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3OwnershipQualifierLoweringContract contract;
  contract.ownership_qualifier_sites =
      sema_parity_surface.type_annotation_ownership_qualifier_sites_total;
  contract.invalid_ownership_qualifier_sites =
      sema_parity_surface.type_annotation_invalid_ownership_qualifier_sites_total;
  contract.object_pointer_type_annotation_sites =
      sema_parity_surface.type_annotation_object_pointer_type_sites_total;
  contract.deterministic =
      sema_parity_surface.type_annotation_surface_summary.deterministic &&
      sema_parity_surface.deterministic_type_annotation_surface_handoff;
  return contract;
}

Objc3RetainReleaseOperationLoweringContract
BuildRetainReleaseOperationLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3RetainReleaseOperationLoweringContract contract;
  contract.ownership_qualified_sites =
      sema_parity_surface.retain_release_operation_ownership_qualified_sites_total;
  contract.retain_insertion_sites =
      sema_parity_surface.retain_release_operation_retain_insertion_sites_total;
  contract.release_insertion_sites =
      sema_parity_surface.retain_release_operation_release_insertion_sites_total;
  contract.autorelease_insertion_sites =
      sema_parity_surface
          .retain_release_operation_autorelease_insertion_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.retain_release_operation_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.retain_release_operation_summary.deterministic &&
      sema_parity_surface.deterministic_retain_release_operation_handoff;
  return contract;
}

Objc3AutoreleasePoolScopeLoweringContract
BuildAutoreleasePoolScopeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3AutoreleasePoolScopeLoweringContract contract;
  contract.scope_sites = sema_parity_surface.autoreleasepool_scope_sites_total;
  contract.scope_symbolized_sites =
      sema_parity_surface.autoreleasepool_scope_symbolized_sites_total;
  contract.max_scope_depth =
      sema_parity_surface.autoreleasepool_scope_max_depth_total;
  contract.scope_entry_transition_sites =
      sema_parity_surface.autoreleasepool_scope_sites_total;
  contract.scope_exit_transition_sites =
      sema_parity_surface.autoreleasepool_scope_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.autoreleasepool_scope_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.autoreleasepool_scope_summary.deterministic &&
      sema_parity_surface.deterministic_autoreleasepool_scope_handoff;
  return contract;
}

Objc3WeakUnownedSemanticsLoweringContract
BuildWeakUnownedSemanticsLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3WeakUnownedSemanticsLoweringContract contract;
  contract.ownership_candidate_sites =
      sema_parity_surface.weak_unowned_semantics_ownership_candidate_sites_total;
  contract.weak_reference_sites =
      sema_parity_surface.weak_unowned_semantics_weak_reference_sites_total;
  contract.unowned_reference_sites =
      sema_parity_surface.weak_unowned_semantics_unowned_reference_sites_total;
  contract.unowned_safe_reference_sites =
      sema_parity_surface
          .weak_unowned_semantics_unowned_safe_reference_sites_total;
  contract.weak_unowned_conflict_sites =
      sema_parity_surface.weak_unowned_semantics_conflict_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.weak_unowned_semantics_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.weak_unowned_semantics_summary.deterministic &&
      sema_parity_surface.deterministic_weak_unowned_semantics_handoff;
  return contract;
}

Objc3ArcDiagnosticsFixitLoweringContract BuildArcDiagnosticsFixitLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3ArcDiagnosticsFixitLoweringContract contract;
  contract.ownership_arc_diagnostic_candidate_sites =
      sema_parity_surface.ownership_arc_diagnostic_candidate_sites_total;
  contract.ownership_arc_fixit_available_sites =
      sema_parity_surface.ownership_arc_fixit_available_sites_total;
  contract.ownership_arc_profiled_sites =
      sema_parity_surface.ownership_arc_profiled_sites_total;
  contract.ownership_arc_weak_unowned_conflict_diagnostic_sites =
      sema_parity_surface
          .ownership_arc_weak_unowned_conflict_diagnostic_sites_total;
  contract.ownership_arc_empty_fixit_hint_sites =
      sema_parity_surface.ownership_arc_empty_fixit_hint_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.ownership_arc_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.arc_diagnostics_fixit_summary.deterministic &&
      sema_parity_surface.deterministic_arc_diagnostics_fixit_handoff;
  return contract;
}

Objc3BlockLiteralCaptureLoweringContract BuildBlockLiteralCaptureLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3BlockLiteralCaptureLoweringContract contract;
  contract.block_literal_sites =
      sema_parity_surface.block_literal_capture_semantics_sites_total;
  contract.block_parameter_entries =
      sema_parity_surface
          .block_literal_capture_semantics_parameter_entries_total;
  contract.block_capture_entries =
      sema_parity_surface.block_literal_capture_semantics_capture_entries_total;
  contract.block_body_statement_entries =
      sema_parity_surface
          .block_literal_capture_semantics_body_statement_entries_total;
  contract.block_empty_capture_sites =
      sema_parity_surface
          .block_literal_capture_semantics_empty_capture_sites_total;
  contract.block_nondeterministic_capture_sites =
      sema_parity_surface
          .block_literal_capture_semantics_nondeterministic_capture_sites_total;
  contract.block_non_normalized_sites =
      sema_parity_surface
          .block_literal_capture_semantics_non_normalized_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface
          .block_literal_capture_semantics_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_literal_capture_semantics_summary
          .deterministic &&
      sema_parity_surface
          .deterministic_block_literal_capture_semantics_handoff;
  return contract;
}

Objc3BlockAbiInvokeTrampolineLoweringContract
BuildBlockAbiInvokeTrampolineLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3BlockAbiInvokeTrampolineLoweringContract contract;
  contract.block_literal_sites =
      sema_parity_surface.block_abi_invoke_trampoline_sites_total;
  contract.invoke_argument_slots_total =
      sema_parity_surface
          .block_abi_invoke_trampoline_invoke_argument_slots_total;
  contract.capture_word_count_total =
      sema_parity_surface
          .block_abi_invoke_trampoline_capture_word_count_total;
  contract.parameter_entries_total =
      sema_parity_surface
          .block_abi_invoke_trampoline_parameter_entries_total;
  contract.capture_entries_total =
      sema_parity_surface.block_abi_invoke_trampoline_capture_entries_total;
  contract.body_statement_entries_total =
      sema_parity_surface
          .block_abi_invoke_trampoline_body_statement_entries_total;
  contract.descriptor_symbolized_sites =
      sema_parity_surface
          .block_abi_invoke_trampoline_descriptor_symbolized_sites_total;
  contract.invoke_trampoline_symbolized_sites =
      sema_parity_surface
          .block_abi_invoke_trampoline_invoke_symbolized_sites_total;
  contract.missing_invoke_trampoline_sites =
      sema_parity_surface
          .block_abi_invoke_trampoline_missing_invoke_sites_total;
  contract.non_normalized_layout_sites =
      sema_parity_surface
          .block_abi_invoke_trampoline_non_normalized_layout_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface
          .block_abi_invoke_trampoline_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_abi_invoke_trampoline_semantics_summary
          .deterministic &&
      sema_parity_surface.deterministic_block_abi_invoke_trampoline_handoff;
  return contract;
}

Objc3BlockStorageEscapeLoweringContract BuildBlockStorageEscapeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3BlockStorageEscapeLoweringContract contract;
  contract.block_literal_sites =
      sema_parity_surface.block_storage_escape_sites_total;
  contract.mutable_capture_count_total =
      sema_parity_surface.block_storage_escape_mutable_capture_count_total;
  contract.byref_slot_count_total =
      sema_parity_surface.block_storage_escape_byref_slot_count_total;
  contract.parameter_entries_total =
      sema_parity_surface.block_storage_escape_parameter_entries_total;
  contract.capture_entries_total =
      sema_parity_surface.block_storage_escape_capture_entries_total;
  contract.body_statement_entries_total =
      sema_parity_surface.block_storage_escape_body_statement_entries_total;
  contract.requires_byref_cells_sites =
      sema_parity_surface
          .block_storage_escape_requires_byref_cells_sites_total;
  contract.escape_analysis_enabled_sites =
      sema_parity_surface
          .block_storage_escape_escape_analysis_enabled_sites_total;
  contract.escape_to_heap_sites =
      sema_parity_surface.block_storage_escape_escape_to_heap_sites_total;
  contract.escape_profile_normalized_sites =
      sema_parity_surface
          .block_storage_escape_escape_profile_normalized_sites_total;
  contract.byref_layout_symbolized_sites =
      sema_parity_surface
          .block_storage_escape_byref_layout_symbolized_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.block_storage_escape_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_storage_escape_semantics_summary.deterministic &&
      sema_parity_surface.deterministic_block_storage_escape_handoff;
  return contract;
}

Objc3BlockCopyDisposeLoweringContract BuildBlockCopyDisposeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3BlockCopyDisposeLoweringContract contract;
  contract.block_literal_sites =
      sema_parity_surface.block_copy_dispose_sites_total;
  contract.mutable_capture_count_total =
      sema_parity_surface.block_copy_dispose_mutable_capture_count_total;
  contract.byref_slot_count_total =
      sema_parity_surface.block_copy_dispose_byref_slot_count_total;
  contract.parameter_entries_total =
      sema_parity_surface.block_copy_dispose_parameter_entries_total;
  contract.capture_entries_total =
      sema_parity_surface.block_copy_dispose_capture_entries_total;
  contract.body_statement_entries_total =
      sema_parity_surface.block_copy_dispose_body_statement_entries_total;
  contract.copy_helper_required_sites =
      sema_parity_surface.block_copy_dispose_copy_helper_required_sites_total;
  contract.dispose_helper_required_sites =
      sema_parity_surface
          .block_copy_dispose_dispose_helper_required_sites_total;
  contract.profile_normalized_sites =
      sema_parity_surface.block_copy_dispose_profile_normalized_sites_total;
  contract.copy_helper_symbolized_sites =
      sema_parity_surface
          .block_copy_dispose_copy_helper_symbolized_sites_total;
  contract.dispose_helper_symbolized_sites =
      sema_parity_surface
          .block_copy_dispose_dispose_helper_symbolized_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.block_copy_dispose_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_copy_dispose_semantics_summary.deterministic &&
      sema_parity_surface.deterministic_block_copy_dispose_handoff;
  return contract;
}

Objc3BlockDeterminismPerfBaselineLoweringContract
BuildBlockDeterminismPerfBaselineLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3BlockDeterminismPerfBaselineLoweringContract contract;
  contract.block_literal_sites =
      sema_parity_surface.block_determinism_perf_baseline_sites_total;
  contract.baseline_weight_total =
      sema_parity_surface.block_determinism_perf_baseline_weight_total;
  contract.parameter_entries_total =
      sema_parity_surface
          .block_determinism_perf_baseline_parameter_entries_total;
  contract.capture_entries_total =
      sema_parity_surface.block_determinism_perf_baseline_capture_entries_total;
  contract.body_statement_entries_total =
      sema_parity_surface
          .block_determinism_perf_baseline_body_statement_entries_total;
  contract.deterministic_capture_sites =
      sema_parity_surface
          .block_determinism_perf_baseline_deterministic_capture_sites_total;
  contract.heavy_tier_sites =
      sema_parity_surface.block_determinism_perf_baseline_heavy_tier_sites_total;
  contract.normalized_profile_sites =
      sema_parity_surface
          .block_determinism_perf_baseline_normalized_profile_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface
          .block_determinism_perf_baseline_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_determinism_perf_baseline_summary
          .deterministic &&
      sema_parity_surface.deterministic_block_determinism_perf_baseline_handoff;
  return contract;
}

std::string BuildOwnershipResourceMoveUseAfterMoveSemanticsSummaryJson(
    const Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"cleanup_owned_local_sites\":"
      << summary.cleanup_owned_local_sites
      << ",\"resource_move_capture_sites\":"
      << summary.resource_move_capture_sites
      << ",\"illegal_non_resource_move_sites\":"
      << summary.illegal_non_resource_move_sites
      << ",\"illegal_use_after_move_sites\":"
      << summary.illegal_use_after_move_sites
      << ",\"illegal_duplicate_move_sites\":"
      << summary.illegal_duplicate_move_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"cleanup_ownership_transfer_enforced\":"
      << (summary.cleanup_ownership_transfer_enforced ? "true" : "false")
      << ",\"use_after_move_fail_closed\":"
      << (summary.use_after_move_fail_closed ? "true" : "false")
      << ",\"duplicate_move_fail_closed\":"
      << (summary.duplicate_move_fail_closed ? "true" : "false")
      << ",\"borrowed_escape_semantics_deferred\":"
      << (summary.borrowed_escape_semantics_deferred ? "true" : "false")
      << ",\"retainable_family_legality_deferred\":"
      << (summary.retainable_family_legality_deferred ? "true" : "false")
      << ",\"lowering_runtime_deferred\":"
      << (summary.lowering_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildOwnershipBorrowedPointerEscapeAnalysisSummaryJson(
    const Objc3OwnershipBorrowedPointerEscapeAnalysisSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"borrowed_parameter_sites\":" << summary.borrowed_parameter_sites
      << ",\"borrowed_return_callable_sites\":"
      << summary.borrowed_return_callable_sites
      << ",\"borrowed_escape_candidate_sites\":"
      << summary.borrowed_escape_candidate_sites
      << ",\"illegal_unproven_call_escape_sites\":"
      << summary.illegal_unproven_call_escape_sites
      << ",\"illegal_escaping_block_capture_sites\":"
      << summary.illegal_escaping_block_capture_sites
      << ",\"illegal_borrowed_return_sites\":"
      << summary.illegal_borrowed_return_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"borrowed_call_boundary_enforced\":"
      << (summary.borrowed_call_boundary_enforced ? "true" : "false")
      << ",\"escaping_block_capture_fail_closed\":"
      << (summary.escaping_block_capture_fail_closed ? "true" : "false")
      << ",\"borrowed_return_contract_enforced\":"
      << (summary.borrowed_return_contract_enforced ? "true" : "false")
      << ",\"retainable_family_legality_deferred\":"
      << (summary.retainable_family_legality_deferred ? "true" : "false")
      << ",\"lowering_runtime_deferred\":"
      << (summary.lowering_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildOwnershipCaptureListRetainableFamilyLegalityCompletionSummaryJson(
    const Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"explicit_capture_list_sites\":"
      << summary.explicit_capture_list_sites
      << ",\"explicit_capture_item_sites\":"
      << summary.explicit_capture_item_sites
      << ",\"explicit_capture_ownership_mode_sites\":"
      << summary.explicit_capture_ownership_mode_sites
      << ",\"retainable_family_callable_sites\":"
      << summary.retainable_family_callable_sites
      << ",\"retainable_family_operation_callable_sites\":"
      << summary.retainable_family_operation_callable_sites
      << ",\"retainable_family_alias_callable_sites\":"
      << summary.retainable_family_alias_callable_sites
      << ",\"illegal_duplicate_explicit_capture_sites\":"
      << summary.illegal_duplicate_explicit_capture_sites
      << ",\"illegal_non_object_capture_mode_sites\":"
      << summary.illegal_non_object_capture_mode_sites
      << ",\"illegal_unused_explicit_capture_sites\":"
      << summary.illegal_unused_explicit_capture_sites
      << ",\"illegal_conflicting_retainable_family_sites\":"
      << summary.illegal_conflicting_retainable_family_sites
      << ",\"illegal_invalid_family_operation_shape_sites\":"
      << summary.illegal_invalid_family_operation_shape_sites
      << ",\"illegal_invalid_family_alias_shape_sites\":"
      << summary.illegal_invalid_family_alias_shape_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"explicit_capture_duplicate_fail_closed\":"
      << (summary.explicit_capture_duplicate_fail_closed ? "true" : "false")
      << ",\"explicit_capture_ownership_mode_enforced\":"
      << (summary.explicit_capture_ownership_mode_enforced ? "true" : "false")
      << ",\"explicit_capture_inventory_enforced\":"
      << (summary.explicit_capture_inventory_enforced ? "true" : "false")
      << ",\"retainable_family_conflict_enforced\":"
      << (summary.retainable_family_conflict_enforced ? "true" : "false")
      << ",\"retainable_family_operation_shape_enforced\":"
      << (summary.retainable_family_operation_shape_enforced ? "true" : "false")
      << ",\"retainable_family_alias_shape_enforced\":"
      << (summary.retainable_family_alias_shape_enforced ? "true" : "false")
      << ",\"lowering_runtime_deferred\":"
      << (summary.lowering_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildOwnershipSystemExtensionLoweringContractJson(
    const Objc3OwnershipSystemExtensionSemanticModelSummary &semantic_summary,
    const Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary &resource_summary,
    const Objc3OwnershipBorrowedPointerEscapeAnalysisSummary &borrowed_summary,
    const Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary
        &family_summary,
    const Objc3OwnershipSystemExtensionLoweringContract &contract,
    const std::string &replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3OwnershipSystemExtensionLoweringContract(contract);
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringSurfacePath)
      << "\",\"semantic_contract_id\":\""
      << EscapeJsonString(semantic_summary.contract_id)
      << "\",\"resource_semantic_contract_id\":\""
      << EscapeJsonString(resource_summary.contract_id)
      << "\",\"borrowed_semantic_contract_id\":\""
      << EscapeJsonString(borrowed_summary.contract_id)
      << "\",\"family_semantic_contract_id\":\""
      << EscapeJsonString(family_summary.contract_id)
      << "\",\"lane_contract_id\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringLaneContract)
      << "\",\"lowering_model\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringDeferredModel)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\",\"cleanup_hook_sites\":" << contract.cleanup_hook_sites
      << ",\"resource_local_sites\":" << contract.resource_local_sites
      << ",\"cleanup_owned_local_sites\":"
      << contract.cleanup_owned_local_sites
      << ",\"resource_move_capture_sites\":"
      << contract.resource_move_capture_sites
      << ",\"borrowed_parameter_sites\":"
      << contract.borrowed_parameter_sites
      << ",\"borrowed_return_callable_sites\":"
      << contract.borrowed_return_callable_sites
      << ",\"borrowed_escape_candidate_sites\":"
      << contract.borrowed_escape_candidate_sites
      << ",\"explicit_capture_item_sites\":"
      << contract.explicit_capture_item_sites
      << ",\"retainable_family_callable_sites\":"
      << contract.retainable_family_callable_sites
      << ",\"retainable_family_operation_callable_sites\":"
      << contract.retainable_family_operation_callable_sites
      << ",\"retainable_family_alias_callable_sites\":"
      << contract.retainable_family_alias_callable_sites
      << ",\"guard_blocked_sites\":" << contract.guard_blocked_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic_handoff\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"ready_for_ir_emission\":"
      << (ready_for_ir_emission ? "true" : "false")
      << "}";
  return out.str();
}

std::string BuildOwnershipBorrowedRetainableAbiCompletionReplayKey(
    const Objc3OwnershipSystemExtensionLoweringContract &contract,
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &source_summary,
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &retainable_summary) {
  std::ostringstream out;
  out << "lowering_replay_key="
      << Objc3OwnershipSystemExtensionLoweringReplayKey(contract)
      << ";returns_borrowed_attribute_sites="
      << source_summary.returns_borrowed_attribute_sites
      << ";family_retain_sites=" << retainable_summary.family_retain_sites
      << ";family_release_sites=" << retainable_summary.family_release_sites
      << ";family_autorelease_sites="
      << retainable_summary.family_autorelease_sites
      << ";compatibility_returns_retained_sites="
      << retainable_summary.compatibility_returns_retained_sites
      << ";compatibility_returns_not_retained_sites="
      << retainable_summary.compatibility_returns_not_retained_sites
      << ";compatibility_consumed_sites="
      << retainable_summary.compatibility_consumed_sites
      << ";deterministic=true;lane_contract="
      << kObjc3OwnershipBorrowedRetainableAbiCompletionLaneContract;
  return out.str();
}

std::string BuildOwnershipBorrowedRetainableAbiCompletionJson(
    const Objc3OwnershipSystemExtensionLoweringContract &contract,
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &source_summary,
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &retainable_summary,
    const std::string &lowering_replay_key,
    const std::string &abi_completion_replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3OwnershipSystemExtensionLoweringContract(contract) &&
      source_summary.returns_borrowed_attribute_sites <=
          contract.borrowed_return_callable_sites &&
      retainable_summary.family_retain_sites +
              retainable_summary.family_release_sites +
              retainable_summary.family_autorelease_sites ==
          contract.retainable_family_operation_callable_sites &&
      retainable_summary.compatibility_returns_retained_sites +
              retainable_summary.compatibility_returns_not_retained_sites +
              retainable_summary.compatibility_consumed_sites ==
          contract.retainable_family_alias_callable_sites;
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3OwnershipBorrowedRetainableAbiCompletionContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3OwnershipBorrowedRetainableAbiCompletionSurfacePath)
      << "\",\"lowering_contract_id\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringContractId)
      << "\",\"artifact_model\":\""
      << EscapeJsonString(kObjc3OwnershipBorrowedRetainableAbiCompletionArtifactModel)
      << "\",\"proof_model\":\""
      << EscapeJsonString(kObjc3OwnershipBorrowedRetainableAbiCompletionProofModel)
      << "\",\"lowering_replay_key\":\""
      << EscapeJsonString(lowering_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(abi_completion_replay_key)
      << "\",\"borrowed_parameter_sites\":"
      << contract.borrowed_parameter_sites
      << ",\"borrowed_return_callable_sites\":"
      << contract.borrowed_return_callable_sites
      << ",\"returns_borrowed_attribute_sites\":"
      << source_summary.returns_borrowed_attribute_sites
      << ",\"retainable_family_callable_sites\":"
      << contract.retainable_family_callable_sites
      << ",\"retainable_family_operation_callable_sites\":"
      << contract.retainable_family_operation_callable_sites
      << ",\"retainable_family_alias_callable_sites\":"
      << contract.retainable_family_alias_callable_sites
      << ",\"family_retain_sites\":"
      << retainable_summary.family_retain_sites
      << ",\"family_release_sites\":"
      << retainable_summary.family_release_sites
      << ",\"family_autorelease_sites\":"
      << retainable_summary.family_autorelease_sites
      << ",\"compatibility_returns_retained_sites\":"
      << retainable_summary.compatibility_returns_retained_sites
      << ",\"compatibility_returns_not_retained_sites\":"
      << retainable_summary.compatibility_returns_not_retained_sites
      << ",\"compatibility_consumed_sites\":"
      << retainable_summary.compatibility_consumed_sites
      << ",\"deterministic_handoff\":true"
      << ",\"ready_for_ir_emission\":"
      << (ready_for_ir_emission ? "true" : "false")
      << "}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
