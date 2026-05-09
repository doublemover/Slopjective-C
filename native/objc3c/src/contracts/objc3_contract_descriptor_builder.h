#pragma once

#include <string_view>

#include "contracts/objc3_native_contract_descriptor.h"

inline constexpr Objc3NativeContractDescriptor
BuildObjc3NativeContractDescriptor(
    Objc3NativeContractId contract_id,
    std::string_view owner,
    std::string_view version) {
  return {contract_id, Objc3NativeContractIdSpelling(contract_id), owner,
          version, true};
}
