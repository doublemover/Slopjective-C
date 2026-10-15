#pragma once

#include <string>

#include "ast/objc3_ast_value_type.h"

inline constexpr const char *kObjc3ValueOptionalContractId =
    "objc3c.value_optional.contract.v1";
inline constexpr const char *kObjc3ValueOptionalAbiLayoutId =
    "objc3.value_optional.inline_presence_payload.v1";
inline constexpr const char *kObjc3ValueOptionalCanonicalSpelling =
    "Optional<T>";
inline constexpr const char *kObjc3ValueOptionalSourceStatus =
    "semantic-type-signature-admitted-packed-and-full-i64-runtime-abi";
inline constexpr const char *kObjc3ValueOptionalAbiLayoutStatus =
    "stable-packed-presence-payload-runtime-lowered";
inline constexpr const char *kObjc3ValueOptionalInterfaceRoundtripStatus =
    "semantic-carrier-roundtrips-bounded-packed-runtime-abi";
inline constexpr const char *kObjc3ValueOptionalPresenceField = "has_value";
inline constexpr const char *kObjc3ValueOptionalPayloadStorageField = "payload";
inline constexpr const char *kObjc3ValueOptionalAbsenceState =
    "has_value=false";
inline constexpr const char *kObjc3ValueOptionalPresenceState =
    "has_value=true";
inline constexpr const char *kObjc3ValueOptionalPayloadCleanupContract =
    "payload-cleanup-after-narrowed-scope";
inline constexpr const char *kObjc3ValueOptionalExecutableLoweringStatus =
    "packed-presence-payload-runtime-abi-lowered";
inline constexpr const char *kObjc3ValueOptionalConstructionContract =
    "explicit-absent-present-constructors-only";
inline constexpr const char *kObjc3ValueOptionalAbsentConstructionKind =
    "absent-without-payload";
inline constexpr const char *kObjc3ValueOptionalPresentConstructionKind =
    "present-with-payload";
inline constexpr const char *kObjc3ValueOptionalBindingFailureDiagnostic =
    "binding-failure-branches-to-absent-path";
inline constexpr const char *kObjc3ValueOptionalUnwrapFailureDiagnostic =
    "unwrap-requires-proven-has-value";
inline constexpr const char *kObjc3ValueOptionalNilBridgeDiagnostic =
    "nil-bridging-to-optional-is-rejected";
inline constexpr const char *kObjc3ValueOptionalRemainingRuntimeBoundary =
    "unchecked-unwrap-property-ivar-storage-and-nullable-pointer-bridging-remain-reserved";
inline constexpr const char *kObjc3ValueOptionalRuntimeAbiPayloadScope =
    "supported-packed-scalar-id-handle-and-full-i64-payload-forms";
inline constexpr const char *kObjc3ValueOptionalFullWidthI64HelperStatus =
    "runtime-helper-supported-language-call-abi-lowered";
inline constexpr const char *kObjc3ValueOptionalFullWidthI64LanguageBoundary =
    "full-width-i64-language-call-return-abi-supported-by-wide-carrier";
inline constexpr const char *kObjc3RuntimeOptionalAbsentI64Symbol =
    "objc3_runtime_optional_absent_i64";
inline constexpr const char *kObjc3RuntimeOptionalAbsentFullI64Symbol =
    "objc3_runtime_optional_absent_full_i64";
inline constexpr const char *kObjc3RuntimeOptionalPresentFullI64Symbol =
    "objc3_runtime_optional_present_full_i64";
inline constexpr const char *kObjc3RuntimeOptionalHasValueFullI64Symbol =
    "objc3_runtime_optional_has_value_full_i64";
inline constexpr const char *kObjc3RuntimeOptionalPayloadOrFullI64Symbol =
    "objc3_runtime_optional_payload_or_full_i64";
inline constexpr const char *kObjc3RuntimeOptionalUnwrapFullI64Symbol =
    "objc3_runtime_optional_unwrap_full_i64";
inline constexpr const char *kObjc3RuntimeOptionalAbsentBoolSymbol =
    "objc3_runtime_optional_absent_bool";
inline constexpr const char *kObjc3RuntimeOptionalAbsentIdSymbol =
    "objc3_runtime_optional_absent_id";
inline constexpr const char *kObjc3RuntimeOptionalPresentI32Symbol =
    "objc3_runtime_optional_present_i32";
