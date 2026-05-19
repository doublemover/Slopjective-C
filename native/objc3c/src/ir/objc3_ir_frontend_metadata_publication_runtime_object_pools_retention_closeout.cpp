#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_pools_retention_closeout.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_binary_inspection.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_closeout.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_pools.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_retention.h"

void EmitObjc3IRRuntimePoolRetentionCloseoutMetadataNodes(
    const Objc3IRFrontendMetadata &metadata,
    const std::string &runtime_metadata_linker_anchor_symbol,
    const std::string &runtime_metadata_discovery_root_symbol,
    std::size_t selector_pool_global_count,
    std::size_t runtime_string_pool_global_count, std::ostringstream &out) {
  EmitObjc3IRRuntimeObjectPoolMetadataNode(selector_pool_global_count,
                                           runtime_string_pool_global_count,
                                           out);
  EmitObjc3IRRuntimeObjectBinaryInspectionMetadataNode(out);
  EmitObjc3IRRuntimeObjectRetentionMetadataNodes(
      metadata, runtime_metadata_linker_anchor_symbol,
      runtime_metadata_discovery_root_symbol, out);
  EmitObjc3IRRuntimeObjectCloseoutMetadataNodes(out);
}
