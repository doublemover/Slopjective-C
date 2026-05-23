#pragma once

#include "ast/objc3_ast_value_optional_type.h"

inline constexpr const char *kObjc3ValueOptionalLoweringContractId =
    "objc3c.value_optional.lowering.contract.v1";
inline constexpr const char *kObjc3ValueOptionalLoweringStatus =
    "type-signature-carrier-only-runtime-lowering-fail-closed";
inline constexpr const char *kObjc3ValueOptionalAbsenceState =
    "has_value=false";
inline constexpr const char *kObjc3ValueOptionalPresenceState =
    "has_value=true";

struct Objc3ValueOptionalLoweringContract {
  const char *contract_id = kObjc3ValueOptionalLoweringContractId;
  const char *abi_layout_id = kObjc3ValueOptionalAbiLayoutId;
  const char *status = kObjc3ValueOptionalLoweringStatus;
  const char *presence_field = "has_value";
  const char *payload_storage_field = "payload";
  bool source_type_signature_admitted = true;
  bool runtime_construction_supported = false;
  bool runtime_unwrap_supported = false;
  bool ir_payload_emission_supported = false;
  bool nil_to_scalar_coercion_allowed = false;
  bool nullable_pointer_conversion_allowed = false;
  bool throws_result_conversion_allowed = false;
};
