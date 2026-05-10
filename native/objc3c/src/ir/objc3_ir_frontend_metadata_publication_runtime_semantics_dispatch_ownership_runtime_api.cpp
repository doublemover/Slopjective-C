#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_runtime_api.h"

#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_memory_management.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_runtime_gate.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_runtime_hooks.h"

void EmitObjc3IROwnershipRuntimeApiMetadataNodes(
    const Objc3IRFrontendMetadata &metadata,
    std::size_t synthesized_property_accessor_count, std::ostringstream &out) {
  EmitObjc3IROwnershipRuntimeHookEmissionMetadataNode(
      synthesized_property_accessor_count, out);
  EmitObjc3IRMemoryManagementRuntimeMetadataNodes(
      metadata, synthesized_property_accessor_count, out);
  EmitObjc3IROwnershipRuntimeGateMetadataNode(out);
}
