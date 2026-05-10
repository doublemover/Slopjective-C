#include "artifacts/objc3_frontend_dispatch_semantic_artifacts.h"

#include <algorithm>
#include <cstddef>
#include <string>

#include "artifacts/objc3_frontend_artifact_dispatch_contract_constants.h"

namespace objc3::artifacts::frontend {
namespace {

template <typename ExprRootVisitor, typename StmtRootVisitor>
void WalkProgramExecutableRoots(const Objc3Program &program,
                                ExprRootVisitor &expr_root_visitor,
                                StmtRootVisitor &stmt_root_visitor) {
  for (const auto &global : program.globals) {
    expr_root_visitor(global.value.get());
  }
  for (const auto &function : program.functions) {
    for (const auto &stmt : function.body) {
      stmt_root_visitor(stmt.get());
    }
  }
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &method_decl : implementation_decl.methods) {
      if (!method_decl.has_body) {
        continue;
      }
      for (const auto &stmt : method_decl.body) {
        stmt_root_visitor(stmt.get());
      }
    }
  }
}

void AccumulateDispatchSurfaceClassificationExpr(
    const Expr *expr, Objc3DispatchSurfaceClassificationContract &contract);

void AccumulateDispatchSurfaceClassificationStmt(
    const Stmt *stmt, Objc3DispatchSurfaceClassificationContract &contract) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
    case Stmt::Kind::Let:
      AccumulateDispatchSurfaceClassificationExpr(stmt->let_stmt->value.get(),
                                                  contract);
      return;
    case Stmt::Kind::Assign:
      AccumulateDispatchSurfaceClassificationExpr(
          stmt->assign_stmt->value.get(), contract);
      return;
    case Stmt::Kind::Return:
      AccumulateDispatchSurfaceClassificationExpr(
          stmt->return_stmt->value.get(), contract);
      return;
    case Stmt::Kind::Expr:
      AccumulateDispatchSurfaceClassificationExpr(stmt->expr_stmt->value.get(),
                                                  contract);
      return;
    case Stmt::Kind::If:
      AccumulateDispatchSurfaceClassificationExpr(
          stmt->if_stmt->condition.get(), contract);
      for (const auto &nested : stmt->if_stmt->then_body) {
        AccumulateDispatchSurfaceClassificationStmt(nested.get(), contract);
      }
      for (const auto &nested : stmt->if_stmt->else_body) {
        AccumulateDispatchSurfaceClassificationStmt(nested.get(), contract);
      }
      return;
    case Stmt::Kind::DoWhile:
      for (const auto &nested : stmt->do_while_stmt->body) {
        AccumulateDispatchSurfaceClassificationStmt(nested.get(), contract);
      }
      AccumulateDispatchSurfaceClassificationExpr(
          stmt->do_while_stmt->condition.get(), contract);
      return;
    case Stmt::Kind::For:
      AccumulateDispatchSurfaceClassificationExpr(
          stmt->for_stmt->init.value.get(), contract);
      AccumulateDispatchSurfaceClassificationExpr(
          stmt->for_stmt->condition.get(), contract);
      AccumulateDispatchSurfaceClassificationExpr(
          stmt->for_stmt->step.value.get(), contract);
      for (const auto &nested : stmt->for_stmt->body) {
        AccumulateDispatchSurfaceClassificationStmt(nested.get(), contract);
      }
      return;
    case Stmt::Kind::Switch:
      AccumulateDispatchSurfaceClassificationExpr(
          stmt->switch_stmt->condition.get(), contract);
      for (const auto &case_stmt : stmt->switch_stmt->cases) {
        for (const auto &nested : case_stmt.body) {
          AccumulateDispatchSurfaceClassificationStmt(nested.get(), contract);
        }
      }
      return;
    case Stmt::Kind::While:
      AccumulateDispatchSurfaceClassificationExpr(
          stmt->while_stmt->condition.get(), contract);
      for (const auto &nested : stmt->while_stmt->body) {
        AccumulateDispatchSurfaceClassificationStmt(nested.get(), contract);
      }
      return;
    case Stmt::Kind::Block:
    case Stmt::Kind::Defer:
      for (const auto &nested : stmt->block_stmt->body) {
        AccumulateDispatchSurfaceClassificationStmt(nested.get(), contract);
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

void AccumulateDispatchSurfaceClassificationExpr(
    const Expr *expr, Objc3DispatchSurfaceClassificationContract &contract) {
  if (expr == nullptr) {
    return;
  }

  switch (expr->kind) {
    case Expr::Kind::MessageSend: {
      if (!expr->dispatch_surface_is_normalized) {
        contract.deterministic = false;
      }
      switch (expr->dispatch_surface_kind) {
        case Expr::DispatchSurfaceKind::Instance:
          ++contract.instance_dispatch_sites;
          break;
        case Expr::DispatchSurfaceKind::Class:
          ++contract.class_dispatch_sites;
          break;
        case Expr::DispatchSurfaceKind::Super:
          ++contract.super_dispatch_sites;
          break;
        case Expr::DispatchSurfaceKind::Direct:
          ++contract.direct_dispatch_sites;
          break;
        case Expr::DispatchSurfaceKind::Dynamic:
          ++contract.dynamic_dispatch_sites;
          break;
        case Expr::DispatchSurfaceKind::Unclassified:
        default:
          ++contract.dynamic_dispatch_sites;
          contract.deterministic = false;
          break;
      }
      AccumulateDispatchSurfaceClassificationExpr(expr->receiver.get(),
                                                  contract);
      for (const auto &arg : expr->args) {
        AccumulateDispatchSurfaceClassificationExpr(arg.get(), contract);
      }
      return;
    }
    case Expr::Kind::Binary:
      AccumulateDispatchSurfaceClassificationExpr(expr->left.get(), contract);
      AccumulateDispatchSurfaceClassificationExpr(expr->right.get(), contract);
      return;
    case Expr::Kind::Conditional:
      AccumulateDispatchSurfaceClassificationExpr(expr->left.get(), contract);
      AccumulateDispatchSurfaceClassificationExpr(expr->right.get(), contract);
      AccumulateDispatchSurfaceClassificationExpr(expr->third.get(), contract);
      return;
    case Expr::Kind::Call:
      AccumulateDispatchSurfaceClassificationExpr(expr->receiver.get(),
                                                  contract);
      for (const auto &arg : expr->args) {
        AccumulateDispatchSurfaceClassificationExpr(arg.get(), contract);
      }
      return;
    case Expr::Kind::BlockLiteral:
      return;
    case Expr::Kind::Number:
    case Expr::Kind::BoolLiteral:
    case Expr::Kind::NilLiteral:
    case Expr::Kind::Identifier:
      return;
  }
}

void AccumulateIdClassSelObjectPointerTypecheckSite(
    bool id_spelling,
    bool class_spelling,
    bool sel_spelling,
    bool object_pointer_type_spelling,
    const std::string &object_pointer_type_name,
    Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  const std::size_t active_spelling_count =
      (id_spelling ? 1u : 0u) + (class_spelling ? 1u : 0u) +
      (sel_spelling ? 1u : 0u) + (object_pointer_type_spelling ? 1u : 0u);
  if (active_spelling_count > 1u) {
    contract.deterministic = false;
  }

  if (id_spelling) {
    ++contract.id_typecheck_sites;
  }
  if (class_spelling) {
    ++contract.class_typecheck_sites;
  }
  if (sel_spelling) {
    ++contract.sel_typecheck_sites;
  }
  if (object_pointer_type_spelling) {
    ++contract.object_pointer_typecheck_sites;
    if (object_pointer_type_name.empty()) {
      contract.deterministic = false;
    }
  }

  if (active_spelling_count > 0u) {
    ++contract.total_typecheck_sites;
  }
}

void AccumulateIdClassSelObjectPointerTypecheckMethod(
    const Objc3MethodDecl &method,
    Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  AccumulateIdClassSelObjectPointerTypecheckSite(
      method.return_id_spelling, method.return_class_spelling,
      method.return_sel_spelling, method.return_object_pointer_type_spelling,
      method.return_object_pointer_type_name, contract);
  for (const auto &param : method.params) {
    AccumulateIdClassSelObjectPointerTypecheckSite(
        param.id_spelling, param.class_spelling, param.sel_spelling,
        param.object_pointer_type_spelling, param.object_pointer_type_name,
        contract);
  }
}

template <typename Container>
void AccumulateIdClassSelObjectPointerTypecheckObjcDeclarations(
    const Container &declarations,
    Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  for (const auto &declaration : declarations) {
    for (const auto &property : declaration.properties) {
      AccumulateIdClassSelObjectPointerTypecheckSite(
          property.id_spelling, property.class_spelling, property.sel_spelling,
          property.object_pointer_type_spelling,
          property.object_pointer_type_name, contract);
    }
    for (const auto &method : declaration.methods) {
      AccumulateIdClassSelObjectPointerTypecheckMethod(method, contract);
    }
  }
}

}  // namespace

