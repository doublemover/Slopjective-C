#include "runtime/strings/borrowed_string.h"

namespace objc3c::runtime {

const char *BorrowRuntimeCString(const std::string &text) {
  return text.empty() ? nullptr : text.c_str();
}

const char *RuntimeCStringSnapshotOwnershipModel() {
  return "runtime-owned-borrowed-c-string-valid-until-next-runtime-mutation";
}

}  // namespace objc3c::runtime
