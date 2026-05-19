#pragma once

#include "runtime/metadata/runtime_realized_records.h"

#include <cstdint>
#include <vector>

namespace objc3c::runtime {

struct RuntimeDispatchFrame {
  int receiver = 0;
  std::uint64_t base_identity = 0;
  const RealizedPropertyAccessor *runtime_property_accessor = nullptr;
  std::vector<int> autorelease_values;
};

struct RuntimeAutoreleasePoolFrame {
  std::vector<int> values;
};

}  // namespace objc3c::runtime