Objc3DispatchDispatchControlLoweringContract
BuildDispatchDispatchControlLoweringSnapshot(
    const Objc3DispatchDispatchIntentSemanticModelSummary &semantic_summary,
    const Objc3DispatchDispatchIntentLegalitySummary &legality_summary,
    const Objc3DispatchDispatchIntentCompatibilitySummary
        &compatibility_summary) {
  Objc3DispatchDispatchControlLoweringContract contract;
  const std::size_t dispatch_intent_callable_capacity =
      compatibility_summary.callable_dispatch_intent_sites;
  const std::size_t dispatch_intent_container_capacity =
      compatibility_summary.container_dispatch_intent_sites;
  const std::size_t dispatch_intent_override_capacity =
      semantic_summary.effective_direct_member_sites +
      dispatch_intent_container_capacity;
  const std::size_t dispatch_intent_guard_capacity =
      dispatch_intent_callable_capacity + dispatch_intent_container_capacity;
  const std::size_t raw_guard_blocked_sites =
      legality_summary.illegal_final_superclass_sites +
      legality_summary.illegal_sealed_superclass_sites +
      legality_summary.illegal_final_override_sites +
      legality_summary.illegal_direct_override_sites +
      compatibility_summary.illegal_direct_dynamic_conflict_sites +
      compatibility_summary.illegal_final_dynamic_conflict_sites +
      compatibility_summary.illegal_non_method_callable_sites +
      compatibility_summary.illegal_protocol_method_sites +
      compatibility_summary.illegal_category_method_sites +
      compatibility_summary.illegal_category_container_sites;
  contract.direct_call_candidate_sites =
      semantic_summary.effective_direct_member_sites;
  contract.direct_members_defaulted_sites =
      semantic_summary.direct_members_defaulted_method_sites;
  contract.dynamic_opt_out_sites =
      semantic_summary.direct_members_dynamic_opt_out_sites;
  contract.final_container_sites = semantic_summary.final_container_sites;
  contract.sealed_container_sites = semantic_summary.sealed_container_sites;
  contract.override_legality_sites =
      std::min(legality_summary.override_sites,
               dispatch_intent_override_capacity);
  contract.metadata_preserved_callable_sites =
      dispatch_intent_callable_capacity;
  contract.metadata_preserved_container_sites =
      dispatch_intent_container_capacity;
  contract.guard_blocked_sites =
      std::min(raw_guard_blocked_sites, dispatch_intent_guard_capacity);
  contract.contract_violation_sites = 0;
  contract.deterministic =
      semantic_summary.deterministic &&
      semantic_summary.ready_for_core_implementation &&
      legality_summary.deterministic &&
      legality_summary.ready_for_lowering_and_runtime &&
      compatibility_summary.deterministic &&
      compatibility_summary.ready_for_lowering_and_runtime;
  return contract;
}