inline constexpr const char *kObjc3RuntimeOptionalPresentBoolSymbol =
    "objc3_runtime_optional_present_bool";
inline constexpr const char *kObjc3RuntimeOptionalPresentIdSymbol =
    "objc3_runtime_optional_present_id";
inline constexpr const char *kObjc3RuntimeOptionalHasValueI32Symbol =
    "objc3_runtime_optional_has_value_i32";
inline constexpr const char *kObjc3RuntimeOptionalHasValueBoolSymbol =
    "objc3_runtime_optional_has_value_bool";
inline constexpr const char *kObjc3RuntimeOptionalHasValueIdSymbol =
    "objc3_runtime_optional_has_value_id";
inline constexpr const char *kObjc3RuntimeOptionalPayloadOrI32Symbol =
    "objc3_runtime_optional_payload_or_i32";
inline constexpr const char *kObjc3RuntimeOptionalPayloadOrBoolSymbol =
    "objc3_runtime_optional_payload_or_bool";
inline constexpr const char *kObjc3RuntimeOptionalPayloadOrIdSymbol =
    "objc3_runtime_optional_payload_or_id";
inline constexpr const char *kObjc3RuntimeOptionalUnwrapI32Symbol =
    "objc3_runtime_optional_unwrap_i32";
inline constexpr const char *kObjc3RuntimeOptionalUnwrapBoolSymbol =
    "objc3_runtime_optional_unwrap_bool";
inline constexpr const char *kObjc3RuntimeOptionalUnwrapIdSymbol =
    "objc3_runtime_optional_unwrap_id";

struct Objc3ValueOptionalTypeDescriptor {
  bool present = false;
  std::string contract_id = kObjc3ValueOptionalContractId;
  std::string canonical_spelling = kObjc3ValueOptionalCanonicalSpelling;
  std::string payload_type_spelling;
  ValueType payload_value_type = ValueType::Unknown;
  bool payload_object_pointer = false;
  std::string payload_object_pointer_type_name;
  bool payload_generic = false;
  bool payload_nested_value_optional = false;
  bool payload_lowercase_optional_alias = false;
  bool payload_full_width_i64 = false;
  std::string abi_layout_id = kObjc3ValueOptionalAbiLayoutId;
  std::string abi_layout_status = kObjc3ValueOptionalAbiLayoutStatus;
  std::string interface_roundtrip_status =
      kObjc3ValueOptionalInterfaceRoundtripStatus;
  std::string presence_field = kObjc3ValueOptionalPresenceField;
  std::string payload_storage_field = kObjc3ValueOptionalPayloadStorageField;
  std::string executable_lowering_status =
      kObjc3ValueOptionalExecutableLoweringStatus;
  std::string construction_contract =
      kObjc3ValueOptionalConstructionContract;
  std::string absent_construction_kind =
      kObjc3ValueOptionalAbsentConstructionKind;
  std::string present_construction_kind =
      kObjc3ValueOptionalPresentConstructionKind;
  std::string binding_failure_diagnostic =
      kObjc3ValueOptionalBindingFailureDiagnostic;
  std::string unwrap_failure_diagnostic =
      kObjc3ValueOptionalUnwrapFailureDiagnostic;
  std::string nil_bridge_diagnostic =
      kObjc3ValueOptionalNilBridgeDiagnostic;
  std::string remaining_runtime_boundary =
      kObjc3ValueOptionalRemainingRuntimeBoundary;
  bool source_type_admitted = false;
  bool semantic_carrier_modeled = false;
  bool semantic_value_model_supported = false;
  bool stable_abi_layout_contract_supported = false;
  bool interface_roundtrip_supported = false;
  bool executable_lowering_contract_supported = false;
  bool explicit_present_absent_construction_modeled = false;
  bool explicit_absent_construction_supported = false;
  bool explicit_present_construction_supported = false;
  bool absent_construction_forbids_payload = true;
  bool present_construction_requires_payload = true;
  bool binding_narrowing_supported = false;
  bool binding_failure_diagnostic_supported = false;
  bool unwrap_requires_presence_check = true;
  bool unwrap_failure_diagnostic_supported = false;
  bool semantic_present_absent_state_supported = false;
  bool nil_bridge_diagnostic_supported = false;
  bool runtime_execution_supported = false;
  bool lowering_supported = false;
  bool ir_payload_emission_supported = false;
  bool call_abi_lowering_supported = false;
  bool full_width_i64_runtime_helper_supported = false;
  bool full_width_i64_language_call_abi_supported = false;
  bool nil_to_scalar_coercion_allowed = false;
  bool implicit_nil_absence_allowed = false;
  bool nullable_pointer_conversion_allowed = false;
  bool throws_result_conversion_allowed = false;
  bool cleanup_contract_preserved = true;
  unsigned line = 1;
  unsigned column = 1;
};

