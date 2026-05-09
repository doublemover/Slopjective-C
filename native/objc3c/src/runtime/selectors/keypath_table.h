#pragma once

#include "runtime/selectors/keypath_descriptor.h"

#include <cstdint>

namespace objc3c::runtime {

struct RuntimeState;

bool MaterializeKeyPathDescriptorUnlocked(
    RuntimeState &state,
    const EmittedKeyPathDescriptor &descriptor,
    std::uint64_t registration_order_ordinal);

}  // namespace objc3c::runtime