Objc3IdClassSelObjectPointerTypecheckContract
BuildIdClassSelObjectPointerTypecheckContract(const Objc3Program &program) {
  Objc3IdClassSelObjectPointerTypecheckContract contract;
  for (const auto &fn : program.functions) {
    AccumulateIdClassSelObjectPointerTypecheckSite(
        fn.return_id_spelling, fn.return_class_spelling,
        fn.return_sel_spelling, fn.return_object_pointer_type_spelling,
        fn.return_object_pointer_type_name, contract);
    for (const auto &param : fn.params) {
      AccumulateIdClassSelObjectPointerTypecheckSite(
          param.id_spelling, param.class_spelling, param.sel_spelling,
          param.object_pointer_type_spelling, param.object_pointer_type_name,
          contract);
    }
  }
  AccumulateIdClassSelObjectPointerTypecheckObjcDeclarations(
      program.protocols, contract);
  AccumulateIdClassSelObjectPointerTypecheckObjcDeclarations(
      program.interfaces, contract);
  AccumulateIdClassSelObjectPointerTypecheckObjcDeclarations(
      program.implementations, contract);
  return contract;
}

Objc3PropertySynthesisIvarBindingContract
BuildPropertySynthesisIvarBindingContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  const Objc3PropertySynthesisIvarBindingSummary &summary =
      sema_parity_surface.property_synthesis_ivar_binding_summary;
  Objc3PropertySynthesisIvarBindingContract contract;
  contract.property_synthesis_sites = summary.property_synthesis_sites;
  contract.property_synthesis_explicit_ivar_bindings =
      summary.property_synthesis_explicit_ivar_bindings;
  contract.property_synthesis_default_ivar_bindings =
      summary.property_synthesis_default_ivar_bindings;
  contract.interface_owned_property_synthesis_sites =
      summary.interface_owned_property_synthesis_sites;
  contract.implementation_property_redeclaration_sites =
      summary.implementation_property_redeclaration_sites;
  contract.ivar_binding_sites = summary.ivar_binding_sites;
  contract.ivar_binding_resolved = summary.ivar_binding_resolved;
  contract.ivar_binding_missing = summary.ivar_binding_missing;
  contract.ivar_binding_conflicts = summary.ivar_binding_conflicts;
  contract.deterministic =
      summary.deterministic &&
      sema_parity_surface.deterministic_property_synthesis_ivar_binding_handoff;
  return contract;
}

