#pragma once

#include <cstddef>

#include "pipeline/objc3_frontend_types.h"

namespace objc3c::support {

inline std::size_t CountRuntimeMetadataProtocolRecordReferences(
    const Objc3RuntimeMetadataProtocolSourceRecord &protocol_record) {
  return protocol_record.inherited_protocols_lexicographic.size();
}

}  // namespace objc3c::support
