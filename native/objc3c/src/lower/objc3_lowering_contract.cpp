#include "lower/objc3_lowering_contract.h"

#include <cctype>
#include <sstream>
#include <string>

namespace {

bool IsRuntimeDispatchSymbolStart(char c) {
  return std::isalpha(static_cast<unsigned char>(c)) != 0 || c == '_' || c == '$' || c == '.';
}

bool IsRuntimeDispatchSymbolBody(char c) {
  return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_' || c == '$' || c == '.';
}

const char *AtomicMemoryOrderToken(Objc3AtomicMemoryOrder order) {
  switch (order) {
    case Objc3AtomicMemoryOrder::Relaxed:
      return kObjc3AtomicMemoryOrderRelaxed;
    case Objc3AtomicMemoryOrder::Acquire:
      return kObjc3AtomicMemoryOrderAcquire;
    case Objc3AtomicMemoryOrder::Release:
      return kObjc3AtomicMemoryOrderRelease;
    case Objc3AtomicMemoryOrder::AcqRel:
      return kObjc3AtomicMemoryOrderAcqRel;
    case Objc3AtomicMemoryOrder::SeqCst:
    default:
      return kObjc3AtomicMemoryOrderSeqCst;
  }
}

bool IsSupportedVectorBaseSpelling(const std::string &base_spelling) {
  return base_spelling == kObjc3SimdVectorBaseI32 || base_spelling == kObjc3SimdVectorBaseBool;
}

std::string VectorTypeSpelling(const std::string &base_spelling, unsigned lane_count) {
  return base_spelling + "x" + std::to_string(lane_count);
}

const char *BoolToken(bool value) { return value ? "true" : "false"; }

}  // namespace

bool IsValidRuntimeDispatchSymbol(const std::string &symbol) {
  if (symbol.empty() || !IsRuntimeDispatchSymbolStart(symbol[0])) {
    return false;
  }
  for (std::size_t i = 1; i < symbol.size(); ++i) {
    if (!IsRuntimeDispatchSymbolBody(symbol[i])) {
      return false;
    }
  }
  return true;
}

bool TryNormalizeObjc3LoweringContract(const Objc3LoweringContract &input,
                                       Objc3LoweringContract &normalized,
                                       std::string &error) {
  if (input.max_message_send_args > kObjc3RuntimeDispatchMaxArgs) {
    error = "invalid lowering contract max_message_send_args: " + std::to_string(input.max_message_send_args) +
            " (expected <= " + std::to_string(kObjc3RuntimeDispatchMaxArgs) + ")";
    return false;
  }
  if (!IsValidRuntimeDispatchSymbol(input.runtime_dispatch_symbol)) {
    error = "invalid lowering contract runtime_dispatch_symbol (expected [A-Za-z_.$][A-Za-z0-9_.$]*): " +
            input.runtime_dispatch_symbol;
    return false;
  }
  normalized.max_message_send_args = input.max_message_send_args;
  normalized.runtime_dispatch_symbol = input.runtime_dispatch_symbol;
  return true;
}

bool TryBuildObjc3LoweringIRBoundary(const Objc3LoweringContract &input,
                                     Objc3LoweringIRBoundary &boundary,
                                     std::string &error) {
  Objc3LoweringContract normalized;
  if (!TryNormalizeObjc3LoweringContract(input, normalized, error)) {
    return false;
  }
  boundary.runtime_dispatch_arg_slots = normalized.max_message_send_args;
  boundary.runtime_dispatch_symbol = normalized.runtime_dispatch_symbol;
  boundary.selector_global_ordering = kObjc3SelectorGlobalOrdering;
  return true;
}

std::string Objc3LoweringIRBoundaryReplayKey(const Objc3LoweringIRBoundary &boundary) {
  return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +
         ";runtime_dispatch_arg_slots=" + std::to_string(boundary.runtime_dispatch_arg_slots) +
         ";selector_global_ordering=" + boundary.selector_global_ordering;
}

std::string Objc3RuntimeDispatchDeclarationReplayKey(const Objc3LoweringIRBoundary &boundary) {
  std::ostringstream out;
  out << "declare i32 @" << boundary.runtime_dispatch_symbol << "(i32, ptr";
  for (std::size_t i = 0; i < boundary.runtime_dispatch_arg_slots; ++i) {
    out << ", i32";
  }
  out << ")";
  return out.str();
}

bool TryGetCompoundAssignmentBinaryOpcode(const std::string &op, std::string &opcode) {
  if (op == "+=") {
    opcode = "add";
    return true;
  }
  if (op == "-=") {
    opcode = "sub";
    return true;
  }
  if (op == "*=") {
    opcode = "mul";
    return true;
  }
  if (op == "/=") {
    opcode = "sdiv";
    return true;
  }
  if (op == "%=") {
    opcode = "srem";
    return true;
  }
  if (op == "&=") {
    opcode = "and";
    return true;
  }
  if (op == "|=") {
    opcode = "or";
    return true;
  }
  if (op == "^=") {
    opcode = "xor";
    return true;
  }
  if (op == "<<=") {
    opcode = "shl";
    return true;
  }
  if (op == ">>=") {
    opcode = "ashr";
    return true;
  }
  return false;
}

bool TryParseObjc3AtomicMemoryOrder(const std::string &token, Objc3AtomicMemoryOrder &order) {
  if (token == kObjc3AtomicMemoryOrderRelaxed) {
    order = Objc3AtomicMemoryOrder::Relaxed;
    return true;
  }
  if (token == kObjc3AtomicMemoryOrderAcquire) {
    order = Objc3AtomicMemoryOrder::Acquire;
    return true;
  }
  if (token == kObjc3AtomicMemoryOrderRelease) {
    order = Objc3AtomicMemoryOrder::Release;
    return true;
  }
  if (token == kObjc3AtomicMemoryOrderAcqRel || token == "acquire_release") {
    order = Objc3AtomicMemoryOrder::AcqRel;
    return true;
  }
  if (token == kObjc3AtomicMemoryOrderSeqCst) {
    order = Objc3AtomicMemoryOrder::SeqCst;
    return true;
  }
  return false;
}

const char *Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder order) {
  switch (order) {
    case Objc3AtomicMemoryOrder::Relaxed:
      return "monotonic";
    case Objc3AtomicMemoryOrder::Acquire:
      return "acquire";
    case Objc3AtomicMemoryOrder::Release:
      return "release";
    case Objc3AtomicMemoryOrder::AcqRel:
      return "acq_rel";
    case Objc3AtomicMemoryOrder::SeqCst:
    default:
      return "seq_cst";
  }
}

std::string Objc3AtomicMemoryOrderMappingReplayKey() {
  return std::string(AtomicMemoryOrderToken(Objc3AtomicMemoryOrder::Relaxed)) + "=" +
         Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder::Relaxed) + ";" +
         std::string(AtomicMemoryOrderToken(Objc3AtomicMemoryOrder::Acquire)) + "=" +
         Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder::Acquire) + ";" +
         std::string(AtomicMemoryOrderToken(Objc3AtomicMemoryOrder::Release)) + "=" +
         Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder::Release) + ";" +
         std::string(AtomicMemoryOrderToken(Objc3AtomicMemoryOrder::AcqRel)) + "=" +
         Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder::AcqRel) + ";" +
         std::string(AtomicMemoryOrderToken(Objc3AtomicMemoryOrder::SeqCst)) + "=" +
         Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder::SeqCst);
}