Objc3DispatchSurfaceClassificationContract
BuildDispatchSurfaceClassificationContract(const Objc3Program &program) {
  Objc3DispatchSurfaceClassificationContract contract;
  auto accumulate_expr = [&](const Expr *expr) {
    AccumulateDispatchSurfaceClassificationExpr(expr, contract);
  };
  auto accumulate_stmt = [&](const Stmt *stmt) {
    AccumulateDispatchSurfaceClassificationStmt(stmt, contract);
  };
  WalkProgramExecutableRoots(program, accumulate_expr, accumulate_stmt);
  return contract;
}

Objc3NilReceiverSemanticsFoldabilityContract
BuildNilReceiverSemanticsFoldabilityContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3NilReceiverSemanticsFoldabilityContract contract;
  contract.message_send_sites =
      sema_parity_surface.nil_receiver_semantics_foldability_sites_total;
  contract.receiver_nil_literal_sites =
      sema_parity_surface
          .nil_receiver_semantics_foldability_receiver_nil_literal_sites_total;
  contract.nil_receiver_semantics_enabled_sites =
      sema_parity_surface
          .nil_receiver_semantics_foldability_enabled_sites_total;
  contract.nil_receiver_foldable_sites =
      sema_parity_surface
          .nil_receiver_semantics_foldability_foldable_sites_total;
  contract.nil_receiver_runtime_dispatch_required_sites =
      sema_parity_surface
          .nil_receiver_semantics_foldability_runtime_dispatch_required_sites_total;
  contract.non_nil_receiver_sites =
      sema_parity_surface
          .nil_receiver_semantics_foldability_non_nil_receiver_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface
          .nil_receiver_semantics_foldability_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.nil_receiver_semantics_foldability_summary
          .deterministic &&
      sema_parity_surface
          .deterministic_nil_receiver_semantics_foldability_handoff;
  return contract;
}

Objc3SuperDispatchMethodFamilyContract BuildSuperDispatchMethodFamilyContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3SuperDispatchMethodFamilyContract contract;
  contract.message_send_sites =
      sema_parity_surface.super_dispatch_method_family_sites_total;
  contract.receiver_super_identifier_sites =
      sema_parity_surface
          .super_dispatch_method_family_receiver_super_identifier_sites_total;
  contract.super_dispatch_enabled_sites =
      sema_parity_surface.super_dispatch_method_family_enabled_sites_total;
  contract.super_dispatch_requires_class_context_sites =
      sema_parity_surface
          .super_dispatch_method_family_requires_class_context_sites_total;
  contract.method_family_init_sites =
      sema_parity_surface.super_dispatch_method_family_init_sites_total;
  contract.method_family_copy_sites =
      sema_parity_surface.super_dispatch_method_family_copy_sites_total;
  contract.method_family_mutable_copy_sites =
      sema_parity_surface.super_dispatch_method_family_mutable_copy_sites_total;
  contract.method_family_new_sites =
      sema_parity_surface.super_dispatch_method_family_new_sites_total;
  contract.method_family_none_sites =
      sema_parity_surface.super_dispatch_method_family_none_sites_total;
  contract.method_family_returns_retained_result_sites =
      sema_parity_surface
          .super_dispatch_method_family_returns_retained_result_sites_total;
  contract.method_family_returns_related_result_sites =
      sema_parity_surface
          .super_dispatch_method_family_returns_related_result_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface
          .super_dispatch_method_family_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.super_dispatch_method_family_summary.deterministic &&
      sema_parity_surface.deterministic_super_dispatch_method_family_handoff;
  return contract;
}

