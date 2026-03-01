#include "pipeline/objc3_frontend_artifacts.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#include "ir/objc3_ir_emitter.h"

namespace {

const char *TypeName(ValueType type) {
  switch (type) {
    case ValueType::I32:
      return "i32";
    case ValueType::Bool:
      return "bool";
    case ValueType::Void:
      return "void";
    case ValueType::Function:
      return "function";
    default:
      return "unknown";
  }
}

const char *CompatibilityModeName(Objc3FrontendCompatibilityMode mode) {
  switch (mode) {
    case Objc3FrontendCompatibilityMode::kLegacy:
      return "legacy";
    case Objc3FrontendCompatibilityMode::kCanonical:
    default:
      return "canonical";
  }
}

std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message) {
  std::ostringstream out;
  out << "error:" << line << ":" << column << ": " << message << " [" << code << "]";
  return out.str();
}

std::vector<std::string> FlattenStageDiagnostics(const Objc3FrontendDiagnosticsBus &diagnostics_bus) {
  std::vector<std::string> diagnostics;
  diagnostics.reserve(diagnostics_bus.size());
  diagnostics.insert(diagnostics.end(), diagnostics_bus.lexer.begin(), diagnostics_bus.lexer.end());
  diagnostics.insert(diagnostics.end(), diagnostics_bus.parser.begin(), diagnostics_bus.parser.end());
  diagnostics.insert(diagnostics.end(), diagnostics_bus.semantic.begin(), diagnostics_bus.semantic.end());
  return diagnostics;
}

Objc3PropertySynthesisIvarBindingContract BuildPropertySynthesisIvarBindingContract(
    const Objc3FrontendPropertyAttributeSummary &summary) {
  return Objc3DefaultPropertySynthesisIvarBindingContract(
      summary.property_declaration_entries,
      summary.deterministic_property_attribute_handoff);
}

void AccumulateIdClassSelObjectPointerTypecheckSite(
    bool id_spelling,
    bool class_spelling,
    bool sel_spelling,
    bool object_pointer_type_spelling,
    const std::string &object_pointer_type_name,
    Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  const std::size_t active_spelling_count =
      (id_spelling ? 1u : 0u) +
      (class_spelling ? 1u : 0u) +
      (sel_spelling ? 1u : 0u) +
      (object_pointer_type_spelling ? 1u : 0u);
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
  AccumulateIdClassSelObjectPointerTypecheckSite(method.return_id_spelling,
                                                 method.return_class_spelling,
                                                 method.return_sel_spelling,
                                                 method.return_object_pointer_type_spelling,
                                                 method.return_object_pointer_type_name,
                                                 contract);
  for (const auto &param : method.params) {
    AccumulateIdClassSelObjectPointerTypecheckSite(param.id_spelling,
                                                   param.class_spelling,
                                                   param.sel_spelling,
                                                   param.object_pointer_type_spelling,
                                                   param.object_pointer_type_name,
                                                   contract);
  }
}

template <typename Container>
void AccumulateIdClassSelObjectPointerTypecheckObjcDeclarations(
    const Container &declarations,
    Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  for (const auto &declaration : declarations) {
    for (const auto &property : declaration.properties) {
      AccumulateIdClassSelObjectPointerTypecheckSite(property.id_spelling,
                                                     property.class_spelling,
                                                     property.sel_spelling,
                                                     property.object_pointer_type_spelling,
                                                     property.object_pointer_type_name,
                                                     contract);
    }
    for (const auto &method : declaration.methods) {
      AccumulateIdClassSelObjectPointerTypecheckMethod(method, contract);
    }
  }
}

Objc3IdClassSelObjectPointerTypecheckContract BuildIdClassSelObjectPointerTypecheckContract(
    const Objc3Program &program) {
  Objc3IdClassSelObjectPointerTypecheckContract contract;
  for (const auto &fn : program.functions) {
    AccumulateIdClassSelObjectPointerTypecheckSite(fn.return_id_spelling,
                                                   fn.return_class_spelling,
                                                   fn.return_sel_spelling,
                                                   fn.return_object_pointer_type_spelling,
                                                   fn.return_object_pointer_type_name,
                                                   contract);
    for (const auto &param : fn.params) {
      AccumulateIdClassSelObjectPointerTypecheckSite(param.id_spelling,
                                                     param.class_spelling,
                                                     param.sel_spelling,
                                                     param.object_pointer_type_spelling,
                                                     param.object_pointer_type_name,
                                                     contract);
    }
  }
  AccumulateIdClassSelObjectPointerTypecheckObjcDeclarations(program.protocols, contract);
  AccumulateIdClassSelObjectPointerTypecheckObjcDeclarations(program.interfaces, contract);
  AccumulateIdClassSelObjectPointerTypecheckObjcDeclarations(program.implementations, contract);
  return contract;
}

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

void AccumulateMessageSendSelectorLoweringExpr(
    const Expr *expr,
    Objc3MessageSendSelectorLoweringContract &contract,
    std::unordered_set<std::string> &selector_literals) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
    case Expr::Kind::MessageSend: {
      ++contract.message_send_sites;
      ++contract.receiver_expression_sites;
      if (expr->args.empty()) {
        ++contract.unary_selector_sites;
      } else {
        ++contract.keyword_selector_sites;
      }
      contract.argument_expression_sites += expr->args.size();
      const std::size_t selector_pieces = CountSelectorPieces(expr->selector);
      contract.selector_piece_sites += selector_pieces;
      if (selector_pieces == 0u) {
        contract.deterministic = false;
      } else {
        selector_literals.insert(expr->selector);
      }
      AccumulateMessageSendSelectorLoweringExpr(expr->receiver.get(), contract, selector_literals);
      for (const auto &arg : expr->args) {
        AccumulateMessageSendSelectorLoweringExpr(arg.get(), contract, selector_literals);
      }
      return;
    }
    case Expr::Kind::Binary:
      AccumulateMessageSendSelectorLoweringExpr(expr->left.get(), contract, selector_literals);
      AccumulateMessageSendSelectorLoweringExpr(expr->right.get(), contract, selector_literals);
      return;
    case Expr::Kind::Conditional:
      AccumulateMessageSendSelectorLoweringExpr(expr->left.get(), contract, selector_literals);
      AccumulateMessageSendSelectorLoweringExpr(expr->right.get(), contract, selector_literals);
      AccumulateMessageSendSelectorLoweringExpr(expr->third.get(), contract, selector_literals);
      return;
    case Expr::Kind::Call:
      for (const auto &arg : expr->args) {
        AccumulateMessageSendSelectorLoweringExpr(arg.get(), contract, selector_literals);
      }
      return;
    default:
      return;
  }
}

void AccumulateMessageSendSelectorLoweringForClause(
    const ForClause &clause,
    Objc3MessageSendSelectorLoweringContract &contract,
    std::unordered_set<std::string> &selector_literals) {
  if (clause.value != nullptr) {
    AccumulateMessageSendSelectorLoweringExpr(clause.value.get(), contract, selector_literals);
  }
}

void AccumulateMessageSendSelectorLoweringStmt(
    const Stmt *stmt,
    Objc3MessageSendSelectorLoweringContract &contract,
    std::unordered_set<std::string> &selector_literals) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (stmt->let_stmt != nullptr) {
        AccumulateMessageSendSelectorLoweringExpr(stmt->let_stmt->value.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        AccumulateMessageSendSelectorLoweringExpr(stmt->assign_stmt->value.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        AccumulateMessageSendSelectorLoweringExpr(stmt->return_stmt->value.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        AccumulateMessageSendSelectorLoweringExpr(stmt->expr_stmt->value.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt == nullptr) {
        return;
      }
      AccumulateMessageSendSelectorLoweringExpr(stmt->if_stmt->condition.get(), contract, selector_literals);
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        AccumulateMessageSendSelectorLoweringStmt(then_stmt.get(), contract, selector_literals);
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        AccumulateMessageSendSelectorLoweringStmt(else_stmt.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt == nullptr) {
        return;
      }
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        AccumulateMessageSendSelectorLoweringStmt(body_stmt.get(), contract, selector_literals);
      }
      AccumulateMessageSendSelectorLoweringExpr(stmt->do_while_stmt->condition.get(), contract, selector_literals);
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt == nullptr) {
        return;
      }
      AccumulateMessageSendSelectorLoweringForClause(stmt->for_stmt->init, contract, selector_literals);
      AccumulateMessageSendSelectorLoweringExpr(stmt->for_stmt->condition.get(), contract, selector_literals);
      AccumulateMessageSendSelectorLoweringForClause(stmt->for_stmt->step, contract, selector_literals);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        AccumulateMessageSendSelectorLoweringStmt(body_stmt.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt == nullptr) {
        return;
      }
      AccumulateMessageSendSelectorLoweringExpr(stmt->switch_stmt->condition.get(), contract, selector_literals);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          AccumulateMessageSendSelectorLoweringStmt(case_stmt.get(), contract, selector_literals);
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt == nullptr) {
        return;
      }
      AccumulateMessageSendSelectorLoweringExpr(stmt->while_stmt->condition.get(), contract, selector_literals);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        AccumulateMessageSendSelectorLoweringStmt(body_stmt.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::Block:
      if (stmt->block_stmt == nullptr) {
        return;
      }
      for (const auto &body_stmt : stmt->block_stmt->body) {
        AccumulateMessageSendSelectorLoweringStmt(body_stmt.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

Objc3MessageSendSelectorLoweringContract BuildMessageSendSelectorLoweringContract(const Objc3Program &program) {
  Objc3MessageSendSelectorLoweringContract contract;
  std::unordered_set<std::string> selector_literals;

  for (const auto &global : program.globals) {
    AccumulateMessageSendSelectorLoweringExpr(global.value.get(), contract, selector_literals);
  }
  for (const auto &function : program.functions) {
    for (const auto &stmt : function.body) {
      AccumulateMessageSendSelectorLoweringStmt(stmt.get(), contract, selector_literals);
    }
  }

  contract.selector_literal_entries = selector_literals.size();
  for (const auto &selector : selector_literals) {
    contract.selector_literal_characters += selector.size();
  }
  return contract;
}

void AccumulateDispatchAbiMarshallingExpr(
    const Expr *expr,
    std::size_t runtime_dispatch_arg_slots,
    Objc3DispatchAbiMarshallingContract &contract) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
    case Expr::Kind::MessageSend: {
      ++contract.message_send_sites;
      ++contract.receiver_slots_marshaled;
      ++contract.selector_slots_marshaled;
      const std::size_t actual_args = expr->args.size();
      const std::size_t marshalled_args = std::min(actual_args, runtime_dispatch_arg_slots);
      contract.argument_value_slots_marshaled += marshalled_args;
      if (actual_args > runtime_dispatch_arg_slots) {
        contract.deterministic = false;
      }
      contract.argument_padding_slots_marshaled += (runtime_dispatch_arg_slots - marshalled_args);
      contract.argument_total_slots_marshaled += runtime_dispatch_arg_slots;
      AccumulateDispatchAbiMarshallingExpr(expr->receiver.get(), runtime_dispatch_arg_slots, contract);
      for (const auto &arg : expr->args) {
        AccumulateDispatchAbiMarshallingExpr(arg.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    }
    case Expr::Kind::Binary:
      AccumulateDispatchAbiMarshallingExpr(expr->left.get(), runtime_dispatch_arg_slots, contract);
      AccumulateDispatchAbiMarshallingExpr(expr->right.get(), runtime_dispatch_arg_slots, contract);
      return;
    case Expr::Kind::Conditional:
      AccumulateDispatchAbiMarshallingExpr(expr->left.get(), runtime_dispatch_arg_slots, contract);
      AccumulateDispatchAbiMarshallingExpr(expr->right.get(), runtime_dispatch_arg_slots, contract);
      AccumulateDispatchAbiMarshallingExpr(expr->third.get(), runtime_dispatch_arg_slots, contract);
      return;
    case Expr::Kind::Call:
      for (const auto &arg : expr->args) {
        AccumulateDispatchAbiMarshallingExpr(arg.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    default:
      return;
  }
}

void AccumulateDispatchAbiMarshallingForClause(
    const ForClause &clause,
    std::size_t runtime_dispatch_arg_slots,
    Objc3DispatchAbiMarshallingContract &contract) {
  if (clause.value != nullptr) {
    AccumulateDispatchAbiMarshallingExpr(clause.value.get(), runtime_dispatch_arg_slots, contract);
  }
}

void AccumulateDispatchAbiMarshallingStmt(
    const Stmt *stmt,
    std::size_t runtime_dispatch_arg_slots,
    Objc3DispatchAbiMarshallingContract &contract) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (stmt->let_stmt != nullptr) {
        AccumulateDispatchAbiMarshallingExpr(stmt->let_stmt->value.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        AccumulateDispatchAbiMarshallingExpr(stmt->assign_stmt->value.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        AccumulateDispatchAbiMarshallingExpr(stmt->return_stmt->value.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        AccumulateDispatchAbiMarshallingExpr(stmt->expr_stmt->value.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt == nullptr) {
        return;
      }
      AccumulateDispatchAbiMarshallingExpr(stmt->if_stmt->condition.get(), runtime_dispatch_arg_slots, contract);
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        AccumulateDispatchAbiMarshallingStmt(then_stmt.get(), runtime_dispatch_arg_slots, contract);
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        AccumulateDispatchAbiMarshallingStmt(else_stmt.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt == nullptr) {
        return;
      }
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        AccumulateDispatchAbiMarshallingStmt(body_stmt.get(), runtime_dispatch_arg_slots, contract);
      }
      AccumulateDispatchAbiMarshallingExpr(stmt->do_while_stmt->condition.get(), runtime_dispatch_arg_slots, contract);
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt == nullptr) {
        return;
      }
      AccumulateDispatchAbiMarshallingForClause(stmt->for_stmt->init, runtime_dispatch_arg_slots, contract);
      AccumulateDispatchAbiMarshallingExpr(stmt->for_stmt->condition.get(), runtime_dispatch_arg_slots, contract);
      AccumulateDispatchAbiMarshallingForClause(stmt->for_stmt->step, runtime_dispatch_arg_slots, contract);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        AccumulateDispatchAbiMarshallingStmt(body_stmt.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt == nullptr) {
        return;
      }
      AccumulateDispatchAbiMarshallingExpr(stmt->switch_stmt->condition.get(), runtime_dispatch_arg_slots, contract);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          AccumulateDispatchAbiMarshallingStmt(case_stmt.get(), runtime_dispatch_arg_slots, contract);
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt == nullptr) {
        return;
      }
      AccumulateDispatchAbiMarshallingExpr(stmt->while_stmt->condition.get(), runtime_dispatch_arg_slots, contract);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        AccumulateDispatchAbiMarshallingStmt(body_stmt.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::Block:
      if (stmt->block_stmt == nullptr) {
        return;
      }
      for (const auto &body_stmt : stmt->block_stmt->body) {
        AccumulateDispatchAbiMarshallingStmt(body_stmt.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

Objc3DispatchAbiMarshallingContract BuildDispatchAbiMarshallingContract(
    const Objc3Program &program,
    std::size_t runtime_dispatch_arg_slots) {
  Objc3DispatchAbiMarshallingContract contract;
  contract.runtime_dispatch_arg_slots = runtime_dispatch_arg_slots;

  for (const auto &global : program.globals) {
    AccumulateDispatchAbiMarshallingExpr(global.value.get(), runtime_dispatch_arg_slots, contract);
  }
  for (const auto &function : program.functions) {
    for (const auto &stmt : function.body) {
      AccumulateDispatchAbiMarshallingStmt(stmt.get(), runtime_dispatch_arg_slots, contract);
    }
  }

  contract.total_marshaled_slots = contract.receiver_slots_marshaled +
                                   contract.selector_slots_marshaled +
                                   contract.argument_total_slots_marshaled;
  return contract;
}

Objc3NilReceiverSemanticsFoldabilityContract BuildNilReceiverSemanticsFoldabilityContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3NilReceiverSemanticsFoldabilityContract contract;
  contract.message_send_sites =
      sema_parity_surface.nil_receiver_semantics_foldability_sites_total;
  contract.receiver_nil_literal_sites =
      sema_parity_surface.nil_receiver_semantics_foldability_receiver_nil_literal_sites_total;
  contract.nil_receiver_semantics_enabled_sites =
      sema_parity_surface.nil_receiver_semantics_foldability_enabled_sites_total;
  contract.nil_receiver_foldable_sites =
      sema_parity_surface.nil_receiver_semantics_foldability_foldable_sites_total;
  contract.nil_receiver_runtime_dispatch_required_sites =
      sema_parity_surface.nil_receiver_semantics_foldability_runtime_dispatch_required_sites_total;
  contract.non_nil_receiver_sites =
      sema_parity_surface.nil_receiver_semantics_foldability_non_nil_receiver_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.nil_receiver_semantics_foldability_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.nil_receiver_semantics_foldability_summary.deterministic &&
      sema_parity_surface.deterministic_nil_receiver_semantics_foldability_handoff;
  return contract;
}

Objc3SuperDispatchMethodFamilyContract BuildSuperDispatchMethodFamilyContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3SuperDispatchMethodFamilyContract contract;
  contract.message_send_sites =
      sema_parity_surface.super_dispatch_method_family_sites_total;
  contract.receiver_super_identifier_sites =
      sema_parity_surface.super_dispatch_method_family_receiver_super_identifier_sites_total;
  contract.super_dispatch_enabled_sites =
      sema_parity_surface.super_dispatch_method_family_enabled_sites_total;
  contract.super_dispatch_requires_class_context_sites =
      sema_parity_surface.super_dispatch_method_family_requires_class_context_sites_total;
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
      sema_parity_surface.super_dispatch_method_family_returns_retained_result_sites_total;
  contract.method_family_returns_related_result_sites =
      sema_parity_surface.super_dispatch_method_family_returns_related_result_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.super_dispatch_method_family_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.super_dispatch_method_family_summary.deterministic &&
      sema_parity_surface.deterministic_super_dispatch_method_family_handoff;
  return contract;
}

Objc3RuntimeShimHostLinkContract BuildRuntimeShimHostLinkContract(
    const Objc3DispatchAbiMarshallingContract &dispatch_abi_marshalling_contract,
    const Objc3NilReceiverSemanticsFoldabilityContract &nil_receiver_semantics_foldability_contract,
    const Objc3FrontendOptions &options) {
  Objc3RuntimeShimHostLinkContract contract;
  contract.message_send_sites = dispatch_abi_marshalling_contract.message_send_sites;
  contract.runtime_shim_required_sites =
      nil_receiver_semantics_foldability_contract.nil_receiver_runtime_dispatch_required_sites;
  if (contract.runtime_shim_required_sites <= contract.message_send_sites) {
    contract.runtime_shim_elided_sites =
        contract.message_send_sites - contract.runtime_shim_required_sites;
  } else {
    contract.runtime_shim_elided_sites = 0;
    contract.contract_violation_sites = 1;
  }
  contract.runtime_dispatch_arg_slots = options.lowering.max_message_send_args;
  contract.runtime_dispatch_declaration_parameter_count = contract.runtime_dispatch_arg_slots + 2u;
  contract.runtime_dispatch_symbol = options.lowering.runtime_dispatch_symbol;
  contract.default_runtime_dispatch_symbol_binding =
      contract.runtime_dispatch_symbol == kObjc3RuntimeDispatchSymbol;
  contract.deterministic =
      dispatch_abi_marshalling_contract.deterministic &&
      nil_receiver_semantics_foldability_contract.deterministic;
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

Objc3RetainReleaseOperationLoweringContract BuildRetainReleaseOperationLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3RetainReleaseOperationLoweringContract contract;
  contract.ownership_qualified_sites =
      sema_parity_surface.retain_release_operation_ownership_qualified_sites_total;
  contract.retain_insertion_sites =
      sema_parity_surface.retain_release_operation_retain_insertion_sites_total;
  contract.release_insertion_sites =
      sema_parity_surface.retain_release_operation_release_insertion_sites_total;
  contract.autorelease_insertion_sites =
      sema_parity_surface.retain_release_operation_autorelease_insertion_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.retain_release_operation_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.retain_release_operation_summary.deterministic &&
      sema_parity_surface.deterministic_retain_release_operation_handoff;
  return contract;
}

Objc3AutoreleasePoolScopeLoweringContract BuildAutoreleasePoolScopeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3AutoreleasePoolScopeLoweringContract contract;
  contract.scope_sites = sema_parity_surface.autoreleasepool_scope_sites_total;
  contract.scope_symbolized_sites = sema_parity_surface.autoreleasepool_scope_symbolized_sites_total;
  contract.max_scope_depth = sema_parity_surface.autoreleasepool_scope_max_depth_total;
  contract.scope_entry_transition_sites = sema_parity_surface.autoreleasepool_scope_sites_total;
  contract.scope_exit_transition_sites = sema_parity_surface.autoreleasepool_scope_sites_total;
  contract.contract_violation_sites = sema_parity_surface.autoreleasepool_scope_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.autoreleasepool_scope_summary.deterministic &&
      sema_parity_surface.deterministic_autoreleasepool_scope_handoff;
  return contract;
}

Objc3WeakUnownedSemanticsLoweringContract BuildWeakUnownedSemanticsLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3WeakUnownedSemanticsLoweringContract contract;
  contract.ownership_candidate_sites =
      sema_parity_surface.weak_unowned_semantics_ownership_candidate_sites_total;
  contract.weak_reference_sites =
      sema_parity_surface.weak_unowned_semantics_weak_reference_sites_total;
  contract.unowned_reference_sites =
      sema_parity_surface.weak_unowned_semantics_unowned_reference_sites_total;
  contract.unowned_safe_reference_sites =
      sema_parity_surface.weak_unowned_semantics_unowned_safe_reference_sites_total;
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
      sema_parity_surface.ownership_arc_weak_unowned_conflict_diagnostic_sites_total;
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
      sema_parity_surface.block_literal_capture_semantics_parameter_entries_total;
  contract.block_capture_entries =
      sema_parity_surface.block_literal_capture_semantics_capture_entries_total;
  contract.block_body_statement_entries =
      sema_parity_surface.block_literal_capture_semantics_body_statement_entries_total;
  contract.block_empty_capture_sites =
      sema_parity_surface.block_literal_capture_semantics_empty_capture_sites_total;
  contract.block_nondeterministic_capture_sites =
      sema_parity_surface.block_literal_capture_semantics_nondeterministic_capture_sites_total;
  contract.block_non_normalized_sites =
      sema_parity_surface.block_literal_capture_semantics_non_normalized_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.block_literal_capture_semantics_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_literal_capture_semantics_summary.deterministic &&
      sema_parity_surface.deterministic_block_literal_capture_semantics_handoff;
  return contract;
}

Objc3BlockAbiInvokeTrampolineLoweringContract BuildBlockAbiInvokeTrampolineLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3BlockAbiInvokeTrampolineLoweringContract contract;
  contract.block_literal_sites =
      sema_parity_surface.block_abi_invoke_trampoline_sites_total;
  contract.invoke_argument_slots_total =
      sema_parity_surface.block_abi_invoke_trampoline_invoke_argument_slots_total;
  contract.capture_word_count_total =
      sema_parity_surface.block_abi_invoke_trampoline_capture_word_count_total;
  contract.parameter_entries_total =
      sema_parity_surface.block_abi_invoke_trampoline_parameter_entries_total;
  contract.capture_entries_total =
      sema_parity_surface.block_abi_invoke_trampoline_capture_entries_total;
  contract.body_statement_entries_total =
      sema_parity_surface.block_abi_invoke_trampoline_body_statement_entries_total;
  contract.descriptor_symbolized_sites =
      sema_parity_surface.block_abi_invoke_trampoline_descriptor_symbolized_sites_total;
  contract.invoke_trampoline_symbolized_sites =
      sema_parity_surface.block_abi_invoke_trampoline_invoke_symbolized_sites_total;
  contract.missing_invoke_trampoline_sites =
      sema_parity_surface.block_abi_invoke_trampoline_missing_invoke_sites_total;
  contract.non_normalized_layout_sites =
      sema_parity_surface.block_abi_invoke_trampoline_non_normalized_layout_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.block_abi_invoke_trampoline_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_abi_invoke_trampoline_semantics_summary.deterministic &&
      sema_parity_surface.deterministic_block_abi_invoke_trampoline_handoff;
  return contract;
}

Objc3BlockStorageEscapeLoweringContract BuildBlockStorageEscapeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3BlockStorageEscapeLoweringContract contract;
  contract.block_literal_sites = sema_parity_surface.block_storage_escape_sites_total;
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
      sema_parity_surface.block_storage_escape_requires_byref_cells_sites_total;
  contract.escape_analysis_enabled_sites =
      sema_parity_surface.block_storage_escape_escape_analysis_enabled_sites_total;
  contract.escape_to_heap_sites =
      sema_parity_surface.block_storage_escape_escape_to_heap_sites_total;
  contract.escape_profile_normalized_sites =
      sema_parity_surface.block_storage_escape_escape_profile_normalized_sites_total;
  contract.byref_layout_symbolized_sites =
      sema_parity_surface.block_storage_escape_byref_layout_symbolized_sites_total;
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
  contract.block_literal_sites = sema_parity_surface.block_copy_dispose_sites_total;
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
      sema_parity_surface.block_copy_dispose_dispose_helper_required_sites_total;
  contract.profile_normalized_sites =
      sema_parity_surface.block_copy_dispose_profile_normalized_sites_total;
  contract.copy_helper_symbolized_sites =
      sema_parity_surface.block_copy_dispose_copy_helper_symbolized_sites_total;
  contract.dispose_helper_symbolized_sites =
      sema_parity_surface.block_copy_dispose_dispose_helper_symbolized_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.block_copy_dispose_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_copy_dispose_semantics_summary.deterministic &&
      sema_parity_surface.deterministic_block_copy_dispose_handoff;
  return contract;
}

Objc3BlockDeterminismPerfBaselineLoweringContract BuildBlockDeterminismPerfBaselineLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3BlockDeterminismPerfBaselineLoweringContract contract;
  contract.block_literal_sites =
      sema_parity_surface.block_determinism_perf_baseline_sites_total;
  contract.baseline_weight_total =
      sema_parity_surface.block_determinism_perf_baseline_weight_total;
  contract.parameter_entries_total =
      sema_parity_surface.block_determinism_perf_baseline_parameter_entries_total;
  contract.capture_entries_total =
      sema_parity_surface.block_determinism_perf_baseline_capture_entries_total;
  contract.body_statement_entries_total =
      sema_parity_surface.block_determinism_perf_baseline_body_statement_entries_total;
  contract.deterministic_capture_sites =
      sema_parity_surface.block_determinism_perf_baseline_deterministic_capture_sites_total;
  contract.heavy_tier_sites =
      sema_parity_surface.block_determinism_perf_baseline_heavy_tier_sites_total;
  contract.normalized_profile_sites =
      sema_parity_surface.block_determinism_perf_baseline_normalized_profile_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.block_determinism_perf_baseline_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_determinism_perf_baseline_summary.deterministic &&
      sema_parity_surface.deterministic_block_determinism_perf_baseline_handoff;
  return contract;
}

Objc3LightweightGenericsConstraintLoweringContract BuildLightweightGenericsConstraintLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3LightweightGenericsConstraintLoweringContract contract;
  contract.generic_constraint_sites =
      sema_parity_surface.lightweight_generic_constraint_sites_total;
  contract.generic_suffix_sites =
      sema_parity_surface.lightweight_generic_constraint_generic_suffix_sites_total;
  contract.object_pointer_type_sites =
      sema_parity_surface.lightweight_generic_constraint_object_pointer_type_sites_total;
  contract.terminated_generic_suffix_sites =
      sema_parity_surface.lightweight_generic_constraint_terminated_generic_suffix_sites_total;
  contract.pointer_declarator_sites =
      sema_parity_surface.lightweight_generic_constraint_pointer_declarator_sites_total;
  contract.normalized_constraint_sites =
      sema_parity_surface.lightweight_generic_constraint_normalized_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.lightweight_generic_constraint_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.lightweight_generic_constraint_summary.deterministic &&
      sema_parity_surface.deterministic_lightweight_generic_constraint_handoff;
  return contract;
}

Objc3NullabilityFlowWarningPrecisionLoweringContract BuildNullabilityFlowWarningPrecisionLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3NullabilityFlowWarningPrecisionLoweringContract contract;
  contract.nullability_flow_sites =
      sema_parity_surface.nullability_flow_sites_total;
  contract.object_pointer_type_sites = std::max(
      sema_parity_surface.nullability_flow_object_pointer_type_sites_total,
      sema_parity_surface.nullability_flow_nullability_suffix_sites_total);
  contract.nullability_suffix_sites =
      sema_parity_surface.nullability_flow_nullability_suffix_sites_total;
  contract.nullable_suffix_sites =
      sema_parity_surface.nullability_flow_nullable_suffix_sites_total;
  contract.nonnull_suffix_sites =
      sema_parity_surface.nullability_flow_nonnull_suffix_sites_total;
  contract.normalized_sites =
      sema_parity_surface.nullability_flow_normalized_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.nullability_flow_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.nullability_flow_warning_precision_summary.deterministic &&
      sema_parity_surface.deterministic_nullability_flow_warning_precision_handoff;
  return contract;
}

Objc3ProtocolQualifiedObjectTypeLoweringContract BuildProtocolQualifiedObjectTypeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3ProtocolQualifiedObjectTypeLoweringContract contract;
  const std::size_t raw_protocol_sites =
      sema_parity_surface.protocol_qualified_object_type_sites_total;
  const std::size_t raw_protocol_composition_sites =
      sema_parity_surface.protocol_qualified_object_type_protocol_composition_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.protocol_qualified_object_type_object_pointer_type_sites_total;
  const std::size_t raw_terminated_sites =
      sema_parity_surface.protocol_qualified_object_type_terminated_protocol_composition_sites_total;
  const std::size_t raw_pointer_sites =
      sema_parity_surface.protocol_qualified_object_type_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.protocol_qualified_object_type_normalized_protocol_composition_sites_total;
  const std::size_t raw_contract_violation_sites =
      sema_parity_surface.protocol_qualified_object_type_contract_violation_sites_total;

  contract.protocol_qualified_object_type_sites = std::max(
      {raw_protocol_sites, raw_protocol_composition_sites, raw_pointer_sites, raw_normalized_sites,
       raw_contract_violation_sites});
  contract.protocol_composition_sites =
      std::min(raw_protocol_composition_sites, contract.protocol_qualified_object_type_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.protocol_composition_sites);
  contract.terminated_protocol_composition_sites =
      std::min(raw_terminated_sites, contract.protocol_composition_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_sites, contract.protocol_qualified_object_type_sites);
  contract.normalized_protocol_composition_sites =
      std::min(raw_normalized_sites, contract.protocol_qualified_object_type_sites);
  contract.contract_violation_sites =
      std::min(raw_contract_violation_sites, contract.protocol_qualified_object_type_sites);

  const bool strict_deterministic =
      sema_parity_surface.protocol_qualified_object_type_summary.deterministic &&
      sema_parity_surface.deterministic_protocol_qualified_object_type_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_protocol_composition_sites == contract.protocol_qualified_object_type_sites;
  contract.deterministic = strict_deterministic;
  return contract;
}

