#pragma once

#include <cstdint>
#include <string_view>

enum class Objc3NativeContractId : std::uint8_t {
  kDiagnosticPayloadV1,
  kFrontendDiagnosticsBusV1,
  kCanonicalLanguageConfigV1,
  kRemovedOptionValidationV1,
};

inline constexpr std::string_view Objc3NativeContractIdSpelling(
    Objc3NativeContractId contract_id) {
  switch (contract_id) {
    case Objc3NativeContractId::kDiagnosticPayloadV1:
      return "objc3c.diagnostic.payload.v1";
    case Objc3NativeContractId::kFrontendDiagnosticsBusV1:
      return "objc3c.frontend.diagnostics_bus.v1";
    case Objc3NativeContractId::kCanonicalLanguageConfigV1:
      return "objc3c.config.canonical_language.v1";
    case Objc3NativeContractId::kRemovedOptionValidationV1:
      return "objc3c.config.removed_option_validation.v1";
  }
  return {};
}

inline constexpr bool Objc3NativeContractIdIsKnown(
    Objc3NativeContractId contract_id) {
  return !Objc3NativeContractIdSpelling(contract_id).empty();
}
