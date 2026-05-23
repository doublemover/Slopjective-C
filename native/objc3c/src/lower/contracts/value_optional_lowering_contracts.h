#pragma once

#include "ast/objc3_ast_value_optional_type.h"

inline constexpr const char *kObjc3ValueOptionalLoweringContractId =
    "objc3c.value_optional.lowering.contract.v1";
inline constexpr const char *kObjc3ValueOptionalLoweringStatus =
    "stable-abi-carrier-contract-runtime-execution-fail-closed";

struct Objc3ValueOptionalLoweringContract {
  const char *contract_id = kObjc3ValueOptionalLoweringContractId;
  const char *abi_layout_id = kObjc3ValueOptionalAbiLayoutId;
  const char *status = kObjc3ValueOptionalLoweringStatus;
  const char *presence_field = kObjc3ValueOptionalPresenceField;
  const char *payload_storage_field = kObjc3ValueOptionalPayloadStorageField;
  const char *payload_cleanup_contract =
      kObjc3ValueOptionalPayloadCleanupContract;
  const char *absence_state = kObjc3ValueOptionalAbsenceState;
  const char *presence_state = kObjc3ValueOptionalPresenceState;
  bool source_type_signature_admitted = true;
  bool semantic_value_model_supported = true;
  bool stable_abi_layout_contract_supported = true;
  bool explicit_present_absent_construction_modeled = true;
  bool binding_narrowing_supported = true;
  bool unwrap_requires_presence_check = true;
  bool runtime_construction_supported = false;
  bool runtime_unwrap_supported = false;
  bool ir_payload_emission_supported = false;
  bool nil_to_scalar_coercion_allowed = false;
  bool implicit_nil_absence_allowed = false;
  bool nullable_pointer_conversion_allowed = false;
  bool throws_result_conversion_allowed = false;
};
