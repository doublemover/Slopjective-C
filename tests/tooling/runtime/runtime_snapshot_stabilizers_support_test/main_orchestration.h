#pragma once

#include "stabilizer_assertion_helpers.h"

namespace objc3c::runtime::probe::runtime_snapshot_stabilizers_support {

inline int RunTestMain() {
  int result = VerifyNullableCStringStabilizerPreservesStorage();
  if (result != 0) {
    return result;
  }

  result = VerifyNullableCStringStabilizerClearsNullSource();
  if (result != 0) {
    return result;
  }

  result = VerifyRegistrationStateStabilizerCopiesRuntimeStrings();
  if (result != 0) {
    return result;
  }

  return VerifyPropertyEntryStabilizerCopiesRuntimeStrings();
}

}  // namespace objc3c::runtime::probe::runtime_snapshot_stabilizers_support
