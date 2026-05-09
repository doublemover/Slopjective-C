#pragma once

#include <cstddef>

#include "pipeline/objc3_frontend_types.h"

namespace objc3c::support {

inline std::size_t CountRuntimeMetadataPropertyRecordReferences(
    const Objc3RuntimeMetadataPropertySourceRecord &property_record) {
  std::size_t references = 0;
  if (!property_record.effective_getter_selector.empty()) {
    ++references;
  }
  if (property_record.effective_setter_available &&
      !property_record.effective_setter_selector.empty()) {
    ++references;
  }
  if (!property_record.ivar_binding_symbol.empty()) {
    ++references;
  }
  return references;
}

}  // namespace objc3c::support
