#include "runtime/blocks/block_capture_storage.h"

namespace objc3c::runtime {

bool RuntimeBlockCaptureStorageNeedsCopyDispose(int has_pointer_capture_storage) {
  return has_pointer_capture_storage != 0;
}

}  // namespace objc3c::runtime
