#include "runtime/strings/borrowed_string.h"

#include "runtime/strings/borrowed_string_contract.h"

namespace objc3c::runtime {

bool RuntimeCStringSnapshotHasValue(const std::string &text) {
  return RuntimeCStringSnapshotHasStoredValue(text);
}

const char *BorrowRuntimeCString(const std::string &text) {
  return BorrowRuntimeCStringData(text);
}

const char *RuntimeCStringSnapshotOwnershipModel() {
  return RuntimeCStringSnapshotOwnershipModelLiteral();
}

const char *RuntimeStringPoolSnapshotBorrowingModel() {
  return RuntimeStringPoolSnapshotBorrowingModelLiteral();
}

const char *RuntimeCStringNullSnapshotModel() {
  return RuntimeCStringNullSnapshotModelLiteral();
}

}  // namespace objc3c::runtime
