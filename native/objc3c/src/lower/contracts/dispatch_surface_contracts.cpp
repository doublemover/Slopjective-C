#include "lower/contracts/dispatch_abi_marshalling_contracts.h"
#include "lower/contracts/dispatch_surface_classification_contracts.h"
#include "lower/contracts/message_send_selector_lowering_contracts.h"
#include "lower/contracts/nil_receiver_semantics_foldability_contracts.h"
#include "lower/contracts/runtime_dispatch_declaration_contracts.h"
#include "lower/contracts/super_dispatch_method_family_contracts.h"

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
