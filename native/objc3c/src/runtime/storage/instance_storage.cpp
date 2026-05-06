#include "runtime/storage/instance_storage.h"

namespace objc3c::runtime {

std::size_t RuntimeInstanceStorageSize(std::size_t layout_size_bytes) {
  return layout_size_bytes == 0 ? 1u : layout_size_bytes;
}

bool RuntimeInstanceReceiverIsManaged(int receiver) {
  return receiver > 0;
}

}  // namespace objc3c::runtime
