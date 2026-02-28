#include "pipeline/objc3_frontend_artifacts.h"

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
