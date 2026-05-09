#pragma once

#include <cstddef>

#include "pipeline/objc3_frontend_types.h"
#include "support/objc3_runtime_metadata_category_reference_counts.h"
#include "support/objc3_runtime_metadata_class_reference_counts.h"
#include "support/objc3_runtime_metadata_method_reference_counts.h"
#include "support/objc3_runtime_metadata_property_reference_counts.h"
#include "support/objc3_runtime_metadata_protocol_reference_counts.h"

namespace objc3c::support {

inline std::size_t CountRuntimeMetadataSourceRecordSetReferences(
    const Objc3RuntimeMetadataSourceRecordSet &record_set) {
  std::size_t references = 0;
  for (const auto &class_record : record_set.classes_lexicographic) {
    references += CountRuntimeMetadataClassRecordReferences(class_record);
  }
  for (const auto &protocol_record : record_set.protocols_lexicographic) {
    references += CountRuntimeMetadataProtocolRecordReferences(protocol_record);
  }
  for (const auto &category_record : record_set.categories_lexicographic) {
    references += CountRuntimeMetadataCategoryRecordReferences(category_record);
  }
  for (const auto &property_record : record_set.properties_lexicographic) {
    references += CountRuntimeMetadataPropertyRecordReferences(property_record);
  }
  for (const auto &method_record : record_set.methods_lexicographic) {
    references += CountRuntimeMetadataMethodRecordReferences(method_record);
  }
  return references;
}

}  // namespace objc3c::support