inline std::string Objc3ValueOptionalPayloadSpellingFromGenericSuffix(
    const std::string &generic_suffix_text) {
  if (generic_suffix_text.size() < 2 || generic_suffix_text.front() != '<' ||
      generic_suffix_text.back() != '>') {
    return {};
  }
  return generic_suffix_text.substr(1, generic_suffix_text.size() - 2);
}

inline ValueType Objc3ValueOptionalPayloadValueType(
    const std::string &payload_type_spelling) {
  if (payload_type_spelling == "i32" ||
      payload_type_spelling == "NSInteger" ||
      payload_type_spelling == "NSUInteger") {
    return ValueType::I32;
  }
  if (payload_type_spelling == "bool" || payload_type_spelling == "BOOL") {
    return ValueType::Bool;
  }
  if (payload_type_spelling == "id") {
    return ValueType::ObjCId;
  }
  if (payload_type_spelling == "Class") {
    return ValueType::ObjCClass;
  }
  if (payload_type_spelling == "SEL") {
    return ValueType::ObjCSel;
  }
  if (payload_type_spelling == "Protocol") {
    return ValueType::ObjCProtocol;
  }
  if (payload_type_spelling == "instancetype") {
    return ValueType::ObjCInstancetype;
  }
  if (payload_type_spelling == "Text" ||
      payload_type_spelling == "Objc3Text") {
    return ValueType::TextHandle;
  }
  if (payload_type_spelling.empty()) {
    return ValueType::Unknown;
  }
  if (payload_type_spelling.back() == '*') {
    return ValueType::ObjCObjectPtr;
  }
  return ValueType::Unknown;
}

inline bool Objc3ValueOptionalPayloadIsFullWidthI64(
    const std::string &payload_type_spelling) {
  return payload_type_spelling == "i64" ||
         payload_type_spelling == "int64_t" ||
         payload_type_spelling == "NSInteger64" ||
         payload_type_spelling == "NSUInteger64";
}

inline bool Objc3ValueOptionalPayloadRuntimeAbiSupported(
    const Objc3ValueOptionalTypeDescriptor &descriptor) {
  return (descriptor.payload_value_type == ValueType::I32 ||
          descriptor.payload_value_type == ValueType::Bool ||
          descriptor.payload_value_type == ValueType::ObjCId ||
          descriptor.payload_full_width_i64) &&
         !descriptor.payload_generic &&
         !descriptor.payload_nested_value_optional &&
         !descriptor.payload_lowercase_optional_alias;
}