bool IsSupportedObjc3SimdVectorLaneCount(unsigned lane_count) {
  return lane_count == 2u || lane_count == 4u || lane_count == 8u || lane_count == 16u;
}

bool TryBuildObjc3SimdVectorLLVMType(const std::string &base_spelling, unsigned lane_count, std::string &llvm_type) {
  if (!IsSupportedVectorBaseSpelling(base_spelling) || !IsSupportedObjc3SimdVectorLaneCount(lane_count)) {
    return false;
  }

  llvm_type = "<" + std::to_string(lane_count) + " x " +
              (base_spelling == kObjc3SimdVectorBaseBool ? "i1" : "i32") + ">";
  return true;
}

std::string Objc3SimdVectorTypeLoweringReplayKey() {
  std::string replay_key;
  const std::string base_spellings[2] = {kObjc3SimdVectorBaseI32, kObjc3SimdVectorBaseBool};
  const unsigned lane_counts[4] = {2u, 4u, 8u, 16u};
  bool first = true;

  for (const std::string &base_spelling : base_spellings) {
    for (const unsigned lane_count : lane_counts) {
      std::string llvm_type;
      if (!TryBuildObjc3SimdVectorLLVMType(base_spelling, lane_count, llvm_type)) {
        continue;
      }
      if (!first) {
        replay_key += ";";
      }
      replay_key += VectorTypeSpelling(base_spelling, lane_count) + "=" + llvm_type;
      first = false;
    }
  }

  replay_key += ";lane_contract=";
  replay_key += kObjc3SimdVectorLaneContract;
  return replay_key;
}

bool IsValidObjc3MethodLookupOverrideConflictContract(const Objc3MethodLookupOverrideConflictContract &contract) {
  if (contract.method_lookup_hits > contract.method_lookup_sites ||
      contract.method_lookup_misses > contract.method_lookup_sites ||
      contract.method_lookup_hits + contract.method_lookup_misses != contract.method_lookup_sites) {
    return false;
  }
  if (contract.override_lookup_hits > contract.override_lookup_sites ||
      contract.override_lookup_misses > contract.override_lookup_sites ||
      contract.override_lookup_hits + contract.override_lookup_misses != contract.override_lookup_sites) {
    return false;
  }
  if (contract.override_conflicts > contract.override_lookup_hits) {
    return false;
  }
  if (contract.unresolved_base_interfaces > contract.override_lookup_misses) {
    return false;
  }
  return true;
}

std::string Objc3MethodLookupOverrideConflictReplayKey(const Objc3MethodLookupOverrideConflictContract &contract) {
  return std::string("method_lookup_sites=") + std::to_string(contract.method_lookup_sites) +
         ";method_lookup_hits=" + std::to_string(contract.method_lookup_hits) +
         ";method_lookup_misses=" + std::to_string(contract.method_lookup_misses) +
         ";override_lookup_sites=" + std::to_string(contract.override_lookup_sites) +
         ";override_lookup_hits=" + std::to_string(contract.override_lookup_hits) +
         ";override_lookup_misses=" + std::to_string(contract.override_lookup_misses) +
         ";override_conflicts=" + std::to_string(contract.override_conflicts) +
         ";unresolved_base_interfaces=" + std::to_string(contract.unresolved_base_interfaces) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3MethodLookupOverrideConflictLaneContract;
}

Objc3PropertySynthesisIvarBindingContract Objc3DefaultPropertySynthesisIvarBindingContract(
    std::size_t property_synthesis_sites,
    bool deterministic) {
  Objc3PropertySynthesisIvarBindingContract contract;
  contract.property_synthesis_sites = property_synthesis_sites;
  contract.property_synthesis_explicit_ivar_bindings = 0;
  contract.property_synthesis_default_ivar_bindings = property_synthesis_sites;
  contract.ivar_binding_sites = property_synthesis_sites;
  contract.ivar_binding_resolved = property_synthesis_sites;
  contract.ivar_binding_missing = 0;
  contract.ivar_binding_conflicts = 0;
  contract.deterministic = deterministic;
  return contract;
}

bool IsValidObjc3PropertySynthesisIvarBindingContract(
    const Objc3PropertySynthesisIvarBindingContract &contract) {
  if (contract.property_synthesis_explicit_ivar_bindings +
              contract.property_synthesis_default_ivar_bindings !=
          contract.property_synthesis_sites ||
      contract.property_synthesis_explicit_ivar_bindings > contract.property_synthesis_sites ||
      contract.property_synthesis_default_ivar_bindings > contract.property_synthesis_sites) {
    return false;
  }
  if (contract.ivar_binding_sites != contract.property_synthesis_sites) {
    return false;
  }
  if (contract.ivar_binding_resolved > contract.ivar_binding_sites ||
      contract.ivar_binding_missing > contract.ivar_binding_sites ||
      contract.ivar_binding_conflicts > contract.ivar_binding_sites ||
      contract.ivar_binding_resolved + contract.ivar_binding_missing + contract.ivar_binding_conflicts !=
          contract.ivar_binding_sites) {
    return false;
  }
  return true;
}

std::string Objc3PropertySynthesisIvarBindingReplayKey(
    const Objc3PropertySynthesisIvarBindingContract &contract) {
  return std::string("property_synthesis_sites=") + std::to_string(contract.property_synthesis_sites) +
         ";property_synthesis_explicit_ivar_bindings=" +
         std::to_string(contract.property_synthesis_explicit_ivar_bindings) +
         ";property_synthesis_default_ivar_bindings=" +
         std::to_string(contract.property_synthesis_default_ivar_bindings) +
         ";ivar_binding_sites=" + std::to_string(contract.ivar_binding_sites) +
         ";ivar_binding_resolved=" + std::to_string(contract.ivar_binding_resolved) +
         ";ivar_binding_missing=" + std::to_string(contract.ivar_binding_missing) +
         ";ivar_binding_conflicts=" + std::to_string(contract.ivar_binding_conflicts) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3PropertySynthesisIvarBindingLaneContract;
}

bool IsValidObjc3IdClassSelObjectPointerTypecheckContract(
    const Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  const std::size_t computed_total = contract.id_typecheck_sites +
                                     contract.class_typecheck_sites +
                                     contract.sel_typecheck_sites +
                                     contract.object_pointer_typecheck_sites;
  return contract.total_typecheck_sites == computed_total;
}

