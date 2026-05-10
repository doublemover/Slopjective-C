#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_protocol_category_category_counts.h"

#include "ir/objc3_ir_frontend_metadata.h"

Objc3IRRuntimeProtocolCategoryCategoryCounts
CollectObjc3IRRuntimeProtocolCategoryCategoryCounts(
    const Objc3IRFrontendMetadata &metadata) {
  Objc3IRRuntimeProtocolCategoryCategoryCounts counts;
  for (const auto &bundle :
       metadata.runtime_metadata_category_bundles_lexicographic) {
    ++counts.bundle_count;
    counts.adopted_reference_total +=
        bundle.adopted_protocol_owner_identities_lexicographic.size();
    counts.attachment_reference_total += 3u;
  }
  return counts;
}