inline Objc3ValueOptionalTypeDescriptor BuildObjc3ValueOptionalDescriptor(
    const std::string &generic_suffix_text,
    unsigned line,
    unsigned column) {
  Objc3ValueOptionalTypeDescriptor descriptor;
  descriptor.present = true;
  descriptor.source_type_admitted = true;
  descriptor.semantic_carrier_modeled = true;
  descriptor.semantic_value_model_supported = true;
  descriptor.stable_abi_layout_contract_supported = true;
  descriptor.interface_roundtrip_supported = true;
  descriptor.executable_lowering_contract_supported = true;
  descriptor.explicit_present_absent_construction_modeled = true;
  descriptor.explicit_absent_construction_supported = true;
  descriptor.explicit_present_construction_supported = true;
  descriptor.absent_construction_forbids_payload = true;
  descriptor.present_construction_requires_payload = true;
  descriptor.binding_narrowing_supported = true;
  descriptor.binding_failure_diagnostic_supported = true;
  descriptor.unwrap_requires_presence_check = true;
  descriptor.unwrap_failure_diagnostic_supported = true;
  descriptor.semantic_present_absent_state_supported = true;
  descriptor.nil_bridge_diagnostic_supported = true;
  descriptor.line = line;
  descriptor.column = column;
  descriptor.payload_type_spelling =
      Objc3ValueOptionalPayloadSpellingFromGenericSuffix(generic_suffix_text);
  descriptor.payload_value_type =
      Objc3ValueOptionalPayloadValueType(descriptor.payload_type_spelling);
  descriptor.payload_object_pointer =
      descriptor.payload_value_type == ValueType::ObjCObjectPtr;
  if (descriptor.payload_object_pointer) {
    descriptor.payload_object_pointer_type_name =
        descriptor.payload_type_spelling;
    while (!descriptor.payload_object_pointer_type_name.empty() &&
           descriptor.payload_object_pointer_type_name.back() == '*') {
      descriptor.payload_object_pointer_type_name.pop_back();
    }
  }
  descriptor.payload_generic =
      descriptor.payload_type_spelling.find('<') != std::string::npos ||
      descriptor.payload_type_spelling.find('>') != std::string::npos;
  descriptor.payload_nested_value_optional =
      descriptor.payload_type_spelling.rfind("Optional<", 0) == 0;
  descriptor.payload_lowercase_optional_alias =
      descriptor.payload_type_spelling.rfind("optional<", 0) == 0 ||
      descriptor.payload_type_spelling.find("<optional<") !=
          std::string::npos;
  descriptor.payload_full_width_i64 =
      Objc3ValueOptionalPayloadIsFullWidthI64(
          descriptor.payload_type_spelling);
  if (descriptor.payload_full_width_i64) {
    descriptor.full_width_i64_runtime_helper_supported = true;
    descriptor.full_width_i64_language_call_abi_supported = true;
    descriptor.remaining_runtime_boundary =
        kObjc3ValueOptionalFullWidthI64LanguageBoundary;
  }
  if (Objc3ValueOptionalPayloadRuntimeAbiSupported(descriptor)) {
    descriptor.runtime_execution_supported = true;
    descriptor.lowering_supported = true;
    descriptor.ir_payload_emission_supported = true;
    descriptor.call_abi_lowering_supported = true;
  }
  return descriptor;
}

inline bool Objc3ValueOptionalHasExecutableLoweringContract(
    const Objc3ValueOptionalTypeDescriptor &descriptor) {
  return descriptor.present && descriptor.source_type_admitted &&
         descriptor.semantic_value_model_supported &&
         descriptor.stable_abi_layout_contract_supported &&
         descriptor.interface_roundtrip_supported &&
         descriptor.executable_lowering_contract_supported &&
         descriptor.explicit_present_absent_construction_modeled &&
         descriptor.explicit_absent_construction_supported &&
         descriptor.explicit_present_construction_supported &&
         descriptor.absent_construction_forbids_payload &&
         descriptor.present_construction_requires_payload &&
         descriptor.binding_narrowing_supported &&
         descriptor.binding_failure_diagnostic_supported &&
         descriptor.unwrap_requires_presence_check &&
         descriptor.unwrap_failure_diagnostic_supported &&
         descriptor.semantic_present_absent_state_supported &&
         descriptor.nil_bridge_diagnostic_supported &&
         descriptor.cleanup_contract_preserved &&
         !descriptor.payload_type_spelling.empty() &&
         !descriptor.nil_to_scalar_coercion_allowed &&
         !descriptor.implicit_nil_absence_allowed &&
         !descriptor.nullable_pointer_conversion_allowed &&
         !descriptor.throws_result_conversion_allowed;
}

inline bool Objc3ValueOptionalRuntimeAbiReady(
    const Objc3ValueOptionalTypeDescriptor &descriptor) {
  return descriptor.present && descriptor.ir_payload_emission_supported &&
         descriptor.call_abi_lowering_supported &&
         descriptor.runtime_execution_supported &&
         descriptor.lowering_supported;
}

inline bool Objc3ValueOptionalRejectsImplicitBridging(
    const Objc3ValueOptionalTypeDescriptor &descriptor) {
  return descriptor.present && descriptor.nil_bridge_diagnostic_supported &&
         !descriptor.nil_to_scalar_coercion_allowed &&
         !descriptor.implicit_nil_absence_allowed &&
         !descriptor.nullable_pointer_conversion_allowed &&
         !descriptor.throws_result_conversion_allowed;
}
