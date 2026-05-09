#pragma once

#include <cstdint>

namespace objc3c::runtime {

struct EmittedKeyPathDescriptor;
struct RuntimeState;

bool RuntimeKeyPathHandleIsValid(std::uint64_t stable_id);
bool RuntimeKeyPathDescriptorIsMaterializable(const char *root_name,
                                              const char *component_path);
bool MaterializeKeyPathDescriptorUnlocked(
    RuntimeState &state,
    const EmittedKeyPathDescriptor &descriptor,
    std::uint64_t registration_order_ordinal);

}  // namespace objc3c::runtime
