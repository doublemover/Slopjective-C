#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary.h"

#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_export.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_section.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_source_ownership.h"

void EmitObjc3IRRuntimeMetadataBoundaryNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRRuntimeMetadataSourceOwnershipNode(metadata, out);
  EmitObjc3IRRuntimeExportBoundaryNodes(metadata, out);
  EmitObjc3IRRuntimeMetadataSectionBoundaryNodes(metadata, out);
}
