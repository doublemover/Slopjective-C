#pragma once

#include <cstddef>

#include "lowering/contracts/typed_sema_lowering_handoff.h"
#include "sema/objc3_sema_contract.h"

inline bool IsObjc3SemanticCanonicalTypeReadyForLowering(
    const Objc3SemanticCanonicalType &type) {
  return type.deterministic &&
         type.kind != Objc3SemanticCanonicalTypeKind::Unknown &&
         !type.canonical_spelling.empty() && !type.has_invalid_type_suffix;
}

inline std::size_t CountObjc3FunctionLoweringTypeContractViolations(
    const Objc3SemanticFunctionTypeMetadata &function_metadata) {
  std::size_t violations = 0;
  if (function_metadata.param_canonical_types.size() !=
      function_metadata.arity) {
    ++violations;
  }
  if (!IsObjc3SemanticCanonicalTypeReadyForLowering(
          function_metadata.return_canonical_type)) {
    ++violations;
  }
  for (const Objc3SemanticCanonicalType &param_type :
       function_metadata.param_canonical_types) {
    if (!IsObjc3SemanticCanonicalTypeReadyForLowering(param_type)) {
      ++violations;
    }
  }
  return violations;
}

inline std::size_t CountObjc3MethodLoweringTypeContractViolations(
    const Objc3SemanticMethodTypeMetadata &method_metadata) {
  std::size_t violations = 0;
  if (method_metadata.param_canonical_types.size() !=
      method_metadata.arity) {
    ++violations;
  }
  if (!IsObjc3SemanticCanonicalTypeReadyForLowering(
          method_metadata.return_canonical_type)) {
    ++violations;
  }
  for (const Objc3SemanticCanonicalType &param_type :
       method_metadata.param_canonical_types) {
    if (!IsObjc3SemanticCanonicalTypeReadyForLowering(param_type)) {
      ++violations;
    }
  }
  return violations;
}

inline Objc3TypedSemaLoweringHandoffContract
BuildObjc3TypedSemaLoweringHandoffContract(
    const Objc3SemanticTypeMetadataHandoff &handoff,
    bool semantic_diagnostics_clear) {
  Objc3TypedSemaLoweringHandoffContract contract;
  contract.global_type_entries = handoff.global_names_lexicographic.size();
  contract.function_type_entries = handoff.functions_lexicographic.size();
  contract.interface_type_entries = handoff.interfaces_lexicographic.size();
  contract.implementation_type_entries =
      handoff.implementations_lexicographic.size();
  contract.type_metadata_deterministic =
      IsDeterministicSemanticTypeMetadataHandoff(handoff);
  contract.semantic_diagnostics_clear = semantic_diagnostics_clear;
  contract.runtime_metadata_ready =
      handoff.runtime_link_host_link_summary.deterministic &&
      handoff.dispatch_abi_marshalling_summary.deterministic &&
      handoff.retain_release_operation_summary.deterministic;

  for (const Objc3SemanticFunctionTypeMetadata &function_metadata :
       handoff.functions_lexicographic) {
    contract.function_param_slots += function_metadata.arity;
    contract.callable_type_contract_violations +=
        CountObjc3FunctionLoweringTypeContractViolations(function_metadata);
  }
  for (const Objc3SemanticInterfaceTypeMetadata &interface_metadata :
       handoff.interfaces_lexicographic) {
    contract.method_type_entries +=
        interface_metadata.methods_lexicographic.size();
    for (const Objc3SemanticMethodTypeMetadata &method_metadata :
         interface_metadata.methods_lexicographic) {
      contract.method_param_slots += method_metadata.arity;
      contract.callable_type_contract_violations +=
          CountObjc3MethodLoweringTypeContractViolations(method_metadata);
    }
  }
  for (const Objc3SemanticImplementationTypeMetadata &implementation_metadata :
       handoff.implementations_lexicographic) {
    contract.method_type_entries +=
        implementation_metadata.methods_lexicographic.size();
    for (const Objc3SemanticMethodTypeMetadata &method_metadata :
         implementation_metadata.methods_lexicographic) {
      contract.method_param_slots += method_metadata.arity;
      contract.callable_type_contract_violations +=
          CountObjc3MethodLoweringTypeContractViolations(method_metadata);
    }
  }

  contract.callable_surfaces_typed =
      contract.callable_type_contract_violations == 0u;
  contract.replay_key =
      BuildObjc3TypedSemaLoweringHandoffReplayKey(contract);
  contract.deterministic = contract.type_metadata_deterministic &&
                           contract.callable_surfaces_typed &&
                           contract.runtime_metadata_ready;
  return contract;
}
