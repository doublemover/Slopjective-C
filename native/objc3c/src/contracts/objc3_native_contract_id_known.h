#pragma once

#include "contracts/objc3_native_contract_id_kind.h"
#include "contracts/objc3_native_contract_id_spelling.h"

inline constexpr bool Objc3NativeContractIdIsKnown(
    Objc3NativeContractId contract_id) {
  return !Objc3NativeContractIdSpelling(contract_id).empty();
}
