#pragma once

#include <cstdint>

namespace objc3c::runtime {

struct RealizedPropertyAccessor;
struct RuntimeState;

struct RuntimeCurrentPropertyBindingResolution {
  int status = 0;
  std::uint64_t base_identity = 0;
  const RealizedPropertyAccessor *accessor = nullptr;
};

RuntimeCurrentPropertyBindingResolution ResolveCurrentPropertyBindingUnlocked(
    RuntimeState &state,
    int receiver,
    const char *class_name,
    const char *property_name);

}  // namespace objc3c::runtime
