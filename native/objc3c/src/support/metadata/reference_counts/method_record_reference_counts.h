#pragma once

#include <cstddef>

#include "pipeline/objc3_frontend_types.h"

namespace objc3c::support {

inline std::size_t CountRuntimeMetadataMethodRecordReferences(
    const Objc3RuntimeMetadataMethodSourceRecord &method_record) {
  return method_record.selector.empty() ? 0u : 1u;
}

}  // namespace objc3c::support
