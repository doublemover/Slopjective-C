#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership.h"

#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_dispatch_surface.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_executable_layout.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_runtime_api.h"

void EmitObjc3IRDispatchOwnershipMetadataNodes(
    const Objc3IRFrontendMetadata &metadata,
    std::size_t synthesized_property_accessor_count, std::ostringstream &out) {
  EmitObjc3IRDispatchSurfaceClassificationMetadataNodes(metadata, out);
  EmitObjc3IRExecutableLayoutAccessorMetadataNodes(
      metadata, synthesized_property_accessor_count, out);
  EmitObjc3IROwnershipRuntimeApiMetadataNodes(
      metadata, synthesized_property_accessor_count, out);
}
