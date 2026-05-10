#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_protocol_category_protocol_counts.h"

#include "ir/objc3_ir_frontend_metadata.h"

Objc3IRRuntimeProtocolCategoryProtocolCounts
CollectObjc3IRRuntimeProtocolCategoryProtocolCounts(
    const Objc3IRFrontendMetadata &metadata) {
  Objc3IRRuntimeProtocolCategoryProtocolCounts counts;
  for (const auto &bundle :
       metadata.runtime_metadata_protocol_bundles_lexicographic) {
    ++counts.bundle_count;
    counts.inherited_reference_total +=
        bundle.inherited_protocol_owner_identities_lexicographic.size();
  }
  return counts;
}
