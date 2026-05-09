#pragma once

#include <string>

namespace objc3c::runtime {

const char *BorrowRuntimeCString(const std::string &text);
const char *RuntimeCStringSnapshotOwnershipModel();

}  // namespace objc3c::runtime
