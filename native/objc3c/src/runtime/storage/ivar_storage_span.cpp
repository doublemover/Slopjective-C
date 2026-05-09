#include "runtime/storage/ivar_storage_span.h"

#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/storage/runtime_instance_records.h"

namespace objc3c::runtime {

std::size_t EffectiveIvarOffset(const RealizedPropertyAccessor &accessor) {
  if (accessor.ivar_descriptor == nullptr) {
    return 0u;
  }
  if (accessor.ivar_descriptor->offset_global != nullptr) {
    return static_cast<std::size_t>(*accessor.ivar_descriptor->offset_global);
  }
  return static_cast<std::size_t>(accessor.ivar_descriptor->offset_bytes);
}

std::size_t EffectiveIvarSize(const RealizedPropertyAccessor &accessor) {
  return accessor.ivar_descriptor != nullptr
             ? static_cast<std::size_t>(accessor.ivar_descriptor->size_bytes)
             : 0u;
}

RuntimeIvarStorageSpan ResolveRuntimeIvarStorageSpan(
    const RuntimeInstanceRecord &instance,
    const RealizedPropertyAccessor &accessor) {
  RuntimeIvarStorageSpan span;
  span.offset = EffectiveIvarOffset(accessor);
  span.size = EffectiveIvarSize(accessor);
  span.addressable = span.size != 0u &&
                     span.offset <= instance.storage_bytes.size() &&
                     span.size <= instance.storage_bytes.size() - span.offset;
  return span;
}

}  // namespace objc3c::runtime
