#pragma once

#include <string>

namespace objc3c::runtime {

bool RuntimeCStringSnapshotHasValue(const std::string &text);
const char *BorrowRuntimeCString(const std::string &text);
const char *RuntimeCStringSnapshotOwnershipModel();
const char *RuntimeStringPoolSnapshotBorrowingModel();
const char *RuntimeCStringNullSnapshotModel();

}  // namespace objc3c::runtime
