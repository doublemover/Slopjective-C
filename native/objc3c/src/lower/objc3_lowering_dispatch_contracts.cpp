#include "lower/objc3_lowering_contract.h"
#include "lower/metadata/lowering_metadata_helpers.h"

#include <string>

bool IsValidRuntimeDispatchSymbol(const std::string &symbol) {
  return symbol == kObjc3RuntimeDispatchSymbol;
}

bool TryNormalizeObjc3LoweringContract(const Objc3LoweringContract &input,
                                       Objc3LoweringContract &normalized,
                                       std::string &error) {
  if (input.max_message_send_args > kObjc3RuntimeDispatchMaxArgs) {
    error = "invalid lowering contract max_message_send_args: " +
            std::to_string(input.max_message_send_args) + " (expected <= " +
            std::to_string(kObjc3RuntimeDispatchMaxArgs) + ")";
    return false;
  }
  if (!IsValidRuntimeDispatchSymbol(input.runtime_dispatch_symbol)) {
    error =
        "invalid lowering contract runtime_dispatch_symbol (expected objc3_runtime_dispatch_i32): " +
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

std::string Objc3LoweringIRBoundaryReplayKey(
    const Objc3LoweringIRBoundary &boundary) {
  return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +
         ";runtime_dispatch_arg_slots=" +
         std::to_string(boundary.runtime_dispatch_arg_slots) +
         ";selector_global_ordering=" + boundary.selector_global_ordering;
}

bool UsesCanonicalObjc3RuntimeDispatchEntrypoint(
    const std::string &dispatch_surface_family) {
  return dispatch_surface_family == kObjc3DispatchSurfaceInstanceFamily ||
         dispatch_surface_family == kObjc3DispatchSurfaceClassFamily ||
         dispatch_surface_family == kObjc3DispatchSurfaceSuperFamily ||
         dispatch_surface_family == kObjc3DispatchSurfaceDynamicFamily;
}

bool RequiresFailClosedObjc3RuntimeDispatchError(
    const std::string &dispatch_surface_family) {
  return dispatch_surface_family == kObjc3DispatchSurfaceDirectFamily;
}

const char *Objc3DispatchSurfaceRuntimeEntrypointSymbol(
    const std::string &dispatch_surface_family) {
  return UsesCanonicalObjc3RuntimeDispatchEntrypoint(dispatch_surface_family)
             ? kObjc3RuntimeDispatchLoweringCanonicalEntrypointSymbol
             : "";
}

bool IsValidObjc3RuntimeLinkHostLinkContract(
    const Objc3RuntimeLinkHostLinkContract &contract) {
  if (!IsValidRuntimeDispatchSymbol(contract.runtime_dispatch_symbol)) {
    return false;
  }
  if (contract.runtime_dispatch_arg_slots > kObjc3RuntimeDispatchMaxArgs) {
    return false;
  }
  if (contract.runtime_link_required_sites > contract.message_send_sites) {
    return false;
  }
  if (contract.runtime_link_required_sites +
          contract.runtime_link_elided_sites !=
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

std::string Objc3RuntimeLinkHostLinkReplayKey(
    const Objc3RuntimeLinkHostLinkContract &contract) {
  return std::string("message_send_sites=") +
         std::to_string(contract.message_send_sites) +
         ";runtime_link_required_sites=" +
         std::to_string(contract.runtime_link_required_sites) +
         ";runtime_link_elided_sites=" +
         std::to_string(contract.runtime_link_elided_sites) +
         ";runtime_dispatch_arg_slots=" +
         std::to_string(contract.runtime_dispatch_arg_slots) +
         ";runtime_dispatch_declaration_parameter_count=" +
         std::to_string(contract.runtime_dispatch_declaration_parameter_count) +
         ";runtime_dispatch_symbol=" + contract.runtime_dispatch_symbol +
         ";default_runtime_dispatch_symbol_binding=" +
         BoolToken(contract.default_runtime_dispatch_symbol_binding) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3RuntimeLinkHostLinkLaneContract;
}

bool IsValidObjc3RuntimeDispatchLoweringAbiContract(
    const Objc3RuntimeDispatchLoweringAbiContract &contract) {
  if (contract.fixed_argument_slot_count > kObjc3RuntimeDispatchMaxArgs) {
    return false;
  }
  if (contract.runtime_dispatch_parameter_count !=
      contract.fixed_argument_slot_count + 2u) {
    return false;
  }
  if (contract.lowering_boundary_model !=
      kObjc3RuntimeDispatchLoweringAbiBoundaryModel) {
    return false;
  }
  if (contract.canonical_runtime_dispatch_symbol !=
      kObjc3RuntimeDispatchLoweringCanonicalEntrypointSymbol) {
    return false;
  }
  if (!IsValidRuntimeDispatchSymbol(contract.default_lowering_target_symbol)) {
    return false;
  }
  if (contract.default_lowering_target_symbol !=
      contract.canonical_runtime_dispatch_symbol) {
    return false;
  }
  if (contract.selector_lookup_symbol !=
      kObjc3RuntimeDispatchLoweringSelectorLookupSymbol) {
    return false;
  }
  if (contract.selector_handle_type !=
      kObjc3RuntimeDispatchLoweringSelectorHandleType) {
    return false;
  }
  if (contract.receiver_abi_type !=
          kObjc3RuntimeDispatchLoweringReceiverAbiType ||
      contract.selector_abi_type !=
          kObjc3RuntimeDispatchLoweringSelectorAbiType ||
      contract.argument_abi_type !=
          kObjc3RuntimeDispatchLoweringArgumentAbiType ||
      contract.result_abi_type != kObjc3RuntimeDispatchLoweringResultAbiType) {
    return false;
  }
  if (contract.selector_operand_model !=
          kObjc3RuntimeDispatchLoweringSelectorOperandModel ||
      contract.selector_handle_model !=
          kObjc3RuntimeDispatchLoweringSelectorHandleModel ||
      contract.argument_padding_model !=
          kObjc3RuntimeDispatchLoweringArgumentPaddingModel ||
      contract.default_lowering_target_model !=
          kObjc3RuntimeDispatchLiveCutoverDefaultTargetModel ||
      contract.strict_dispatch_error_model !=
          kObjc3RuntimeDispatchLoweringStrictDispatchErrorModel ||
      contract.deferred_cases_model !=
          kObjc3RuntimeDispatchLiveCutoverDeferredCasesModel) {
    return false;
  }
  return contract.fail_closed;
}

std::string Objc3RuntimeDispatchLoweringAbiReplayKey(
    const Objc3RuntimeDispatchLoweringAbiContract &contract) {
  return std::string("message_send_sites=") +
         std::to_string(contract.message_send_sites) +
         ";fixed_argument_slot_count=" +
         std::to_string(contract.fixed_argument_slot_count) +
         ";runtime_dispatch_parameter_count=" +
         std::to_string(contract.runtime_dispatch_parameter_count) +
         ";lowering_boundary_model=" + contract.lowering_boundary_model +
         ";canonical_runtime_dispatch_symbol=" +
         contract.canonical_runtime_dispatch_symbol +
         ";default_lowering_target_symbol=" +
         contract.default_lowering_target_symbol +
         ";selector_lookup_symbol=" + contract.selector_lookup_symbol +
         ";selector_handle_type=" + contract.selector_handle_type +
         ";receiver_abi_type=" + contract.receiver_abi_type +
         ";selector_abi_type=" + contract.selector_abi_type +
         ";argument_abi_type=" + contract.argument_abi_type +
         ";result_abi_type=" + contract.result_abi_type +
         ";selector_operand_model=" + contract.selector_operand_model +
         ";selector_handle_model=" + contract.selector_handle_model +
         ";argument_padding_model=" + contract.argument_padding_model +
         ";default_lowering_target_model=" +
         contract.default_lowering_target_model +
         ";strict_dispatch_error_model=" +
         contract.strict_dispatch_error_model +
         ";deferred_cases_model=" + contract.deferred_cases_model +
         ";fail_closed=" + BoolToken(contract.fail_closed) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";contract_id=" + kObjc3RuntimeDispatchLoweringAbiContractId;
}

std::string Objc3RuntimeDispatchLoweringAbiBoundarySummary(
    const Objc3RuntimeDispatchLoweringAbiContract &contract) {
  return std::string("contract=") +
         kObjc3RuntimeDispatchLoweringAbiContractId +
         ";canonical_runtime_dispatch_symbol=" +
         contract.canonical_runtime_dispatch_symbol +
         ";default_lowering_target_symbol=" +
         contract.default_lowering_target_symbol +
         ";selector_lookup_symbol=" + contract.selector_lookup_symbol +
         ";selector_handle_type=" + contract.selector_handle_type +
         ";receiver_abi_type=" + contract.receiver_abi_type +
         ";argument_abi_type=" + contract.argument_abi_type +
         ";fixed_argument_slot_count=" +
         std::to_string(contract.fixed_argument_slot_count) +
         ";result_abi_type=" + contract.result_abi_type +
         ";default_lowering_target_model=" +
         contract.default_lowering_target_model +
         ";deferred_cases_model=" + contract.deferred_cases_model;
}
