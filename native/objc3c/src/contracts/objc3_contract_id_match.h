#pragma once

#include <string_view>

#include "contracts/objc3_native_contract_ids.h"

inline constexpr bool Objc3ContractIdMatches(
    std::string_view candidate,
    Objc3NativeContractId contract_id) {
  return Objc3NativeContractIdIsKnown(contract_id) &&
         candidate == Objc3NativeContractIdSpelling(contract_id);
}
