#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_protocol_category.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_protocol_category_category_counts.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_protocol_category_protocol_counts.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_protocol_category_row.h"

void EmitObjc3IRRuntimeProtocolCategoryMetadataNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  const auto protocol_counts =
      CollectObjc3IRRuntimeProtocolCategoryProtocolCounts(metadata);
  const auto category_counts =
      CollectObjc3IRRuntimeProtocolCategoryCategoryCounts(metadata);
  EmitObjc3IRRuntimeProtocolCategoryMetadataRow(
      metadata, protocol_counts, category_counts, out);
}
