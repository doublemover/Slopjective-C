#pragma once

#include "contracts/objc3_contract_id_match.h"
#include "contracts/objc3_native_contract_descriptor.h"

inline constexpr bool Objc3NativeContractDescriptorIsValid(
    const Objc3NativeContractDescriptor &descriptor) {
  return descriptor.valid &&
         Objc3ContractIdMatches(descriptor.spelling, descriptor.id) &&
         !descriptor.owner.empty() && !descriptor.version.empty();
}
