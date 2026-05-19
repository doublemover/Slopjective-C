#pragma once

#include <cstddef>

#include "pipeline/objc3_frontend_types.h"

namespace objc3c::support {

inline std::size_t CountRuntimeMetadataSourceRecordSetDeclarations(
    const Objc3RuntimeMetadataSourceRecordSet &record_set) {
  return record_set.classes_lexicographic.size() +
         record_set.protocols_lexicographic.size() +
         record_set.categories_lexicographic.size() +
         record_set.properties_lexicographic.size() +
         record_set.methods_lexicographic.size() +
         record_set.ivars_lexicographic.size();
}

}  // namespace objc3c::support
