#pragma once

#include <cstddef>

#include "pipeline/objc3_frontend_types.h"

namespace objc3c::support {

inline std::size_t CountRuntimeMetadataCategoryRecordReferences(
    const Objc3RuntimeMetadataCategorySourceRecord &category_record) {
  return category_record.adopted_protocols_lexicographic.size();
}

}  // namespace objc3c::support
