#include "pipeline/frontend_type_system_source_closure_helpers.h"

#include "pipeline/frontend_source_closure_replay_keys.h"

namespace objc3c::pipeline::orchestration {
namespace {

void CollectTypeSystemTypeSourceClosureExprSites(
    const Expr *expr, Objc3FrontendTypeSystemTypeSourceClosureSummary &summary) {
  if (expr == nullptr) {
    return;
  }
  if (expr->kind == Expr::Kind::Binary && expr->op == "??") {
    ++summary.nil_coalescing_sites;
  }
  if (expr->kind == Expr::Kind::MessageSend && expr->optional_send_enabled) {
    ++summary.optional_send_sites;
  }
  if (expr->kind == Expr::Kind::MessageSend &&
      expr->optional_member_access_enabled) {
    ++summary.optional_member_access_sites;
  }
  if (expr->typed_keypath_literal_enabled) {
    ++summary.typed_keypath_literal_sites;
  }
  CollectTypeSystemTypeSourceClosureExprSites(expr->receiver.get(), summary);
  CollectTypeSystemTypeSourceClosureExprSites(expr->left.get(), summary);
  CollectTypeSystemTypeSourceClosureExprSites(expr->right.get(), summary);
  CollectTypeSystemTypeSourceClosureExprSites(expr->third.get(), summary);
  for (const auto &arg : expr->args) {
    CollectTypeSystemTypeSourceClosureExprSites(arg.get(), summary);
  }
  for (const auto &key : expr->collection_keys) {
    CollectTypeSystemTypeSourceClosureExprSites(key.get(), summary);
  }
  for (const auto &value : expr->collection_values) {
    CollectTypeSystemTypeSourceClosureExprSites(value.get(), summary);
  }
  for (const auto &stmt : expr->block_body) {
    if (stmt != nullptr) {
      const Stmt *nested = stmt.get();
      switch (nested->kind) {
      case Stmt::Kind::Let:
        if (nested->let_stmt != nullptr) {
          CollectTypeSystemTypeSourceClosureExprSites(
              nested->let_stmt->value.get(), summary);
        }
        break;
      case Stmt::Kind::Assign:
        if (nested->assign_stmt != nullptr) {
          CollectTypeSystemTypeSourceClosureExprSites(
              nested->assign_stmt->value.get(), summary);
        }
        break;
      case Stmt::Kind::CollectionMutation:
        if (nested->collection_mutation_stmt != nullptr) {
          CollectTypeSystemTypeSourceClosureExprSites(
              nested->collection_mutation_stmt->key_or_index.get(), summary);
          CollectTypeSystemTypeSourceClosureExprSites(
              nested->collection_mutation_stmt->value.get(), summary);
        }
        break;
      case Stmt::Kind::Return:
        if (nested->return_stmt != nullptr) {
          CollectTypeSystemTypeSourceClosureExprSites(
              nested->return_stmt->value.get(), summary);
        }
        break;
      case Stmt::Kind::Expr:
        if (nested->expr_stmt != nullptr) {
          CollectTypeSystemTypeSourceClosureExprSites(
              nested->expr_stmt->value.get(), summary);
        }
        break;
      case Stmt::Kind::If:
      case Stmt::Kind::DoWhile:
      case Stmt::Kind::For:
      case Stmt::Kind::ForIn:
      case Stmt::Kind::Switch:
      case Stmt::Kind::While:
      case Stmt::Kind::Block:
      case Stmt::Kind::Defer:
      case Stmt::Kind::Break:
      case Stmt::Kind::Continue:
      case Stmt::Kind::Empty:
        break;
      }
    }
  }
}

void CollectTypeSystemTypeSourceClosureStmtSites(
    const Stmt *stmt, Objc3FrontendTypeSystemTypeSourceClosureSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectTypeSystemTypeSourceClosureExprSites(
          stmt->let_stmt->value.get(), summary);
    }
    break;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectTypeSystemTypeSourceClosureExprSites(
          stmt->assign_stmt->value.get(), summary);
    }
    break;
  case Stmt::Kind::CollectionMutation:
    if (stmt->collection_mutation_stmt != nullptr) {
      CollectTypeSystemTypeSourceClosureExprSites(
          stmt->collection_mutation_stmt->key_or_index.get(), summary);
      CollectTypeSystemTypeSourceClosureExprSites(
          stmt->collection_mutation_stmt->value.get(), summary);
    }
    break;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectTypeSystemTypeSourceClosureExprSites(
          stmt->return_stmt->value.get(), summary);
    }
    break;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectTypeSystemTypeSourceClosureExprSites(
          stmt->expr_stmt->value.get(), summary);
    }
    break;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      CollectTypeSystemTypeSourceClosureExprSites(
          stmt->if_stmt->condition.get(), summary);
      for (const auto &child : stmt->if_stmt->then_body) {
        CollectTypeSystemTypeSourceClosureStmtSites(child.get(), summary);
      }
      for (const auto &child : stmt->if_stmt->else_body) {
        CollectTypeSystemTypeSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      CollectTypeSystemTypeSourceClosureExprSites(
          stmt->do_while_stmt->condition.get(), summary);
      for (const auto &child : stmt->do_while_stmt->body) {
        CollectTypeSystemTypeSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectTypeSystemTypeSourceClosureExprSites(
          stmt->for_stmt->init.value.get(), summary);
      CollectTypeSystemTypeSourceClosureExprSites(
          stmt->for_stmt->condition.get(), summary);
      CollectTypeSystemTypeSourceClosureExprSites(
          stmt->for_stmt->step.value.get(), summary);
      for (const auto &child : stmt->for_stmt->body) {
        CollectTypeSystemTypeSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::ForIn:
    if (stmt->for_in_stmt != nullptr) {
      CollectTypeSystemTypeSourceClosureExprSites(
          stmt->for_in_stmt->collection.get(), summary);
      for (const auto &child : stmt->for_in_stmt->body) {
        CollectTypeSystemTypeSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      CollectTypeSystemTypeSourceClosureExprSites(
          stmt->switch_stmt->condition.get(), summary);
      for (const auto &case_stmt : stmt->switch_stmt->cases) {
        for (const auto &child : case_stmt.body) {
          CollectTypeSystemTypeSourceClosureStmtSites(child.get(), summary);
        }
      }
    }
    break;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      CollectTypeSystemTypeSourceClosureExprSites(
          stmt->while_stmt->condition.get(), summary);
      for (const auto &child : stmt->while_stmt->body) {
        CollectTypeSystemTypeSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &child : stmt->block_stmt->body) {
        CollectTypeSystemTypeSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    break;
  }
}

void CollectValueOptionalFunctionTypeSites(
    const FunctionDecl &fn,
    Objc3FrontendTypeSystemTypeSourceClosureSummary &summary) {
  if (fn.return_value_optional.present) {
    ++summary.value_optional_type_signature_sites;
  }
  for (const FuncParam &param : fn.params) {
    if (param.value_optional.present) {
      ++summary.value_optional_type_signature_sites;
    }
  }
}

void CollectValueOptionalMethodTypeSites(
    const Objc3MethodDecl &method,
    Objc3FrontendTypeSystemTypeSourceClosureSummary &summary) {
  if (method.return_value_optional.present) {
    ++summary.value_optional_type_signature_sites;
  }
  for (const FuncParam &param : method.params) {
    if (param.value_optional.present) {
      ++summary.value_optional_type_signature_sites;
    }
  }
}

}  // namespace

Objc3FrontendTypeSystemTypeSourceClosureSummary
BuildTypeSystemTypeSourceClosureSummary(
    const Objc3Program &program,
    const Objc3FrontendObjectPointerNullabilityGenericsSummary
        &object_pointer_summary) {
  Objc3FrontendTypeSystemTypeSourceClosureSummary summary;
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method_decl : protocol_decl.methods) {
      if (method_decl.protocol_requirement_kind ==
          Objc3ProtocolRequirementKind::Optional) {
        ++summary.protocol_optional_method_count;
      } else {
        ++summary.protocol_required_method_count;
      }
    }
    for (const auto &property_decl : protocol_decl.properties) {
      if (property_decl.protocol_requirement_kind ==
          Objc3ProtocolRequirementKind::Optional) {
        ++summary.protocol_optional_property_count;
      } else {
        ++summary.protocol_required_property_count;
      }
    }
  }

  summary.object_pointer_type_spelling_sites =
      object_pointer_summary.object_pointer_type_spellings;
  summary.pointer_declarator_entries =
      object_pointer_summary.pointer_declarator_entries;
  summary.nullability_suffix_entries =
      object_pointer_summary.nullability_suffix_entries;
  summary.generic_suffix_entries =
      object_pointer_summary.generic_suffix_entries;
  summary.protocol_optional_partition_source_supported = true;
  summary.object_pointer_nullability_source_supported = true;
  summary.pragmatic_generic_suffix_source_supported = true;
  summary.optional_binding_source_supported = true;
  summary.optional_send_source_supported = true;
  summary.nil_coalescing_source_supported = true;
  summary.typed_keypath_literal_source_supported = true;
  summary.value_optional_type_signature_source_supported = true;
  summary.value_optional_semantic_type_admission_supported = true;
  summary.value_optional_stable_layout_contract_supported = true;
  summary.value_optional_binding_narrowing_contract_supported = true;
  summary.value_optional_interface_roundtrip_supported = true;
  summary.optional_member_access_fail_closed = false;
  summary.value_optional_runtime_execution_fail_closed = true;
  for (const auto &fn : program.functions) {
    CollectValueOptionalFunctionTypeSites(fn, summary);
    for (const auto &stmt : fn.body) {
      CollectTypeSystemTypeSourceClosureStmtSites(stmt.get(), summary);
    }
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      CollectValueOptionalMethodTypeSites(method, summary);
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method : protocol_decl.methods) {
      CollectValueOptionalMethodTypeSites(method, summary);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      CollectValueOptionalMethodTypeSites(method, summary);
      for (const auto &stmt : method.body) {
        CollectTypeSystemTypeSourceClosureStmtSites(stmt.get(), summary);
      }
    }
  }
  summary.deterministic_handoff =
      object_pointer_summary
          .deterministic_object_pointer_nullability_generics_handoff &&
      summary.protocol_required_method_count +
              summary.protocol_optional_method_count >=
          summary.protocol_optional_method_count &&
      summary.protocol_required_property_count +
              summary.protocol_optional_property_count >=
          summary.protocol_optional_property_count;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key = BuildTypeSystemTypeSourceClosureReplayKey(summary);
  return summary;
}

}  // namespace objc3c::pipeline::orchestration