Objc3VarianceBridgeCastLoweringContract BuildVarianceBridgeCastLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3VarianceBridgeCastLoweringContract contract;
  const std::size_t raw_sites = sema_parity_surface.variance_bridge_cast_sites_total;
  const std::size_t raw_protocol_sites =
      sema_parity_surface.variance_bridge_cast_protocol_composition_sites_total;
  const std::size_t raw_ownership_sites =
      sema_parity_surface.variance_bridge_cast_ownership_qualifier_sites_total;
  const std::size_t raw_object_sites =
      sema_parity_surface.variance_bridge_cast_object_pointer_type_sites_total;
  const std::size_t raw_pointer_sites =
      sema_parity_surface.variance_bridge_cast_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.variance_bridge_cast_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.variance_bridge_cast_contract_violation_sites_total;

  contract.variance_bridge_cast_sites = std::max(
      {raw_sites, raw_protocol_sites, raw_ownership_sites, raw_pointer_sites, raw_normalized_sites,
       raw_violation_sites});
  contract.protocol_composition_sites =
      std::min(raw_protocol_sites, contract.variance_bridge_cast_sites);
  contract.ownership_qualifier_sites =
      std::min(raw_ownership_sites, contract.variance_bridge_cast_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_sites, contract.protocol_composition_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_sites, contract.variance_bridge_cast_sites);
  contract.normalized_sites = std::min(raw_normalized_sites, contract.variance_bridge_cast_sites);
  contract.contract_violation_sites = std::min(raw_violation_sites, contract.variance_bridge_cast_sites);

  contract.deterministic = sema_parity_surface.variance_bridge_cast_summary.deterministic &&
                           sema_parity_surface.deterministic_variance_bridge_cast_handoff &&
                           contract.contract_violation_sites == 0 &&
                           contract.normalized_sites == contract.variance_bridge_cast_sites;
  return contract;
}

Objc3GenericMetadataAbiLoweringContract BuildGenericMetadataAbiLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3GenericMetadataAbiLoweringContract contract;
  const std::size_t raw_sites = sema_parity_surface.generic_metadata_abi_sites_total;
  const std::size_t raw_generic_suffix_sites =
      sema_parity_surface.generic_metadata_abi_generic_suffix_sites_total;
  const std::size_t raw_protocol_sites =
      sema_parity_surface.generic_metadata_abi_protocol_composition_sites_total;
  const std::size_t raw_ownership_sites =
      sema_parity_surface.generic_metadata_abi_ownership_qualifier_sites_total;
  const std::size_t raw_object_sites =
      sema_parity_surface.generic_metadata_abi_object_pointer_type_sites_total;
  const std::size_t raw_pointer_sites =
      sema_parity_surface.generic_metadata_abi_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.generic_metadata_abi_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.generic_metadata_abi_contract_violation_sites_total;

  contract.generic_metadata_abi_sites = std::max(
      {raw_sites, raw_generic_suffix_sites, raw_protocol_sites, raw_ownership_sites, raw_pointer_sites,
       raw_normalized_sites, raw_violation_sites});
  contract.generic_suffix_sites =
      std::min(raw_generic_suffix_sites, contract.generic_metadata_abi_sites);
  contract.protocol_composition_sites =
      std::min(raw_protocol_sites, contract.generic_metadata_abi_sites);
  contract.ownership_qualifier_sites =
      std::min(raw_ownership_sites, contract.generic_metadata_abi_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_sites, contract.protocol_composition_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_sites, contract.generic_metadata_abi_sites);
  contract.normalized_sites = std::min(raw_normalized_sites, contract.generic_metadata_abi_sites);
  contract.contract_violation_sites = std::min(raw_violation_sites, contract.generic_metadata_abi_sites);

  contract.deterministic = sema_parity_surface.generic_metadata_abi_summary.deterministic &&
                           sema_parity_surface.deterministic_generic_metadata_abi_handoff &&
                           contract.contract_violation_sites == 0 &&
                           contract.normalized_sites == contract.generic_metadata_abi_sites;
  return contract;
}

Objc3ModuleImportGraphLoweringContract BuildModuleImportGraphLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3ModuleImportGraphLoweringContract contract;
  const std::size_t raw_sites = sema_parity_surface.module_import_graph_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface.module_import_graph_import_edge_candidate_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface.module_import_graph_namespace_segment_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.module_import_graph_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.module_import_graph_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.module_import_graph_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.module_import_graph_contract_violation_sites_total;

  contract.module_import_graph_sites =
      std::max({raw_sites, raw_import_edge_sites, raw_namespace_segment_sites, raw_pointer_declarator_sites,
                raw_normalized_sites, raw_violation_sites});
  contract.import_edge_candidate_sites =
      std::min(raw_import_edge_sites, contract.module_import_graph_sites);
  contract.namespace_segment_sites =
      std::min(raw_namespace_segment_sites, contract.module_import_graph_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.module_import_graph_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.module_import_graph_sites);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.module_import_graph_sites);
  contract.deterministic = sema_parity_surface.module_import_graph_summary.deterministic &&
                           sema_parity_surface.deterministic_module_import_graph_handoff &&
                           contract.contract_violation_sites == 0 &&
                           contract.normalized_sites == contract.module_import_graph_sites;
  return contract;
}

Objc3NamespaceCollisionShadowingLoweringContract
BuildNamespaceCollisionShadowingLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3NamespaceCollisionShadowingLoweringContract contract;
  const std::size_t raw_sites = sema_parity_surface.namespace_collision_shadowing_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface.namespace_collision_shadowing_namespace_segment_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface.namespace_collision_shadowing_import_edge_candidate_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.namespace_collision_shadowing_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.namespace_collision_shadowing_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.namespace_collision_shadowing_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.namespace_collision_shadowing_contract_violation_sites_total;

  contract.namespace_collision_shadowing_sites =
      std::max({raw_sites, raw_namespace_segment_sites, raw_import_edge_sites, raw_pointer_declarator_sites,
                raw_normalized_sites, raw_violation_sites});
  contract.namespace_segment_sites =
      std::min(raw_namespace_segment_sites, contract.namespace_collision_shadowing_sites);
  contract.import_edge_candidate_sites =
      std::min(raw_import_edge_sites, contract.namespace_collision_shadowing_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.namespace_collision_shadowing_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.namespace_collision_shadowing_sites);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.namespace_collision_shadowing_sites);
  contract.deterministic = sema_parity_surface.namespace_collision_shadowing_summary.deterministic &&
                           sema_parity_surface.deterministic_namespace_collision_shadowing_handoff &&
                           contract.contract_violation_sites == 0 &&
                           contract.normalized_sites == contract.namespace_collision_shadowing_sites;
  return contract;
}

Objc3PublicPrivateApiPartitionLoweringContract
BuildPublicPrivateApiPartitionLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3PublicPrivateApiPartitionLoweringContract contract;
  const std::size_t raw_sites = sema_parity_surface.public_private_api_partition_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface.public_private_api_partition_namespace_segment_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface.public_private_api_partition_import_edge_candidate_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.public_private_api_partition_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.public_private_api_partition_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.public_private_api_partition_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.public_private_api_partition_contract_violation_sites_total;

  contract.public_private_api_partition_sites =
      std::max({raw_sites, raw_namespace_segment_sites, raw_import_edge_sites, raw_pointer_declarator_sites,
                raw_normalized_sites, raw_violation_sites});
  contract.namespace_segment_sites =
      std::min(raw_namespace_segment_sites, contract.public_private_api_partition_sites);
  contract.import_edge_candidate_sites =
      std::min(raw_import_edge_sites, contract.public_private_api_partition_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.public_private_api_partition_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.public_private_api_partition_sites);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.public_private_api_partition_sites);
  contract.deterministic = sema_parity_surface.public_private_api_partition_summary.deterministic &&
                           sema_parity_surface.deterministic_public_private_api_partition_handoff &&
                           contract.contract_violation_sites == 0 &&
                           contract.normalized_sites == contract.public_private_api_partition_sites;
  return contract;
}

Objc3IncrementalModuleCacheInvalidationLoweringContract
BuildIncrementalModuleCacheInvalidationLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3IncrementalModuleCacheInvalidationLoweringContract contract;
  const std::size_t raw_sites =
      sema_parity_surface.incremental_module_cache_invalidation_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface.incremental_module_cache_invalidation_namespace_segment_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface.incremental_module_cache_invalidation_import_edge_candidate_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.incremental_module_cache_invalidation_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.incremental_module_cache_invalidation_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.incremental_module_cache_invalidation_normalized_sites_total;
  const std::size_t raw_cache_candidate_sites =
      sema_parity_surface.incremental_module_cache_invalidation_cache_invalidation_candidate_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.incremental_module_cache_invalidation_contract_violation_sites_total;

  contract.incremental_module_cache_invalidation_sites =
      std::max({raw_sites, raw_namespace_segment_sites, raw_import_edge_sites, raw_pointer_declarator_sites,
                raw_normalized_sites, raw_cache_candidate_sites, raw_violation_sites});
  contract.namespace_segment_sites =
      std::min(raw_namespace_segment_sites, contract.incremental_module_cache_invalidation_sites);
  contract.import_edge_candidate_sites =
      std::min(raw_import_edge_sites, contract.incremental_module_cache_invalidation_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.incremental_module_cache_invalidation_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.incremental_module_cache_invalidation_sites);
  const std::size_t normalized_budget =
      (contract.incremental_module_cache_invalidation_sites >= contract.normalized_sites)
          ? (contract.incremental_module_cache_invalidation_sites - contract.normalized_sites)
          : 0;
  contract.cache_invalidation_candidate_sites =
      std::min(raw_cache_candidate_sites, normalized_budget);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.incremental_module_cache_invalidation_sites);
  contract.deterministic =
      sema_parity_surface.incremental_module_cache_invalidation_summary
          .deterministic &&
      sema_parity_surface
          .deterministic_incremental_module_cache_invalidation_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_sites ==
          contract.incremental_module_cache_invalidation_sites;
  return contract;
}

Objc3CrossModuleConformanceLoweringContract
BuildCrossModuleConformanceLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3CrossModuleConformanceLoweringContract contract;
  const std::size_t raw_sites = sema_parity_surface.cross_module_conformance_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface.cross_module_conformance_namespace_segment_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface.cross_module_conformance_import_edge_candidate_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.cross_module_conformance_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.cross_module_conformance_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.cross_module_conformance_normalized_sites_total;
  const std::size_t raw_cache_candidate_sites =
      sema_parity_surface.cross_module_conformance_cache_invalidation_candidate_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.cross_module_conformance_contract_violation_sites_total;

  contract.cross_module_conformance_sites =
      std::max({raw_sites, raw_namespace_segment_sites, raw_import_edge_sites, raw_pointer_declarator_sites,
                raw_normalized_sites, raw_cache_candidate_sites, raw_violation_sites});
  contract.namespace_segment_sites =
      std::min(raw_namespace_segment_sites, contract.cross_module_conformance_sites);
  contract.import_edge_candidate_sites =
      std::min(raw_import_edge_sites, contract.cross_module_conformance_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.cross_module_conformance_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.cross_module_conformance_sites);
  const std::size_t normalized_budget =
      (contract.cross_module_conformance_sites >= contract.normalized_sites)
          ? (contract.cross_module_conformance_sites - contract.normalized_sites)
          : 0;
  contract.cache_invalidation_candidate_sites =
      std::min(raw_cache_candidate_sites, normalized_budget);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.cross_module_conformance_sites);
  contract.deterministic = sema_parity_surface.cross_module_conformance_summary.deterministic &&
                           sema_parity_surface.deterministic_cross_module_conformance_handoff &&
                           contract.contract_violation_sites == 0 &&
                           contract.normalized_sites == contract.cross_module_conformance_sites;
  return contract;
}

Objc3ThrowsPropagationLoweringContract
BuildThrowsPropagationLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3ThrowsPropagationLoweringContract contract;
  const std::size_t raw_sites = sema_parity_surface.throws_propagation_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface.throws_propagation_namespace_segment_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface.throws_propagation_import_edge_candidate_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.throws_propagation_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.throws_propagation_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.throws_propagation_normalized_sites_total;
  const std::size_t raw_cache_candidate_sites =
      sema_parity_surface.throws_propagation_cache_invalidation_candidate_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.throws_propagation_contract_violation_sites_total;

  contract.throws_propagation_sites =
      std::max({raw_sites, raw_namespace_segment_sites, raw_import_edge_sites, raw_pointer_declarator_sites,
                raw_normalized_sites, raw_cache_candidate_sites, raw_violation_sites});
  contract.namespace_segment_sites =
      std::min(raw_namespace_segment_sites, contract.throws_propagation_sites);
  contract.import_edge_candidate_sites =
      std::min(raw_import_edge_sites, contract.throws_propagation_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.throws_propagation_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.throws_propagation_sites);
  const std::size_t normalized_budget =
      (contract.throws_propagation_sites >= contract.normalized_sites)
          ? (contract.throws_propagation_sites - contract.normalized_sites)
          : 0;
  contract.cache_invalidation_candidate_sites =
      std::min(raw_cache_candidate_sites, normalized_budget);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.throws_propagation_sites);
  contract.deterministic = sema_parity_surface.throws_propagation_summary.deterministic &&
                           sema_parity_surface.deterministic_throws_propagation_handoff &&
                           contract.contract_violation_sites == 0 &&
                           contract.normalized_sites == contract.throws_propagation_sites;
  return contract;
}

}  // namespace

