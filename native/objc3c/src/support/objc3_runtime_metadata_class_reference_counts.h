#pragma once

#include <cstddef>

#include "pipeline/objc3_frontend_types.h"

namespace objc3c::support {

inline std::size_t CountRuntimeMetadataClassRecordReferences(
    const Objc3RuntimeMetadataClassSourceRecord &class_record) {
  std::size_t references = 0;
  if (class_record.has_super && !class_record.super_name.empty()) {
    ++references;
  }
  references += class_record.adopted_protocols_lexicographic.size();
  return references;
}

}  // namespace objc3c::support
