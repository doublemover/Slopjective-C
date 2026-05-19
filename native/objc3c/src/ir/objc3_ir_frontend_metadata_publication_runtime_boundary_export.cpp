#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_export.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_export_enforcement_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_export_runtime_boundary_rows.h"

void EmitObjc3IRRuntimeExportBoundaryNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRRuntimeExportBoundaryMetadataNode(metadata, out);
  EmitObjc3IRRuntimeExportEnforcementMetadataNode(metadata, out);
}