Objc3RuntimeLinkHostLinkContract BuildRuntimeLinkHostLinkContract(
    const Objc3DispatchAbiMarshallingContract &dispatch_abi_marshalling_contract,
    const Objc3NilReceiverSemanticsFoldabilityContract
        &nil_receiver_semantics_foldability_contract,
    const Objc3FrontendOptions &options) {
  Objc3RuntimeLinkHostLinkContract contract;
  contract.message_send_sites =
      dispatch_abi_marshalling_contract.message_send_sites;
  contract.runtime_link_required_sites =
      nil_receiver_semantics_foldability_contract
          .nil_receiver_runtime_dispatch_required_sites;
  if (contract.runtime_link_required_sites <= contract.message_send_sites) {
    contract.runtime_link_elided_sites =
        contract.message_send_sites - contract.runtime_link_required_sites;
  } else {
    contract.runtime_link_elided_sites = 0;
    contract.contract_violation_sites = 1;
  }
  contract.runtime_dispatch_arg_slots =
      options.lowering.max_message_send_args;
  contract.runtime_dispatch_declaration_parameter_count =
      contract.runtime_dispatch_arg_slots + 2u;
  contract.runtime_dispatch_symbol = options.lowering.runtime_dispatch_symbol;
  contract.default_runtime_dispatch_symbol_binding =
      contract.runtime_dispatch_symbol == kObjc3ArtifactRuntimeDispatchSymbol;
  contract.deterministic =
      dispatch_abi_marshalling_contract.deterministic &&
      nil_receiver_semantics_foldability_contract.deterministic;
  return contract;
}

Objc3RuntimeDispatchLoweringAbiContract BuildRuntimeDispatchLoweringAbiContract(
    const Objc3DispatchAbiMarshallingContract &dispatch_abi_marshalling_contract,
    const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api_summary) {
  Objc3RuntimeDispatchLoweringAbiContract contract;
  contract.message_send_sites =
      dispatch_abi_marshalling_contract.message_send_sites;
  contract.fixed_argument_slot_count =
      runtime_link_host_link_contract.runtime_dispatch_arg_slots;
  contract.runtime_dispatch_parameter_count =
      runtime_link_host_link_contract
          .runtime_dispatch_declaration_parameter_count;
  contract.canonical_runtime_dispatch_symbol =
      runtime_bootstrap_api_summary.dispatch_entrypoint_symbol;
  contract.default_lowering_target_symbol =
      contract.canonical_runtime_dispatch_symbol;
  contract.default_lowering_target_model =
      kObjc3ArtifactRuntimeDispatchLiveCutoverDefaultTargetModel;
  contract.strict_dispatch_error_model =
      kObjc3ArtifactRuntimeDispatchLiveCutoverStrictDispatchModel;
  contract.deferred_cases_model =
      kObjc3ArtifactRuntimeDispatchLiveCutoverDeferredCasesModel;
  contract.selector_lookup_symbol =
      runtime_bootstrap_api_summary.selector_lookup_symbol;
  contract.selector_handle_type =
      runtime_bootstrap_api_summary.selector_handle_type;
  contract.fail_closed =
      IsReadyObjc3RuntimeBootstrapApiSummary(runtime_bootstrap_api_summary);
  contract.deterministic =
      dispatch_abi_marshalling_contract.deterministic &&
      runtime_link_host_link_contract.deterministic &&
      !runtime_bootstrap_api_summary.replay_key.empty();
  return contract;
}

}  // namespace objc3::artifacts::frontend
