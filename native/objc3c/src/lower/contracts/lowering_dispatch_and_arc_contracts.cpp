#include "lower/objc3_lowering_contract.h"

#include "lower/metadata/lowering_metadata_helpers.h"

#include <sstream>
#include <string>

std::string Objc3RuntimeDispatchDeclarationReplayKey(
    const Objc3LoweringIRBoundary &boundary) {
  std::ostringstream out;
  out << "declare i32 @" << boundary.runtime_dispatch_symbol << "(i32, ptr";
  for (std::size_t i = 0; i < boundary.runtime_dispatch_arg_slots; ++i) {
    out << ", i32";
  }
  out << ")";
  return out.str();
}

bool IsValidObjc3MethodLookupOverrideConflictContract(
    const Objc3MethodLookupOverrideConflictContract &contract) {
  if (contract.method_lookup_hits > contract.method_lookup_sites ||
      contract.method_lookup_misses > contract.method_lookup_sites ||
      contract.method_lookup_hits + contract.method_lookup_misses !=
          contract.method_lookup_sites) {
    return false;
  }
  if (contract.override_lookup_hits > contract.override_lookup_sites ||
      contract.override_lookup_misses > contract.override_lookup_sites ||
      contract.override_lookup_hits + contract.override_lookup_misses !=
          contract.override_lookup_sites) {
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

std::string Objc3MethodLookupOverrideConflictReplayKey(
    const Objc3MethodLookupOverrideConflictContract &contract) {
  return std::string("method_lookup_sites=") +
         std::to_string(contract.method_lookup_sites) +
         ";method_lookup_hits=" +
         std::to_string(contract.method_lookup_hits) +
         ";method_lookup_misses=" +
         std::to_string(contract.method_lookup_misses) +
         ";override_lookup_sites=" +
         std::to_string(contract.override_lookup_sites) +
         ";override_lookup_hits=" +
         std::to_string(contract.override_lookup_hits) +
         ";override_lookup_misses=" +
         std::to_string(contract.override_lookup_misses) +
         ";override_conflicts=" +
         std::to_string(contract.override_conflicts) +
         ";unresolved_base_interfaces=" +
         std::to_string(contract.unresolved_base_interfaces) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3MethodLookupOverrideConflictLaneContract;
}

Objc3PropertySynthesisIvarBindingContract
Objc3DefaultPropertySynthesisIvarBindingContract(
    std::size_t property_synthesis_sites,
    bool deterministic) {
  Objc3PropertySynthesisIvarBindingContract contract;
  contract.property_synthesis_sites = property_synthesis_sites;
  contract.property_synthesis_explicit_ivar_bindings = 0;
  contract.property_synthesis_default_ivar_bindings = property_synthesis_sites;
  contract.interface_owned_property_synthesis_sites = property_synthesis_sites;
  contract.implementation_property_redeclaration_sites = 0;
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
      contract.property_synthesis_explicit_ivar_bindings >
          contract.property_synthesis_sites ||
      contract.property_synthesis_default_ivar_bindings >
          contract.property_synthesis_sites) {
    return false;
  }
  if (contract.interface_owned_property_synthesis_sites !=
          contract.property_synthesis_sites ||
      contract.implementation_property_redeclaration_sites >
          contract.property_synthesis_sites) {
    return false;
  }
  if (contract.ivar_binding_sites != contract.property_synthesis_sites) {
    return false;
  }
  if (contract.ivar_binding_resolved > contract.ivar_binding_sites ||
      contract.ivar_binding_missing > contract.ivar_binding_sites ||
      contract.ivar_binding_conflicts > contract.ivar_binding_sites ||
      contract.ivar_binding_resolved + contract.ivar_binding_missing +
              contract.ivar_binding_conflicts !=
          contract.ivar_binding_sites) {
    return false;
  }
  return true;
}

std::string Objc3PropertySynthesisIvarBindingReplayKey(
    const Objc3PropertySynthesisIvarBindingContract &contract) {
  return std::string("property_synthesis_sites=") +
         std::to_string(contract.property_synthesis_sites) +
         ";property_synthesis_explicit_ivar_bindings=" +
         std::to_string(contract.property_synthesis_explicit_ivar_bindings) +
         ";property_synthesis_default_ivar_bindings=" +
         std::to_string(contract.property_synthesis_default_ivar_bindings) +
         ";interface_owned_property_synthesis_sites=" +
         std::to_string(contract.interface_owned_property_synthesis_sites) +
         ";implementation_property_redeclaration_sites=" +
         std::to_string(contract.implementation_property_redeclaration_sites) +
         ";ivar_binding_sites=" + std::to_string(contract.ivar_binding_sites) +
         ";ivar_binding_resolved=" +
         std::to_string(contract.ivar_binding_resolved) +
         ";ivar_binding_missing=" +
         std::to_string(contract.ivar_binding_missing) +
         ";ivar_binding_conflicts=" +
         std::to_string(contract.ivar_binding_conflicts) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3PropertySynthesisIvarBindingLaneContract;
}

bool IsValidObjc3IdClassSelObjectPointerTypecheckContract(
    const Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  const std::size_t computed_total =
      contract.id_typecheck_sites + contract.class_typecheck_sites +
      contract.sel_typecheck_sites + contract.object_pointer_typecheck_sites;
  return contract.total_typecheck_sites == computed_total;
}

std::string Objc3IdClassSelObjectPointerTypecheckReplayKey(
    const Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  return std::string("id_typecheck_sites=") +
         std::to_string(contract.id_typecheck_sites) +
         ";class_typecheck_sites=" +
         std::to_string(contract.class_typecheck_sites) +
         ";sel_typecheck_sites=" +
         std::to_string(contract.sel_typecheck_sites) +
         ";object_pointer_typecheck_sites=" +
         std::to_string(contract.object_pointer_typecheck_sites) +
         ";total_typecheck_sites=" +
         std::to_string(contract.total_typecheck_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3IdClassSelObjectPointerTypecheckLaneContract;
}

bool IsValidObjc3DispatchSurfaceClassificationContract(
    const Objc3DispatchSurfaceClassificationContract &contract) {
  const bool live_runtime_bindings_ok =
      contract.instance_entrypoint_family ==
          kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily &&
      contract.class_entrypoint_family ==
          kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily &&
      contract.super_entrypoint_family ==
          kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily &&
      contract.dynamic_entrypoint_family ==
          kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily;
  return live_runtime_bindings_ok &&
         contract.direct_entrypoint_family ==
             kObjc3DispatchSurfaceDirectDispatchBinding;
}

std::string Objc3DispatchSurfaceClassificationReplayKey(
    const Objc3DispatchSurfaceClassificationContract &contract) {
  return std::string("instance_dispatch_sites=") +
         std::to_string(contract.instance_dispatch_sites) +
         ";class_dispatch_sites=" +
         std::to_string(contract.class_dispatch_sites) +
         ";super_dispatch_sites=" +
         std::to_string(contract.super_dispatch_sites) +
         ";direct_dispatch_sites=" +
         std::to_string(contract.direct_dispatch_sites) +
         ";dynamic_dispatch_sites=" +
         std::to_string(contract.dynamic_dispatch_sites) +
         ";instance_entrypoint_family=" + contract.instance_entrypoint_family +
         ";class_entrypoint_family=" + contract.class_entrypoint_family +
         ";super_entrypoint_family=" + contract.super_entrypoint_family +
         ";direct_entrypoint_family=" + contract.direct_entrypoint_family +
         ";dynamic_entrypoint_family=" + contract.dynamic_entrypoint_family +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";contract_id=" + kObjc3DispatchSurfaceClassificationContractId;
}

bool IsValidObjc3MessageSendSelectorLoweringContract(
    const Objc3MessageSendSelectorLoweringContract &contract) {
  if (contract.unary_selector_sites + contract.keyword_selector_sites !=
      contract.message_send_sites) {
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
  if (contract.selector_literal_entries == 0 &&
      contract.selector_literal_characters != 0) {
    return false;
  }
  return true;
}

std::string Objc3MessageSendSelectorLoweringReplayKey(
    const Objc3MessageSendSelectorLoweringContract &contract) {
  return std::string("message_send_sites=") +
         std::to_string(contract.message_send_sites) +
         ";unary_selector_sites=" +
         std::to_string(contract.unary_selector_sites) +
         ";keyword_selector_sites=" +
         std::to_string(contract.keyword_selector_sites) +
         ";selector_piece_sites=" +
         std::to_string(contract.selector_piece_sites) +
         ";argument_expression_sites=" +
         std::to_string(contract.argument_expression_sites) +
         ";receiver_expression_sites=" +
         std::to_string(contract.receiver_expression_sites) +
         ";selector_literal_entries=" +
         std::to_string(contract.selector_literal_entries) +
         ";selector_literal_characters=" +
         std::to_string(contract.selector_literal_characters) +
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
  if (contract.argument_value_slots_marshaled >
      contract.argument_total_slots_marshaled) {
    return false;
  }
  if (contract.argument_padding_slots_marshaled +
          contract.argument_value_slots_marshaled !=
      contract.argument_total_slots_marshaled) {
    return false;
  }
  const std::size_t expected_total =
      contract.receiver_slots_marshaled + contract.selector_slots_marshaled +
      contract.argument_total_slots_marshaled;
  return contract.total_marshaled_slots == expected_total;
}

std::string Objc3DispatchAbiMarshallingReplayKey(
    const Objc3DispatchAbiMarshallingContract &contract) {
  return std::string("message_send_sites=") +
         std::to_string(contract.message_send_sites) +
         ";receiver_slots_marshaled=" +
         std::to_string(contract.receiver_slots_marshaled) +
         ";selector_slots_marshaled=" +
         std::to_string(contract.selector_slots_marshaled) +
         ";argument_value_slots_marshaled=" +
         std::to_string(contract.argument_value_slots_marshaled) +
         ";argument_padding_slots_marshaled=" +
         std::to_string(contract.argument_padding_slots_marshaled) +
         ";argument_total_slots_marshaled=" +
         std::to_string(contract.argument_total_slots_marshaled) +
         ";total_marshaled_slots=" +
         std::to_string(contract.total_marshaled_slots) +
         ";runtime_dispatch_arg_slots=" +
         std::to_string(contract.runtime_dispatch_arg_slots) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3DispatchAbiMarshallingLaneContract;
}

bool IsValidObjc3NilReceiverSemanticsFoldabilityContract(
    const Objc3NilReceiverSemanticsFoldabilityContract &contract) {
  if (contract.receiver_nil_literal_sites !=
      contract.nil_receiver_semantics_enabled_sites) {
    return false;
  }
  if (contract.nil_receiver_foldable_sites >
      contract.nil_receiver_semantics_enabled_sites) {
    return false;
  }
  if (contract.nil_receiver_runtime_dispatch_required_sites +
          contract.nil_receiver_foldable_sites !=
      contract.message_send_sites) {
    return false;
  }
  if (contract.nil_receiver_semantics_enabled_sites +
          contract.non_nil_receiver_sites !=
      contract.message_send_sites) {
    return false;
  }
  return contract.contract_violation_sites <= contract.message_send_sites;
}

std::string Objc3NilReceiverSemanticsFoldabilityReplayKey(
    const Objc3NilReceiverSemanticsFoldabilityContract &contract) {
  return std::string("message_send_sites=") +
         std::to_string(contract.message_send_sites) +
         ";receiver_nil_literal_sites=" +
         std::to_string(contract.receiver_nil_literal_sites) +
         ";nil_receiver_semantics_enabled_sites=" +
         std::to_string(contract.nil_receiver_semantics_enabled_sites) +
         ";nil_receiver_foldable_sites=" +
         std::to_string(contract.nil_receiver_foldable_sites) +
         ";nil_receiver_runtime_dispatch_required_sites=" +
         std::to_string(contract.nil_receiver_runtime_dispatch_required_sites) +
         ";non_nil_receiver_sites=" +
         std::to_string(contract.non_nil_receiver_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3NilReceiverSemanticsFoldabilityLaneContract;
}

bool IsValidObjc3SuperDispatchMethodFamilyContract(
    const Objc3SuperDispatchMethodFamilyContract &contract) {
  if (contract.receiver_super_identifier_sites !=
      contract.super_dispatch_enabled_sites) {
    return false;
  }
  if (contract.super_dispatch_requires_class_context_sites !=
      contract.super_dispatch_enabled_sites) {
    return false;
  }
  if (contract.method_family_init_sites + contract.method_family_copy_sites +
          contract.method_family_mutable_copy_sites +
          contract.method_family_new_sites + contract.method_family_none_sites !=
      contract.message_send_sites) {
    return false;
  }
  if (contract.method_family_returns_related_result_sites >
      contract.method_family_init_sites) {
    return false;
  }
  if (contract.method_family_returns_retained_result_sites >
      contract.message_send_sites) {
    return false;
  }
  return contract.contract_violation_sites <= contract.message_send_sites;
}

std::string Objc3SuperDispatchMethodFamilyReplayKey(
    const Objc3SuperDispatchMethodFamilyContract &contract) {
  return std::string("message_send_sites=") +
         std::to_string(contract.message_send_sites) +
         ";receiver_super_identifier_sites=" +
         std::to_string(contract.receiver_super_identifier_sites) +
         ";super_dispatch_enabled_sites=" +
         std::to_string(contract.super_dispatch_enabled_sites) +
         ";super_dispatch_requires_class_context_sites=" +
         std::to_string(contract.super_dispatch_requires_class_context_sites) +
         ";method_family_init_sites=" +
         std::to_string(contract.method_family_init_sites) +
         ";method_family_copy_sites=" +
         std::to_string(contract.method_family_copy_sites) +
         ";method_family_mutable_copy_sites=" +
         std::to_string(contract.method_family_mutable_copy_sites) +
         ";method_family_new_sites=" +
         std::to_string(contract.method_family_new_sites) +
         ";method_family_none_sites=" +
         std::to_string(contract.method_family_none_sites) +
         ";method_family_returns_retained_result_sites=" +
         std::to_string(contract.method_family_returns_retained_result_sites) +
         ";method_family_returns_related_result_sites=" +
         std::to_string(contract.method_family_returns_related_result_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3SuperDispatchMethodFamilyLaneContract;
}

bool IsValidObjc3OwnershipQualifierLoweringContract(
    const Objc3OwnershipQualifierLoweringContract &contract) {
  return contract.invalid_ownership_qualifier_sites <=
             contract.ownership_qualifier_sites &&
         contract.ownership_qualifier_sites <=
             contract.object_pointer_type_annotation_sites;
}

std::string Objc3OwnershipQualifierLoweringReplayKey(
    const Objc3OwnershipQualifierLoweringContract &contract) {
  return std::string("ownership_qualifier_sites=") +
         std::to_string(contract.ownership_qualifier_sites) +
         ";invalid_ownership_qualifier_sites=" +
         std::to_string(contract.invalid_ownership_qualifier_sites) +
         ";object_pointer_type_annotation_sites=" +
         std::to_string(contract.object_pointer_type_annotation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3OwnershipQualifierLoweringLaneContract;
}

bool IsValidObjc3RetainReleaseOperationLoweringContract(
    const Objc3RetainReleaseOperationLoweringContract &contract) {
  const std::size_t qualified_or_violation =
      contract.ownership_qualified_sites + contract.contract_violation_sites;
  return contract.retain_insertion_sites <= qualified_or_violation &&
         contract.release_insertion_sites <= qualified_or_violation &&
         contract.autorelease_insertion_sites <= qualified_or_violation;
}

std::string Objc3RetainReleaseOperationLoweringReplayKey(
    const Objc3RetainReleaseOperationLoweringContract &contract) {
  return std::string("ownership_qualified_sites=") +
         std::to_string(contract.ownership_qualified_sites) +
         ";retain_insertion_sites=" +
         std::to_string(contract.retain_insertion_sites) +
         ";release_insertion_sites=" +
         std::to_string(contract.release_insertion_sites) +
         ";autorelease_insertion_sites=" +
         std::to_string(contract.autorelease_insertion_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
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
  return std::string("scope_sites=") +
         std::to_string(contract.scope_sites) +
         ";scope_symbolized_sites=" +
         std::to_string(contract.scope_symbolized_sites) +
         ";max_scope_depth=" + std::to_string(contract.max_scope_depth) +
         ";scope_entry_transition_sites=" +
         std::to_string(contract.scope_entry_transition_sites) +
         ";scope_exit_transition_sites=" +
         std::to_string(contract.scope_exit_transition_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3AutoreleasePoolScopeLoweringLaneContract;
}
