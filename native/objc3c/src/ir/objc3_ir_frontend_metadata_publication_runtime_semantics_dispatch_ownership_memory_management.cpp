#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_memory_management.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_memory_management_api_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_memory_management_implementation_rows.h"

void EmitObjc3IRMemoryManagementRuntimeMetadataNodes(
    const Objc3IRFrontendMetadata &metadata,
    std::size_t synthesized_property_accessor_count, std::ostringstream &out) {
  EmitObjc3IRMemoryManagementRuntimeApiMetadataNode(
      synthesized_property_accessor_count, out);
  EmitObjc3IRMemoryManagementRuntimeImplementationMetadataNode(
      metadata, synthesized_property_accessor_count, out);
}
