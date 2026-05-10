#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_retention.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_retention_archive_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_retention_link_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_retention_packaging_rows.h"

void EmitObjc3IRRuntimeObjectRetentionMetadataNodes(
    const Objc3IRFrontendMetadata &metadata,
    const std::string &runtime_metadata_linker_anchor_symbol,
    const std::string &runtime_metadata_discovery_root_symbol,
    std::ostringstream &out) {
  EmitObjc3IRRuntimeObjectPackagingRetentionMetadataNode(out);
  EmitObjc3IRRuntimeObjectLinkerRetentionMetadataNode(
      runtime_metadata_linker_anchor_symbol,
      runtime_metadata_discovery_root_symbol, out);
  EmitObjc3IRRuntimeObjectArchiveStaticLinkDiscoveryMetadataNode(metadata, out);
}
