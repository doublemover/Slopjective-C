#include "ir/objc3_ir_frontend_metadata_publication_runtime_support.h"

#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_debug_projection.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_object_inspection.h"

void EmitObjc3IRRuntimeSupportMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRRuntimeObjectInspectionMetadataNode(metadata, out);
  EmitObjc3IRRuntimeSupportLibraryMetadataNodes(metadata, out);
  EmitObjc3IRExecutableDebugProjectionMetadataNode(metadata, out);
}
