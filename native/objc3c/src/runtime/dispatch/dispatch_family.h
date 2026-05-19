#pragma once

namespace objc3c::runtime {

enum class DispatchFamily {
  Invalid = 0,
  Instance = 1,
  Class = 2,
};

bool RuntimeDispatchFamilyIsValid(DispatchFamily family);
const char *RuntimeDispatchFamilyName(DispatchFamily family);

}  // namespace objc3c::runtime
