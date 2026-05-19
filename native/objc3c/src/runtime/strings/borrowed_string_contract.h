#pragma once

#include <string>

namespace objc3c::runtime {

inline bool RuntimeCStringSnapshotHasStoredValue(const std::string &text) {
  return !text.empty();
}

inline const char *BorrowRuntimeCStringData(const std::string &text) {
  return RuntimeCStringSnapshotHasStoredValue(text) ? text.c_str() : nullptr;
}

inline const char *RuntimeCStringSnapshotOwnershipModelLiteral() {
  return "runtime-owned-borrowed-c-string-valid-until-next-runtime-mutation";
}

inline const char *RuntimeStringPoolSnapshotBorrowingModelLiteral() {
  return "image-backed-string-pools-feed-runtime-owned-snapshot-fields-without-secondary-copies";
}

inline const char *RuntimeCStringNullSnapshotModelLiteral() {
  return "empty-runtime-string-snapshot-fields-publish-null-instead-of-empty-aliases";
}

}  // namespace objc3c::runtime
