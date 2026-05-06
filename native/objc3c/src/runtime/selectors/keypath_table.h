#pragma once

#include <cstdint>

namespace objc3c::runtime {

bool RuntimeKeyPathHandleIsValid(std::uint64_t stable_id);
bool RuntimeKeyPathDescriptorIsMaterializable(const char *root_name,
                                              const char *component_path);

}  // namespace objc3c::runtime
