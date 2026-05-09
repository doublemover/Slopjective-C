#pragma once

#include <cstddef>

namespace objc3c::runtime {

struct RealizedPropertyAccessor;
struct RuntimeInstanceRecord;

struct RuntimeIvarStorageSpan {
  std::size_t offset = 0u;
  std::size_t size = 0u;
  bool addressable = false;
};

std::size_t EffectiveIvarOffset(const RealizedPropertyAccessor &accessor);
std::size_t EffectiveIvarSize(const RealizedPropertyAccessor &accessor);
RuntimeIvarStorageSpan ResolveRuntimeIvarStorageSpan(
    const RuntimeInstanceRecord &instance,
    const RealizedPropertyAccessor &accessor);

}  // namespace objc3c::runtime
