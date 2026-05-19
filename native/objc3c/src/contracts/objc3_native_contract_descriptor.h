#pragma once

#include <string_view>

#include "contracts/objc3_native_contract_ids.h"

struct Objc3NativeContractDescriptor {
  Objc3NativeContractId id = Objc3NativeContractId::kDiagnosticPayloadV1;
  std::string_view spelling;
  std::string_view owner;
  std::string_view version;
  bool valid = false;
};
