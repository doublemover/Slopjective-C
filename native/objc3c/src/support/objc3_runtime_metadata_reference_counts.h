#pragma once

#include <cstddef>

#include "pipeline/objc3_frontend_types.h"

namespace objc3c::support {

inline std::size_t CountRuntimeMetadataSourceRecordSetReferences(
    const Objc3RuntimeMetadataSourceRecordSet &record_set) {
  std::size_t references = 0;
  for (const auto &class_record : record_set.classes_lexicographic) {
    if (class_record.has_super && !class_record.super_name.empty()) {
      ++references;
    }
    references += class_record.adopted_protocols_lexicographic.size();
  }
  for (const auto &protocol_record : record_set.protocols_lexicographic) {
    references += protocol_record.inherited_protocols_lexicographic.size();
  }
  for (const auto &category_record : record_set.categories_lexicographic) {
    references += category_record.adopted_protocols_lexicographic.size();
  }
  for (const auto &property_record : record_set.properties_lexicographic) {
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
  }
  for (const auto &method_record : record_set.methods_lexicographic) {
    if (!method_record.selector.empty()) {
      ++references;
    }
  }
  return references;
}

}  // namespace objc3c::support
