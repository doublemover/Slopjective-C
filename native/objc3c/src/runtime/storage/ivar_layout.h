#pragma once

#include <cstddef>

namespace objc3c::runtime {

bool RuntimeIvarLayoutSlotIsAddressable(std::size_t offset_bytes,
                                        std::size_t size_bytes,
                                        std::size_t instance_size_bytes);

}  // namespace objc3c::runtime
