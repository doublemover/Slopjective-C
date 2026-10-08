#pragma once

#include <string>

#include "ast/objc3_ast_value_optional_type.h"

enum class Objc3IRValueOptionalCarrierKind {
  PackedI64,
  FullI64,
};

struct Objc3IRValueOptionalCarrierMetadata {
  bool present = false;
  std::string payload_type_spelling;
  ValueType payload_value_type = ValueType::Unknown;
  bool payload_full_width_i64 = false;
  std::string abi_layout_id;
  bool lowering_supported = false;
  bool ir_payload_emission_supported = false;
  bool call_abi_lowering_supported = false;
  bool runtime_execution_supported = false;
  bool full_width_i64_runtime_helper_supported = false;
  bool full_width_i64_language_call_abi_supported = false;
  std::string remaining_runtime_boundary;

  bool operator==(const Objc3IRValueOptionalCarrierMetadata &) const = default;
};

inline Objc3IRValueOptionalCarrierMetadata
BuildObjc3IRValueOptionalCarrierMetadata(
    const Objc3ValueOptionalTypeDescriptor &descriptor) {
  Objc3IRValueOptionalCarrierMetadata metadata;
  if (!descriptor.present) {
    return metadata;
  }
  metadata.present = true;
  metadata.payload_type_spelling = descriptor.payload_type_spelling;
  metadata.payload_value_type = descriptor.payload_value_type;
  metadata.payload_full_width_i64 = descriptor.payload_full_width_i64;
  metadata.abi_layout_id = descriptor.abi_layout_id;
  metadata.lowering_supported = descriptor.lowering_supported;
  metadata.ir_payload_emission_supported =
      descriptor.ir_payload_emission_supported;
  metadata.call_abi_lowering_supported =
      descriptor.call_abi_lowering_supported;
  metadata.runtime_execution_supported = descriptor.runtime_execution_supported;
  metadata.full_width_i64_runtime_helper_supported =
      descriptor.full_width_i64_runtime_helper_supported;
  metadata.full_width_i64_language_call_abi_supported =
      descriptor.full_width_i64_language_call_abi_supported;
  metadata.remaining_runtime_boundary = descriptor.remaining_runtime_boundary;
  return metadata;
}

inline Objc3IRValueOptionalCarrierKind Objc3IRValueOptionalCarrierKindFor(
    const Objc3ValueOptionalTypeDescriptor &descriptor) {
  return descriptor.present && descriptor.payload_full_width_i64
             ? Objc3IRValueOptionalCarrierKind::FullI64
             : Objc3IRValueOptionalCarrierKind::PackedI64;
}

inline Objc3IRValueOptionalCarrierKind Objc3IRValueOptionalCarrierKindFor(
    const Objc3IRValueOptionalCarrierMetadata &metadata) {
  return metadata.present && metadata.payload_full_width_i64
             ? Objc3IRValueOptionalCarrierKind::FullI64
             : Objc3IRValueOptionalCarrierKind::PackedI64;
}

inline const char *Objc3IRValueOptionalCarrierLLVMType(
    Objc3IRValueOptionalCarrierKind carrier) {
  return carrier == Objc3IRValueOptionalCarrierKind::FullI64 ? "{ i8, i64 }"
                                                             : "i64";
}

inline unsigned Objc3IRValueOptionalCarrierLLVMAlignment(
    Objc3IRValueOptionalCarrierKind carrier) {
  (void)carrier;
  return 8u;
}

inline const char *Objc3IRValueOptionalCarrierZeroValue(
    Objc3IRValueOptionalCarrierKind carrier) {
  return carrier == Objc3IRValueOptionalCarrierKind::FullI64
             ? "zeroinitializer"
             : "0";
}

inline bool Objc3IRValueOptionalCarrierRequiresWideLanguageAbi(
    const Objc3IRValueOptionalCarrierMetadata &metadata) {
  return metadata.present && metadata.payload_full_width_i64 &&
         !metadata.full_width_i64_language_call_abi_supported;
}