std::string Objc3IdClassSelObjectPointerTypecheckReplayKey(
    const Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  return std::string("id_typecheck_sites=") + std::to_string(contract.id_typecheck_sites) +
         ";class_typecheck_sites=" + std::to_string(contract.class_typecheck_sites) +
         ";sel_typecheck_sites=" + std::to_string(contract.sel_typecheck_sites) +
         ";object_pointer_typecheck_sites=" + std::to_string(contract.object_pointer_typecheck_sites) +
         ";total_typecheck_sites=" + std::to_string(contract.total_typecheck_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3IdClassSelObjectPointerTypecheckLaneContract;
}

bool IsValidObjc3MessageSendSelectorLoweringContract(
    const Objc3MessageSendSelectorLoweringContract &contract) {
  if (contract.unary_selector_sites + contract.keyword_selector_sites != contract.message_send_sites) {
    return false;
  }
  if (contract.receiver_expression_sites != contract.message_send_sites) {
    return false;
  }
  if (contract.selector_piece_sites < contract.message_send_sites) {
    return false;
  }
  if (contract.argument_expression_sites < contract.keyword_selector_sites) {
    return false;
  }
  if (contract.selector_literal_entries > contract.message_send_sites) {
    return false;
  }
  if (contract.selector_literal_entries == 0 && contract.selector_literal_characters != 0) {
    return false;
  }
  return true;
}

std::string Objc3MessageSendSelectorLoweringReplayKey(
    const Objc3MessageSendSelectorLoweringContract &contract) {
  return std::string("message_send_sites=") + std::to_string(contract.message_send_sites) +
         ";unary_selector_sites=" + std::to_string(contract.unary_selector_sites) +
         ";keyword_selector_sites=" + std::to_string(contract.keyword_selector_sites) +
         ";selector_piece_sites=" + std::to_string(contract.selector_piece_sites) +
         ";argument_expression_sites=" + std::to_string(contract.argument_expression_sites) +
         ";receiver_expression_sites=" + std::to_string(contract.receiver_expression_sites) +
         ";selector_literal_entries=" + std::to_string(contract.selector_literal_entries) +
         ";selector_literal_characters=" + std::to_string(contract.selector_literal_characters) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3MessageSendSelectorLoweringLaneContract;
}

bool IsValidObjc3DispatchAbiMarshallingContract(
    const Objc3DispatchAbiMarshallingContract &contract) {
  const std::size_t expected_argument_total =
      contract.message_send_sites * contract.runtime_dispatch_arg_slots;
  if (contract.receiver_slots_marshaled != contract.message_send_sites ||
      contract.selector_slots_marshaled != contract.message_send_sites) {
    return false;
  }
  if (contract.argument_total_slots_marshaled != expected_argument_total) {
    return false;
  }
  if (contract.argument_value_slots_marshaled > contract.argument_total_slots_marshaled) {
    return false;
  }
  if (contract.argument_padding_slots_marshaled + contract.argument_value_slots_marshaled !=
      contract.argument_total_slots_marshaled) {
    return false;
  }
  const std::size_t expected_total = contract.receiver_slots_marshaled +
                                     contract.selector_slots_marshaled +
                                     contract.argument_total_slots_marshaled;
  return contract.total_marshaled_slots == expected_total;
}

std::string Objc3DispatchAbiMarshallingReplayKey(
    const Objc3DispatchAbiMarshallingContract &contract) {
  return std::string("message_send_sites=") + std::to_string(contract.message_send_sites) +
         ";receiver_slots_marshaled=" + std::to_string(contract.receiver_slots_marshaled) +
         ";selector_slots_marshaled=" + std::to_string(contract.selector_slots_marshaled) +
         ";argument_value_slots_marshaled=" + std::to_string(contract.argument_value_slots_marshaled) +
         ";argument_padding_slots_marshaled=" + std::to_string(contract.argument_padding_slots_marshaled) +
         ";argument_total_slots_marshaled=" + std::to_string(contract.argument_total_slots_marshaled) +
         ";total_marshaled_slots=" + std::to_string(contract.total_marshaled_slots) +
         ";runtime_dispatch_arg_slots=" + std::to_string(contract.runtime_dispatch_arg_slots) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3DispatchAbiMarshallingLaneContract;
}

bool IsValidObjc3NilReceiverSemanticsFoldabilityContract(
    const Objc3NilReceiverSemanticsFoldabilityContract &contract) {
  if (contract.receiver_nil_literal_sites != contract.nil_receiver_semantics_enabled_sites) {
    return false;
  }
  if (contract.nil_receiver_foldable_sites > contract.nil_receiver_semantics_enabled_sites) {
    return false;
  }
  if (contract.nil_receiver_runtime_dispatch_required_sites + contract.nil_receiver_foldable_sites !=
      contract.message_send_sites) {
    return false;
  }
  if (contract.nil_receiver_semantics_enabled_sites + contract.non_nil_receiver_sites !=
      contract.message_send_sites) {
    return false;
  }
  return contract.contract_violation_sites <= contract.message_send_sites;
}

std::string Objc3NilReceiverSemanticsFoldabilityReplayKey(
    const Objc3NilReceiverSemanticsFoldabilityContract &contract) {
  return std::string("message_send_sites=") + std::to_string(contract.message_send_sites) +
         ";receiver_nil_literal_sites=" + std::to_string(contract.receiver_nil_literal_sites) +
         ";nil_receiver_semantics_enabled_sites=" + std::to_string(contract.nil_receiver_semantics_enabled_sites) +
         ";nil_receiver_foldable_sites=" + std::to_string(contract.nil_receiver_foldable_sites) +
         ";nil_receiver_runtime_dispatch_required_sites=" +
         std::to_string(contract.nil_receiver_runtime_dispatch_required_sites) +
         ";non_nil_receiver_sites=" + std::to_string(contract.non_nil_receiver_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3NilReceiverSemanticsFoldabilityLaneContract;
}

bool IsValidObjc3SuperDispatchMethodFamilyContract(
    const Objc3SuperDispatchMethodFamilyContract &contract) {
  if (contract.receiver_super_identifier_sites != contract.super_dispatch_enabled_sites) {
    return false;
  }
  if (contract.super_dispatch_requires_class_context_sites != contract.super_dispatch_enabled_sites) {
    return false;
  }
  if (contract.method_family_init_sites + contract.method_family_copy_sites +
              contract.method_family_mutable_copy_sites + contract.method_family_new_sites +
              contract.method_family_none_sites !=
          contract.message_send_sites) {
    return false;
  }
  if (contract.method_family_returns_related_result_sites > contract.method_family_init_sites) {
    return false;
  }
  if (contract.method_family_returns_retained_result_sites > contract.message_send_sites) {
    return false;
  }
  return contract.contract_violation_sites <= contract.message_send_sites;
}

std::string Objc3SuperDispatchMethodFamilyReplayKey(
    const Objc3SuperDispatchMethodFamilyContract &contract) {
  return std::string("message_send_sites=") + std::to_string(contract.message_send_sites) +
         ";receiver_super_identifier_sites=" + std::to_string(contract.receiver_super_identifier_sites) +
         ";super_dispatch_enabled_sites=" + std::to_string(contract.super_dispatch_enabled_sites) +
         ";super_dispatch_requires_class_context_sites=" +
         std::to_string(contract.super_dispatch_requires_class_context_sites) +
         ";method_family_init_sites=" + std::to_string(contract.method_family_init_sites) +
         ";method_family_copy_sites=" + std::to_string(contract.method_family_copy_sites) +
         ";method_family_mutable_copy_sites=" + std::to_string(contract.method_family_mutable_copy_sites) +
         ";method_family_new_sites=" + std::to_string(contract.method_family_new_sites) +
         ";method_family_none_sites=" + std::to_string(contract.method_family_none_sites) +
         ";method_family_returns_retained_result_sites=" +
         std::to_string(contract.method_family_returns_retained_result_sites) +
         ";method_family_returns_related_result_sites=" +
         std::to_string(contract.method_family_returns_related_result_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3SuperDispatchMethodFamilyLaneContract;
}

bool IsValidObjc3RuntimeShimHostLinkContract(
    const Objc3RuntimeShimHostLinkContract &contract) {
  if (!IsValidRuntimeDispatchSymbol(contract.runtime_dispatch_symbol)) {
    return false;
  }
  if (contract.runtime_dispatch_arg_slots > kObjc3RuntimeDispatchMaxArgs) {
    return false;
  }
  if (contract.runtime_shim_required_sites > contract.message_send_sites) {
    return false;
  }
  if (contract.runtime_shim_required_sites + contract.runtime_shim_elided_sites !=
      contract.message_send_sites) {
    return false;
  }
  if (contract.runtime_dispatch_declaration_parameter_count !=
      contract.runtime_dispatch_arg_slots + 2u) {
    return false;
  }
  if (contract.default_runtime_dispatch_symbol_binding !=
      (contract.runtime_dispatch_symbol == kObjc3RuntimeDispatchSymbol)) {
    return false;
  }
  return contract.contract_violation_sites <= contract.message_send_sites;
}

std::string Objc3RuntimeShimHostLinkReplayKey(
    const Objc3RuntimeShimHostLinkContract &contract) {
  return std::string("message_send_sites=") + std::to_string(contract.message_send_sites) +
         ";runtime_shim_required_sites=" + std::to_string(contract.runtime_shim_required_sites) +
         ";runtime_shim_elided_sites=" + std::to_string(contract.runtime_shim_elided_sites) +
         ";runtime_dispatch_arg_slots=" + std::to_string(contract.runtime_dispatch_arg_slots) +
         ";runtime_dispatch_declaration_parameter_count=" +
         std::to_string(contract.runtime_dispatch_declaration_parameter_count) +
         ";runtime_dispatch_symbol=" + contract.runtime_dispatch_symbol +
         ";default_runtime_dispatch_symbol_binding=" +
         BoolToken(contract.default_runtime_dispatch_symbol_binding) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3RuntimeShimHostLinkLaneContract;
}

bool IsValidObjc3OwnershipQualifierLoweringContract(
    const Objc3OwnershipQualifierLoweringContract &contract) {
  return contract.invalid_ownership_qualifier_sites <= contract.ownership_qualifier_sites &&
         contract.ownership_qualifier_sites <= contract.object_pointer_type_annotation_sites;
}

std::string Objc3OwnershipQualifierLoweringReplayKey(
    const Objc3OwnershipQualifierLoweringContract &contract) {
  return std::string("ownership_qualifier_sites=") + std::to_string(contract.ownership_qualifier_sites) +
         ";invalid_ownership_qualifier_sites=" + std::to_string(contract.invalid_ownership_qualifier_sites) +
         ";object_pointer_type_annotation_sites=" + std::to_string(contract.object_pointer_type_annotation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3OwnershipQualifierLoweringLaneContract;
}

bool IsValidObjc3RetainReleaseOperationLoweringContract(
    const Objc3RetainReleaseOperationLoweringContract &contract) {
  const std::size_t qualified_or_violation = contract.ownership_qualified_sites + contract.contract_violation_sites;
  return contract.retain_insertion_sites <= qualified_or_violation &&
         contract.release_insertion_sites <= qualified_or_violation &&
         contract.autorelease_insertion_sites <= qualified_or_violation;
}

std::string Objc3RetainReleaseOperationLoweringReplayKey(
    const Objc3RetainReleaseOperationLoweringContract &contract) {
  return std::string("ownership_qualified_sites=") + std::to_string(contract.ownership_qualified_sites) +
         ";retain_insertion_sites=" + std::to_string(contract.retain_insertion_sites) +
         ";release_insertion_sites=" + std::to_string(contract.release_insertion_sites) +
         ";autorelease_insertion_sites=" + std::to_string(contract.autorelease_insertion_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3RetainReleaseOperationLoweringLaneContract;
}

bool IsValidObjc3AutoreleasePoolScopeLoweringContract(
    const Objc3AutoreleasePoolScopeLoweringContract &contract) {
  return contract.scope_symbolized_sites <= contract.scope_sites &&
         contract.contract_violation_sites <= contract.scope_sites &&
         contract.scope_entry_transition_sites == contract.scope_sites &&
         contract.scope_exit_transition_sites == contract.scope_sites &&
         (contract.scope_sites > 0u || contract.max_scope_depth == 0u) &&
         contract.max_scope_depth <= static_cast<unsigned>(contract.scope_sites);
}

std::string Objc3AutoreleasePoolScopeLoweringReplayKey(
    const Objc3AutoreleasePoolScopeLoweringContract &contract) {
  return std::string("scope_sites=") + std::to_string(contract.scope_sites) +
         ";scope_symbolized_sites=" + std::to_string(contract.scope_symbolized_sites) +
         ";max_scope_depth=" + std::to_string(contract.max_scope_depth) +
         ";scope_entry_transition_sites=" + std::to_string(contract.scope_entry_transition_sites) +
         ";scope_exit_transition_sites=" + std::to_string(contract.scope_exit_transition_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3AutoreleasePoolScopeLoweringLaneContract;
}

bool IsValidObjc3WeakUnownedSemanticsLoweringContract(
    const Objc3WeakUnownedSemanticsLoweringContract &contract) {
  return contract.weak_reference_sites <= contract.ownership_candidate_sites &&
         contract.unowned_reference_sites <= contract.ownership_candidate_sites &&
         contract.unowned_safe_reference_sites <= contract.unowned_reference_sites &&
         contract.weak_unowned_conflict_sites <= contract.ownership_candidate_sites &&
         contract.contract_violation_sites <=
             contract.ownership_candidate_sites + contract.weak_unowned_conflict_sites;
}

std::string Objc3WeakUnownedSemanticsLoweringReplayKey(
    const Objc3WeakUnownedSemanticsLoweringContract &contract) {
  return std::string("ownership_candidate_sites=") + std::to_string(contract.ownership_candidate_sites) +
         ";weak_reference_sites=" + std::to_string(contract.weak_reference_sites) +
         ";unowned_reference_sites=" + std::to_string(contract.unowned_reference_sites) +
         ";unowned_safe_reference_sites=" + std::to_string(contract.unowned_safe_reference_sites) +
         ";weak_unowned_conflict_sites=" + std::to_string(contract.weak_unowned_conflict_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3WeakUnownedSemanticsLoweringLaneContract;
}

bool IsValidObjc3ArcDiagnosticsFixitLoweringContract(
    const Objc3ArcDiagnosticsFixitLoweringContract &contract) {
  return contract.ownership_arc_fixit_available_sites <=
             contract.ownership_arc_diagnostic_candidate_sites + contract.contract_violation_sites &&
         contract.ownership_arc_profiled_sites <=
             contract.ownership_arc_diagnostic_candidate_sites + contract.contract_violation_sites &&
         contract.ownership_arc_weak_unowned_conflict_diagnostic_sites <=
             contract.ownership_arc_diagnostic_candidate_sites + contract.contract_violation_sites &&
         contract.ownership_arc_empty_fixit_hint_sites <=
             contract.ownership_arc_fixit_available_sites + contract.contract_violation_sites;
}

std::string Objc3ArcDiagnosticsFixitLoweringReplayKey(
    const Objc3ArcDiagnosticsFixitLoweringContract &contract) {
  return std::string("ownership_arc_diagnostic_candidate_sites=") +
             std::to_string(contract.ownership_arc_diagnostic_candidate_sites) +
         ";ownership_arc_fixit_available_sites=" +
             std::to_string(contract.ownership_arc_fixit_available_sites) +
         ";ownership_arc_profiled_sites=" + std::to_string(contract.ownership_arc_profiled_sites) +
         ";ownership_arc_weak_unowned_conflict_diagnostic_sites=" +
             std::to_string(contract.ownership_arc_weak_unowned_conflict_diagnostic_sites) +
         ";ownership_arc_empty_fixit_hint_sites=" +
             std::to_string(contract.ownership_arc_empty_fixit_hint_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3ArcDiagnosticsFixitLoweringLaneContract;
}

bool IsValidObjc3BlockLiteralCaptureLoweringContract(
    const Objc3BlockLiteralCaptureLoweringContract &contract) {
  if (contract.block_empty_capture_sites > contract.block_literal_sites ||
      contract.block_nondeterministic_capture_sites > contract.block_literal_sites ||
      contract.block_non_normalized_sites > contract.block_literal_sites ||
      contract.contract_violation_sites > contract.block_literal_sites) {
    return false;
  }
  if (contract.block_literal_sites == 0) {
    return contract.block_parameter_entries == 0 && contract.block_capture_entries == 0 &&
           contract.block_body_statement_entries == 0;
  }
  if (contract.block_nondeterministic_capture_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3BlockLiteralCaptureLoweringReplayKey(
    const Objc3BlockLiteralCaptureLoweringContract &contract) {
  return std::string("block_literal_sites=") + std::to_string(contract.block_literal_sites) +
         ";block_parameter_entries=" + std::to_string(contract.block_parameter_entries) +
         ";block_capture_entries=" + std::to_string(contract.block_capture_entries) +
         ";block_body_statement_entries=" + std::to_string(contract.block_body_statement_entries) +
         ";block_empty_capture_sites=" + std::to_string(contract.block_empty_capture_sites) +
         ";block_nondeterministic_capture_sites=" +
             std::to_string(contract.block_nondeterministic_capture_sites) +
         ";block_non_normalized_sites=" + std::to_string(contract.block_non_normalized_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3BlockLiteralCaptureLoweringLaneContract;
}

bool IsValidObjc3BlockAbiInvokeTrampolineLoweringContract(
    const Objc3BlockAbiInvokeTrampolineLoweringContract &contract) {
  if (contract.descriptor_symbolized_sites > contract.block_literal_sites ||
      contract.invoke_trampoline_symbolized_sites > contract.block_literal_sites ||
      contract.missing_invoke_trampoline_sites > contract.block_literal_sites ||
      contract.non_normalized_layout_sites > contract.block_literal_sites ||
      contract.contract_violation_sites > contract.block_literal_sites) {
    return false;
  }
  if (contract.block_literal_sites == 0) {
    return contract.invoke_argument_slots_total == 0 &&
           contract.capture_word_count_total == 0 &&
           contract.parameter_entries_total == 0 &&
           contract.capture_entries_total == 0 &&
           contract.body_statement_entries_total == 0;
  }
  if (contract.invoke_trampoline_symbolized_sites + contract.missing_invoke_trampoline_sites !=
      contract.block_literal_sites) {
    return false;
  }
  if (contract.invoke_argument_slots_total != contract.parameter_entries_total ||
      contract.capture_word_count_total != contract.capture_entries_total) {
    return false;
  }
  if ((contract.missing_invoke_trampoline_sites > 0 || contract.non_normalized_layout_sites > 0) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3BlockAbiInvokeTrampolineLoweringReplayKey(
    const Objc3BlockAbiInvokeTrampolineLoweringContract &contract) {
  return std::string("block_literal_sites=") + std::to_string(contract.block_literal_sites) +
         ";invoke_argument_slots_total=" + std::to_string(contract.invoke_argument_slots_total) +
         ";capture_word_count_total=" + std::to_string(contract.capture_word_count_total) +
         ";parameter_entries_total=" + std::to_string(contract.parameter_entries_total) +
         ";capture_entries_total=" + std::to_string(contract.capture_entries_total) +
         ";body_statement_entries_total=" + std::to_string(contract.body_statement_entries_total) +
         ";descriptor_symbolized_sites=" + std::to_string(contract.descriptor_symbolized_sites) +
         ";invoke_trampoline_symbolized_sites=" +
             std::to_string(contract.invoke_trampoline_symbolized_sites) +
         ";missing_invoke_trampoline_sites=" + std::to_string(contract.missing_invoke_trampoline_sites) +
         ";non_normalized_layout_sites=" + std::to_string(contract.non_normalized_layout_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3BlockAbiInvokeTrampolineLoweringLaneContract;
}

bool IsValidObjc3BlockStorageEscapeLoweringContract(
    const Objc3BlockStorageEscapeLoweringContract &contract) {
  if (contract.requires_byref_cells_sites > contract.block_literal_sites ||
      contract.escape_analysis_enabled_sites > contract.block_literal_sites ||
      contract.escape_to_heap_sites > contract.block_literal_sites ||
      contract.escape_profile_normalized_sites > contract.block_literal_sites ||
      contract.byref_layout_symbolized_sites > contract.block_literal_sites ||
      contract.contract_violation_sites > contract.block_literal_sites) {
    return false;
  }
  if (contract.block_literal_sites == 0) {
    return contract.mutable_capture_count_total == 0 &&
           contract.byref_slot_count_total == 0 &&
           contract.parameter_entries_total == 0 &&
           contract.capture_entries_total == 0 &&
           contract.body_statement_entries_total == 0;
  }
  if (contract.mutable_capture_count_total != contract.capture_entries_total ||
      contract.byref_slot_count_total != contract.capture_entries_total ||
      contract.escape_analysis_enabled_sites != contract.block_literal_sites ||
      contract.requires_byref_cells_sites != contract.escape_to_heap_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 || contract.escape_profile_normalized_sites !=
                                            contract.block_literal_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3BlockStorageEscapeLoweringReplayKey(
    const Objc3BlockStorageEscapeLoweringContract &contract) {
  return std::string("block_literal_sites=") + std::to_string(contract.block_literal_sites) +
         ";mutable_capture_count_total=" + std::to_string(contract.mutable_capture_count_total) +
         ";byref_slot_count_total=" + std::to_string(contract.byref_slot_count_total) +
         ";parameter_entries_total=" + std::to_string(contract.parameter_entries_total) +
         ";capture_entries_total=" + std::to_string(contract.capture_entries_total) +
         ";body_statement_entries_total=" + std::to_string(contract.body_statement_entries_total) +
         ";requires_byref_cells_sites=" + std::to_string(contract.requires_byref_cells_sites) +
         ";escape_analysis_enabled_sites=" + std::to_string(contract.escape_analysis_enabled_sites) +
         ";escape_to_heap_sites=" + std::to_string(contract.escape_to_heap_sites) +
         ";escape_profile_normalized_sites=" + std::to_string(contract.escape_profile_normalized_sites) +
         ";byref_layout_symbolized_sites=" + std::to_string(contract.byref_layout_symbolized_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3BlockStorageEscapeLoweringLaneContract;
}

bool IsValidObjc3BlockCopyDisposeLoweringContract(
    const Objc3BlockCopyDisposeLoweringContract &contract) {
  if (contract.copy_helper_required_sites > contract.block_literal_sites ||
      contract.dispose_helper_required_sites > contract.block_literal_sites ||
      contract.profile_normalized_sites > contract.block_literal_sites ||
      contract.copy_helper_symbolized_sites > contract.block_literal_sites ||
      contract.dispose_helper_symbolized_sites > contract.block_literal_sites ||
      contract.contract_violation_sites > contract.block_literal_sites) {
    return false;
  }
  if (contract.block_literal_sites == 0) {
    return contract.mutable_capture_count_total == 0 &&
           contract.byref_slot_count_total == 0 &&
           contract.parameter_entries_total == 0 &&
           contract.capture_entries_total == 0 &&
           contract.body_statement_entries_total == 0;
  }
  if (contract.mutable_capture_count_total != contract.capture_entries_total ||
      contract.byref_slot_count_total != contract.capture_entries_total ||
      contract.copy_helper_required_sites != contract.dispose_helper_required_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 || contract.profile_normalized_sites !=
                                            contract.block_literal_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3BlockCopyDisposeLoweringReplayKey(
    const Objc3BlockCopyDisposeLoweringContract &contract) {
  return std::string("block_literal_sites=") + std::to_string(contract.block_literal_sites) +
         ";mutable_capture_count_total=" + std::to_string(contract.mutable_capture_count_total) +
         ";byref_slot_count_total=" + std::to_string(contract.byref_slot_count_total) +
         ";parameter_entries_total=" + std::to_string(contract.parameter_entries_total) +
         ";capture_entries_total=" + std::to_string(contract.capture_entries_total) +
         ";body_statement_entries_total=" + std::to_string(contract.body_statement_entries_total) +
         ";copy_helper_required_sites=" + std::to_string(contract.copy_helper_required_sites) +
         ";dispose_helper_required_sites=" + std::to_string(contract.dispose_helper_required_sites) +
         ";profile_normalized_sites=" + std::to_string(contract.profile_normalized_sites) +
         ";copy_helper_symbolized_sites=" + std::to_string(contract.copy_helper_symbolized_sites) +
         ";dispose_helper_symbolized_sites=" + std::to_string(contract.dispose_helper_symbolized_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3BlockCopyDisposeLoweringLaneContract;
}

bool IsValidObjc3BlockDeterminismPerfBaselineLoweringContract(
    const Objc3BlockDeterminismPerfBaselineLoweringContract &contract) {
  if (contract.deterministic_capture_sites > contract.block_literal_sites ||
      contract.heavy_tier_sites > contract.block_literal_sites ||
      contract.normalized_profile_sites > contract.block_literal_sites ||
      contract.contract_violation_sites > contract.block_literal_sites) {
    return false;
  }
  if (contract.block_literal_sites == 0) {
    return contract.baseline_weight_total == 0 &&
           contract.parameter_entries_total == 0 &&
           contract.capture_entries_total == 0 &&
           contract.body_statement_entries_total == 0;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_profile_sites != contract.block_literal_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3BlockDeterminismPerfBaselineLoweringReplayKey(
    const Objc3BlockDeterminismPerfBaselineLoweringContract &contract) {
  return std::string("block_literal_sites=") + std::to_string(contract.block_literal_sites) +
         ";baseline_weight_total=" + std::to_string(contract.baseline_weight_total) +
         ";parameter_entries_total=" + std::to_string(contract.parameter_entries_total) +
         ";capture_entries_total=" + std::to_string(contract.capture_entries_total) +
         ";body_statement_entries_total=" + std::to_string(contract.body_statement_entries_total) +
         ";deterministic_capture_sites=" + std::to_string(contract.deterministic_capture_sites) +
         ";heavy_tier_sites=" + std::to_string(contract.heavy_tier_sites) +
         ";normalized_profile_sites=" + std::to_string(contract.normalized_profile_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3BlockDeterminismPerfBaselineLoweringLaneContract;
}

bool IsValidObjc3LightweightGenericsConstraintLoweringContract(
    const Objc3LightweightGenericsConstraintLoweringContract &contract) {
  if (contract.generic_suffix_sites > contract.generic_constraint_sites ||
      contract.object_pointer_type_sites > contract.generic_constraint_sites ||
      contract.terminated_generic_suffix_sites > contract.generic_suffix_sites ||
      contract.pointer_declarator_sites > contract.generic_constraint_sites ||
      contract.normalized_constraint_sites > contract.generic_constraint_sites ||
      contract.contract_violation_sites > contract.generic_constraint_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_constraint_sites != contract.generic_constraint_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3LightweightGenericsConstraintLoweringReplayKey(
    const Objc3LightweightGenericsConstraintLoweringContract &contract) {
  return std::string("generic_constraint_sites=") + std::to_string(contract.generic_constraint_sites) +
         ";generic_suffix_sites=" + std::to_string(contract.generic_suffix_sites) +
         ";object_pointer_type_sites=" + std::to_string(contract.object_pointer_type_sites) +
         ";terminated_generic_suffix_sites=" + std::to_string(contract.terminated_generic_suffix_sites) +
         ";pointer_declarator_sites=" + std::to_string(contract.pointer_declarator_sites) +
         ";normalized_constraint_sites=" + std::to_string(contract.normalized_constraint_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3LightweightGenericsConstraintLoweringLaneContract;
}

bool IsValidObjc3NullabilityFlowWarningPrecisionLoweringContract(
    const Objc3NullabilityFlowWarningPrecisionLoweringContract &contract) {
  if (contract.nullability_suffix_sites > contract.nullability_flow_sites ||
      contract.nullable_suffix_sites > contract.nullability_suffix_sites ||
      contract.nonnull_suffix_sites > contract.nullability_suffix_sites ||
      contract.object_pointer_type_sites < contract.nullability_suffix_sites ||
      contract.normalized_sites > contract.nullability_flow_sites ||
      contract.contract_violation_sites > contract.nullability_flow_sites) {
    return false;
  }
  if (contract.nullability_suffix_sites !=
      contract.nullable_suffix_sites + contract.nonnull_suffix_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.nullability_flow_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3NullabilityFlowWarningPrecisionLoweringReplayKey(
    const Objc3NullabilityFlowWarningPrecisionLoweringContract &contract) {
  return std::string("nullability_flow_sites=") + std::to_string(contract.nullability_flow_sites) +
         ";object_pointer_type_sites=" + std::to_string(contract.object_pointer_type_sites) +
         ";nullability_suffix_sites=" + std::to_string(contract.nullability_suffix_sites) +
         ";nullable_suffix_sites=" + std::to_string(contract.nullable_suffix_sites) +
         ";nonnull_suffix_sites=" + std::to_string(contract.nonnull_suffix_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3NullabilityFlowWarningPrecisionLoweringLaneContract;
}

bool IsValidObjc3ProtocolQualifiedObjectTypeLoweringContract(
    const Objc3ProtocolQualifiedObjectTypeLoweringContract &contract) {
  if (contract.protocol_composition_sites > contract.protocol_qualified_object_type_sites ||
      contract.object_pointer_type_sites < contract.protocol_composition_sites ||
      contract.terminated_protocol_composition_sites > contract.protocol_composition_sites ||
      contract.pointer_declarator_sites > contract.protocol_qualified_object_type_sites ||
      contract.normalized_protocol_composition_sites > contract.protocol_qualified_object_type_sites ||
      contract.contract_violation_sites > contract.protocol_qualified_object_type_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_protocol_composition_sites != contract.protocol_qualified_object_type_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ProtocolQualifiedObjectTypeLoweringReplayKey(
    const Objc3ProtocolQualifiedObjectTypeLoweringContract &contract) {
  return std::string("protocol_qualified_object_type_sites=") +
         std::to_string(contract.protocol_qualified_object_type_sites) +
         ";protocol_composition_sites=" + std::to_string(contract.protocol_composition_sites) +
         ";object_pointer_type_sites=" + std::to_string(contract.object_pointer_type_sites) +
         ";terminated_protocol_composition_sites=" + std::to_string(contract.terminated_protocol_composition_sites) +
         ";pointer_declarator_sites=" + std::to_string(contract.pointer_declarator_sites) +
         ";normalized_protocol_composition_sites=" +
         std::to_string(contract.normalized_protocol_composition_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3ProtocolQualifiedObjectTypeLoweringLaneContract;
}

bool IsValidObjc3VarianceBridgeCastLoweringContract(
    const Objc3VarianceBridgeCastLoweringContract &contract) {
  if (contract.protocol_composition_sites > contract.variance_bridge_cast_sites ||
      contract.ownership_qualifier_sites > contract.variance_bridge_cast_sites ||
      contract.object_pointer_type_sites < contract.protocol_composition_sites ||
      contract.pointer_declarator_sites > contract.variance_bridge_cast_sites ||
      contract.normalized_sites > contract.variance_bridge_cast_sites ||
      contract.contract_violation_sites > contract.variance_bridge_cast_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.variance_bridge_cast_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3VarianceBridgeCastLoweringReplayKey(
    const Objc3VarianceBridgeCastLoweringContract &contract) {
  return std::string("variance_bridge_cast_sites=") + std::to_string(contract.variance_bridge_cast_sites) +
         ";protocol_composition_sites=" + std::to_string(contract.protocol_composition_sites) +
         ";ownership_qualifier_sites=" + std::to_string(contract.ownership_qualifier_sites) +
         ";object_pointer_type_sites=" + std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" + std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3VarianceBridgeCastLoweringLaneContract;
}

bool IsValidObjc3GenericMetadataAbiLoweringContract(
    const Objc3GenericMetadataAbiLoweringContract &contract) {
  if (contract.generic_suffix_sites > contract.generic_metadata_abi_sites ||
      contract.protocol_composition_sites > contract.generic_metadata_abi_sites ||
      contract.ownership_qualifier_sites > contract.generic_metadata_abi_sites ||
      contract.object_pointer_type_sites < contract.protocol_composition_sites ||
      contract.pointer_declarator_sites > contract.generic_metadata_abi_sites ||
      contract.normalized_sites > contract.generic_metadata_abi_sites ||
      contract.contract_violation_sites > contract.generic_metadata_abi_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.generic_metadata_abi_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3GenericMetadataAbiLoweringReplayKey(
    const Objc3GenericMetadataAbiLoweringContract &contract) {
  return std::string("generic_metadata_abi_sites=") + std::to_string(contract.generic_metadata_abi_sites) +
         ";generic_suffix_sites=" + std::to_string(contract.generic_suffix_sites) +
         ";protocol_composition_sites=" + std::to_string(contract.protocol_composition_sites) +
         ";ownership_qualifier_sites=" + std::to_string(contract.ownership_qualifier_sites) +
         ";object_pointer_type_sites=" + std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" + std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3GenericMetadataAbiLoweringLaneContract;
}

bool IsValidObjc3ModuleImportGraphLoweringContract(
    const Objc3ModuleImportGraphLoweringContract &contract) {
  if (contract.import_edge_candidate_sites > contract.module_import_graph_sites ||
      contract.namespace_segment_sites > contract.module_import_graph_sites ||
      contract.object_pointer_type_sites < contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites > contract.module_import_graph_sites ||
      contract.normalized_sites > contract.module_import_graph_sites ||
      contract.contract_violation_sites > contract.module_import_graph_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.module_import_graph_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ModuleImportGraphLoweringReplayKey(
    const Objc3ModuleImportGraphLoweringContract &contract) {
  return std::string("module_import_graph_sites=") + std::to_string(contract.module_import_graph_sites) +
         ";import_edge_candidate_sites=" + std::to_string(contract.import_edge_candidate_sites) +
         ";namespace_segment_sites=" + std::to_string(contract.namespace_segment_sites) +
         ";object_pointer_type_sites=" + std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" + std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";contract_violation_sites=" + std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3ModuleImportGraphLoweringLaneContract;
}

bool IsValidObjc3NamespaceCollisionShadowingLoweringContract(
    const Objc3NamespaceCollisionShadowingLoweringContract &contract) {
  if (contract.namespace_segment_sites > contract.namespace_collision_shadowing_sites ||
      contract.import_edge_candidate_sites > contract.namespace_collision_shadowing_sites ||
      contract.object_pointer_type_sites < contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites > contract.namespace_collision_shadowing_sites ||
      contract.normalized_sites > contract.namespace_collision_shadowing_sites ||
      contract.contract_violation_sites > contract.namespace_collision_shadowing_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.namespace_collision_shadowing_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3NamespaceCollisionShadowingLoweringReplayKey(
    const Objc3NamespaceCollisionShadowingLoweringContract &contract) {
  return std::string("namespace_collision_shadowing_sites=") +
             std::to_string(contract.namespace_collision_shadowing_sites) +
         ";namespace_segment_sites=" + std::to_string(contract.namespace_segment_sites) +
         ";import_edge_candidate_sites=" +
             std::to_string(contract.import_edge_candidate_sites) +
         ";object_pointer_type_sites=" +
             std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" +
             std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";contract_violation_sites=" +
             std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3NamespaceCollisionShadowingLoweringLaneContract;
}

bool IsValidObjc3PublicPrivateApiPartitionLoweringContract(
    const Objc3PublicPrivateApiPartitionLoweringContract &contract) {
  if (contract.namespace_segment_sites >
          contract.public_private_api_partition_sites ||
      contract.import_edge_candidate_sites >
          contract.public_private_api_partition_sites ||
      contract.object_pointer_type_sites <
          contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites >
          contract.public_private_api_partition_sites ||
      contract.normalized_sites > contract.public_private_api_partition_sites ||
      contract.contract_violation_sites >
          contract.public_private_api_partition_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.public_private_api_partition_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3PublicPrivateApiPartitionLoweringReplayKey(
    const Objc3PublicPrivateApiPartitionLoweringContract &contract) {
  return std::string("public_private_api_partition_sites=") +
             std::to_string(contract.public_private_api_partition_sites) +
         ";namespace_segment_sites=" +
         std::to_string(contract.namespace_segment_sites) +
         ";import_edge_candidate_sites=" +
         std::to_string(contract.import_edge_candidate_sites) +
         ";object_pointer_type_sites=" +
         std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" +
         std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3PublicPrivateApiPartitionLoweringLaneContract;
}

bool IsValidObjc3IncrementalModuleCacheInvalidationLoweringContract(
    const Objc3IncrementalModuleCacheInvalidationLoweringContract &contract) {
  if (contract.namespace_segment_sites >
          contract.incremental_module_cache_invalidation_sites ||
      contract.import_edge_candidate_sites >
          contract.incremental_module_cache_invalidation_sites ||
      contract.object_pointer_type_sites < contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites >
          contract.incremental_module_cache_invalidation_sites ||
      contract.normalized_sites >
          contract.incremental_module_cache_invalidation_sites ||
      contract.cache_invalidation_candidate_sites >
          contract.incremental_module_cache_invalidation_sites ||
      contract.contract_violation_sites >
          contract.incremental_module_cache_invalidation_sites) {
    return false;
  }
  if (contract.normalized_sites +
          contract.cache_invalidation_candidate_sites >
      contract.incremental_module_cache_invalidation_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites !=
           contract.incremental_module_cache_invalidation_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3IncrementalModuleCacheInvalidationLoweringReplayKey(
    const Objc3IncrementalModuleCacheInvalidationLoweringContract &contract) {
  return std::string("incremental_module_cache_invalidation_sites=") +
             std::to_string(contract.incremental_module_cache_invalidation_sites) +
         ";namespace_segment_sites=" +
         std::to_string(contract.namespace_segment_sites) +
         ";import_edge_candidate_sites=" +
         std::to_string(contract.import_edge_candidate_sites) +
         ";object_pointer_type_sites=" +
         std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" +
         std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";cache_invalidation_candidate_sites=" +
         std::to_string(contract.cache_invalidation_candidate_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3IncrementalModuleCacheInvalidationLoweringLaneContract;
}

bool IsValidObjc3CrossModuleConformanceLoweringContract(
    const Objc3CrossModuleConformanceLoweringContract &contract) {
  if (contract.namespace_segment_sites >
          contract.cross_module_conformance_sites ||
      contract.import_edge_candidate_sites >
          contract.cross_module_conformance_sites ||
      contract.object_pointer_type_sites < contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites >
          contract.cross_module_conformance_sites ||
      contract.normalized_sites > contract.cross_module_conformance_sites ||
      contract.cache_invalidation_candidate_sites >
          contract.cross_module_conformance_sites ||
      contract.contract_violation_sites >
          contract.cross_module_conformance_sites) {
    return false;
  }
  if (contract.normalized_sites +
          contract.cache_invalidation_candidate_sites >
      contract.cross_module_conformance_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.cross_module_conformance_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3CrossModuleConformanceLoweringReplayKey(
    const Objc3CrossModuleConformanceLoweringContract &contract) {
  return std::string("cross_module_conformance_sites=") +
             std::to_string(contract.cross_module_conformance_sites) +
         ";namespace_segment_sites=" +
         std::to_string(contract.namespace_segment_sites) +
         ";import_edge_candidate_sites=" +
         std::to_string(contract.import_edge_candidate_sites) +
         ";object_pointer_type_sites=" +
         std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" +
         std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";cache_invalidation_candidate_sites=" +
         std::to_string(contract.cache_invalidation_candidate_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3CrossModuleConformanceLoweringLaneContract;
}

bool IsValidObjc3ThrowsPropagationLoweringContract(
    const Objc3ThrowsPropagationLoweringContract &contract) {
  if (contract.namespace_segment_sites > contract.throws_propagation_sites ||
      contract.import_edge_candidate_sites > contract.throws_propagation_sites ||
      contract.object_pointer_type_sites < contract.import_edge_candidate_sites ||
      contract.pointer_declarator_sites > contract.throws_propagation_sites ||
      contract.normalized_sites > contract.throws_propagation_sites ||
      contract.cache_invalidation_candidate_sites >
          contract.throws_propagation_sites ||
      contract.contract_violation_sites > contract.throws_propagation_sites) {
    return false;
  }
  if (contract.normalized_sites +
          contract.cache_invalidation_candidate_sites >
      contract.throws_propagation_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.throws_propagation_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ThrowsPropagationLoweringReplayKey(
    const Objc3ThrowsPropagationLoweringContract &contract) {
  return std::string("throws_propagation_sites=") +
             std::to_string(contract.throws_propagation_sites) +
         ";namespace_segment_sites=" +
         std::to_string(contract.namespace_segment_sites) +
         ";import_edge_candidate_sites=" +
         std::to_string(contract.import_edge_candidate_sites) +
         ";object_pointer_type_sites=" +
         std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" +
         std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";cache_invalidation_candidate_sites=" +
         std::to_string(contract.cache_invalidation_candidate_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3ThrowsPropagationLoweringLaneContract;
}

bool IsValidObjc3ResultLikeLoweringContract(
    const Objc3ResultLikeLoweringContract &contract) {
  if (contract.result_success_sites > contract.result_like_sites ||
      contract.result_failure_sites > contract.result_like_sites ||
      contract.result_branch_sites > contract.result_like_sites ||
      contract.result_payload_sites > contract.result_like_sites ||
      contract.normalized_sites > contract.result_like_sites ||
      contract.branch_merge_sites > contract.result_like_sites ||
      contract.contract_violation_sites > contract.result_like_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.branch_merge_sites !=
      contract.result_like_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ResultLikeLoweringReplayKey(
    const Objc3ResultLikeLoweringContract &contract) {
  return std::string("result_like_sites=") +
             std::to_string(contract.result_like_sites) +
         ";result_success_sites=" +
         std::to_string(contract.result_success_sites) +
         ";result_failure_sites=" +
         std::to_string(contract.result_failure_sites) +
         ";result_branch_sites=" +
         std::to_string(contract.result_branch_sites) +
         ";result_payload_sites=" +
         std::to_string(contract.result_payload_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";branch_merge_sites=" + std::to_string(contract.branch_merge_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3ResultLikeLoweringLaneContract;
}

bool IsValidObjc3NSErrorBridgingLoweringContract(
    const Objc3NSErrorBridgingLoweringContract &contract) {
  if (contract.ns_error_parameter_sites > contract.ns_error_bridging_sites ||
      contract.ns_error_out_parameter_sites > contract.ns_error_parameter_sites ||
      contract.ns_error_bridge_path_sites > contract.ns_error_out_parameter_sites ||
      contract.ns_error_bridge_path_sites > contract.failable_call_sites ||
      contract.failable_call_sites > contract.ns_error_bridging_sites ||
      contract.normalized_sites > contract.ns_error_bridging_sites ||
      contract.bridge_boundary_sites > contract.ns_error_bridging_sites ||
      contract.contract_violation_sites > contract.ns_error_bridging_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.bridge_boundary_sites !=
      contract.ns_error_bridging_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3NSErrorBridgingLoweringReplayKey(
    const Objc3NSErrorBridgingLoweringContract &contract) {
  return std::string("ns_error_bridging_sites=") +
             std::to_string(contract.ns_error_bridging_sites) +
         ";ns_error_parameter_sites=" +
         std::to_string(contract.ns_error_parameter_sites) +
         ";ns_error_out_parameter_sites=" +
         std::to_string(contract.ns_error_out_parameter_sites) +
         ";ns_error_bridge_path_sites=" +
         std::to_string(contract.ns_error_bridge_path_sites) +
         ";failable_call_sites=" +
         std::to_string(contract.failable_call_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";bridge_boundary_sites=" + std::to_string(contract.bridge_boundary_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3NSErrorBridgingLoweringLaneContract;
}

bool IsValidObjc3UnsafePointerExtensionLoweringContract(
    const Objc3UnsafePointerExtensionLoweringContract &contract) {
  if (contract.unsafe_keyword_sites > contract.unsafe_pointer_extension_sites ||
      contract.pointer_arithmetic_sites >
          contract.unsafe_pointer_extension_sites ||
      contract.raw_pointer_type_sites > contract.unsafe_pointer_extension_sites ||
      contract.unsafe_operation_sites > contract.unsafe_pointer_extension_sites ||
      contract.normalized_sites > contract.unsafe_pointer_extension_sites ||
      contract.gate_blocked_sites > contract.unsafe_pointer_extension_sites ||
      contract.contract_violation_sites > contract.unsafe_pointer_extension_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.gate_blocked_sites !=
      contract.unsafe_pointer_extension_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3UnsafePointerExtensionLoweringReplayKey(
    const Objc3UnsafePointerExtensionLoweringContract &contract) {
  return std::string("unsafe_pointer_extension_sites=") +
             std::to_string(contract.unsafe_pointer_extension_sites) +
         ";unsafe_keyword_sites=" +
         std::to_string(contract.unsafe_keyword_sites) +
         ";pointer_arithmetic_sites=" +
         std::to_string(contract.pointer_arithmetic_sites) +
         ";raw_pointer_type_sites=" +
         std::to_string(contract.raw_pointer_type_sites) +
         ";unsafe_operation_sites=" +
         std::to_string(contract.unsafe_operation_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";gate_blocked_sites=" + std::to_string(contract.gate_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3UnsafePointerExtensionLoweringLaneContract;
}
