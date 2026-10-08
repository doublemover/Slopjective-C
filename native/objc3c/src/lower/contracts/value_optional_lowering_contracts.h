#pragma once

#include "ast/objc3_ast_value_optional_type.h"

inline constexpr const char *kObjc3ValueOptionalLoweringContractId =
    "objc3c.value_optional.lowering.contract.v1";
inline constexpr const char *kObjc3ValueOptionalLoweringStatus =
    "stable-packed-abi-carrier-runtime-execution-supported";

struct Objc3ValueOptionalLoweringContract {
  const char *contract_id = kObjc3ValueOptionalLoweringContractId;
  const char *abi_layout_id = kObjc3ValueOptionalAbiLayoutId;
  const char *status = kObjc3ValueOptionalLoweringStatus;
  const char *executable_lowering_status =
      kObjc3ValueOptionalExecutableLoweringStatus;
  const char *presence_field = kObjc3ValueOptionalPresenceField;
  const char *payload_storage_field = kObjc3ValueOptionalPayloadStorageField;
  const char *payload_cleanup_contract =
      kObjc3ValueOptionalPayloadCleanupContract;
  const char *absence_state = kObjc3ValueOptionalAbsenceState;
  const char *presence_state = kObjc3ValueOptionalPresenceState;
  const char *construction_contract =
      kObjc3ValueOptionalConstructionContract;
  const char *absent_construction_kind =
      kObjc3ValueOptionalAbsentConstructionKind;
  const char *present_construction_kind =
      kObjc3ValueOptionalPresentConstructionKind;
  const char *binding_failure_diagnostic =
      kObjc3ValueOptionalBindingFailureDiagnostic;
  const char *unwrap_failure_diagnostic =
      kObjc3ValueOptionalUnwrapFailureDiagnostic;
  const char *nil_bridge_diagnostic =
      kObjc3ValueOptionalNilBridgeDiagnostic;
  const char *remaining_runtime_boundary =
      kObjc3ValueOptionalRemainingRuntimeBoundary;
  bool source_type_signature_admitted = true;
  bool semantic_value_model_supported = true;
  bool stable_abi_layout_contract_supported = true;
  bool executable_lowering_contract_supported = true;
  bool explicit_present_absent_construction_modeled = true;
  bool explicit_absent_construction_supported = true;
  bool explicit_present_construction_supported = true;
  bool absent_construction_forbids_payload = true;
  bool present_construction_requires_payload = true;
  bool binding_narrowing_supported = true;
  bool binding_failure_diagnostic_supported = true;
  bool unwrap_requires_presence_check = true;
  bool unwrap_failure_diagnostic_supported = true;
  bool nil_bridge_diagnostic_supported = true;
  bool runtime_construction_supported = true;
  bool runtime_unwrap_supported = true;
  bool unchecked_unwrap_supported = false;
  bool ir_payload_emission_supported = true;
  bool call_abi_lowering_supported = true;
  bool nil_to_scalar_coercion_allowed = false;
  bool implicit_nil_absence_allowed = false;
  bool nullable_pointer_conversion_allowed = false;
  bool throws_result_conversion_allowed = false;
};

inline bool Objc3ValueOptionalContractTextEquals(
    const char *actual,
    const char *expected) {
  return actual != nullptr && expected != nullptr &&
         std::string(actual) == expected;
}

