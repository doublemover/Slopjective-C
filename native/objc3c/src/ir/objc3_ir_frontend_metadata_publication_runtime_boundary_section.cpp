#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_section.h"

#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_section_policy.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_section_publication.h"

void EmitObjc3IRRuntimeMetadataSectionBoundaryNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRRuntimeMetadataSectionPolicyNode(metadata, out);
  EmitObjc3IRRuntimeMetadataSectionPublicationNode(metadata, out);
}