Objc3FrontendArtifactBundle BuildObjc3FrontendArtifacts(const std::filesystem::path &input_path,
                                                        const Objc3FrontendPipelineResult &pipeline_result,
                                                        const Objc3FrontendOptions &options) {
  Objc3FrontendArtifactBundle bundle;
  const Objc3Program &program = Objc3ParsedProgramAst(pipeline_result.program);
  bundle.stage_diagnostics = pipeline_result.stage_diagnostics;
  bundle.diagnostics = FlattenStageDiagnostics(bundle.stage_diagnostics);
  if (!bundle.diagnostics.empty()) {
    return bundle;
  }

  std::vector<const FunctionDecl *> manifest_functions;
  manifest_functions.reserve(program.functions.size());
  std::unordered_set<std::string> manifest_function_names;
  for (const auto &fn : program.functions) {
    if (manifest_function_names.insert(fn.name).second) {
      manifest_functions.push_back(&fn);
    }
  }

  std::size_t scalar_return_i32 = 0;
  std::size_t scalar_return_bool = 0;
  std::size_t scalar_return_void = 0;
  std::size_t scalar_param_i32 = 0;
  std::size_t scalar_param_bool = 0;
  std::size_t vector_signature_functions = 0;
  std::size_t vector_return_signatures = 0;
  std::size_t vector_param_signatures = 0;
  std::size_t vector_i32_signatures = 0;
  std::size_t vector_bool_signatures = 0;
  std::size_t vector_lane2_signatures = 0;
  std::size_t vector_lane4_signatures = 0;
  std::size_t vector_lane8_signatures = 0;
  std::size_t vector_lane16_signatures = 0;
  for (const auto &entry : pipeline_result.integration_surface.functions) {
    const FunctionInfo &signature = entry.second;
    if (signature.return_type == ValueType::Bool) {
      ++scalar_return_bool;
    } else if (signature.return_type == ValueType::Void) {
      ++scalar_return_void;
    } else {
      ++scalar_return_i32;
    }
    for (const ValueType param_type : signature.param_types) {
      if (param_type == ValueType::Bool) {
        ++scalar_param_bool;
      } else {
        ++scalar_param_i32;
      }
    }
  }
  for (const FunctionDecl *fn : manifest_functions) {
    bool has_vector_signature = false;
    if (fn->return_vector_spelling) {
      has_vector_signature = true;
      ++vector_return_signatures;
      if (fn->return_vector_base_spelling == kObjc3SimdVectorBaseBool) {
        ++vector_bool_signatures;
      } else {
        ++vector_i32_signatures;
      }
      if (fn->return_vector_lane_count == 2u) {
        ++vector_lane2_signatures;
      } else if (fn->return_vector_lane_count == 4u) {
        ++vector_lane4_signatures;
      } else if (fn->return_vector_lane_count == 8u) {
        ++vector_lane8_signatures;
      } else if (fn->return_vector_lane_count == 16u) {
        ++vector_lane16_signatures;
      }
    }
    for (const FuncParam &param : fn->params) {
      if (!param.vector_spelling) {
        continue;
      }
      has_vector_signature = true;
      ++vector_param_signatures;
      if (param.vector_base_spelling == kObjc3SimdVectorBaseBool) {
        ++vector_bool_signatures;
      } else {
        ++vector_i32_signatures;
      }
      if (param.vector_lane_count == 2u) {
        ++vector_lane2_signatures;
      } else if (param.vector_lane_count == 4u) {
        ++vector_lane4_signatures;
      } else if (param.vector_lane_count == 8u) {
        ++vector_lane8_signatures;
      } else if (param.vector_lane_count == 16u) {
        ++vector_lane16_signatures;
      }
    }
    if (has_vector_signature) {
      ++vector_signature_functions;
    }
  }
  const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff = pipeline_result.sema_type_metadata_handoff;
  const Objc3InterfaceImplementationSummary &interface_implementation_summary =
      type_metadata_handoff.interface_implementation_summary;
  const Objc3FrontendProtocolCategorySummary &protocol_category_summary = pipeline_result.protocol_category_summary;
  const Objc3FrontendClassProtocolCategoryLinkingSummary &class_protocol_category_linking_summary =
      pipeline_result.class_protocol_category_linking_summary;
  const Objc3FrontendSelectorNormalizationSummary &selector_normalization_summary =
      pipeline_result.selector_normalization_summary;
  const Objc3FrontendPropertyAttributeSummary &property_attribute_summary =
      pipeline_result.property_attribute_summary;
  const Objc3FrontendObjectPointerNullabilityGenericsSummary &object_pointer_nullability_generics_summary =
      pipeline_result.object_pointer_nullability_generics_summary;
  const Objc3FrontendSymbolGraphScopeResolutionSummary &symbol_graph_scope_resolution_summary =
      pipeline_result.symbol_graph_scope_resolution_summary;
  const Objc3PropertySynthesisIvarBindingContract property_synthesis_ivar_binding_contract =
      BuildPropertySynthesisIvarBindingContract(property_attribute_summary);
  if (!IsValidObjc3PropertySynthesisIvarBindingContract(property_synthesis_ivar_binding_contract)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(
        1,
        1,
        "O3L300",
        "LLVM IR emission failed: invalid property synthesis/ivar binding lowering contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string property_synthesis_ivar_binding_replay_key =
      Objc3PropertySynthesisIvarBindingReplayKey(property_synthesis_ivar_binding_contract);
  const Objc3IdClassSelObjectPointerTypecheckContract id_class_sel_object_pointer_typecheck_contract =
      BuildIdClassSelObjectPointerTypecheckContract(program);
  if (!IsValidObjc3IdClassSelObjectPointerTypecheckContract(id_class_sel_object_pointer_typecheck_contract)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(
        1,
        1,
        "O3L300",
        "LLVM IR emission failed: invalid id/Class/SEL/object-pointer typecheck lowering contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string id_class_sel_object_pointer_typecheck_replay_key =
      Objc3IdClassSelObjectPointerTypecheckReplayKey(id_class_sel_object_pointer_typecheck_contract);
  const Objc3MessageSendSelectorLoweringContract message_send_selector_lowering_contract =
      BuildMessageSendSelectorLoweringContract(program);
  if (!IsValidObjc3MessageSendSelectorLoweringContract(message_send_selector_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(
        1,
        1,
        "O3L300",
        "LLVM IR emission failed: invalid message-send selector lowering contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string message_send_selector_lowering_replay_key =
      Objc3MessageSendSelectorLoweringReplayKey(message_send_selector_lowering_contract);
  const Objc3DispatchAbiMarshallingContract dispatch_abi_marshalling_contract =
      BuildDispatchAbiMarshallingContract(program, options.lowering.max_message_send_args);
  if (!IsValidObjc3DispatchAbiMarshallingContract(dispatch_abi_marshalling_contract)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(
        1,
        1,
        "O3L300",
        "LLVM IR emission failed: invalid dispatch ABI marshalling contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string dispatch_abi_marshalling_replay_key =
      Objc3DispatchAbiMarshallingReplayKey(dispatch_abi_marshalling_contract);
  const Objc3NilReceiverSemanticsFoldabilityContract nil_receiver_semantics_foldability_contract =
      BuildNilReceiverSemanticsFoldabilityContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3NilReceiverSemanticsFoldabilityContract(nil_receiver_semantics_foldability_contract)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(
        1,
        1,
        "O3L300",
        "LLVM IR emission failed: invalid nil-receiver semantics/foldability contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string nil_receiver_semantics_foldability_replay_key =
      Objc3NilReceiverSemanticsFoldabilityReplayKey(nil_receiver_semantics_foldability_contract);
  const Objc3SuperDispatchMethodFamilyContract super_dispatch_method_family_contract =
      BuildSuperDispatchMethodFamilyContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3SuperDispatchMethodFamilyContract(super_dispatch_method_family_contract)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(
        1,
        1,
        "O3L300",
        "LLVM IR emission failed: invalid super-dispatch/method-family contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string super_dispatch_method_family_replay_key =
      Objc3SuperDispatchMethodFamilyReplayKey(super_dispatch_method_family_contract);
  const Objc3RuntimeShimHostLinkContract runtime_shim_host_link_contract =
      BuildRuntimeShimHostLinkContract(
          dispatch_abi_marshalling_contract,
          nil_receiver_semantics_foldability_contract,
          options);
  if (!IsValidObjc3RuntimeShimHostLinkContract(runtime_shim_host_link_contract)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(
        1,
        1,
        "O3L300",
        "LLVM IR emission failed: invalid runtime shim/host-link contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string runtime_shim_host_link_replay_key =
      Objc3RuntimeShimHostLinkReplayKey(runtime_shim_host_link_contract);
  const Objc3OwnershipQualifierLoweringContract ownership_qualifier_lowering_contract =
      BuildOwnershipQualifierLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3OwnershipQualifierLoweringContract(ownership_qualifier_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(
        1,
        1,
        "O3L300",
        "LLVM IR emission failed: invalid ownership-qualifier lowering contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string ownership_qualifier_lowering_replay_key =
      Objc3OwnershipQualifierLoweringReplayKey(ownership_qualifier_lowering_contract);
  const Objc3RetainReleaseOperationLoweringContract retain_release_operation_lowering_contract =
      BuildRetainReleaseOperationLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3RetainReleaseOperationLoweringContract(retain_release_operation_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(
        1,
        1,
        "O3L300",
        "LLVM IR emission failed: invalid retain-release operation lowering contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string retain_release_operation_lowering_replay_key =
      Objc3RetainReleaseOperationLoweringReplayKey(retain_release_operation_lowering_contract);
  const Objc3AutoreleasePoolScopeLoweringContract autoreleasepool_scope_lowering_contract =
      BuildAutoreleasePoolScopeLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3AutoreleasePoolScopeLoweringContract(autoreleasepool_scope_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(
        1,
        1,
        "O3L300",
        "LLVM IR emission failed: invalid autoreleasepool scope lowering contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string autoreleasepool_scope_lowering_replay_key =
      Objc3AutoreleasePoolScopeLoweringReplayKey(autoreleasepool_scope_lowering_contract);
  const Objc3WeakUnownedSemanticsLoweringContract weak_unowned_semantics_lowering_contract =
      BuildWeakUnownedSemanticsLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3WeakUnownedSemanticsLoweringContract(weak_unowned_semantics_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(
        1,
        1,
        "O3L300",
        "LLVM IR emission failed: invalid weak-unowned semantics lowering contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string weak_unowned_semantics_lowering_replay_key =
      Objc3WeakUnownedSemanticsLoweringReplayKey(weak_unowned_semantics_lowering_contract);
  const Objc3ArcDiagnosticsFixitLoweringContract arc_diagnostics_fixit_lowering_contract =
      BuildArcDiagnosticsFixitLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3ArcDiagnosticsFixitLoweringContract(arc_diagnostics_fixit_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(
        1,
        1,
        "O3L300",
        "LLVM IR emission failed: invalid ARC diagnostics/fix-it lowering contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string arc_diagnostics_fixit_lowering_replay_key =
      Objc3ArcDiagnosticsFixitLoweringReplayKey(arc_diagnostics_fixit_lowering_contract);
  const Objc3BlockLiteralCaptureLoweringContract block_literal_capture_lowering_contract =
      BuildBlockLiteralCaptureLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3BlockLiteralCaptureLoweringContract(block_literal_capture_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(
        1,
        1,
        "O3L300",
        "LLVM IR emission failed: invalid block literal capture lowering contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string block_literal_capture_lowering_replay_key =
      Objc3BlockLiteralCaptureLoweringReplayKey(block_literal_capture_lowering_contract);
  const Objc3BlockAbiInvokeTrampolineLoweringContract block_abi_invoke_trampoline_lowering_contract =
      BuildBlockAbiInvokeTrampolineLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3BlockAbiInvokeTrampolineLoweringContract(
          block_abi_invoke_trampoline_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(
        1,
        1,
        "O3L300",
        "LLVM IR emission failed: invalid block ABI invoke-trampoline lowering contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string block_abi_invoke_trampoline_lowering_replay_key =
      Objc3BlockAbiInvokeTrampolineLoweringReplayKey(
          block_abi_invoke_trampoline_lowering_contract);
  const Objc3BlockStorageEscapeLoweringContract block_storage_escape_lowering_contract =
      BuildBlockStorageEscapeLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3BlockStorageEscapeLoweringContract(
          block_storage_escape_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(
        1,
        1,
        "O3L300",
        "LLVM IR emission failed: invalid block storage escape lowering contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string block_storage_escape_lowering_replay_key =
      Objc3BlockStorageEscapeLoweringReplayKey(block_storage_escape_lowering_contract);
  const Objc3BlockCopyDisposeLoweringContract block_copy_dispose_lowering_contract =
      BuildBlockCopyDisposeLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3BlockCopyDisposeLoweringContract(
          block_copy_dispose_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(
        1,
        1,
        "O3L300",
        "LLVM IR emission failed: invalid block copy-dispose lowering contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string block_copy_dispose_lowering_replay_key =
      Objc3BlockCopyDisposeLoweringReplayKey(block_copy_dispose_lowering_contract);
  const Objc3BlockDeterminismPerfBaselineLoweringContract block_determinism_perf_baseline_lowering_contract =
      BuildBlockDeterminismPerfBaselineLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3BlockDeterminismPerfBaselineLoweringContract(
          block_determinism_perf_baseline_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(
        1,
        1,
        "O3L300",
        "LLVM IR emission failed: invalid block determinism/perf baseline lowering contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string block_determinism_perf_baseline_lowering_replay_key =
      Objc3BlockDeterminismPerfBaselineLoweringReplayKey(
          block_determinism_perf_baseline_lowering_contract);
  const Objc3LightweightGenericsConstraintLoweringContract lightweight_generic_constraint_lowering_contract =
      BuildLightweightGenericsConstraintLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3LightweightGenericsConstraintLoweringContract(
          lightweight_generic_constraint_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(
        1,
        1,
        "O3L300",
        "LLVM IR emission failed: invalid lightweight generics constraint lowering contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string lightweight_generic_constraint_lowering_replay_key =
      Objc3LightweightGenericsConstraintLoweringReplayKey(
          lightweight_generic_constraint_lowering_contract);
  const Objc3NullabilityFlowWarningPrecisionLoweringContract nullability_flow_warning_precision_lowering_contract =
      BuildNullabilityFlowWarningPrecisionLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3NullabilityFlowWarningPrecisionLoweringContract(
          nullability_flow_warning_precision_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(
        1,
        1,
        "O3L300",
        "LLVM IR emission failed: invalid nullability-flow warning-precision lowering contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string nullability_flow_warning_precision_lowering_replay_key =
      Objc3NullabilityFlowWarningPrecisionLoweringReplayKey(
          nullability_flow_warning_precision_lowering_contract);
  const Objc3ProtocolQualifiedObjectTypeLoweringContract protocol_qualified_object_type_lowering_contract =
      BuildProtocolQualifiedObjectTypeLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3ProtocolQualifiedObjectTypeLoweringContract(
          protocol_qualified_object_type_lowering_contract)) {
    const std::string protocol_contract_replay_key =
        Objc3ProtocolQualifiedObjectTypeLoweringReplayKey(
            protocol_qualified_object_type_lowering_contract);
    bundle.post_pipeline_diagnostics = {MakeDiag(
        1,
        1,
        "O3L300",
        "LLVM IR emission failed: invalid protocol-qualified object type lowering contract (" +
            protocol_contract_replay_key + ")")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string protocol_qualified_object_type_lowering_replay_key =
      Objc3ProtocolQualifiedObjectTypeLoweringReplayKey(
          protocol_qualified_object_type_lowering_contract);
  const Objc3VarianceBridgeCastLoweringContract variance_bridge_cast_lowering_contract =
      BuildVarianceBridgeCastLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3VarianceBridgeCastLoweringContract(
          variance_bridge_cast_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(
        1,
        1,
        "O3L300",
        "LLVM IR emission failed: invalid variance/bridged-cast lowering contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string variance_bridge_cast_lowering_replay_key =
      Objc3VarianceBridgeCastLoweringReplayKey(
          variance_bridge_cast_lowering_contract);
  const Objc3GenericMetadataAbiLoweringContract generic_metadata_abi_lowering_contract =
      BuildGenericMetadataAbiLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3GenericMetadataAbiLoweringContract(
          generic_metadata_abi_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(
        1,
        1,
        "O3L300",
        "LLVM IR emission failed: invalid generic metadata ABI lowering contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string generic_metadata_abi_lowering_replay_key =
      Objc3GenericMetadataAbiLoweringReplayKey(
          generic_metadata_abi_lowering_contract);
  const Objc3ModuleImportGraphLoweringContract module_import_graph_lowering_contract =
      BuildModuleImportGraphLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3ModuleImportGraphLoweringContract(
          module_import_graph_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(
        1,
        1,
        "O3L300",
        "LLVM IR emission failed: invalid module import graph lowering contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string module_import_graph_lowering_replay_key =
      Objc3ModuleImportGraphLoweringReplayKey(
          module_import_graph_lowering_contract);
  const Objc3NamespaceCollisionShadowingLoweringContract
      namespace_collision_shadowing_lowering_contract =
          BuildNamespaceCollisionShadowingLoweringContract(
              pipeline_result.sema_parity_surface);
  if (!IsValidObjc3NamespaceCollisionShadowingLoweringContract(
          namespace_collision_shadowing_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {
        MakeDiag(1,
                 1,
                 "O3L300",
                 "LLVM IR emission failed: invalid namespace collision "
                 "shadowing lowering contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string namespace_collision_shadowing_lowering_replay_key =
      Objc3NamespaceCollisionShadowingLoweringReplayKey(
          namespace_collision_shadowing_lowering_contract);
  const Objc3PublicPrivateApiPartitionLoweringContract
      public_private_api_partition_lowering_contract =
          BuildPublicPrivateApiPartitionLoweringContract(
              pipeline_result.sema_parity_surface);
  if (!IsValidObjc3PublicPrivateApiPartitionLoweringContract(
          public_private_api_partition_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {
        MakeDiag(1,
                 1,
                 "O3L300",
                 "LLVM IR emission failed: invalid public-private API "
                 "partition lowering contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string public_private_api_partition_lowering_replay_key =
      Objc3PublicPrivateApiPartitionLoweringReplayKey(
          public_private_api_partition_lowering_contract);
  const Objc3IncrementalModuleCacheInvalidationLoweringContract
      incremental_module_cache_invalidation_lowering_contract =
          BuildIncrementalModuleCacheInvalidationLoweringContract(
              pipeline_result.sema_parity_surface);
  if (!IsValidObjc3IncrementalModuleCacheInvalidationLoweringContract(
          incremental_module_cache_invalidation_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {
        MakeDiag(
            1,
            1,
            "O3L300",
            "LLVM IR emission failed: invalid incremental module cache "
            "invalidation lowering contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string incremental_module_cache_invalidation_lowering_replay_key =
      Objc3IncrementalModuleCacheInvalidationLoweringReplayKey(
          incremental_module_cache_invalidation_lowering_contract);
  const Objc3CrossModuleConformanceLoweringContract
      cross_module_conformance_lowering_contract =
          BuildCrossModuleConformanceLoweringContract(
              pipeline_result.sema_parity_surface);
  if (!IsValidObjc3CrossModuleConformanceLoweringContract(
          cross_module_conformance_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {
        MakeDiag(1,
                 1,
                 "O3L300",
                 "LLVM IR emission failed: invalid cross-module conformance "
                 "lowering contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string cross_module_conformance_lowering_replay_key =
      Objc3CrossModuleConformanceLoweringReplayKey(
          cross_module_conformance_lowering_contract);
  const Objc3ThrowsPropagationLoweringContract
      throws_propagation_lowering_contract =
          BuildThrowsPropagationLoweringContract(
              pipeline_result.sema_parity_surface);
  if (!IsValidObjc3ThrowsPropagationLoweringContract(
          throws_propagation_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {
        MakeDiag(1,
                 1,
                 "O3L300",
                 "LLVM IR emission failed: invalid throws propagation "
                 "lowering contract")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }
  const std::string throws_propagation_lowering_replay_key =
      Objc3ThrowsPropagationLoweringReplayKey(
          throws_propagation_lowering_contract);
  std::size_t interface_class_method_symbols = 0;
  std::size_t interface_instance_method_symbols = 0;
  for (const auto &interface_metadata : type_metadata_handoff.interfaces_lexicographic) {
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      if (method_metadata.is_class_method) {
        ++interface_class_method_symbols;
      } else {
        ++interface_instance_method_symbols;
      }
    }
  }
  std::size_t implementation_class_method_symbols = 0;
  std::size_t implementation_instance_method_symbols = 0;
  std::size_t implementation_methods_with_body = 0;
  for (const auto &implementation_metadata : type_metadata_handoff.implementations_lexicographic) {
    for (const auto &method_metadata : implementation_metadata.methods_lexicographic) {
      if (method_metadata.is_class_method) {
        ++implementation_class_method_symbols;
      } else {
        ++implementation_instance_method_symbols;
      }
      if (method_metadata.has_definition) {
        ++implementation_methods_with_body;
      }
    }
  }

  std::vector<int> resolved_global_values;
  if (!ResolveGlobalInitializerValues(program.globals, resolved_global_values) ||
      resolved_global_values.size() != program.globals.size()) {
    bundle.post_pipeline_diagnostics = {
        MakeDiag(1, 1, "O3L300", "LLVM IR emission failed: global initializer failed const evaluation")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }

  std::ostringstream manifest;
  manifest << "{\n";
  manifest << "  \"source\": \"" << input_path.generic_string() << "\",\n";
  manifest << "  \"module\": \"" << program.module_name << "\",\n";
  manifest << "  \"frontend\": {\n";
  manifest << "    \"language_version\":" << static_cast<unsigned>(options.language_version) << ",\n";
  manifest << "    \"compatibility_mode\":\"" << CompatibilityModeName(options.compatibility_mode) << "\",\n";
  manifest << "    \"migration_assist\":" << (options.migration_assist ? "true" : "false") << ",\n";
  manifest << "    \"migration_hints\":{\"legacy_yes\":" << pipeline_result.migration_hints.legacy_yes_count
           << ",\"legacy_no\":" << pipeline_result.migration_hints.legacy_no_count << ",\"legacy_null\":"
           << pipeline_result.migration_hints.legacy_null_count
           << ",\"legacy_total\":" << pipeline_result.migration_hints.legacy_total() << "},\n";
  manifest << "    \"language_version_pragma_contract\":{\"seen\":"
           << (pipeline_result.language_version_pragma_contract.seen ? "true" : "false")
           << ",\"directive_count\":" << pipeline_result.language_version_pragma_contract.directive_count
           << ",\"duplicate\":" << (pipeline_result.language_version_pragma_contract.duplicate ? "true" : "false")
           << ",\"non_leading\":"
           << (pipeline_result.language_version_pragma_contract.non_leading ? "true" : "false")
           << ",\"first_line\":" << pipeline_result.language_version_pragma_contract.first_line
           << ",\"first_column\":" << pipeline_result.language_version_pragma_contract.first_column
           << ",\"last_line\":" << pipeline_result.language_version_pragma_contract.last_line
           << ",\"last_column\":" << pipeline_result.language_version_pragma_contract.last_column << "},\n";
  manifest << "    \"max_message_send_args\":" << options.lowering.max_message_send_args << ",\n";
  manifest << "    \"pipeline\": {\n";
  manifest << "      \"semantic_skipped\": " << (pipeline_result.integration_surface.built ? "false" : "true")
           << ",\n";
  manifest << "      \"stages\": {\n";
  manifest << "        \"lexer\": {\"diagnostics\":" << bundle.stage_diagnostics.lexer.size() << "},\n";
  manifest << "        \"parser\": {\"diagnostics\":" << bundle.stage_diagnostics.parser.size() << "},\n";
  manifest << "        \"semantic\": {\"diagnostics\":" << bundle.stage_diagnostics.semantic.size()
           << "}\n";
  manifest << "      },\n";
  manifest << "      \"sema_pass_manager\": {\"diagnostics_after_build\":"
           << pipeline_result.sema_diagnostics_after_pass[0] << ",\"diagnostics_after_validate_bodies\":"
           << pipeline_result.sema_diagnostics_after_pass[1] << ",\"diagnostics_after_validate_pure_contract\":"
           << pipeline_result.sema_diagnostics_after_pass[2] << ",\"diagnostics_emitted_by_build\":"
           << pipeline_result.sema_parity_surface.diagnostics_emitted_by_pass[0]
           << ",\"diagnostics_emitted_by_validate_bodies\":"
           << pipeline_result.sema_parity_surface.diagnostics_emitted_by_pass[1]
           << ",\"diagnostics_emitted_by_validate_pure_contract\":"
           << pipeline_result.sema_parity_surface.diagnostics_emitted_by_pass[2] << ",\"diagnostics_monotonic\":"
           << (pipeline_result.sema_parity_surface.diagnostics_after_pass_monotonic ? "true" : "false")
           << ",\"diagnostics_total\":"
           << pipeline_result.sema_parity_surface.diagnostics_total
           << ",\"deterministic_semantic_diagnostics\":"
           << (pipeline_result.sema_parity_surface.deterministic_semantic_diagnostics ? "true" : "false")
           << ",\"deterministic_type_metadata_handoff\":"
           << (pipeline_result.sema_parity_surface.deterministic_type_metadata_handoff ? "true" : "false")
           << ",\"deterministic_atomic_memory_order_mapping\":"
           << (pipeline_result.sema_parity_surface.deterministic_atomic_memory_order_mapping ? "true" : "false")
           << ",\"atomic_memory_order_mapping_total\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.total()
           << ",\"atomic_relaxed_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.relaxed
           << ",\"atomic_acquire_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.acquire
           << ",\"atomic_release_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.release
           << ",\"atomic_acq_rel_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.acq_rel
           << ",\"atomic_seq_cst_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.seq_cst
           << ",\"atomic_unmapped_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.unsupported
           << ",\"deterministic_vector_type_lowering\":"
           << (pipeline_result.sema_parity_surface.deterministic_vector_type_lowering ? "true" : "false")
           << ",\"vector_type_lowering_total\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.total()
           << ",\"vector_return_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.return_annotations
           << ",\"vector_param_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.param_annotations
           << ",\"vector_i32_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.i32_annotations
           << ",\"vector_bool_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.bool_annotations
           << ",\"vector_lane2_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.lane2_annotations
           << ",\"vector_lane4_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.lane4_annotations
           << ",\"vector_lane8_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.lane8_annotations
           << ",\"vector_lane16_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.lane16_annotations
           << ",\"vector_unsupported_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.unsupported_annotations
           << ",\"ready\":"
           << (pipeline_result.sema_parity_surface.ready ? "true" : "false")
           << ",\"parity_ready\":"
           << (IsReadyObjc3SemaParityContractSurface(pipeline_result.sema_parity_surface) ? "true" : "false")
           << ",\"globals_total\":"
           << pipeline_result.sema_parity_surface.globals_total
           << ",\"functions_total\":"
           << pipeline_result.sema_parity_surface.functions_total
           << ",\"type_metadata_global_entries\":"
           << pipeline_result.sema_parity_surface.type_metadata_global_entries
           << ",\"type_metadata_function_entries\":"
           << pipeline_result.sema_parity_surface.type_metadata_function_entries
           << ",\"deterministic_interface_implementation_handoff\":"
           << (pipeline_result.sema_parity_surface.deterministic_interface_implementation_handoff ? "true" : "false")
           << ",\"interfaces_total\":"
           << pipeline_result.sema_parity_surface.interfaces_total
           << ",\"implementations_total\":"
           << pipeline_result.sema_parity_surface.implementations_total
           << ",\"type_metadata_interface_entries\":"
           << pipeline_result.sema_parity_surface.type_metadata_interface_entries
           << ",\"type_metadata_implementation_entries\":"
           << pipeline_result.sema_parity_surface.type_metadata_implementation_entries
           << ",\"declared_interfaces\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.declared_interfaces
           << ",\"declared_implementations\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.declared_implementations
           << ",\"resolved_interfaces\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.resolved_interfaces
           << ",\"resolved_implementations\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.resolved_implementations
           << ",\"interface_method_symbols_total\":"
           << pipeline_result.sema_parity_surface.interface_method_symbols_total
           << ",\"implementation_method_symbols_total\":"
           << pipeline_result.sema_parity_surface.implementation_method_symbols_total
           << ",\"linked_implementation_symbols_total\":"
           << pipeline_result.sema_parity_surface.linked_implementation_symbols_total
           << ",\"deterministic_interface_implementation_summary\":"
           << (pipeline_result.sema_parity_surface.interface_implementation_summary.deterministic ? "true" : "false")
           << ",\"deterministic_protocol_category_handoff\":"
           << (protocol_category_summary.deterministic_protocol_category_handoff ? "true" : "false")
           << ",\"type_metadata_protocol_entries\":"
           << protocol_category_summary.resolved_protocol_symbols
           << ",\"type_metadata_category_entries\":"
           << protocol_category_summary.resolved_category_symbols
           << ",\"deterministic_class_protocol_category_linking_handoff\":"
           << (class_protocol_category_linking_summary.deterministic_class_protocol_category_linking_handoff
                   ? "true"
                   : "false")
           << ",\"class_protocol_category_declared_class_interfaces\":"
           << class_protocol_category_linking_summary.declared_class_interfaces
           << ",\"class_protocol_category_declared_class_implementations\":"
           << class_protocol_category_linking_summary.declared_class_implementations
           << ",\"class_protocol_category_resolved_class_interfaces\":"
           << class_protocol_category_linking_summary.resolved_class_interfaces
           << ",\"class_protocol_category_resolved_class_implementations\":"
           << class_protocol_category_linking_summary.resolved_class_implementations
           << ",\"class_protocol_category_linked_class_method_symbols\":"
           << class_protocol_category_linking_summary.linked_class_method_symbols
           << ",\"class_protocol_category_linked_category_method_symbols\":"
           << class_protocol_category_linking_summary.linked_category_method_symbols
           << ",\"class_protocol_category_protocol_composition_sites\":"
           << class_protocol_category_linking_summary.protocol_composition_sites
           << ",\"class_protocol_category_protocol_composition_symbols\":"
           << class_protocol_category_linking_summary.protocol_composition_symbols
           << ",\"class_protocol_category_category_composition_sites\":"
           << class_protocol_category_linking_summary.category_composition_sites
           << ",\"class_protocol_category_category_composition_symbols\":"
           << class_protocol_category_linking_summary.category_composition_symbols
           << ",\"class_protocol_category_invalid_protocol_composition_sites\":"
           << class_protocol_category_linking_summary.invalid_protocol_composition_sites
           << ",\"deterministic_selector_normalization_handoff\":"
           << (selector_normalization_summary.deterministic_selector_normalization_handoff ? "true" : "false")
           << ",\"selector_method_declaration_entries\":"
           << selector_normalization_summary.method_declaration_entries
           << ",\"selector_normalized_method_declarations\":"
           << selector_normalization_summary.normalized_method_declarations
           << ",\"selector_piece_entries\":"
           << selector_normalization_summary.selector_piece_entries
           << ",\"selector_piece_parameter_links\":"
           << selector_normalization_summary.selector_piece_parameter_links
           << ",\"deterministic_property_attribute_handoff\":"
           << (property_attribute_summary.deterministic_property_attribute_handoff ? "true" : "false")
           << ",\"property_declaration_entries\":"
           << property_attribute_summary.property_declaration_entries
           << ",\"property_attribute_entries\":"
           << property_attribute_summary.property_attribute_entries
           << ",\"property_attribute_value_entries\":"
           << property_attribute_summary.property_attribute_value_entries
           << ",\"property_accessor_modifier_entries\":"
           << property_attribute_summary.property_accessor_modifier_entries
           << ",\"property_getter_selector_entries\":"
           << property_attribute_summary.property_getter_selector_entries
           << ",\"property_setter_selector_entries\":"
           << property_attribute_summary.property_setter_selector_entries
           << ",\"deterministic_property_synthesis_ivar_binding_handoff\":"
           << (property_synthesis_ivar_binding_contract.deterministic ? "true" : "false")
           << ",\"property_synthesis_sites\":"
           << property_synthesis_ivar_binding_contract.property_synthesis_sites
           << ",\"property_synthesis_explicit_ivar_bindings\":"
           << property_synthesis_ivar_binding_contract.property_synthesis_explicit_ivar_bindings
           << ",\"property_synthesis_default_ivar_bindings\":"
           << property_synthesis_ivar_binding_contract.property_synthesis_default_ivar_bindings
           << ",\"ivar_binding_sites\":"
           << property_synthesis_ivar_binding_contract.ivar_binding_sites
           << ",\"ivar_binding_resolved\":"
           << property_synthesis_ivar_binding_contract.ivar_binding_resolved
           << ",\"ivar_binding_missing\":"
           << property_synthesis_ivar_binding_contract.ivar_binding_missing
           << ",\"ivar_binding_conflicts\":"
           << property_synthesis_ivar_binding_contract.ivar_binding_conflicts
           << ",\"lowering_property_synthesis_ivar_binding_replay_key\":\""
           << property_synthesis_ivar_binding_replay_key
           << "\""
           << ",\"deterministic_id_class_sel_object_pointer_typecheck_handoff\":"
           << (id_class_sel_object_pointer_typecheck_contract.deterministic ? "true" : "false")
           << ",\"id_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.id_typecheck_sites
           << ",\"class_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.class_typecheck_sites
           << ",\"sel_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.sel_typecheck_sites
           << ",\"object_pointer_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.object_pointer_typecheck_sites
           << ",\"id_class_sel_object_pointer_typecheck_sites_total\":"
           << id_class_sel_object_pointer_typecheck_contract.total_typecheck_sites
           << ",\"lowering_id_class_sel_object_pointer_typecheck_replay_key\":\""
           << id_class_sel_object_pointer_typecheck_replay_key
           << "\""
           << ",\"deterministic_message_send_selector_lowering_handoff\":"
           << (message_send_selector_lowering_contract.deterministic ? "true" : "false")
           << ",\"message_send_selector_lowering_sites\":"
           << message_send_selector_lowering_contract.message_send_sites
           << ",\"message_send_selector_lowering_unary_sites\":"
           << message_send_selector_lowering_contract.unary_selector_sites
           << ",\"message_send_selector_lowering_keyword_sites\":"
           << message_send_selector_lowering_contract.keyword_selector_sites
           << ",\"message_send_selector_lowering_selector_piece_sites\":"
           << message_send_selector_lowering_contract.selector_piece_sites
           << ",\"message_send_selector_lowering_argument_expression_sites\":"
           << message_send_selector_lowering_contract.argument_expression_sites
           << ",\"message_send_selector_lowering_receiver_sites\":"
           << message_send_selector_lowering_contract.receiver_expression_sites
           << ",\"message_send_selector_lowering_selector_literal_entries\":"
           << message_send_selector_lowering_contract.selector_literal_entries
           << ",\"message_send_selector_lowering_selector_literal_characters\":"
           << message_send_selector_lowering_contract.selector_literal_characters
           << ",\"lowering_message_send_selector_lowering_replay_key\":\""
           << message_send_selector_lowering_replay_key
           << "\""
           << ",\"deterministic_dispatch_abi_marshalling_handoff\":"
           << (dispatch_abi_marshalling_contract.deterministic ? "true" : "false")
           << ",\"dispatch_abi_marshalling_message_send_sites\":"
           << dispatch_abi_marshalling_contract.message_send_sites
           << ",\"dispatch_abi_marshalling_receiver_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.receiver_slots_marshaled
           << ",\"dispatch_abi_marshalling_selector_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.selector_slots_marshaled
           << ",\"dispatch_abi_marshalling_argument_value_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_value_slots_marshaled
           << ",\"dispatch_abi_marshalling_argument_padding_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_padding_slots_marshaled
           << ",\"dispatch_abi_marshalling_argument_total_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_total_slots_marshaled
           << ",\"dispatch_abi_marshalling_total_marshaled_slots\":"
           << dispatch_abi_marshalling_contract.total_marshaled_slots
           << ",\"dispatch_abi_marshalling_runtime_dispatch_arg_slots\":"
           << dispatch_abi_marshalling_contract.runtime_dispatch_arg_slots
           << ",\"lowering_dispatch_abi_marshalling_replay_key\":\""
           << dispatch_abi_marshalling_replay_key
           << "\""
           << ",\"deterministic_nil_receiver_semantics_foldability_handoff\":"
           << (nil_receiver_semantics_foldability_contract.deterministic ? "true" : "false")
           << ",\"nil_receiver_semantics_foldability_message_send_sites\":"
           << nil_receiver_semantics_foldability_contract.message_send_sites
           << ",\"nil_receiver_semantics_foldability_receiver_nil_literal_sites\":"
           << nil_receiver_semantics_foldability_contract.receiver_nil_literal_sites
           << ",\"nil_receiver_semantics_foldability_enabled_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_semantics_enabled_sites
           << ",\"nil_receiver_semantics_foldability_foldable_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_foldable_sites
           << ",\"nil_receiver_semantics_foldability_runtime_dispatch_required_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_runtime_dispatch_required_sites
           << ",\"nil_receiver_semantics_foldability_non_nil_receiver_sites\":"
           << nil_receiver_semantics_foldability_contract.non_nil_receiver_sites
           << ",\"nil_receiver_semantics_foldability_contract_violation_sites\":"
           << nil_receiver_semantics_foldability_contract.contract_violation_sites
           << ",\"lowering_nil_receiver_semantics_foldability_replay_key\":\""
           << nil_receiver_semantics_foldability_replay_key
           << "\""
           << ",\"deterministic_super_dispatch_method_family_handoff\":"
           << (super_dispatch_method_family_contract.deterministic ? "true" : "false")
           << ",\"super_dispatch_method_family_message_send_sites\":"
           << super_dispatch_method_family_contract.message_send_sites
           << ",\"super_dispatch_method_family_receiver_super_identifier_sites\":"
           << super_dispatch_method_family_contract.receiver_super_identifier_sites
           << ",\"super_dispatch_method_family_enabled_sites\":"
           << super_dispatch_method_family_contract.super_dispatch_enabled_sites
           << ",\"super_dispatch_method_family_requires_class_context_sites\":"
           << super_dispatch_method_family_contract.super_dispatch_requires_class_context_sites
           << ",\"super_dispatch_method_family_init_sites\":"
           << super_dispatch_method_family_contract.method_family_init_sites
           << ",\"super_dispatch_method_family_copy_sites\":"
           << super_dispatch_method_family_contract.method_family_copy_sites
           << ",\"super_dispatch_method_family_mutable_copy_sites\":"
           << super_dispatch_method_family_contract.method_family_mutable_copy_sites
           << ",\"super_dispatch_method_family_new_sites\":"
           << super_dispatch_method_family_contract.method_family_new_sites
           << ",\"super_dispatch_method_family_none_sites\":"
           << super_dispatch_method_family_contract.method_family_none_sites
           << ",\"super_dispatch_method_family_returns_retained_result_sites\":"
           << super_dispatch_method_family_contract.method_family_returns_retained_result_sites
           << ",\"super_dispatch_method_family_returns_related_result_sites\":"
           << super_dispatch_method_family_contract.method_family_returns_related_result_sites
           << ",\"super_dispatch_method_family_contract_violation_sites\":"
           << super_dispatch_method_family_contract.contract_violation_sites
           << ",\"lowering_super_dispatch_method_family_replay_key\":\""
           << super_dispatch_method_family_replay_key
           << "\""
           << ",\"deterministic_runtime_shim_host_link_handoff\":"
           << (runtime_shim_host_link_contract.deterministic ? "true" : "false")
           << ",\"runtime_shim_host_link_message_send_sites\":"
           << runtime_shim_host_link_contract.message_send_sites
           << ",\"runtime_shim_host_link_required_runtime_shim_sites\":"
           << runtime_shim_host_link_contract.runtime_shim_required_sites
           << ",\"runtime_shim_host_link_elided_runtime_shim_sites\":"
           << runtime_shim_host_link_contract.runtime_shim_elided_sites
           << ",\"runtime_shim_host_link_runtime_dispatch_arg_slots\":"
           << runtime_shim_host_link_contract.runtime_dispatch_arg_slots
           << ",\"runtime_shim_host_link_runtime_dispatch_declaration_parameter_count\":"
           << runtime_shim_host_link_contract.runtime_dispatch_declaration_parameter_count
           << ",\"runtime_shim_host_link_runtime_dispatch_symbol\":\""
           << runtime_shim_host_link_contract.runtime_dispatch_symbol
           << "\""
           << ",\"runtime_shim_host_link_default_runtime_dispatch_symbol_binding\":"
           << (runtime_shim_host_link_contract.default_runtime_dispatch_symbol_binding ? "true" : "false")
           << ",\"runtime_shim_host_link_contract_violation_sites\":"
           << runtime_shim_host_link_contract.contract_violation_sites
           << ",\"lowering_runtime_shim_host_link_replay_key\":\""
           << runtime_shim_host_link_replay_key
           << "\""
           << ",\"deterministic_ownership_qualifier_lowering_handoff\":"
           << (ownership_qualifier_lowering_contract.deterministic ? "true" : "false")
           << ",\"ownership_qualifier_lowering_type_annotation_ownership_qualifier_sites\":"
           << ownership_qualifier_lowering_contract.ownership_qualifier_sites
           << ",\"ownership_qualifier_lowering_type_annotation_invalid_ownership_qualifier_sites\":"
           << ownership_qualifier_lowering_contract.invalid_ownership_qualifier_sites
           << ",\"ownership_qualifier_lowering_type_annotation_object_pointer_type_sites\":"
           << ownership_qualifier_lowering_contract.object_pointer_type_annotation_sites
           << ",\"lowering_ownership_qualifier_replay_key\":\""
           << ownership_qualifier_lowering_replay_key
           << "\""
           << ",\"deterministic_retain_release_operation_lowering_handoff\":"
           << (retain_release_operation_lowering_contract.deterministic ? "true" : "false")
           << ",\"retain_release_operation_lowering_ownership_qualified_sites\":"
           << retain_release_operation_lowering_contract.ownership_qualified_sites
           << ",\"retain_release_operation_lowering_retain_insertion_sites\":"
           << retain_release_operation_lowering_contract.retain_insertion_sites
           << ",\"retain_release_operation_lowering_release_insertion_sites\":"
           << retain_release_operation_lowering_contract.release_insertion_sites
           << ",\"retain_release_operation_lowering_autorelease_insertion_sites\":"
           << retain_release_operation_lowering_contract.autorelease_insertion_sites
           << ",\"retain_release_operation_lowering_contract_violation_sites\":"
           << retain_release_operation_lowering_contract.contract_violation_sites
           << ",\"lowering_retain_release_operation_replay_key\":\""
           << retain_release_operation_lowering_replay_key
           << "\""
           << ",\"deterministic_autoreleasepool_scope_lowering_handoff\":"
           << (autoreleasepool_scope_lowering_contract.deterministic ? "true" : "false")
           << ",\"autoreleasepool_scope_lowering_scope_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_sites
           << ",\"autoreleasepool_scope_lowering_scope_symbolized_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_symbolized_sites
           << ",\"autoreleasepool_scope_lowering_max_scope_depth\":"
           << autoreleasepool_scope_lowering_contract.max_scope_depth
           << ",\"autoreleasepool_scope_lowering_scope_entry_transition_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_entry_transition_sites
           << ",\"autoreleasepool_scope_lowering_scope_exit_transition_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_exit_transition_sites
           << ",\"autoreleasepool_scope_lowering_contract_violation_sites\":"
           << autoreleasepool_scope_lowering_contract.contract_violation_sites
           << ",\"lowering_autoreleasepool_scope_replay_key\":\""
           << autoreleasepool_scope_lowering_replay_key
           << "\""
           << ",\"deterministic_weak_unowned_semantics_lowering_handoff\":"
           << (weak_unowned_semantics_lowering_contract.deterministic ? "true" : "false")
           << ",\"weak_unowned_semantics_lowering_ownership_candidate_sites\":"
           << weak_unowned_semantics_lowering_contract.ownership_candidate_sites
           << ",\"weak_unowned_semantics_lowering_weak_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.weak_reference_sites
           << ",\"weak_unowned_semantics_lowering_unowned_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.unowned_reference_sites
           << ",\"weak_unowned_semantics_lowering_unowned_safe_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.unowned_safe_reference_sites
           << ",\"weak_unowned_semantics_lowering_conflict_sites\":"
           << weak_unowned_semantics_lowering_contract.weak_unowned_conflict_sites
           << ",\"weak_unowned_semantics_lowering_contract_violation_sites\":"
           << weak_unowned_semantics_lowering_contract.contract_violation_sites
           << ",\"lowering_weak_unowned_semantics_replay_key\":\""
           << weak_unowned_semantics_lowering_replay_key
           << "\""
           << ",\"deterministic_arc_diagnostics_fixit_lowering_handoff\":"
           << (arc_diagnostics_fixit_lowering_contract.deterministic ? "true" : "false")
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_diagnostic_candidate_sites
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_fixit_available_sites
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_profiled_sites
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_weak_unowned_conflict_diagnostic_sites
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_empty_fixit_hint_sites
           << ",\"arc_diagnostics_fixit_lowering_contract_violation_sites\":"
           << arc_diagnostics_fixit_lowering_contract.contract_violation_sites
           << ",\"lowering_arc_diagnostics_fixit_replay_key\":\""
           << arc_diagnostics_fixit_lowering_replay_key
           << "\""
           << ",\"deterministic_block_literal_capture_lowering_handoff\":"
           << (block_literal_capture_lowering_contract.deterministic ? "true" : "false")
           << ",\"block_literal_capture_lowering_block_literal_sites\":"
           << block_literal_capture_lowering_contract.block_literal_sites
           << ",\"block_literal_capture_lowering_block_parameter_entries\":"
           << block_literal_capture_lowering_contract.block_parameter_entries
           << ",\"block_literal_capture_lowering_block_capture_entries\":"
           << block_literal_capture_lowering_contract.block_capture_entries
           << ",\"block_literal_capture_lowering_block_body_statement_entries\":"
           << block_literal_capture_lowering_contract.block_body_statement_entries
           << ",\"block_literal_capture_lowering_block_empty_capture_sites\":"
           << block_literal_capture_lowering_contract.block_empty_capture_sites
           << ",\"block_literal_capture_lowering_block_nondeterministic_capture_sites\":"
           << block_literal_capture_lowering_contract.block_nondeterministic_capture_sites
           << ",\"block_literal_capture_lowering_block_non_normalized_sites\":"
           << block_literal_capture_lowering_contract.block_non_normalized_sites
           << ",\"block_literal_capture_lowering_contract_violation_sites\":"
           << block_literal_capture_lowering_contract.contract_violation_sites
           << ",\"lowering_block_literal_capture_replay_key\":\""
           << block_literal_capture_lowering_replay_key
           << "\""
           << ",\"deterministic_block_abi_invoke_trampoline_lowering_handoff\":"
           << (block_abi_invoke_trampoline_lowering_contract.deterministic ? "true" : "false")
           << ",\"block_abi_invoke_trampoline_lowering_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.block_literal_sites
           << ",\"block_abi_invoke_trampoline_lowering_invoke_argument_slots\":"
           << block_abi_invoke_trampoline_lowering_contract.invoke_argument_slots_total
           << ",\"block_abi_invoke_trampoline_lowering_capture_word_count\":"
           << block_abi_invoke_trampoline_lowering_contract.capture_word_count_total
           << ",\"block_abi_invoke_trampoline_lowering_parameter_entries\":"
           << block_abi_invoke_trampoline_lowering_contract.parameter_entries_total
           << ",\"block_abi_invoke_trampoline_lowering_capture_entries\":"
           << block_abi_invoke_trampoline_lowering_contract.capture_entries_total
           << ",\"block_abi_invoke_trampoline_lowering_body_statement_entries\":"
           << block_abi_invoke_trampoline_lowering_contract.body_statement_entries_total
           << ",\"block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.descriptor_symbolized_sites
           << ",\"block_abi_invoke_trampoline_lowering_invoke_symbolized_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.invoke_trampoline_symbolized_sites
           << ",\"block_abi_invoke_trampoline_lowering_missing_invoke_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.missing_invoke_trampoline_sites
           << ",\"block_abi_invoke_trampoline_lowering_non_normalized_layout_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.non_normalized_layout_sites
           << ",\"block_abi_invoke_trampoline_lowering_contract_violation_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.contract_violation_sites
           << ",\"lowering_block_abi_invoke_trampoline_replay_key\":\""
           << block_abi_invoke_trampoline_lowering_replay_key
           << "\""
           << ",\"deterministic_block_storage_escape_lowering_handoff\":"
           << (block_storage_escape_lowering_contract.deterministic ? "true" : "false")
           << ",\"block_storage_escape_lowering_sites\":"
           << block_storage_escape_lowering_contract.block_literal_sites
           << ",\"block_storage_escape_lowering_mutable_capture_count\":"
           << block_storage_escape_lowering_contract.mutable_capture_count_total
           << ",\"block_storage_escape_lowering_byref_slot_count\":"
           << block_storage_escape_lowering_contract.byref_slot_count_total
           << ",\"block_storage_escape_lowering_parameter_entries\":"
           << block_storage_escape_lowering_contract.parameter_entries_total
           << ",\"block_storage_escape_lowering_capture_entries\":"
           << block_storage_escape_lowering_contract.capture_entries_total
           << ",\"block_storage_escape_lowering_body_statement_entries\":"
           << block_storage_escape_lowering_contract.body_statement_entries_total
           << ",\"block_storage_escape_lowering_requires_byref_cells_sites\":"
           << block_storage_escape_lowering_contract.requires_byref_cells_sites
           << ",\"block_storage_escape_lowering_escape_analysis_enabled_sites\":"
           << block_storage_escape_lowering_contract.escape_analysis_enabled_sites
           << ",\"block_storage_escape_lowering_escape_to_heap_sites\":"
           << block_storage_escape_lowering_contract.escape_to_heap_sites
           << ",\"block_storage_escape_lowering_escape_profile_normalized_sites\":"
           << block_storage_escape_lowering_contract.escape_profile_normalized_sites
           << ",\"block_storage_escape_lowering_byref_layout_symbolized_sites\":"
           << block_storage_escape_lowering_contract.byref_layout_symbolized_sites
           << ",\"block_storage_escape_lowering_contract_violation_sites\":"
           << block_storage_escape_lowering_contract.contract_violation_sites
           << ",\"lowering_block_storage_escape_replay_key\":\""
           << block_storage_escape_lowering_replay_key
           << "\""
           << ",\"deterministic_block_copy_dispose_lowering_handoff\":"
           << (block_copy_dispose_lowering_contract.deterministic ? "true" : "false")
           << ",\"block_copy_dispose_lowering_sites\":"
           << block_copy_dispose_lowering_contract.block_literal_sites
           << ",\"block_copy_dispose_lowering_mutable_capture_count\":"
           << block_copy_dispose_lowering_contract.mutable_capture_count_total
           << ",\"block_copy_dispose_lowering_byref_slot_count\":"
           << block_copy_dispose_lowering_contract.byref_slot_count_total
           << ",\"block_copy_dispose_lowering_parameter_entries\":"
           << block_copy_dispose_lowering_contract.parameter_entries_total
           << ",\"block_copy_dispose_lowering_capture_entries\":"
           << block_copy_dispose_lowering_contract.capture_entries_total
           << ",\"block_copy_dispose_lowering_body_statement_entries\":"
           << block_copy_dispose_lowering_contract.body_statement_entries_total
           << ",\"block_copy_dispose_lowering_copy_helper_required_sites\":"
           << block_copy_dispose_lowering_contract.copy_helper_required_sites
           << ",\"block_copy_dispose_lowering_dispose_helper_required_sites\":"
           << block_copy_dispose_lowering_contract.dispose_helper_required_sites
           << ",\"block_copy_dispose_lowering_profile_normalized_sites\":"
           << block_copy_dispose_lowering_contract.profile_normalized_sites
           << ",\"block_copy_dispose_lowering_copy_helper_symbolized_sites\":"
           << block_copy_dispose_lowering_contract.copy_helper_symbolized_sites
           << ",\"block_copy_dispose_lowering_dispose_helper_symbolized_sites\":"
           << block_copy_dispose_lowering_contract.dispose_helper_symbolized_sites
           << ",\"block_copy_dispose_lowering_contract_violation_sites\":"
           << block_copy_dispose_lowering_contract.contract_violation_sites
           << ",\"lowering_block_copy_dispose_replay_key\":\""
           << block_copy_dispose_lowering_replay_key
           << "\""
           << ",\"deterministic_block_determinism_perf_baseline_lowering_handoff\":"
           << (block_determinism_perf_baseline_lowering_contract.deterministic ? "true" : "false")
           << ",\"block_determinism_perf_baseline_lowering_sites\":"
           << block_determinism_perf_baseline_lowering_contract.block_literal_sites
           << ",\"block_determinism_perf_baseline_lowering_weight_total\":"
           << block_determinism_perf_baseline_lowering_contract.baseline_weight_total
           << ",\"block_determinism_perf_baseline_lowering_parameter_entries\":"
           << block_determinism_perf_baseline_lowering_contract.parameter_entries_total
           << ",\"block_determinism_perf_baseline_lowering_capture_entries\":"
           << block_determinism_perf_baseline_lowering_contract.capture_entries_total
           << ",\"block_determinism_perf_baseline_lowering_body_statement_entries\":"
           << block_determinism_perf_baseline_lowering_contract.body_statement_entries_total
           << ",\"block_determinism_perf_baseline_lowering_deterministic_capture_sites\":"
           << block_determinism_perf_baseline_lowering_contract.deterministic_capture_sites
           << ",\"block_determinism_perf_baseline_lowering_heavy_tier_sites\":"
           << block_determinism_perf_baseline_lowering_contract.heavy_tier_sites
           << ",\"block_determinism_perf_baseline_lowering_normalized_profile_sites\":"
           << block_determinism_perf_baseline_lowering_contract.normalized_profile_sites
           << ",\"block_determinism_perf_baseline_lowering_contract_violation_sites\":"
           << block_determinism_perf_baseline_lowering_contract.contract_violation_sites
           << ",\"lowering_block_determinism_perf_baseline_replay_key\":\""
           << block_determinism_perf_baseline_lowering_replay_key
           << "\""
           << ",\"deterministic_lightweight_generic_constraint_lowering_handoff\":"
           << (lightweight_generic_constraint_lowering_contract.deterministic ? "true" : "false")
           << ",\"lightweight_generic_constraint_lowering_sites\":"
           << lightweight_generic_constraint_lowering_contract.generic_constraint_sites
           << ",\"lightweight_generic_constraint_lowering_generic_suffix_sites\":"
           << lightweight_generic_constraint_lowering_contract.generic_suffix_sites
           << ",\"lightweight_generic_constraint_lowering_object_pointer_type_sites\":"
           << lightweight_generic_constraint_lowering_contract.object_pointer_type_sites
           << ",\"lightweight_generic_constraint_lowering_terminated_generic_suffix_sites\":"
           << lightweight_generic_constraint_lowering_contract.terminated_generic_suffix_sites
           << ",\"lightweight_generic_constraint_lowering_pointer_declarator_sites\":"
           << lightweight_generic_constraint_lowering_contract.pointer_declarator_sites
           << ",\"lightweight_generic_constraint_lowering_normalized_sites\":"
           << lightweight_generic_constraint_lowering_contract.normalized_constraint_sites
           << ",\"lightweight_generic_constraint_lowering_contract_violation_sites\":"
           << lightweight_generic_constraint_lowering_contract.contract_violation_sites
           << ",\"lowering_lightweight_generic_constraint_replay_key\":\""
           << lightweight_generic_constraint_lowering_replay_key
           << "\""
           << ",\"deterministic_nullability_flow_warning_precision_lowering_handoff\":"
           << (nullability_flow_warning_precision_lowering_contract.deterministic ? "true" : "false")
           << ",\"nullability_flow_warning_precision_lowering_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullability_flow_sites
           << ",\"nullability_flow_warning_precision_lowering_object_pointer_type_sites\":"
           << nullability_flow_warning_precision_lowering_contract.object_pointer_type_sites
           << ",\"nullability_flow_warning_precision_lowering_nullability_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullability_suffix_sites
           << ",\"nullability_flow_warning_precision_lowering_nullable_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullable_suffix_sites
           << ",\"nullability_flow_warning_precision_lowering_nonnull_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nonnull_suffix_sites
           << ",\"nullability_flow_warning_precision_lowering_normalized_sites\":"
           << nullability_flow_warning_precision_lowering_contract.normalized_sites
           << ",\"nullability_flow_warning_precision_lowering_contract_violation_sites\":"
           << nullability_flow_warning_precision_lowering_contract.contract_violation_sites
           << ",\"lowering_nullability_flow_warning_precision_replay_key\":\""
           << nullability_flow_warning_precision_lowering_replay_key
           << "\""
           << ",\"deterministic_protocol_qualified_object_type_lowering_handoff\":"
           << (protocol_qualified_object_type_lowering_contract.deterministic ? "true" : "false")
           << ",\"protocol_qualified_object_type_lowering_sites\":"
           << protocol_qualified_object_type_lowering_contract.protocol_qualified_object_type_sites
           << ",\"protocol_qualified_object_type_lowering_protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.protocol_composition_sites
           << ",\"protocol_qualified_object_type_lowering_object_pointer_type_sites\":"
           << protocol_qualified_object_type_lowering_contract.object_pointer_type_sites
           << ",\"protocol_qualified_object_type_lowering_terminated_protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.terminated_protocol_composition_sites
           << ",\"protocol_qualified_object_type_lowering_pointer_declarator_sites\":"
           << protocol_qualified_object_type_lowering_contract.pointer_declarator_sites
           << ",\"protocol_qualified_object_type_lowering_normalized_protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.normalized_protocol_composition_sites
           << ",\"protocol_qualified_object_type_lowering_contract_violation_sites\":"
           << protocol_qualified_object_type_lowering_contract.contract_violation_sites
           << ",\"lowering_protocol_qualified_object_type_replay_key\":\""
           << protocol_qualified_object_type_lowering_replay_key
           << "\""
           << ",\"deterministic_variance_bridge_cast_lowering_handoff\":"
           << (variance_bridge_cast_lowering_contract.deterministic ? "true" : "false")
           << ",\"variance_bridge_cast_lowering_sites\":"
           << variance_bridge_cast_lowering_contract.variance_bridge_cast_sites
           << ",\"variance_bridge_cast_lowering_protocol_composition_sites\":"
           << variance_bridge_cast_lowering_contract.protocol_composition_sites
           << ",\"variance_bridge_cast_lowering_ownership_qualifier_sites\":"
           << variance_bridge_cast_lowering_contract.ownership_qualifier_sites
           << ",\"variance_bridge_cast_lowering_object_pointer_type_sites\":"
           << variance_bridge_cast_lowering_contract.object_pointer_type_sites
           << ",\"variance_bridge_cast_lowering_pointer_declarator_sites\":"
           << variance_bridge_cast_lowering_contract.pointer_declarator_sites
           << ",\"variance_bridge_cast_lowering_normalized_sites\":"
           << variance_bridge_cast_lowering_contract.normalized_sites
           << ",\"variance_bridge_cast_lowering_contract_violation_sites\":"
           << variance_bridge_cast_lowering_contract.contract_violation_sites
           << ",\"lowering_variance_bridge_cast_replay_key\":\""
           << variance_bridge_cast_lowering_replay_key
           << "\""
           << ",\"deterministic_generic_metadata_abi_lowering_handoff\":"
           << (generic_metadata_abi_lowering_contract.deterministic ? "true" : "false")
           << ",\"generic_metadata_abi_lowering_sites\":"
           << generic_metadata_abi_lowering_contract.generic_metadata_abi_sites
           << ",\"generic_metadata_abi_lowering_generic_suffix_sites\":"
           << generic_metadata_abi_lowering_contract.generic_suffix_sites
           << ",\"generic_metadata_abi_lowering_protocol_composition_sites\":"
           << generic_metadata_abi_lowering_contract.protocol_composition_sites
           << ",\"generic_metadata_abi_lowering_ownership_qualifier_sites\":"
           << generic_metadata_abi_lowering_contract.ownership_qualifier_sites
           << ",\"generic_metadata_abi_lowering_object_pointer_type_sites\":"
           << generic_metadata_abi_lowering_contract.object_pointer_type_sites
           << ",\"generic_metadata_abi_lowering_pointer_declarator_sites\":"
           << generic_metadata_abi_lowering_contract.pointer_declarator_sites
           << ",\"generic_metadata_abi_lowering_normalized_sites\":"
           << generic_metadata_abi_lowering_contract.normalized_sites
           << ",\"generic_metadata_abi_lowering_contract_violation_sites\":"
           << generic_metadata_abi_lowering_contract.contract_violation_sites
           << ",\"lowering_generic_metadata_abi_replay_key\":\""
           << generic_metadata_abi_lowering_replay_key
           << "\""
           << ",\"deterministic_module_import_graph_lowering_handoff\":"
           << (module_import_graph_lowering_contract.deterministic ? "true" : "false")
           << ",\"module_import_graph_lowering_sites\":"
           << module_import_graph_lowering_contract.module_import_graph_sites
           << ",\"module_import_graph_lowering_import_edge_candidate_sites\":"
           << module_import_graph_lowering_contract.import_edge_candidate_sites
           << ",\"module_import_graph_lowering_namespace_segment_sites\":"
           << module_import_graph_lowering_contract.namespace_segment_sites
           << ",\"module_import_graph_lowering_object_pointer_type_sites\":"
           << module_import_graph_lowering_contract.object_pointer_type_sites
           << ",\"module_import_graph_lowering_pointer_declarator_sites\":"
           << module_import_graph_lowering_contract.pointer_declarator_sites
           << ",\"module_import_graph_lowering_normalized_sites\":"
           << module_import_graph_lowering_contract.normalized_sites
           << ",\"module_import_graph_lowering_contract_violation_sites\":"
           << module_import_graph_lowering_contract.contract_violation_sites
           << ",\"lowering_module_import_graph_replay_key\":\""
           << module_import_graph_lowering_replay_key
           << "\""
           << ",\"deterministic_namespace_collision_shadowing_lowering_handoff\":"
           << (namespace_collision_shadowing_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << ",\"namespace_collision_shadowing_lowering_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .namespace_collision_shadowing_sites
           << ",\"namespace_collision_shadowing_lowering_namespace_segment_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .namespace_segment_sites
           << ",\"namespace_collision_shadowing_lowering_import_edge_candidate_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .import_edge_candidate_sites
           << ",\"namespace_collision_shadowing_lowering_object_pointer_type_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .object_pointer_type_sites
           << ",\"namespace_collision_shadowing_lowering_pointer_declarator_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .pointer_declarator_sites
           << ",\"namespace_collision_shadowing_lowering_normalized_sites\":"
           << namespace_collision_shadowing_lowering_contract.normalized_sites
           << ",\"namespace_collision_shadowing_lowering_contract_violation_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .contract_violation_sites
           << ",\"lowering_namespace_collision_shadowing_replay_key\":\""
           << namespace_collision_shadowing_lowering_replay_key
           << "\""
           << ",\"deterministic_public_private_api_partition_lowering_handoff\":"
           << (public_private_api_partition_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << ",\"public_private_api_partition_lowering_sites\":"
           << public_private_api_partition_lowering_contract
                  .public_private_api_partition_sites
           << ",\"public_private_api_partition_lowering_namespace_segment_sites\":"
           << public_private_api_partition_lowering_contract
                  .namespace_segment_sites
           << ",\"public_private_api_partition_lowering_import_edge_candidate_sites\":"
           << public_private_api_partition_lowering_contract
                  .import_edge_candidate_sites
           << ",\"public_private_api_partition_lowering_object_pointer_type_sites\":"
           << public_private_api_partition_lowering_contract
                  .object_pointer_type_sites
           << ",\"public_private_api_partition_lowering_pointer_declarator_sites\":"
           << public_private_api_partition_lowering_contract
                  .pointer_declarator_sites
           << ",\"public_private_api_partition_lowering_normalized_sites\":"
           << public_private_api_partition_lowering_contract.normalized_sites
           << ",\"public_private_api_partition_lowering_contract_violation_sites\":"
           << public_private_api_partition_lowering_contract
                  .contract_violation_sites
           << ",\"lowering_public_private_api_partition_replay_key\":\""
           << public_private_api_partition_lowering_replay_key
           << "\""
           << ",\"deterministic_incremental_module_cache_invalidation_lowering_handoff\":"
           << (incremental_module_cache_invalidation_lowering_contract
                       .deterministic
                   ? "true"
                   : "false")
           << ",\"incremental_module_cache_invalidation_lowering_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .incremental_module_cache_invalidation_sites
           << ",\"incremental_module_cache_invalidation_lowering_namespace_segment_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .namespace_segment_sites
           << ",\"incremental_module_cache_invalidation_lowering_import_edge_candidate_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .import_edge_candidate_sites
           << ",\"incremental_module_cache_invalidation_lowering_object_pointer_type_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .object_pointer_type_sites
           << ",\"incremental_module_cache_invalidation_lowering_pointer_declarator_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .pointer_declarator_sites
           << ",\"incremental_module_cache_invalidation_lowering_normalized_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .normalized_sites
           << ",\"incremental_module_cache_invalidation_lowering_cache_invalidation_candidate_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"incremental_module_cache_invalidation_lowering_contract_violation_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .contract_violation_sites
           << ",\"lowering_incremental_module_cache_invalidation_replay_key\":\""
           << incremental_module_cache_invalidation_lowering_replay_key
           << "\""
           << ",\"deterministic_cross_module_conformance_lowering_handoff\":"
           << (cross_module_conformance_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << ",\"cross_module_conformance_lowering_sites\":"
           << cross_module_conformance_lowering_contract
                  .cross_module_conformance_sites
           << ",\"cross_module_conformance_lowering_namespace_segment_sites\":"
           << cross_module_conformance_lowering_contract.namespace_segment_sites
           << ",\"cross_module_conformance_lowering_import_edge_candidate_sites\":"
           << cross_module_conformance_lowering_contract
                  .import_edge_candidate_sites
           << ",\"cross_module_conformance_lowering_object_pointer_type_sites\":"
           << cross_module_conformance_lowering_contract.object_pointer_type_sites
           << ",\"cross_module_conformance_lowering_pointer_declarator_sites\":"
           << cross_module_conformance_lowering_contract.pointer_declarator_sites
           << ",\"cross_module_conformance_lowering_normalized_sites\":"
           << cross_module_conformance_lowering_contract.normalized_sites
           << ",\"cross_module_conformance_lowering_cache_invalidation_candidate_sites\":"
           << cross_module_conformance_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"cross_module_conformance_lowering_contract_violation_sites\":"
           << cross_module_conformance_lowering_contract.contract_violation_sites
           << ",\"lowering_cross_module_conformance_replay_key\":\""
           << cross_module_conformance_lowering_replay_key
           << "\""
           << ",\"deterministic_throws_propagation_lowering_handoff\":"
           << (throws_propagation_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << ",\"throws_propagation_lowering_sites\":"
           << throws_propagation_lowering_contract.throws_propagation_sites
           << ",\"throws_propagation_lowering_namespace_segment_sites\":"
           << throws_propagation_lowering_contract.namespace_segment_sites
           << ",\"throws_propagation_lowering_import_edge_candidate_sites\":"
           << throws_propagation_lowering_contract.import_edge_candidate_sites
           << ",\"throws_propagation_lowering_object_pointer_type_sites\":"
           << throws_propagation_lowering_contract.object_pointer_type_sites
           << ",\"throws_propagation_lowering_pointer_declarator_sites\":"
           << throws_propagation_lowering_contract.pointer_declarator_sites
           << ",\"throws_propagation_lowering_normalized_sites\":"
           << throws_propagation_lowering_contract.normalized_sites
           << ",\"throws_propagation_lowering_cache_invalidation_candidate_sites\":"
           << throws_propagation_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"throws_propagation_lowering_contract_violation_sites\":"
           << throws_propagation_lowering_contract.contract_violation_sites
           << ",\"lowering_throws_propagation_replay_key\":\""
           << throws_propagation_lowering_replay_key
           << "\""
           << ",\"deterministic_object_pointer_nullability_generics_handoff\":"
           << (object_pointer_nullability_generics_summary.deterministic_object_pointer_nullability_generics_handoff
                   ? "true"
                   : "false")
           << ",\"object_pointer_type_spellings\":"
           << object_pointer_nullability_generics_summary.object_pointer_type_spellings
           << ",\"pointer_declarator_entries\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_entries
           << ",\"pointer_declarator_depth_total\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_depth_total
           << ",\"pointer_declarator_token_entries\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_token_entries
           << ",\"nullability_suffix_entries\":"
           << object_pointer_nullability_generics_summary.nullability_suffix_entries
           << ",\"generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.generic_suffix_entries
           << ",\"terminated_generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.terminated_generic_suffix_entries
           << ",\"unterminated_generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.unterminated_generic_suffix_entries
           << ",\"symbol_graph_global_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.global_symbol_nodes
           << ",\"symbol_graph_function_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.function_symbol_nodes
           << ",\"symbol_graph_interface_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_symbol_nodes
           << ",\"symbol_graph_implementation_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_symbol_nodes
           << ",\"symbol_graph_interface_property_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_property_symbol_nodes
           << ",\"symbol_graph_implementation_property_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes
           << ",\"symbol_graph_interface_method_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_method_symbol_nodes
           << ",\"symbol_graph_implementation_method_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes
           << ",\"scope_resolution_top_level_scope_symbols\":"
           << symbol_graph_scope_resolution_summary.top_level_scope_symbols
           << ",\"scope_resolution_nested_scope_symbols\":"
           << symbol_graph_scope_resolution_summary.nested_scope_symbols
           << ",\"scope_resolution_scope_frames_total\":"
           << symbol_graph_scope_resolution_summary.scope_frames_total
           << ",\"scope_resolution_implementation_interface_resolution_sites\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites
           << ",\"scope_resolution_implementation_interface_resolution_hits\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits
           << ",\"scope_resolution_implementation_interface_resolution_misses\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses
           << ",\"scope_resolution_method_resolution_sites\":"
           << symbol_graph_scope_resolution_summary.method_resolution_sites
           << ",\"scope_resolution_method_resolution_hits\":"
           << symbol_graph_scope_resolution_summary.method_resolution_hits
           << ",\"scope_resolution_method_resolution_misses\":"
           << symbol_graph_scope_resolution_summary.method_resolution_misses
           << ",\"deterministic_symbol_graph_handoff\":"
           << (symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff ? "true" : "false")
           << ",\"deterministic_scope_resolution_handoff\":"
           << (symbol_graph_scope_resolution_summary.deterministic_scope_resolution_handoff ? "true" : "false")
           << ",\"symbol_graph_scope_resolution_handoff_key\":\""
           << symbol_graph_scope_resolution_summary.deterministic_handoff_key
           << "\"},\n";
  manifest << "      \"vector_signature_surface\":{\"vector_signature_functions\":" << vector_signature_functions
           << ",\"vector_return_signatures\":" << vector_return_signatures
           << ",\"vector_param_signatures\":" << vector_param_signatures
           << ",\"vector_i32_signatures\":" << vector_i32_signatures
           << ",\"vector_bool_signatures\":" << vector_bool_signatures
           << ",\"lane2\":" << vector_lane2_signatures
           << ",\"lane4\":" << vector_lane4_signatures << ",\"lane8\":" << vector_lane8_signatures
           << ",\"lane16\":" << vector_lane16_signatures << "},\n";
  manifest << "      \"semantic_surface\": {\"declared_globals\":" << program.globals.size()
           << ",\"declared_functions\":" << manifest_functions.size()
           << ",\"declared_interfaces\":" << program.interfaces.size()
           << ",\"declared_implementations\":" << program.implementations.size()
           << ",\"resolved_global_symbols\":" << pipeline_result.integration_surface.globals.size()
           << ",\"resolved_function_symbols\":" << pipeline_result.integration_surface.functions.size()
           << ",\"resolved_interface_symbols\":" << pipeline_result.integration_surface.interfaces.size()
           << ",\"resolved_implementation_symbols\":" << pipeline_result.integration_surface.implementations.size()
           << ",\"declared_protocols\":" << protocol_category_summary.declared_protocols
           << ",\"declared_categories\":" << protocol_category_summary.declared_categories
           << ",\"resolved_protocol_symbols\":" << protocol_category_summary.resolved_protocol_symbols
           << ",\"resolved_category_symbols\":" << protocol_category_summary.resolved_category_symbols
           << ",\"interface_method_symbols\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.interface_method_symbols
           << ",\"implementation_method_symbols\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.implementation_method_symbols
           << ",\"protocol_method_symbols\":" << protocol_category_summary.protocol_method_symbols
           << ",\"category_method_symbols\":" << protocol_category_summary.category_method_symbols
           << ",\"linked_implementation_symbols\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.linked_implementation_symbols
           << ",\"linked_category_symbols\":" << protocol_category_summary.linked_category_symbols
           << ",\"objc_interface_implementation_surface\":{\"interface_class_method_symbols\":"
           << interface_class_method_symbols
           << ",\"interface_instance_method_symbols\":"
           << interface_instance_method_symbols
           << ",\"implementation_class_method_symbols\":"
           << implementation_class_method_symbols
           << ",\"implementation_instance_method_symbols\":"
           << implementation_instance_method_symbols
           << ",\"implementation_methods_with_body\":"
           << implementation_methods_with_body
           << ",\"deterministic_handoff\":"
           << (pipeline_result.sema_parity_surface.deterministic_interface_implementation_handoff ? "true" : "false")
           << "}"
           << ",\"objc_protocol_category_surface\":{\"protocol_method_symbols\":"
           << protocol_category_summary.protocol_method_symbols
           << ",\"category_method_symbols\":"
           << protocol_category_summary.category_method_symbols
           << ",\"linked_category_symbols\":"
           << protocol_category_summary.linked_category_symbols
           << ",\"deterministic_handoff\":"
           << (protocol_category_summary.deterministic_protocol_category_handoff ? "true" : "false")
           << "}"
           << ",\"objc_class_protocol_category_linking_surface\":{\"declared_class_interfaces\":"
           << class_protocol_category_linking_summary.declared_class_interfaces
           << ",\"declared_class_implementations\":"
           << class_protocol_category_linking_summary.declared_class_implementations
           << ",\"resolved_class_interfaces\":"
           << class_protocol_category_linking_summary.resolved_class_interfaces
           << ",\"resolved_class_implementations\":"
           << class_protocol_category_linking_summary.resolved_class_implementations
           << ",\"linked_class_method_symbols\":"
           << class_protocol_category_linking_summary.linked_class_method_symbols
           << ",\"linked_category_method_symbols\":"
           << class_protocol_category_linking_summary.linked_category_method_symbols
           << ",\"protocol_composition_sites\":"
           << class_protocol_category_linking_summary.protocol_composition_sites
           << ",\"protocol_composition_symbols\":"
           << class_protocol_category_linking_summary.protocol_composition_symbols
           << ",\"category_composition_sites\":"
           << class_protocol_category_linking_summary.category_composition_sites
           << ",\"category_composition_symbols\":"
           << class_protocol_category_linking_summary.category_composition_symbols
           << ",\"invalid_protocol_composition_sites\":"
           << class_protocol_category_linking_summary.invalid_protocol_composition_sites
           << ",\"deterministic_handoff\":"
           << (class_protocol_category_linking_summary.deterministic_class_protocol_category_linking_handoff
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_selector_normalization_surface\":{\"method_declaration_entries\":"
           << selector_normalization_summary.method_declaration_entries
           << ",\"normalized_method_declarations\":"
           << selector_normalization_summary.normalized_method_declarations
           << ",\"selector_piece_entries\":"
           << selector_normalization_summary.selector_piece_entries
           << ",\"selector_piece_parameter_links\":"
           << selector_normalization_summary.selector_piece_parameter_links
           << ",\"deterministic_handoff\":"
           << (selector_normalization_summary.deterministic_selector_normalization_handoff ? "true" : "false")
           << "}"
           << ",\"objc_property_attribute_surface\":{\"property_declaration_entries\":"
           << property_attribute_summary.property_declaration_entries
           << ",\"property_attribute_entries\":"
           << property_attribute_summary.property_attribute_entries
           << ",\"property_attribute_value_entries\":"
           << property_attribute_summary.property_attribute_value_entries
           << ",\"property_accessor_modifier_entries\":"
           << property_attribute_summary.property_accessor_modifier_entries
           << ",\"property_getter_selector_entries\":"
           << property_attribute_summary.property_getter_selector_entries
           << ",\"property_setter_selector_entries\":"
           << property_attribute_summary.property_setter_selector_entries
           << ",\"deterministic_handoff\":"
           << (property_attribute_summary.deterministic_property_attribute_handoff ? "true" : "false")
           << "}"
           << ",\"objc_property_synthesis_ivar_binding_surface\":{\"property_synthesis_sites\":"
           << property_synthesis_ivar_binding_contract.property_synthesis_sites
           << ",\"property_synthesis_explicit_ivar_bindings\":"
           << property_synthesis_ivar_binding_contract.property_synthesis_explicit_ivar_bindings
           << ",\"property_synthesis_default_ivar_bindings\":"
           << property_synthesis_ivar_binding_contract.property_synthesis_default_ivar_bindings
           << ",\"ivar_binding_sites\":"
           << property_synthesis_ivar_binding_contract.ivar_binding_sites
           << ",\"ivar_binding_resolved\":"
           << property_synthesis_ivar_binding_contract.ivar_binding_resolved
           << ",\"ivar_binding_missing\":"
           << property_synthesis_ivar_binding_contract.ivar_binding_missing
           << ",\"ivar_binding_conflicts\":"
           << property_synthesis_ivar_binding_contract.ivar_binding_conflicts
           << ",\"replay_key\":\""
           << property_synthesis_ivar_binding_replay_key
           << "\",\"deterministic_handoff\":"
           << (property_synthesis_ivar_binding_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_id_class_sel_object_pointer_typecheck_surface\":{\"id_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.id_typecheck_sites
           << ",\"class_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.class_typecheck_sites
           << ",\"sel_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.sel_typecheck_sites
           << ",\"object_pointer_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.object_pointer_typecheck_sites
           << ",\"total_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.total_typecheck_sites
           << ",\"replay_key\":\""
           << id_class_sel_object_pointer_typecheck_replay_key
           << "\",\"deterministic_handoff\":"
           << (id_class_sel_object_pointer_typecheck_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_message_send_selector_lowering_surface\":{\"message_send_sites\":"
           << message_send_selector_lowering_contract.message_send_sites
           << ",\"unary_selector_sites\":"
           << message_send_selector_lowering_contract.unary_selector_sites
           << ",\"keyword_selector_sites\":"
           << message_send_selector_lowering_contract.keyword_selector_sites
           << ",\"selector_piece_sites\":"
           << message_send_selector_lowering_contract.selector_piece_sites
           << ",\"argument_expression_sites\":"
           << message_send_selector_lowering_contract.argument_expression_sites
           << ",\"receiver_expression_sites\":"
           << message_send_selector_lowering_contract.receiver_expression_sites
           << ",\"selector_literal_entries\":"
           << message_send_selector_lowering_contract.selector_literal_entries
           << ",\"selector_literal_characters\":"
           << message_send_selector_lowering_contract.selector_literal_characters
           << ",\"replay_key\":\""
           << message_send_selector_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (message_send_selector_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_dispatch_abi_marshalling_surface\":{\"message_send_sites\":"
           << dispatch_abi_marshalling_contract.message_send_sites
           << ",\"receiver_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.receiver_slots_marshaled
           << ",\"selector_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.selector_slots_marshaled
           << ",\"argument_value_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_value_slots_marshaled
           << ",\"argument_padding_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_padding_slots_marshaled
           << ",\"argument_total_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_total_slots_marshaled
           << ",\"total_marshaled_slots\":"
           << dispatch_abi_marshalling_contract.total_marshaled_slots
           << ",\"runtime_dispatch_arg_slots\":"
           << dispatch_abi_marshalling_contract.runtime_dispatch_arg_slots
           << ",\"replay_key\":\""
           << dispatch_abi_marshalling_replay_key
           << "\",\"deterministic_handoff\":"
           << (dispatch_abi_marshalling_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_nil_receiver_semantics_foldability_surface\":{\"message_send_sites\":"
           << nil_receiver_semantics_foldability_contract.message_send_sites
           << ",\"receiver_nil_literal_sites\":"
           << nil_receiver_semantics_foldability_contract.receiver_nil_literal_sites
           << ",\"nil_receiver_semantics_enabled_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_semantics_enabled_sites
           << ",\"nil_receiver_foldable_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_foldable_sites
           << ",\"nil_receiver_runtime_dispatch_required_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_runtime_dispatch_required_sites
           << ",\"non_nil_receiver_sites\":"
           << nil_receiver_semantics_foldability_contract.non_nil_receiver_sites
           << ",\"contract_violation_sites\":"
           << nil_receiver_semantics_foldability_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << nil_receiver_semantics_foldability_replay_key
           << "\",\"deterministic_handoff\":"
           << (nil_receiver_semantics_foldability_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_super_dispatch_method_family_surface\":{\"message_send_sites\":"
           << super_dispatch_method_family_contract.message_send_sites
           << ",\"receiver_super_identifier_sites\":"
           << super_dispatch_method_family_contract.receiver_super_identifier_sites
           << ",\"super_dispatch_enabled_sites\":"
           << super_dispatch_method_family_contract.super_dispatch_enabled_sites
           << ",\"super_dispatch_requires_class_context_sites\":"
           << super_dispatch_method_family_contract.super_dispatch_requires_class_context_sites
           << ",\"method_family_init_sites\":"
           << super_dispatch_method_family_contract.method_family_init_sites
           << ",\"method_family_copy_sites\":"
           << super_dispatch_method_family_contract.method_family_copy_sites
           << ",\"method_family_mutable_copy_sites\":"
           << super_dispatch_method_family_contract.method_family_mutable_copy_sites
           << ",\"method_family_new_sites\":"
           << super_dispatch_method_family_contract.method_family_new_sites
           << ",\"method_family_none_sites\":"
           << super_dispatch_method_family_contract.method_family_none_sites
           << ",\"method_family_returns_retained_result_sites\":"
           << super_dispatch_method_family_contract.method_family_returns_retained_result_sites
           << ",\"method_family_returns_related_result_sites\":"
           << super_dispatch_method_family_contract.method_family_returns_related_result_sites
           << ",\"contract_violation_sites\":"
           << super_dispatch_method_family_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << super_dispatch_method_family_replay_key
           << "\",\"deterministic_handoff\":"
           << (super_dispatch_method_family_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_runtime_shim_host_link_surface\":{\"message_send_sites\":"
           << runtime_shim_host_link_contract.message_send_sites
           << ",\"runtime_shim_required_sites\":"
           << runtime_shim_host_link_contract.runtime_shim_required_sites
           << ",\"runtime_shim_elided_sites\":"
           << runtime_shim_host_link_contract.runtime_shim_elided_sites
           << ",\"runtime_dispatch_arg_slots\":"
           << runtime_shim_host_link_contract.runtime_dispatch_arg_slots
           << ",\"runtime_dispatch_declaration_parameter_count\":"
           << runtime_shim_host_link_contract.runtime_dispatch_declaration_parameter_count
           << ",\"runtime_dispatch_symbol\":\""
           << runtime_shim_host_link_contract.runtime_dispatch_symbol
           << "\",\"default_runtime_dispatch_symbol_binding\":"
           << (runtime_shim_host_link_contract.default_runtime_dispatch_symbol_binding ? "true" : "false")
           << ",\"contract_violation_sites\":"
           << runtime_shim_host_link_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << runtime_shim_host_link_replay_key
           << "\",\"deterministic_handoff\":"
           << (runtime_shim_host_link_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_ownership_qualifier_lowering_surface\":{\"ownership_qualifier_sites\":"
           << ownership_qualifier_lowering_contract.ownership_qualifier_sites
           << ",\"invalid_ownership_qualifier_sites\":"
           << ownership_qualifier_lowering_contract.invalid_ownership_qualifier_sites
           << ",\"object_pointer_type_annotation_sites\":"
           << ownership_qualifier_lowering_contract.object_pointer_type_annotation_sites
           << ",\"replay_key\":\""
           << ownership_qualifier_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (ownership_qualifier_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_retain_release_operation_lowering_surface\":{\"ownership_qualified_sites\":"
           << retain_release_operation_lowering_contract.ownership_qualified_sites
           << ",\"retain_insertion_sites\":"
           << retain_release_operation_lowering_contract.retain_insertion_sites
           << ",\"release_insertion_sites\":"
           << retain_release_operation_lowering_contract.release_insertion_sites
           << ",\"autorelease_insertion_sites\":"
           << retain_release_operation_lowering_contract.autorelease_insertion_sites
           << ",\"contract_violation_sites\":"
           << retain_release_operation_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << retain_release_operation_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (retain_release_operation_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_autoreleasepool_scope_lowering_surface\":{\"scope_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_sites
           << ",\"scope_symbolized_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_symbolized_sites
           << ",\"max_scope_depth\":"
           << autoreleasepool_scope_lowering_contract.max_scope_depth
           << ",\"scope_entry_transition_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_entry_transition_sites
           << ",\"scope_exit_transition_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_exit_transition_sites
           << ",\"contract_violation_sites\":"
           << autoreleasepool_scope_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << autoreleasepool_scope_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (autoreleasepool_scope_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_weak_unowned_semantics_lowering_surface\":{\"ownership_candidate_sites\":"
           << weak_unowned_semantics_lowering_contract.ownership_candidate_sites
           << ",\"weak_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.weak_reference_sites
           << ",\"unowned_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.unowned_reference_sites
           << ",\"unowned_safe_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.unowned_safe_reference_sites
           << ",\"weak_unowned_conflict_sites\":"
           << weak_unowned_semantics_lowering_contract.weak_unowned_conflict_sites
           << ",\"contract_violation_sites\":"
           << weak_unowned_semantics_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << weak_unowned_semantics_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (weak_unowned_semantics_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_arc_diagnostics_fixit_lowering_surface\":{\"ownership_arc_diagnostic_candidate_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_diagnostic_candidate_sites
           << ",\"ownership_arc_fixit_available_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_fixit_available_sites
           << ",\"ownership_arc_profiled_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_profiled_sites
           << ",\"ownership_arc_weak_unowned_conflict_diagnostic_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_weak_unowned_conflict_diagnostic_sites
           << ",\"ownership_arc_empty_fixit_hint_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_empty_fixit_hint_sites
           << ",\"contract_violation_sites\":"
           << arc_diagnostics_fixit_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << arc_diagnostics_fixit_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (arc_diagnostics_fixit_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_block_literal_capture_lowering_surface\":{\"block_literal_sites\":"
           << block_literal_capture_lowering_contract.block_literal_sites
           << ",\"block_parameter_entries\":"
           << block_literal_capture_lowering_contract.block_parameter_entries
           << ",\"block_capture_entries\":"
           << block_literal_capture_lowering_contract.block_capture_entries
           << ",\"block_body_statement_entries\":"
           << block_literal_capture_lowering_contract.block_body_statement_entries
           << ",\"block_empty_capture_sites\":"
           << block_literal_capture_lowering_contract.block_empty_capture_sites
           << ",\"block_nondeterministic_capture_sites\":"
           << block_literal_capture_lowering_contract.block_nondeterministic_capture_sites
           << ",\"block_non_normalized_sites\":"
           << block_literal_capture_lowering_contract.block_non_normalized_sites
           << ",\"contract_violation_sites\":"
           << block_literal_capture_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << block_literal_capture_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (block_literal_capture_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_block_abi_invoke_trampoline_lowering_surface\":{\"block_literal_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.block_literal_sites
           << ",\"invoke_argument_slots_total\":"
           << block_abi_invoke_trampoline_lowering_contract.invoke_argument_slots_total
           << ",\"capture_word_count_total\":"
           << block_abi_invoke_trampoline_lowering_contract.capture_word_count_total
           << ",\"parameter_entries_total\":"
           << block_abi_invoke_trampoline_lowering_contract.parameter_entries_total
           << ",\"capture_entries_total\":"
           << block_abi_invoke_trampoline_lowering_contract.capture_entries_total
           << ",\"body_statement_entries_total\":"
           << block_abi_invoke_trampoline_lowering_contract.body_statement_entries_total
           << ",\"descriptor_symbolized_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.descriptor_symbolized_sites
           << ",\"invoke_trampoline_symbolized_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.invoke_trampoline_symbolized_sites
           << ",\"missing_invoke_trampoline_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.missing_invoke_trampoline_sites
           << ",\"non_normalized_layout_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.non_normalized_layout_sites
           << ",\"contract_violation_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << block_abi_invoke_trampoline_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (block_abi_invoke_trampoline_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_block_storage_escape_lowering_surface\":{\"block_literal_sites\":"
           << block_storage_escape_lowering_contract.block_literal_sites
           << ",\"mutable_capture_count_total\":"
           << block_storage_escape_lowering_contract.mutable_capture_count_total
           << ",\"byref_slot_count_total\":"
           << block_storage_escape_lowering_contract.byref_slot_count_total
           << ",\"parameter_entries_total\":"
           << block_storage_escape_lowering_contract.parameter_entries_total
           << ",\"capture_entries_total\":"
           << block_storage_escape_lowering_contract.capture_entries_total
           << ",\"body_statement_entries_total\":"
           << block_storage_escape_lowering_contract.body_statement_entries_total
           << ",\"requires_byref_cells_sites\":"
           << block_storage_escape_lowering_contract.requires_byref_cells_sites
           << ",\"escape_analysis_enabled_sites\":"
           << block_storage_escape_lowering_contract.escape_analysis_enabled_sites
           << ",\"escape_to_heap_sites\":"
           << block_storage_escape_lowering_contract.escape_to_heap_sites
           << ",\"escape_profile_normalized_sites\":"
           << block_storage_escape_lowering_contract.escape_profile_normalized_sites
           << ",\"byref_layout_symbolized_sites\":"
           << block_storage_escape_lowering_contract.byref_layout_symbolized_sites
           << ",\"contract_violation_sites\":"
           << block_storage_escape_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << block_storage_escape_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (block_storage_escape_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_block_copy_dispose_lowering_surface\":{\"block_literal_sites\":"
           << block_copy_dispose_lowering_contract.block_literal_sites
           << ",\"mutable_capture_count_total\":"
           << block_copy_dispose_lowering_contract.mutable_capture_count_total
           << ",\"byref_slot_count_total\":"
           << block_copy_dispose_lowering_contract.byref_slot_count_total
           << ",\"parameter_entries_total\":"
           << block_copy_dispose_lowering_contract.parameter_entries_total
           << ",\"capture_entries_total\":"
           << block_copy_dispose_lowering_contract.capture_entries_total
           << ",\"body_statement_entries_total\":"
           << block_copy_dispose_lowering_contract.body_statement_entries_total
           << ",\"copy_helper_required_sites\":"
           << block_copy_dispose_lowering_contract.copy_helper_required_sites
           << ",\"dispose_helper_required_sites\":"
           << block_copy_dispose_lowering_contract.dispose_helper_required_sites
           << ",\"profile_normalized_sites\":"
           << block_copy_dispose_lowering_contract.profile_normalized_sites
           << ",\"copy_helper_symbolized_sites\":"
           << block_copy_dispose_lowering_contract.copy_helper_symbolized_sites
           << ",\"dispose_helper_symbolized_sites\":"
           << block_copy_dispose_lowering_contract.dispose_helper_symbolized_sites
           << ",\"contract_violation_sites\":"
           << block_copy_dispose_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << block_copy_dispose_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (block_copy_dispose_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_block_determinism_perf_baseline_lowering_surface\":{\"block_literal_sites\":"
           << block_determinism_perf_baseline_lowering_contract.block_literal_sites
           << ",\"baseline_weight_total\":"
           << block_determinism_perf_baseline_lowering_contract.baseline_weight_total
           << ",\"parameter_entries_total\":"
           << block_determinism_perf_baseline_lowering_contract.parameter_entries_total
           << ",\"capture_entries_total\":"
           << block_determinism_perf_baseline_lowering_contract.capture_entries_total
           << ",\"body_statement_entries_total\":"
           << block_determinism_perf_baseline_lowering_contract.body_statement_entries_total
           << ",\"deterministic_capture_sites\":"
           << block_determinism_perf_baseline_lowering_contract.deterministic_capture_sites
           << ",\"heavy_tier_sites\":"
           << block_determinism_perf_baseline_lowering_contract.heavy_tier_sites
           << ",\"normalized_profile_sites\":"
           << block_determinism_perf_baseline_lowering_contract.normalized_profile_sites
           << ",\"contract_violation_sites\":"
           << block_determinism_perf_baseline_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << block_determinism_perf_baseline_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (block_determinism_perf_baseline_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_lightweight_generic_constraint_lowering_surface\":{\"generic_constraint_sites\":"
           << lightweight_generic_constraint_lowering_contract.generic_constraint_sites
           << ",\"generic_suffix_sites\":"
           << lightweight_generic_constraint_lowering_contract.generic_suffix_sites
           << ",\"object_pointer_type_sites\":"
           << lightweight_generic_constraint_lowering_contract.object_pointer_type_sites
           << ",\"terminated_generic_suffix_sites\":"
           << lightweight_generic_constraint_lowering_contract.terminated_generic_suffix_sites
           << ",\"pointer_declarator_sites\":"
           << lightweight_generic_constraint_lowering_contract.pointer_declarator_sites
           << ",\"normalized_constraint_sites\":"
           << lightweight_generic_constraint_lowering_contract.normalized_constraint_sites
           << ",\"contract_violation_sites\":"
           << lightweight_generic_constraint_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << lightweight_generic_constraint_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (lightweight_generic_constraint_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_nullability_flow_warning_precision_lowering_surface\":{\"nullability_flow_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullability_flow_sites
           << ",\"object_pointer_type_sites\":"
           << nullability_flow_warning_precision_lowering_contract.object_pointer_type_sites
           << ",\"nullability_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullability_suffix_sites
           << ",\"nullable_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullable_suffix_sites
           << ",\"nonnull_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nonnull_suffix_sites
           << ",\"normalized_sites\":"
           << nullability_flow_warning_precision_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << nullability_flow_warning_precision_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << nullability_flow_warning_precision_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (nullability_flow_warning_precision_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_protocol_qualified_object_type_lowering_surface\":{\"protocol_qualified_object_type_sites\":"
           << protocol_qualified_object_type_lowering_contract.protocol_qualified_object_type_sites
           << ",\"protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.protocol_composition_sites
           << ",\"object_pointer_type_sites\":"
           << protocol_qualified_object_type_lowering_contract.object_pointer_type_sites
           << ",\"terminated_protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.terminated_protocol_composition_sites
           << ",\"pointer_declarator_sites\":"
           << protocol_qualified_object_type_lowering_contract.pointer_declarator_sites
           << ",\"normalized_protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.normalized_protocol_composition_sites
           << ",\"contract_violation_sites\":"
           << protocol_qualified_object_type_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << protocol_qualified_object_type_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (protocol_qualified_object_type_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_variance_bridge_cast_lowering_surface\":{\"variance_bridge_cast_sites\":"
           << variance_bridge_cast_lowering_contract.variance_bridge_cast_sites
           << ",\"protocol_composition_sites\":"
           << variance_bridge_cast_lowering_contract.protocol_composition_sites
           << ",\"ownership_qualifier_sites\":"
           << variance_bridge_cast_lowering_contract.ownership_qualifier_sites
           << ",\"object_pointer_type_sites\":"
           << variance_bridge_cast_lowering_contract.object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << variance_bridge_cast_lowering_contract.pointer_declarator_sites
           << ",\"normalized_sites\":"
           << variance_bridge_cast_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << variance_bridge_cast_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << variance_bridge_cast_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (variance_bridge_cast_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_generic_metadata_abi_lowering_surface\":{\"generic_metadata_abi_sites\":"
           << generic_metadata_abi_lowering_contract.generic_metadata_abi_sites
           << ",\"generic_suffix_sites\":"
           << generic_metadata_abi_lowering_contract.generic_suffix_sites
           << ",\"protocol_composition_sites\":"
           << generic_metadata_abi_lowering_contract.protocol_composition_sites
           << ",\"ownership_qualifier_sites\":"
           << generic_metadata_abi_lowering_contract.ownership_qualifier_sites
           << ",\"object_pointer_type_sites\":"
           << generic_metadata_abi_lowering_contract.object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << generic_metadata_abi_lowering_contract.pointer_declarator_sites
           << ",\"normalized_sites\":"
           << generic_metadata_abi_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << generic_metadata_abi_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << generic_metadata_abi_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (generic_metadata_abi_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_module_import_graph_lowering_surface\":{\"module_import_graph_sites\":"
           << module_import_graph_lowering_contract.module_import_graph_sites
           << ",\"import_edge_candidate_sites\":"
           << module_import_graph_lowering_contract.import_edge_candidate_sites
           << ",\"namespace_segment_sites\":"
           << module_import_graph_lowering_contract.namespace_segment_sites
           << ",\"object_pointer_type_sites\":"
           << module_import_graph_lowering_contract.object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << module_import_graph_lowering_contract.pointer_declarator_sites
           << ",\"normalized_sites\":"
           << module_import_graph_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << module_import_graph_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << module_import_graph_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (module_import_graph_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_namespace_collision_shadowing_lowering_surface\":{\"namespace_collision_shadowing_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .namespace_collision_shadowing_sites
           << ",\"namespace_segment_sites\":"
           << namespace_collision_shadowing_lowering_contract.namespace_segment_sites
           << ",\"import_edge_candidate_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .import_edge_candidate_sites
           << ",\"object_pointer_type_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .pointer_declarator_sites
           << ",\"normalized_sites\":"
           << namespace_collision_shadowing_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .contract_violation_sites
           << ",\"replay_key\":\""
           << namespace_collision_shadowing_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (namespace_collision_shadowing_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_public_private_api_partition_lowering_surface\":{\"public_private_api_partition_sites\":"
           << public_private_api_partition_lowering_contract
                  .public_private_api_partition_sites
           << ",\"namespace_segment_sites\":"
           << public_private_api_partition_lowering_contract
                  .namespace_segment_sites
           << ",\"import_edge_candidate_sites\":"
           << public_private_api_partition_lowering_contract
                  .import_edge_candidate_sites
           << ",\"object_pointer_type_sites\":"
           << public_private_api_partition_lowering_contract
                  .object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << public_private_api_partition_lowering_contract
                  .pointer_declarator_sites
           << ",\"normalized_sites\":"
           << public_private_api_partition_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << public_private_api_partition_lowering_contract
                  .contract_violation_sites
           << ",\"replay_key\":\""
           << public_private_api_partition_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (public_private_api_partition_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_incremental_module_cache_invalidation_lowering_surface\":{\"incremental_module_cache_invalidation_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .incremental_module_cache_invalidation_sites
           << ",\"namespace_segment_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .namespace_segment_sites
           << ",\"import_edge_candidate_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .import_edge_candidate_sites
           << ",\"object_pointer_type_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .pointer_declarator_sites
           << ",\"normalized_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .normalized_sites
           << ",\"cache_invalidation_candidate_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"contract_violation_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .contract_violation_sites
           << ",\"replay_key\":\""
           << incremental_module_cache_invalidation_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (incremental_module_cache_invalidation_lowering_contract
                       .deterministic
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_cross_module_conformance_lowering_surface\":{\"cross_module_conformance_sites\":"
           << cross_module_conformance_lowering_contract
                  .cross_module_conformance_sites
           << ",\"namespace_segment_sites\":"
           << cross_module_conformance_lowering_contract.namespace_segment_sites
           << ",\"import_edge_candidate_sites\":"
           << cross_module_conformance_lowering_contract.import_edge_candidate_sites
           << ",\"object_pointer_type_sites\":"
           << cross_module_conformance_lowering_contract.object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << cross_module_conformance_lowering_contract.pointer_declarator_sites
           << ",\"normalized_sites\":"
           << cross_module_conformance_lowering_contract.normalized_sites
           << ",\"cache_invalidation_candidate_sites\":"
           << cross_module_conformance_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"contract_violation_sites\":"
           << cross_module_conformance_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << cross_module_conformance_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (cross_module_conformance_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_throws_propagation_lowering_surface\":{\"throws_propagation_sites\":"
           << throws_propagation_lowering_contract.throws_propagation_sites
           << ",\"namespace_segment_sites\":"
           << throws_propagation_lowering_contract.namespace_segment_sites
           << ",\"import_edge_candidate_sites\":"
           << throws_propagation_lowering_contract.import_edge_candidate_sites
           << ",\"object_pointer_type_sites\":"
           << throws_propagation_lowering_contract.object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << throws_propagation_lowering_contract.pointer_declarator_sites
           << ",\"normalized_sites\":"
           << throws_propagation_lowering_contract.normalized_sites
           << ",\"cache_invalidation_candidate_sites\":"
           << throws_propagation_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"contract_violation_sites\":"
           << throws_propagation_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << throws_propagation_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (throws_propagation_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_object_pointer_nullability_generics_surface\":{\"object_pointer_type_spellings\":"
           << object_pointer_nullability_generics_summary.object_pointer_type_spellings
           << ",\"pointer_declarator_entries\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_entries
           << ",\"pointer_declarator_depth_total\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_depth_total
           << ",\"pointer_declarator_token_entries\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_token_entries
           << ",\"nullability_suffix_entries\":"
           << object_pointer_nullability_generics_summary.nullability_suffix_entries
           << ",\"generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.generic_suffix_entries
           << ",\"terminated_generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.terminated_generic_suffix_entries
           << ",\"unterminated_generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.unterminated_generic_suffix_entries
           << ",\"deterministic_handoff\":"
           << (object_pointer_nullability_generics_summary.deterministic_object_pointer_nullability_generics_handoff
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_symbol_graph_scope_resolution_surface\":{\"global_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.global_symbol_nodes
           << ",\"function_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.function_symbol_nodes
           << ",\"interface_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_symbol_nodes
           << ",\"implementation_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_symbol_nodes
           << ",\"interface_property_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_property_symbol_nodes
           << ",\"implementation_property_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes
           << ",\"interface_method_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_method_symbol_nodes
           << ",\"implementation_method_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes
           << ",\"top_level_scope_symbols\":"
           << symbol_graph_scope_resolution_summary.top_level_scope_symbols
           << ",\"nested_scope_symbols\":"
           << symbol_graph_scope_resolution_summary.nested_scope_symbols
           << ",\"scope_frames_total\":"
           << symbol_graph_scope_resolution_summary.scope_frames_total
           << ",\"implementation_interface_resolution_sites\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites
           << ",\"implementation_interface_resolution_hits\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits
           << ",\"implementation_interface_resolution_misses\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses
           << ",\"method_resolution_sites\":"
           << symbol_graph_scope_resolution_summary.method_resolution_sites
           << ",\"method_resolution_hits\":"
           << symbol_graph_scope_resolution_summary.method_resolution_hits
           << ",\"method_resolution_misses\":"
           << symbol_graph_scope_resolution_summary.method_resolution_misses
           << ",\"deterministic_symbol_graph_handoff\":"
           << (symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff ? "true" : "false")
           << ",\"deterministic_scope_resolution_handoff\":"
           << (symbol_graph_scope_resolution_summary.deterministic_scope_resolution_handoff ? "true" : "false")
           << ",\"deterministic_handoff_key\":\""
           << symbol_graph_scope_resolution_summary.deterministic_handoff_key
           << "\"}"
           << ",\"function_signature_surface\":{\"scalar_return_i32\":" << scalar_return_i32
           << ",\"scalar_return_bool\":" << scalar_return_bool
           << ",\"scalar_return_void\":" << scalar_return_void << ",\"scalar_param_i32\":" << scalar_param_i32
           << ",\"scalar_param_bool\":" << scalar_param_bool << "}}\n";
  manifest << "    }\n";
  manifest << "  },\n";
  manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol
           << "\",\"runtime_dispatch_arg_slots\":" << options.lowering.max_message_send_args
           << ",\"selector_global_ordering\":\"lexicographic\"},\n";
  manifest << "  \"lowering_vector_abi\":{\"replay_key\":\"" << Objc3SimdVectorTypeLoweringReplayKey()
           << "\",\"lane_contract\":\"" << kObjc3SimdVectorLaneContract
           << "\",\"vector_signature_functions\":" << vector_signature_functions << "},\n";
  manifest << "  \"lowering_property_synthesis_ivar_binding\":{\"replay_key\":\""
           << property_synthesis_ivar_binding_replay_key
           << "\",\"lane_contract\":\"" << kObjc3PropertySynthesisIvarBindingLaneContract
           << "\",\"deterministic_handoff\":"
           << (property_synthesis_ivar_binding_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_id_class_sel_object_pointer_typecheck\":{\"replay_key\":\""
           << id_class_sel_object_pointer_typecheck_replay_key
           << "\",\"lane_contract\":\"" << kObjc3IdClassSelObjectPointerTypecheckLaneContract
           << "\",\"deterministic_handoff\":"
           << (id_class_sel_object_pointer_typecheck_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_message_send_selector_lowering\":{\"replay_key\":\""
           << message_send_selector_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3MessageSendSelectorLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (message_send_selector_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_dispatch_abi_marshalling\":{\"replay_key\":\""
           << dispatch_abi_marshalling_replay_key
           << "\",\"lane_contract\":\"" << kObjc3DispatchAbiMarshallingLaneContract
           << "\",\"deterministic_handoff\":"
           << (dispatch_abi_marshalling_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_nil_receiver_semantics_foldability\":{\"replay_key\":\""
           << nil_receiver_semantics_foldability_replay_key
           << "\",\"lane_contract\":\"" << kObjc3NilReceiverSemanticsFoldabilityLaneContract
           << "\",\"deterministic_handoff\":"
           << (nil_receiver_semantics_foldability_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_super_dispatch_method_family\":{\"replay_key\":\""
           << super_dispatch_method_family_replay_key
           << "\",\"lane_contract\":\"" << kObjc3SuperDispatchMethodFamilyLaneContract
           << "\",\"deterministic_handoff\":"
           << (super_dispatch_method_family_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_runtime_shim_host_link\":{\"replay_key\":\""
           << runtime_shim_host_link_replay_key
           << "\",\"lane_contract\":\"" << kObjc3RuntimeShimHostLinkLaneContract
           << "\",\"deterministic_handoff\":"
           << (runtime_shim_host_link_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_ownership_qualifier\":{\"replay_key\":\""
           << ownership_qualifier_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3OwnershipQualifierLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (ownership_qualifier_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_retain_release_operation\":{\"replay_key\":\""
           << retain_release_operation_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3RetainReleaseOperationLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (retain_release_operation_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_autoreleasepool_scope\":{\"replay_key\":\""
           << autoreleasepool_scope_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3AutoreleasePoolScopeLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (autoreleasepool_scope_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_weak_unowned_semantics\":{\"replay_key\":\""
           << weak_unowned_semantics_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3WeakUnownedSemanticsLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (weak_unowned_semantics_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_arc_diagnostics_fixit\":{\"replay_key\":\""
           << arc_diagnostics_fixit_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3ArcDiagnosticsFixitLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (arc_diagnostics_fixit_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_block_literal_capture\":{\"replay_key\":\""
           << block_literal_capture_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3BlockLiteralCaptureLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (block_literal_capture_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_block_abi_invoke_trampoline\":{\"replay_key\":\""
           << block_abi_invoke_trampoline_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3BlockAbiInvokeTrampolineLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (block_abi_invoke_trampoline_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_block_storage_escape\":{\"replay_key\":\""
           << block_storage_escape_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3BlockStorageEscapeLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (block_storage_escape_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_block_copy_dispose\":{\"replay_key\":\""
           << block_copy_dispose_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3BlockCopyDisposeLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (block_copy_dispose_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_block_determinism_perf_baseline\":{\"replay_key\":\""
           << block_determinism_perf_baseline_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3BlockDeterminismPerfBaselineLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (block_determinism_perf_baseline_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_lightweight_generic_constraint\":{\"replay_key\":\""
           << lightweight_generic_constraint_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3LightweightGenericsConstraintLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (lightweight_generic_constraint_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_nullability_flow_warning_precision\":{\"replay_key\":\""
           << nullability_flow_warning_precision_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3NullabilityFlowWarningPrecisionLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (nullability_flow_warning_precision_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_protocol_qualified_object_type\":{\"replay_key\":\""
           << protocol_qualified_object_type_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3ProtocolQualifiedObjectTypeLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (protocol_qualified_object_type_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_variance_bridge_cast\":{\"replay_key\":\""
           << variance_bridge_cast_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3VarianceBridgeCastLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (variance_bridge_cast_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_generic_metadata_abi\":{\"replay_key\":\""
           << generic_metadata_abi_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3GenericMetadataAbiLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (generic_metadata_abi_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_module_import_graph\":{\"replay_key\":\""
           << module_import_graph_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3ModuleImportGraphLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (module_import_graph_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_namespace_collision_shadowing\":{\"replay_key\":\""
           << namespace_collision_shadowing_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3NamespaceCollisionShadowingLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (namespace_collision_shadowing_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"lowering_public_private_api_partition\":{\"replay_key\":\""
           << public_private_api_partition_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3PublicPrivateApiPartitionLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (public_private_api_partition_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"lowering_incremental_module_cache_invalidation\":{\"replay_key\":\""
           << incremental_module_cache_invalidation_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3IncrementalModuleCacheInvalidationLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (incremental_module_cache_invalidation_lowering_contract
                       .deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"lowering_cross_module_conformance\":{\"replay_key\":\""
           << cross_module_conformance_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3CrossModuleConformanceLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (cross_module_conformance_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"lowering_throws_propagation\":{\"replay_key\":\""
           << throws_propagation_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3ThrowsPropagationLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (throws_propagation_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"globals\": [\n";
  for (std::size_t i = 0; i < program.globals.size(); ++i) {
    manifest << "    {\"name\":\"" << program.globals[i].name << "\",\"value\":" << resolved_global_values[i]
             << ",\"line\":" << program.globals[i].line << ",\"column\":" << program.globals[i].column << "}";
    if (i + 1 != program.globals.size()) {
      manifest << ",";
    }
    manifest << "\n";
  }
  manifest << "  ],\n";
  manifest << "  \"functions\": [\n";
  for (std::size_t i = 0; i < manifest_functions.size(); ++i) {
    const auto &fn = *manifest_functions[i];
    manifest << "    {\"name\":\"" << fn.name << "\",\"params\":" << fn.params.size() << ",\"param_types\":[";
    for (std::size_t p = 0; p < fn.params.size(); ++p) {
      manifest << "\"" << TypeName(fn.params[p].type) << "\"";
      if (p + 1 != fn.params.size()) {
        manifest << ",";
      }
    }
    manifest << "]"
             << ",\"return\":\"" << TypeName(fn.return_type) << "\""
             << ",\"line\":" << fn.line << ",\"column\":" << fn.column << "}";
    if (i + 1 != manifest_functions.size()) {
      manifest << ",";
    }
    manifest << "\n";
  }
  manifest << "  ],\n";
  manifest << "  \"interfaces\": [\n";
  for (std::size_t i = 0; i < type_metadata_handoff.interfaces_lexicographic.size(); ++i) {
    const auto &interface_metadata = type_metadata_handoff.interfaces_lexicographic[i];
    manifest << "    {\"name\":\"" << interface_metadata.name << "\",\"super\":\"" << interface_metadata.super_name
             << "\",\"method_count\":" << interface_metadata.methods_lexicographic.size() << ",\"selectors\":[";
    for (std::size_t s = 0; s < interface_metadata.methods_lexicographic.size(); ++s) {
      const auto &method_metadata = interface_metadata.methods_lexicographic[s];
      manifest << "\"" << method_metadata.selector << "\"";
      if (s + 1 != interface_metadata.methods_lexicographic.size()) {
        manifest << ",";
      }
    }
    manifest << "]}";
    if (i + 1 != type_metadata_handoff.interfaces_lexicographic.size()) {
      manifest << ",";
    }
    manifest << "\n";
  }
  manifest << "  ],\n";
  manifest << "  \"implementations\": [\n";
  for (std::size_t i = 0; i < type_metadata_handoff.implementations_lexicographic.size(); ++i) {
    const auto &implementation_metadata = type_metadata_handoff.implementations_lexicographic[i];
    manifest << "    {\"name\":\"" << implementation_metadata.name << "\",\"has_matching_interface\":"
             << (implementation_metadata.has_matching_interface ? "true" : "false")
             << ",\"method_count\":" << implementation_metadata.methods_lexicographic.size()
             << ",\"selectors\":[";
    for (std::size_t s = 0; s < implementation_metadata.methods_lexicographic.size(); ++s) {
      const auto &method_metadata = implementation_metadata.methods_lexicographic[s];
      manifest << "{\"selector\":\"" << method_metadata.selector << "\",\"is_class_method\":"
               << (method_metadata.is_class_method ? "true" : "false")
               << ",\"has_body\":" << (method_metadata.has_definition ? "true" : "false") << "}";
      if (s + 1 != implementation_metadata.methods_lexicographic.size()) {
        manifest << ",";
      }
    }
    manifest << "]}";
    if (i + 1 != type_metadata_handoff.implementations_lexicographic.size()) {
      manifest << ",";
    }
    manifest << "\n";
  }
  manifest << "  ],\n";
  manifest << "  \"protocols\": [\n";
  manifest << "  ],\n";
  manifest << "  \"categories\": [\n";
  manifest << "  ]\n";
  manifest << "}\n";
  bundle.manifest_json = manifest.str();

  Objc3IRFrontendMetadata ir_frontend_metadata;
  ir_frontend_metadata.language_version = options.language_version;
  ir_frontend_metadata.compatibility_mode = CompatibilityModeName(options.compatibility_mode);
  ir_frontend_metadata.migration_assist = options.migration_assist;
  ir_frontend_metadata.migration_legacy_yes = pipeline_result.migration_hints.legacy_yes_count;
  ir_frontend_metadata.migration_legacy_no = pipeline_result.migration_hints.legacy_no_count;
  ir_frontend_metadata.migration_legacy_null = pipeline_result.migration_hints.legacy_null_count;
  ir_frontend_metadata.declared_interfaces = interface_implementation_summary.declared_interfaces;
  ir_frontend_metadata.declared_implementations = interface_implementation_summary.declared_implementations;
  ir_frontend_metadata.resolved_interface_symbols = interface_implementation_summary.resolved_interfaces;
  ir_frontend_metadata.resolved_implementation_symbols = interface_implementation_summary.resolved_implementations;
  ir_frontend_metadata.interface_method_symbols = interface_implementation_summary.interface_method_symbols;
  ir_frontend_metadata.implementation_method_symbols = interface_implementation_summary.implementation_method_symbols;
  ir_frontend_metadata.linked_implementation_symbols = interface_implementation_summary.linked_implementation_symbols;
  ir_frontend_metadata.declared_protocols = protocol_category_summary.declared_protocols;
  ir_frontend_metadata.declared_categories = protocol_category_summary.declared_categories;
  ir_frontend_metadata.resolved_protocol_symbols = protocol_category_summary.resolved_protocol_symbols;
  ir_frontend_metadata.resolved_category_symbols = protocol_category_summary.resolved_category_symbols;
  ir_frontend_metadata.protocol_method_symbols = protocol_category_summary.protocol_method_symbols;
  ir_frontend_metadata.category_method_symbols = protocol_category_summary.category_method_symbols;
  ir_frontend_metadata.linked_category_symbols = protocol_category_summary.linked_category_symbols;
  ir_frontend_metadata.declared_class_interfaces = class_protocol_category_linking_summary.declared_class_interfaces;
  ir_frontend_metadata.declared_class_implementations =
      class_protocol_category_linking_summary.declared_class_implementations;
  ir_frontend_metadata.resolved_class_interfaces = class_protocol_category_linking_summary.resolved_class_interfaces;
  ir_frontend_metadata.resolved_class_implementations =
      class_protocol_category_linking_summary.resolved_class_implementations;
  ir_frontend_metadata.linked_class_method_symbols =
      class_protocol_category_linking_summary.linked_class_method_symbols;
  ir_frontend_metadata.linked_category_method_symbols =
      class_protocol_category_linking_summary.linked_category_method_symbols;
  ir_frontend_metadata.protocol_composition_sites =
      class_protocol_category_linking_summary.protocol_composition_sites;
  ir_frontend_metadata.protocol_composition_symbols =
      class_protocol_category_linking_summary.protocol_composition_symbols;
  ir_frontend_metadata.category_composition_sites =
      class_protocol_category_linking_summary.category_composition_sites;
  ir_frontend_metadata.category_composition_symbols =
      class_protocol_category_linking_summary.category_composition_symbols;
  ir_frontend_metadata.invalid_protocol_composition_sites =
      class_protocol_category_linking_summary.invalid_protocol_composition_sites;
  ir_frontend_metadata.selector_method_declaration_entries = selector_normalization_summary.method_declaration_entries;
  ir_frontend_metadata.selector_normalized_method_declarations =
      selector_normalization_summary.normalized_method_declarations;
  ir_frontend_metadata.selector_piece_entries = selector_normalization_summary.selector_piece_entries;
  ir_frontend_metadata.selector_piece_parameter_links = selector_normalization_summary.selector_piece_parameter_links;
  ir_frontend_metadata.property_declaration_entries = property_attribute_summary.property_declaration_entries;
  ir_frontend_metadata.property_attribute_entries = property_attribute_summary.property_attribute_entries;
  ir_frontend_metadata.property_attribute_value_entries = property_attribute_summary.property_attribute_value_entries;
  ir_frontend_metadata.property_accessor_modifier_entries = property_attribute_summary.property_accessor_modifier_entries;
  ir_frontend_metadata.property_getter_selector_entries = property_attribute_summary.property_getter_selector_entries;
  ir_frontend_metadata.property_setter_selector_entries = property_attribute_summary.property_setter_selector_entries;
  ir_frontend_metadata.lowering_property_synthesis_ivar_binding_replay_key =
      property_synthesis_ivar_binding_replay_key;
  ir_frontend_metadata.lowering_id_class_sel_object_pointer_typecheck_replay_key =
      id_class_sel_object_pointer_typecheck_replay_key;
  ir_frontend_metadata.id_typecheck_sites = id_class_sel_object_pointer_typecheck_contract.id_typecheck_sites;
  ir_frontend_metadata.class_typecheck_sites = id_class_sel_object_pointer_typecheck_contract.class_typecheck_sites;
  ir_frontend_metadata.sel_typecheck_sites = id_class_sel_object_pointer_typecheck_contract.sel_typecheck_sites;
  ir_frontend_metadata.object_pointer_typecheck_sites =
      id_class_sel_object_pointer_typecheck_contract.object_pointer_typecheck_sites;
  ir_frontend_metadata.id_class_sel_object_pointer_typecheck_sites_total =
      id_class_sel_object_pointer_typecheck_contract.total_typecheck_sites;
  ir_frontend_metadata.lowering_message_send_selector_lowering_replay_key =
      message_send_selector_lowering_replay_key;
  ir_frontend_metadata.message_send_selector_lowering_sites =
      message_send_selector_lowering_contract.message_send_sites;
  ir_frontend_metadata.message_send_selector_lowering_unary_sites =
      message_send_selector_lowering_contract.unary_selector_sites;
  ir_frontend_metadata.message_send_selector_lowering_keyword_sites =
      message_send_selector_lowering_contract.keyword_selector_sites;
  ir_frontend_metadata.message_send_selector_lowering_selector_piece_sites =
      message_send_selector_lowering_contract.selector_piece_sites;
  ir_frontend_metadata.message_send_selector_lowering_argument_expression_sites =
      message_send_selector_lowering_contract.argument_expression_sites;
  ir_frontend_metadata.message_send_selector_lowering_receiver_sites =
      message_send_selector_lowering_contract.receiver_expression_sites;
  ir_frontend_metadata.message_send_selector_lowering_selector_literal_entries =
      message_send_selector_lowering_contract.selector_literal_entries;
  ir_frontend_metadata.message_send_selector_lowering_selector_literal_characters =
      message_send_selector_lowering_contract.selector_literal_characters;
  ir_frontend_metadata.lowering_dispatch_abi_marshalling_replay_key = dispatch_abi_marshalling_replay_key;
  ir_frontend_metadata.dispatch_abi_marshalling_message_send_sites =
      dispatch_abi_marshalling_contract.message_send_sites;
  ir_frontend_metadata.dispatch_abi_marshalling_receiver_slots_marshaled =
      dispatch_abi_marshalling_contract.receiver_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_selector_slots_marshaled =
      dispatch_abi_marshalling_contract.selector_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_argument_value_slots_marshaled =
      dispatch_abi_marshalling_contract.argument_value_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_argument_padding_slots_marshaled =
      dispatch_abi_marshalling_contract.argument_padding_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_argument_total_slots_marshaled =
      dispatch_abi_marshalling_contract.argument_total_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_total_marshaled_slots =
      dispatch_abi_marshalling_contract.total_marshaled_slots;
  ir_frontend_metadata.dispatch_abi_marshalling_runtime_dispatch_arg_slots =
      dispatch_abi_marshalling_contract.runtime_dispatch_arg_slots;
  ir_frontend_metadata.lowering_nil_receiver_semantics_foldability_replay_key =
      nil_receiver_semantics_foldability_replay_key;
  ir_frontend_metadata.nil_receiver_semantics_foldability_message_send_sites =
      nil_receiver_semantics_foldability_contract.message_send_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_receiver_nil_literal_sites =
      nil_receiver_semantics_foldability_contract.receiver_nil_literal_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_enabled_sites =
      nil_receiver_semantics_foldability_contract.nil_receiver_semantics_enabled_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_foldable_sites =
      nil_receiver_semantics_foldability_contract.nil_receiver_foldable_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_runtime_dispatch_required_sites =
      nil_receiver_semantics_foldability_contract.nil_receiver_runtime_dispatch_required_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_non_nil_receiver_sites =
      nil_receiver_semantics_foldability_contract.non_nil_receiver_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_contract_violation_sites =
      nil_receiver_semantics_foldability_contract.contract_violation_sites;
  ir_frontend_metadata.lowering_super_dispatch_method_family_replay_key =
      super_dispatch_method_family_replay_key;
  ir_frontend_metadata.super_dispatch_method_family_message_send_sites =
      super_dispatch_method_family_contract.message_send_sites;
  ir_frontend_metadata.super_dispatch_method_family_receiver_super_identifier_sites =
      super_dispatch_method_family_contract.receiver_super_identifier_sites;
  ir_frontend_metadata.super_dispatch_method_family_enabled_sites =
      super_dispatch_method_family_contract.super_dispatch_enabled_sites;
  ir_frontend_metadata.super_dispatch_method_family_requires_class_context_sites =
      super_dispatch_method_family_contract.super_dispatch_requires_class_context_sites;
  ir_frontend_metadata.super_dispatch_method_family_init_sites =
      super_dispatch_method_family_contract.method_family_init_sites;
  ir_frontend_metadata.super_dispatch_method_family_copy_sites =
      super_dispatch_method_family_contract.method_family_copy_sites;
  ir_frontend_metadata.super_dispatch_method_family_mutable_copy_sites =
      super_dispatch_method_family_contract.method_family_mutable_copy_sites;
  ir_frontend_metadata.super_dispatch_method_family_new_sites =
      super_dispatch_method_family_contract.method_family_new_sites;
  ir_frontend_metadata.super_dispatch_method_family_none_sites =
      super_dispatch_method_family_contract.method_family_none_sites;
  ir_frontend_metadata.super_dispatch_method_family_returns_retained_result_sites =
      super_dispatch_method_family_contract.method_family_returns_retained_result_sites;
  ir_frontend_metadata.super_dispatch_method_family_returns_related_result_sites =
      super_dispatch_method_family_contract.method_family_returns_related_result_sites;
  ir_frontend_metadata.super_dispatch_method_family_contract_violation_sites =
      super_dispatch_method_family_contract.contract_violation_sites;
  ir_frontend_metadata.lowering_runtime_shim_host_link_replay_key = runtime_shim_host_link_replay_key;
  ir_frontend_metadata.runtime_shim_host_link_message_send_sites =
      runtime_shim_host_link_contract.message_send_sites;
  ir_frontend_metadata.runtime_shim_host_link_required_sites =
      runtime_shim_host_link_contract.runtime_shim_required_sites;
  ir_frontend_metadata.runtime_shim_host_link_elided_sites =
      runtime_shim_host_link_contract.runtime_shim_elided_sites;
  ir_frontend_metadata.runtime_shim_host_link_runtime_dispatch_arg_slots =
      runtime_shim_host_link_contract.runtime_dispatch_arg_slots;
  ir_frontend_metadata.runtime_shim_host_link_runtime_dispatch_declaration_parameter_count =
      runtime_shim_host_link_contract.runtime_dispatch_declaration_parameter_count;
  ir_frontend_metadata.runtime_shim_host_link_contract_violation_sites =
      runtime_shim_host_link_contract.contract_violation_sites;
  ir_frontend_metadata.runtime_shim_host_link_runtime_dispatch_symbol =
      runtime_shim_host_link_contract.runtime_dispatch_symbol;
  ir_frontend_metadata.runtime_shim_host_link_default_runtime_dispatch_symbol_binding =
      runtime_shim_host_link_contract.default_runtime_dispatch_symbol_binding;
  ir_frontend_metadata.lowering_ownership_qualifier_replay_key =
      ownership_qualifier_lowering_replay_key;
  ir_frontend_metadata.ownership_qualifier_lowering_ownership_qualifier_sites =
      ownership_qualifier_lowering_contract.ownership_qualifier_sites;
  ir_frontend_metadata.ownership_qualifier_lowering_invalid_ownership_qualifier_sites =
      ownership_qualifier_lowering_contract.invalid_ownership_qualifier_sites;
  ir_frontend_metadata.ownership_qualifier_lowering_object_pointer_type_annotation_sites =
      ownership_qualifier_lowering_contract.object_pointer_type_annotation_sites;
  ir_frontend_metadata.lowering_retain_release_operation_replay_key =
      retain_release_operation_lowering_replay_key;
  ir_frontend_metadata.retain_release_operation_lowering_ownership_qualified_sites =
      retain_release_operation_lowering_contract.ownership_qualified_sites;
  ir_frontend_metadata.retain_release_operation_lowering_retain_insertion_sites =
      retain_release_operation_lowering_contract.retain_insertion_sites;
  ir_frontend_metadata.retain_release_operation_lowering_release_insertion_sites =
      retain_release_operation_lowering_contract.release_insertion_sites;
  ir_frontend_metadata.retain_release_operation_lowering_autorelease_insertion_sites =
      retain_release_operation_lowering_contract.autorelease_insertion_sites;
  ir_frontend_metadata.retain_release_operation_lowering_contract_violation_sites =
      retain_release_operation_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.lowering_autoreleasepool_scope_replay_key =
      autoreleasepool_scope_lowering_replay_key;
  ir_frontend_metadata.autoreleasepool_scope_lowering_scope_sites =
      autoreleasepool_scope_lowering_contract.scope_sites;
  ir_frontend_metadata.autoreleasepool_scope_lowering_scope_symbolized_sites =
      autoreleasepool_scope_lowering_contract.scope_symbolized_sites;
  ir_frontend_metadata.autoreleasepool_scope_lowering_max_scope_depth =
      autoreleasepool_scope_lowering_contract.max_scope_depth;
  ir_frontend_metadata.autoreleasepool_scope_lowering_scope_entry_transition_sites =
      autoreleasepool_scope_lowering_contract.scope_entry_transition_sites;
  ir_frontend_metadata.autoreleasepool_scope_lowering_scope_exit_transition_sites =
      autoreleasepool_scope_lowering_contract.scope_exit_transition_sites;
  ir_frontend_metadata.autoreleasepool_scope_lowering_contract_violation_sites =
      autoreleasepool_scope_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.lowering_weak_unowned_semantics_replay_key =
      weak_unowned_semantics_lowering_replay_key;
  ir_frontend_metadata.weak_unowned_semantics_lowering_ownership_candidate_sites =
      weak_unowned_semantics_lowering_contract.ownership_candidate_sites;
  ir_frontend_metadata.weak_unowned_semantics_lowering_weak_reference_sites =
      weak_unowned_semantics_lowering_contract.weak_reference_sites;
  ir_frontend_metadata.weak_unowned_semantics_lowering_unowned_reference_sites =
      weak_unowned_semantics_lowering_contract.unowned_reference_sites;
  ir_frontend_metadata.weak_unowned_semantics_lowering_unowned_safe_reference_sites =
      weak_unowned_semantics_lowering_contract.unowned_safe_reference_sites;
  ir_frontend_metadata.weak_unowned_semantics_lowering_conflict_sites =
      weak_unowned_semantics_lowering_contract.weak_unowned_conflict_sites;
  ir_frontend_metadata.weak_unowned_semantics_lowering_contract_violation_sites =
      weak_unowned_semantics_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_weak_unowned_semantics_lowering_handoff =
      weak_unowned_semantics_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_arc_diagnostics_fixit_replay_key =
      arc_diagnostics_fixit_lowering_replay_key;
  ir_frontend_metadata.arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites =
      arc_diagnostics_fixit_lowering_contract.ownership_arc_diagnostic_candidate_sites;
  ir_frontend_metadata.arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites =
      arc_diagnostics_fixit_lowering_contract.ownership_arc_fixit_available_sites;
  ir_frontend_metadata.arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites =
      arc_diagnostics_fixit_lowering_contract.ownership_arc_profiled_sites;
  ir_frontend_metadata
      .arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites =
      arc_diagnostics_fixit_lowering_contract.ownership_arc_weak_unowned_conflict_diagnostic_sites;
  ir_frontend_metadata.arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites =
      arc_diagnostics_fixit_lowering_contract.ownership_arc_empty_fixit_hint_sites;
  ir_frontend_metadata.arc_diagnostics_fixit_lowering_contract_violation_sites =
      arc_diagnostics_fixit_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_arc_diagnostics_fixit_lowering_handoff =
      arc_diagnostics_fixit_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_block_literal_capture_replay_key =
      block_literal_capture_lowering_replay_key;
  ir_frontend_metadata.block_literal_capture_lowering_block_literal_sites =
      block_literal_capture_lowering_contract.block_literal_sites;
  ir_frontend_metadata.block_literal_capture_lowering_block_parameter_entries =
      block_literal_capture_lowering_contract.block_parameter_entries;
  ir_frontend_metadata.block_literal_capture_lowering_block_capture_entries =
      block_literal_capture_lowering_contract.block_capture_entries;
  ir_frontend_metadata.block_literal_capture_lowering_block_body_statement_entries =
      block_literal_capture_lowering_contract.block_body_statement_entries;
  ir_frontend_metadata.block_literal_capture_lowering_block_empty_capture_sites =
      block_literal_capture_lowering_contract.block_empty_capture_sites;
  ir_frontend_metadata.block_literal_capture_lowering_block_nondeterministic_capture_sites =
      block_literal_capture_lowering_contract.block_nondeterministic_capture_sites;
  ir_frontend_metadata.block_literal_capture_lowering_block_non_normalized_sites =
      block_literal_capture_lowering_contract.block_non_normalized_sites;
  ir_frontend_metadata.block_literal_capture_lowering_contract_violation_sites =
      block_literal_capture_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_literal_capture_lowering_handoff =
      block_literal_capture_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_block_abi_invoke_trampoline_replay_key =
      block_abi_invoke_trampoline_lowering_replay_key;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_block_literal_sites =
      block_abi_invoke_trampoline_lowering_contract.block_literal_sites;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_invoke_argument_slots_total =
      block_abi_invoke_trampoline_lowering_contract.invoke_argument_slots_total;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_capture_word_count_total =
      block_abi_invoke_trampoline_lowering_contract.capture_word_count_total;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_parameter_entries_total =
      block_abi_invoke_trampoline_lowering_contract.parameter_entries_total;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_capture_entries_total =
      block_abi_invoke_trampoline_lowering_contract.capture_entries_total;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_body_statement_entries_total =
      block_abi_invoke_trampoline_lowering_contract.body_statement_entries_total;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites =
      block_abi_invoke_trampoline_lowering_contract.descriptor_symbolized_sites;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_invoke_symbolized_sites =
      block_abi_invoke_trampoline_lowering_contract.invoke_trampoline_symbolized_sites;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_missing_invoke_sites =
      block_abi_invoke_trampoline_lowering_contract.missing_invoke_trampoline_sites;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_non_normalized_layout_sites =
      block_abi_invoke_trampoline_lowering_contract.non_normalized_layout_sites;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_contract_violation_sites =
      block_abi_invoke_trampoline_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_abi_invoke_trampoline_lowering_handoff =
      block_abi_invoke_trampoline_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_block_storage_escape_replay_key =
      block_storage_escape_lowering_replay_key;
  ir_frontend_metadata.block_storage_escape_lowering_block_literal_sites =
      block_storage_escape_lowering_contract.block_literal_sites;
  ir_frontend_metadata.block_storage_escape_lowering_mutable_capture_count_total =
      block_storage_escape_lowering_contract.mutable_capture_count_total;
  ir_frontend_metadata.block_storage_escape_lowering_byref_slot_count_total =
      block_storage_escape_lowering_contract.byref_slot_count_total;
  ir_frontend_metadata.block_storage_escape_lowering_parameter_entries_total =
      block_storage_escape_lowering_contract.parameter_entries_total;
  ir_frontend_metadata.block_storage_escape_lowering_capture_entries_total =
      block_storage_escape_lowering_contract.capture_entries_total;
  ir_frontend_metadata.block_storage_escape_lowering_body_statement_entries_total =
      block_storage_escape_lowering_contract.body_statement_entries_total;
  ir_frontend_metadata.block_storage_escape_lowering_requires_byref_cells_sites =
      block_storage_escape_lowering_contract.requires_byref_cells_sites;
  ir_frontend_metadata.block_storage_escape_lowering_escape_analysis_enabled_sites =
      block_storage_escape_lowering_contract.escape_analysis_enabled_sites;
  ir_frontend_metadata.block_storage_escape_lowering_escape_to_heap_sites =
      block_storage_escape_lowering_contract.escape_to_heap_sites;
  ir_frontend_metadata.block_storage_escape_lowering_escape_profile_normalized_sites =
      block_storage_escape_lowering_contract.escape_profile_normalized_sites;
  ir_frontend_metadata.block_storage_escape_lowering_byref_layout_symbolized_sites =
      block_storage_escape_lowering_contract.byref_layout_symbolized_sites;
  ir_frontend_metadata.block_storage_escape_lowering_contract_violation_sites =
      block_storage_escape_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_storage_escape_lowering_handoff =
      block_storage_escape_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_block_copy_dispose_replay_key =
      block_copy_dispose_lowering_replay_key;
  ir_frontend_metadata.block_copy_dispose_lowering_block_literal_sites =
      block_copy_dispose_lowering_contract.block_literal_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_mutable_capture_count_total =
      block_copy_dispose_lowering_contract.mutable_capture_count_total;
  ir_frontend_metadata.block_copy_dispose_lowering_byref_slot_count_total =
      block_copy_dispose_lowering_contract.byref_slot_count_total;
  ir_frontend_metadata.block_copy_dispose_lowering_parameter_entries_total =
      block_copy_dispose_lowering_contract.parameter_entries_total;
  ir_frontend_metadata.block_copy_dispose_lowering_capture_entries_total =
      block_copy_dispose_lowering_contract.capture_entries_total;
  ir_frontend_metadata.block_copy_dispose_lowering_body_statement_entries_total =
      block_copy_dispose_lowering_contract.body_statement_entries_total;
  ir_frontend_metadata.block_copy_dispose_lowering_copy_helper_required_sites =
      block_copy_dispose_lowering_contract.copy_helper_required_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_dispose_helper_required_sites =
      block_copy_dispose_lowering_contract.dispose_helper_required_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_profile_normalized_sites =
      block_copy_dispose_lowering_contract.profile_normalized_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_copy_helper_symbolized_sites =
      block_copy_dispose_lowering_contract.copy_helper_symbolized_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_dispose_helper_symbolized_sites =
      block_copy_dispose_lowering_contract.dispose_helper_symbolized_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_contract_violation_sites =
      block_copy_dispose_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_copy_dispose_lowering_handoff =
      block_copy_dispose_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_block_determinism_perf_baseline_replay_key =
      block_determinism_perf_baseline_lowering_replay_key;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_block_literal_sites =
      block_determinism_perf_baseline_lowering_contract.block_literal_sites;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_baseline_weight_total =
      block_determinism_perf_baseline_lowering_contract.baseline_weight_total;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_parameter_entries_total =
      block_determinism_perf_baseline_lowering_contract.parameter_entries_total;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_capture_entries_total =
      block_determinism_perf_baseline_lowering_contract.capture_entries_total;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_body_statement_entries_total =
      block_determinism_perf_baseline_lowering_contract.body_statement_entries_total;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_deterministic_capture_sites =
      block_determinism_perf_baseline_lowering_contract.deterministic_capture_sites;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_heavy_tier_sites =
      block_determinism_perf_baseline_lowering_contract.heavy_tier_sites;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_normalized_profile_sites =
      block_determinism_perf_baseline_lowering_contract.normalized_profile_sites;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_contract_violation_sites =
      block_determinism_perf_baseline_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_determinism_perf_baseline_lowering_handoff =
      block_determinism_perf_baseline_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_lightweight_generic_constraint_replay_key =
      lightweight_generic_constraint_lowering_replay_key;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_generic_constraint_sites =
      lightweight_generic_constraint_lowering_contract.generic_constraint_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_generic_suffix_sites =
      lightweight_generic_constraint_lowering_contract.generic_suffix_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_object_pointer_type_sites =
      lightweight_generic_constraint_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_terminated_generic_suffix_sites =
      lightweight_generic_constraint_lowering_contract.terminated_generic_suffix_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_pointer_declarator_sites =
      lightweight_generic_constraint_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_normalized_constraint_sites =
      lightweight_generic_constraint_lowering_contract.normalized_constraint_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_contract_violation_sites =
      lightweight_generic_constraint_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_lightweight_generic_constraint_lowering_handoff =
      lightweight_generic_constraint_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_nullability_flow_warning_precision_replay_key =
      nullability_flow_warning_precision_lowering_replay_key;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_sites =
      nullability_flow_warning_precision_lowering_contract.nullability_flow_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_object_pointer_type_sites =
      nullability_flow_warning_precision_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_nullability_suffix_sites =
      nullability_flow_warning_precision_lowering_contract.nullability_suffix_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_nullable_suffix_sites =
      nullability_flow_warning_precision_lowering_contract.nullable_suffix_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_nonnull_suffix_sites =
      nullability_flow_warning_precision_lowering_contract.nonnull_suffix_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_normalized_sites =
      nullability_flow_warning_precision_lowering_contract.normalized_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_contract_violation_sites =
      nullability_flow_warning_precision_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_nullability_flow_warning_precision_lowering_handoff =
      nullability_flow_warning_precision_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_protocol_qualified_object_type_replay_key =
      protocol_qualified_object_type_lowering_replay_key;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_sites =
      protocol_qualified_object_type_lowering_contract.protocol_qualified_object_type_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_protocol_composition_sites =
      protocol_qualified_object_type_lowering_contract.protocol_composition_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_object_pointer_type_sites =
      protocol_qualified_object_type_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_terminated_protocol_composition_sites =
      protocol_qualified_object_type_lowering_contract.terminated_protocol_composition_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_pointer_declarator_sites =
      protocol_qualified_object_type_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_normalized_protocol_composition_sites =
      protocol_qualified_object_type_lowering_contract.normalized_protocol_composition_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_contract_violation_sites =
      protocol_qualified_object_type_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_protocol_qualified_object_type_lowering_handoff =
      protocol_qualified_object_type_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_variance_bridge_cast_replay_key =
      variance_bridge_cast_lowering_replay_key;
  ir_frontend_metadata.variance_bridge_cast_lowering_sites =
      variance_bridge_cast_lowering_contract.variance_bridge_cast_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_protocol_composition_sites =
      variance_bridge_cast_lowering_contract.protocol_composition_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_ownership_qualifier_sites =
      variance_bridge_cast_lowering_contract.ownership_qualifier_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_object_pointer_type_sites =
      variance_bridge_cast_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_pointer_declarator_sites =
      variance_bridge_cast_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_normalized_sites =
      variance_bridge_cast_lowering_contract.normalized_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_contract_violation_sites =
      variance_bridge_cast_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_variance_bridge_cast_lowering_handoff =
      variance_bridge_cast_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_generic_metadata_abi_replay_key =
      generic_metadata_abi_lowering_replay_key;
  ir_frontend_metadata.generic_metadata_abi_lowering_sites =
      generic_metadata_abi_lowering_contract.generic_metadata_abi_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_generic_suffix_sites =
      generic_metadata_abi_lowering_contract.generic_suffix_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_protocol_composition_sites =
      generic_metadata_abi_lowering_contract.protocol_composition_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_ownership_qualifier_sites =
      generic_metadata_abi_lowering_contract.ownership_qualifier_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_object_pointer_type_sites =
      generic_metadata_abi_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_pointer_declarator_sites =
      generic_metadata_abi_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_normalized_sites =
      generic_metadata_abi_lowering_contract.normalized_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_contract_violation_sites =
      generic_metadata_abi_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_generic_metadata_abi_lowering_handoff =
      generic_metadata_abi_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_module_import_graph_replay_key =
      module_import_graph_lowering_replay_key;
  ir_frontend_metadata.module_import_graph_lowering_sites =
      module_import_graph_lowering_contract.module_import_graph_sites;
  ir_frontend_metadata.module_import_graph_lowering_import_edge_candidate_sites =
      module_import_graph_lowering_contract.import_edge_candidate_sites;
  ir_frontend_metadata.module_import_graph_lowering_namespace_segment_sites =
      module_import_graph_lowering_contract.namespace_segment_sites;
  ir_frontend_metadata.module_import_graph_lowering_object_pointer_type_sites =
      module_import_graph_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.module_import_graph_lowering_pointer_declarator_sites =
      module_import_graph_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.module_import_graph_lowering_normalized_sites =
      module_import_graph_lowering_contract.normalized_sites;
  ir_frontend_metadata.module_import_graph_lowering_contract_violation_sites =
      module_import_graph_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_module_import_graph_lowering_handoff =
      module_import_graph_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_namespace_collision_shadowing_replay_key =
      namespace_collision_shadowing_lowering_replay_key;
  ir_frontend_metadata.namespace_collision_shadowing_lowering_sites =
      namespace_collision_shadowing_lowering_contract
          .namespace_collision_shadowing_sites;
  ir_frontend_metadata
      .namespace_collision_shadowing_lowering_namespace_segment_sites =
      namespace_collision_shadowing_lowering_contract.namespace_segment_sites;
  ir_frontend_metadata
      .namespace_collision_shadowing_lowering_import_edge_candidate_sites =
      namespace_collision_shadowing_lowering_contract.import_edge_candidate_sites;
  ir_frontend_metadata
      .namespace_collision_shadowing_lowering_object_pointer_type_sites =
      namespace_collision_shadowing_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata
      .namespace_collision_shadowing_lowering_pointer_declarator_sites =
      namespace_collision_shadowing_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.namespace_collision_shadowing_lowering_normalized_sites =
      namespace_collision_shadowing_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .namespace_collision_shadowing_lowering_contract_violation_sites =
      namespace_collision_shadowing_lowering_contract.contract_violation_sites;
  ir_frontend_metadata
      .deterministic_namespace_collision_shadowing_lowering_handoff =
      namespace_collision_shadowing_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_public_private_api_partition_replay_key =
      public_private_api_partition_lowering_replay_key;
  ir_frontend_metadata.public_private_api_partition_lowering_sites =
      public_private_api_partition_lowering_contract
          .public_private_api_partition_sites;
  ir_frontend_metadata
      .public_private_api_partition_lowering_namespace_segment_sites =
      public_private_api_partition_lowering_contract.namespace_segment_sites;
  ir_frontend_metadata
      .public_private_api_partition_lowering_import_edge_candidate_sites =
      public_private_api_partition_lowering_contract
          .import_edge_candidate_sites;
  ir_frontend_metadata
      .public_private_api_partition_lowering_object_pointer_type_sites =
      public_private_api_partition_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata
      .public_private_api_partition_lowering_pointer_declarator_sites =
      public_private_api_partition_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.public_private_api_partition_lowering_normalized_sites =
      public_private_api_partition_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .public_private_api_partition_lowering_contract_violation_sites =
      public_private_api_partition_lowering_contract.contract_violation_sites;
  ir_frontend_metadata
      .deterministic_public_private_api_partition_lowering_handoff =
      public_private_api_partition_lowering_contract.deterministic;
  ir_frontend_metadata
      .lowering_incremental_module_cache_invalidation_replay_key =
      incremental_module_cache_invalidation_lowering_replay_key;
  ir_frontend_metadata.incremental_module_cache_invalidation_lowering_sites =
      incremental_module_cache_invalidation_lowering_contract
          .incremental_module_cache_invalidation_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_namespace_segment_sites =
      incremental_module_cache_invalidation_lowering_contract
          .namespace_segment_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_import_edge_candidate_sites =
      incremental_module_cache_invalidation_lowering_contract
          .import_edge_candidate_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_object_pointer_type_sites =
      incremental_module_cache_invalidation_lowering_contract
          .object_pointer_type_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_pointer_declarator_sites =
      incremental_module_cache_invalidation_lowering_contract
          .pointer_declarator_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_normalized_sites =
      incremental_module_cache_invalidation_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_cache_invalidation_candidate_sites =
      incremental_module_cache_invalidation_lowering_contract
          .cache_invalidation_candidate_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_contract_violation_sites =
      incremental_module_cache_invalidation_lowering_contract
          .contract_violation_sites;
  ir_frontend_metadata
      .deterministic_incremental_module_cache_invalidation_lowering_handoff =
      incremental_module_cache_invalidation_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_cross_module_conformance_replay_key =
      cross_module_conformance_lowering_replay_key;
  ir_frontend_metadata.cross_module_conformance_lowering_sites =
      cross_module_conformance_lowering_contract.cross_module_conformance_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_namespace_segment_sites =
      cross_module_conformance_lowering_contract.namespace_segment_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_import_edge_candidate_sites =
      cross_module_conformance_lowering_contract.import_edge_candidate_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_object_pointer_type_sites =
      cross_module_conformance_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_pointer_declarator_sites =
      cross_module_conformance_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.cross_module_conformance_lowering_normalized_sites =
      cross_module_conformance_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_cache_invalidation_candidate_sites =
      cross_module_conformance_lowering_contract
          .cache_invalidation_candidate_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_contract_violation_sites =
      cross_module_conformance_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_cross_module_conformance_lowering_handoff =
      cross_module_conformance_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_throws_propagation_replay_key =
      throws_propagation_lowering_replay_key;
  ir_frontend_metadata.throws_propagation_lowering_sites =
      throws_propagation_lowering_contract.throws_propagation_sites;
  ir_frontend_metadata.throws_propagation_lowering_namespace_segment_sites =
      throws_propagation_lowering_contract.namespace_segment_sites;
  ir_frontend_metadata.throws_propagation_lowering_import_edge_candidate_sites =
      throws_propagation_lowering_contract.import_edge_candidate_sites;
  ir_frontend_metadata.throws_propagation_lowering_object_pointer_type_sites =
      throws_propagation_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.throws_propagation_lowering_pointer_declarator_sites =
      throws_propagation_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.throws_propagation_lowering_normalized_sites =
      throws_propagation_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .throws_propagation_lowering_cache_invalidation_candidate_sites =
      throws_propagation_lowering_contract
          .cache_invalidation_candidate_sites;
  ir_frontend_metadata.throws_propagation_lowering_contract_violation_sites =
      throws_propagation_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_throws_propagation_lowering_handoff =
      throws_propagation_lowering_contract.deterministic;
  ir_frontend_metadata.object_pointer_type_spellings =
      object_pointer_nullability_generics_summary.object_pointer_type_spellings;
  ir_frontend_metadata.pointer_declarator_entries =
      object_pointer_nullability_generics_summary.pointer_declarator_entries;
  ir_frontend_metadata.pointer_declarator_depth_total =
      object_pointer_nullability_generics_summary.pointer_declarator_depth_total;
  ir_frontend_metadata.pointer_declarator_token_entries =
      object_pointer_nullability_generics_summary.pointer_declarator_token_entries;
  ir_frontend_metadata.nullability_suffix_entries =
      object_pointer_nullability_generics_summary.nullability_suffix_entries;
  ir_frontend_metadata.generic_suffix_entries = object_pointer_nullability_generics_summary.generic_suffix_entries;
  ir_frontend_metadata.terminated_generic_suffix_entries =
      object_pointer_nullability_generics_summary.terminated_generic_suffix_entries;
  ir_frontend_metadata.unterminated_generic_suffix_entries =
      object_pointer_nullability_generics_summary.unterminated_generic_suffix_entries;
  ir_frontend_metadata.global_symbol_nodes = symbol_graph_scope_resolution_summary.global_symbol_nodes;
  ir_frontend_metadata.function_symbol_nodes = symbol_graph_scope_resolution_summary.function_symbol_nodes;
  ir_frontend_metadata.interface_symbol_nodes = symbol_graph_scope_resolution_summary.interface_symbol_nodes;
  ir_frontend_metadata.implementation_symbol_nodes = symbol_graph_scope_resolution_summary.implementation_symbol_nodes;
  ir_frontend_metadata.interface_property_symbol_nodes =
      symbol_graph_scope_resolution_summary.interface_property_symbol_nodes;
  ir_frontend_metadata.implementation_property_symbol_nodes =
      symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes;
  ir_frontend_metadata.interface_method_symbol_nodes = symbol_graph_scope_resolution_summary.interface_method_symbol_nodes;
  ir_frontend_metadata.implementation_method_symbol_nodes =
      symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes;
  ir_frontend_metadata.top_level_scope_symbols = symbol_graph_scope_resolution_summary.top_level_scope_symbols;
  ir_frontend_metadata.nested_scope_symbols = symbol_graph_scope_resolution_summary.nested_scope_symbols;
  ir_frontend_metadata.scope_frames_total = symbol_graph_scope_resolution_summary.scope_frames_total;
  ir_frontend_metadata.implementation_interface_resolution_sites =
      symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites;
  ir_frontend_metadata.implementation_interface_resolution_hits =
      symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits;
  ir_frontend_metadata.implementation_interface_resolution_misses =
      symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses;
  ir_frontend_metadata.method_resolution_sites = symbol_graph_scope_resolution_summary.method_resolution_sites;
  ir_frontend_metadata.method_resolution_hits = symbol_graph_scope_resolution_summary.method_resolution_hits;
  ir_frontend_metadata.method_resolution_misses = symbol_graph_scope_resolution_summary.method_resolution_misses;
  ir_frontend_metadata.deterministic_interface_implementation_handoff =
      pipeline_result.sema_parity_surface.deterministic_interface_implementation_handoff &&
      interface_implementation_summary.deterministic;
  ir_frontend_metadata.deterministic_protocol_category_handoff =
      protocol_category_summary.deterministic_protocol_category_handoff;
  ir_frontend_metadata.deterministic_class_protocol_category_linking_handoff =
      class_protocol_category_linking_summary.deterministic_class_protocol_category_linking_handoff;
  ir_frontend_metadata.deterministic_selector_normalization_handoff =
      selector_normalization_summary.deterministic_selector_normalization_handoff;
  ir_frontend_metadata.deterministic_property_attribute_handoff =
      property_attribute_summary.deterministic_property_attribute_handoff;
  ir_frontend_metadata.deterministic_id_class_sel_object_pointer_typecheck_handoff =
      id_class_sel_object_pointer_typecheck_contract.deterministic;
  ir_frontend_metadata.deterministic_message_send_selector_lowering_handoff =
      message_send_selector_lowering_contract.deterministic;
  ir_frontend_metadata.deterministic_dispatch_abi_marshalling_handoff =
      dispatch_abi_marshalling_contract.deterministic;
  ir_frontend_metadata.deterministic_nil_receiver_semantics_foldability_handoff =
      nil_receiver_semantics_foldability_contract.deterministic;
  ir_frontend_metadata.deterministic_super_dispatch_method_family_handoff =
      super_dispatch_method_family_contract.deterministic;
  ir_frontend_metadata.deterministic_runtime_shim_host_link_handoff =
      runtime_shim_host_link_contract.deterministic;
  ir_frontend_metadata.deterministic_ownership_qualifier_lowering_handoff =
      ownership_qualifier_lowering_contract.deterministic;
  ir_frontend_metadata.deterministic_retain_release_operation_lowering_handoff =
      retain_release_operation_lowering_contract.deterministic;
  ir_frontend_metadata.deterministic_autoreleasepool_scope_lowering_handoff =
      autoreleasepool_scope_lowering_contract.deterministic;
  ir_frontend_metadata.deterministic_object_pointer_nullability_generics_handoff =
      object_pointer_nullability_generics_summary.deterministic_object_pointer_nullability_generics_handoff;
  ir_frontend_metadata.deterministic_symbol_graph_handoff =
      symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff;
  ir_frontend_metadata.deterministic_scope_resolution_handoff =
      symbol_graph_scope_resolution_summary.deterministic_scope_resolution_handoff;
  ir_frontend_metadata.deterministic_symbol_graph_scope_resolution_handoff_key =
      symbol_graph_scope_resolution_summary.deterministic_handoff_key;

  std::string ir_error;
  if (!EmitObjc3IRText(pipeline_result.program, options.lowering, ir_frontend_metadata, bundle.ir_text, ir_error)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(1, 1, "O3L300", "LLVM IR emission failed: " + ir_error)};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    bundle.manifest_json.clear();
    bundle.ir_text.clear();
    return bundle;
  }

  return bundle;
}