inline bool IsReadyObjc3ValueOptionalLoweringContract(
    const Objc3ValueOptionalLoweringContract &contract) {
  return Objc3ValueOptionalContractTextEquals(
             contract.contract_id, kObjc3ValueOptionalLoweringContractId) &&
         Objc3ValueOptionalContractTextEquals(
             contract.abi_layout_id, kObjc3ValueOptionalAbiLayoutId) &&
         Objc3ValueOptionalContractTextEquals(
             contract.status, kObjc3ValueOptionalLoweringStatus) &&
         Objc3ValueOptionalContractTextEquals(
             contract.executable_lowering_status,
             kObjc3ValueOptionalExecutableLoweringStatus) &&
         Objc3ValueOptionalContractTextEquals(
             contract.presence_field, kObjc3ValueOptionalPresenceField) &&
         Objc3ValueOptionalContractTextEquals(
             contract.payload_storage_field,
             kObjc3ValueOptionalPayloadStorageField) &&
         Objc3ValueOptionalContractTextEquals(
             contract.payload_cleanup_contract,
             kObjc3ValueOptionalPayloadCleanupContract) &&
         Objc3ValueOptionalContractTextEquals(
             contract.absence_state, kObjc3ValueOptionalAbsenceState) &&
         Objc3ValueOptionalContractTextEquals(
             contract.presence_state, kObjc3ValueOptionalPresenceState) &&
         Objc3ValueOptionalContractTextEquals(
             contract.construction_contract,
             kObjc3ValueOptionalConstructionContract) &&
         Objc3ValueOptionalContractTextEquals(
             contract.absent_construction_kind,
             kObjc3ValueOptionalAbsentConstructionKind) &&
         Objc3ValueOptionalContractTextEquals(
             contract.present_construction_kind,
             kObjc3ValueOptionalPresentConstructionKind) &&
         Objc3ValueOptionalContractTextEquals(
             contract.binding_failure_diagnostic,
             kObjc3ValueOptionalBindingFailureDiagnostic) &&
         Objc3ValueOptionalContractTextEquals(
             contract.unwrap_failure_diagnostic,
             kObjc3ValueOptionalUnwrapFailureDiagnostic) &&
         Objc3ValueOptionalContractTextEquals(
             contract.nil_bridge_diagnostic,
             kObjc3ValueOptionalNilBridgeDiagnostic) &&
         Objc3ValueOptionalContractTextEquals(
             contract.remaining_runtime_boundary,
             kObjc3ValueOptionalRemainingRuntimeBoundary) &&
         contract.source_type_signature_admitted &&
         contract.semantic_value_model_supported &&
         contract.stable_abi_layout_contract_supported &&
         contract.executable_lowering_contract_supported &&
         contract.explicit_present_absent_construction_modeled &&
         contract.explicit_absent_construction_supported &&
         contract.explicit_present_construction_supported &&
         contract.absent_construction_forbids_payload &&
         contract.present_construction_requires_payload &&
         contract.binding_narrowing_supported &&
         contract.binding_failure_diagnostic_supported &&
         contract.unwrap_requires_presence_check &&
         contract.unwrap_failure_diagnostic_supported &&
         contract.nil_bridge_diagnostic_supported &&
         contract.runtime_construction_supported &&
         contract.runtime_unwrap_supported &&
         !contract.unchecked_unwrap_supported &&
         contract.ir_payload_emission_supported &&
         contract.call_abi_lowering_supported &&
         !contract.nil_to_scalar_coercion_allowed &&
         !contract.implicit_nil_absence_allowed &&
         !contract.nullable_pointer_conversion_allowed &&
         !contract.throws_result_conversion_allowed;
}

inline bool Objc3ValueOptionalDescriptorMatchesLoweringContract(
    const Objc3ValueOptionalTypeDescriptor &descriptor,
    const Objc3ValueOptionalLoweringContract &contract) {
  return Objc3ValueOptionalHasExecutableLoweringContract(descriptor) &&
         Objc3ValueOptionalRuntimeAbiReady(descriptor) &&
         IsReadyObjc3ValueOptionalLoweringContract(contract) &&
         descriptor.abi_layout_id == contract.abi_layout_id &&
         descriptor.presence_field == contract.presence_field &&
         descriptor.payload_storage_field == contract.payload_storage_field &&
         descriptor.executable_lowering_status ==
             contract.executable_lowering_status &&
         descriptor.construction_contract == contract.construction_contract &&
         descriptor.absent_construction_kind ==
             contract.absent_construction_kind &&
         descriptor.present_construction_kind ==
             contract.present_construction_kind &&
         descriptor.binding_failure_diagnostic ==
             contract.binding_failure_diagnostic &&
         descriptor.unwrap_failure_diagnostic ==
             contract.unwrap_failure_diagnostic &&
         descriptor.nil_bridge_diagnostic == contract.nil_bridge_diagnostic &&
         descriptor.remaining_runtime_boundary ==
             contract.remaining_runtime_boundary;
}
