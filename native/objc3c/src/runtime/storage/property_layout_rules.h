#pragma once

#include <cstddef>

namespace objc3c::runtime {

struct EmittedIvarDescriptor;

std::size_t AlignRuntimePropertyStorageSize(std::size_t value,
                                            std::size_t alignment);
bool RuntimePropertyIvarDescriptorHasStrictPublishedLayout(
    const EmittedIvarDescriptor &descriptor,
    std::size_t effective_offset,
    std::size_t effective_alignment);
bool RuntimePropertyIvarStorageExtentIsAddressable(std::size_t offset,
                                                   std::size_t size);

}  // namespace objc3c::runtime
