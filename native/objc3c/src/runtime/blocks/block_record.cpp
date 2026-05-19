#include "runtime/blocks/block_record.h"

namespace objc3c::runtime {

bool RuntimeBlockStorageSizeIsSupported(std::size_t storage_size_bytes) {
  return storage_size_bytes > 0;
}

}  // namespace objc3c::runtime
