#pragma once

#include <cstddef>

namespace objc3c::runtime {

std::size_t RuntimeInstanceStorageSize(std::size_t layout_size_bytes);
bool RuntimeInstanceReceiverIsManaged(int receiver);

}  // namespace objc3c::runtime
