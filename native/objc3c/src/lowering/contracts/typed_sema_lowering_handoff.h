#pragma once

#include "lower/contracts/lowering_ownership_contracts.h"

#include <cstddef>
#include <string>

struct Objc3TypedSemaLoweringHandoffContract {
  std::string typed_semantic_handoff_owner = kObjc3TypedSemanticHandoffOwner;
  std::string lowering_consumer_owner =
      kObjc3LoweringSemanticBoundaryConsumerOwner;
  std::string owner_model = kObjc3LoweringNoRetiredRouteOwnerModel;
  std::size_t global_type_entries = 0;
  std::size_t function_type_entries = 0;
  std::size_t function_param_slots = 0;
  std::size_t method_type_entries = 0;
  std::size_t method_param_slots = 0;
  std::size_t interface_type_entries = 0;
  std::size_t implementation_type_entries = 0;
  std::size_t callable_type_contract_violations = 0;
  bool type_metadata_deterministic = false;
  bool semantic_diagnostics_clear = false;
  bool callable_surfaces_typed = false;
  bool runtime_metadata_ready = false;
  bool owner_contract_recorded = false;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
  bool deterministic = false;
  std::string replay_key;
};

inline std::size_t Objc3TypedSemaLoweringHandoffTypeEntryCount(
    const Objc3TypedSemaLoweringHandoffContract &contract) {
  return contract.global_type_entries + contract.function_type_entries +
         contract.method_type_entries + contract.interface_type_entries +
         contract.implementation_type_entries;
}

inline std::string BuildObjc3TypedSemaLoweringHandoffReplayKey(
    const Objc3TypedSemaLoweringHandoffContract &contract) {
  return "typed-sema-lowering:globals=" +
         std::to_string(contract.global_type_entries) +
         ";functions=" + std::to_string(contract.function_type_entries) +
         ";function_params=" + std::to_string(contract.function_param_slots) +
         ";methods=" + std::to_string(contract.method_type_entries) +
         ";method_params=" + std::to_string(contract.method_param_slots) +
         ";interfaces=" + std::to_string(contract.interface_type_entries) +
         ";implementations=" +
         std::to_string(contract.implementation_type_entries) +
         ";callable_violations=" +
         std::to_string(contract.callable_type_contract_violations) +
         ";owner_contract_recorded=" +
         (contract.owner_contract_recorded ? "true" : "false") +
         ";typed_semantic_handoff_owner=" +
         contract.typed_semantic_handoff_owner +
         ";lowering_consumer_owner=" + contract.lowering_consumer_owner +
         ";owner_model=" + contract.owner_model +
         ";strict_no_retired_route=" +
         (contract.strict_no_retired_route ? "true" : "false") +
         ";strict_no_compatibility=" +
         (contract.strict_no_compatibility ? "true" : "false");
}

inline bool IsReadyObjc3TypedSemaLoweringHandoffContract(
    const Objc3TypedSemaLoweringHandoffContract &contract) {
  return contract.type_metadata_deterministic &&
         contract.semantic_diagnostics_clear &&
         contract.callable_surfaces_typed &&
         contract.runtime_metadata_ready &&
         contract.owner_contract_recorded &&
         Objc3LoweringStrictOwnerModelIsReady(
             contract.typed_semantic_handoff_owner,
             contract.owner_model,
             contract.strict_no_retired_route,
             contract.strict_no_compatibility) &&
         Objc3LoweringStrictOwnerModelIsReady(
             contract.lowering_consumer_owner,
             contract.owner_model,
             contract.strict_no_retired_route,
             contract.strict_no_compatibility) &&
         contract.callable_type_contract_violations == 0u &&
         contract.deterministic && !contract.replay_key.empty();
}
