#include "runtime/storage/ivar_layout.h"

namespace objc3c::runtime {

bool RuntimeIvarLayoutSlotIsAddressable(std::size_t offset_bytes,
                                        std::size_t size_bytes,
                                        std::size_t instance_size_bytes) {
  return size_bytes != 0 && offset_bytes <= instance_size_bytes &&
         size_bytes <= instance_size_bytes - offset_bytes;
}

}  // namespace objc3c::runtime
